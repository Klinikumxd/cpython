"""Print a summary of specialization stats for all files in the
default stats folders.
"""

from __future__ import annotations

# NOTE: Bytecode introspection modules (opcode, dis, etc.) should only
# be imported when loading a single dataset. When comparing datasets, it
# could get it wrong, leading to subtle errors.

import argparse
import collections
from collections.abc import KeysView
from datetime import date
import enum
import functools
import itertools
import json
from operator import itemgetter
import os
from pathlib import Path
import re
import sys
from typing import Any, Callable, TextIO, TypeAlias


RawData: TypeAlias = dict[str, Any]
Rows: TypeAlias = list[tuple]
Columns: TypeAlias = tuple[str, ...]
RowCalculator: TypeAlias = Callable[["Stats"], Rows]


# TODO: Check for parity


if os.name == "nt":
    DEFAULT_DIR = "c:\\temp\\py_stats\\"
else:
    DEFAULT_DIR = "/tmp/py_stats/"


SOURCE_DIR = Path(__file__).parents[2]


TOTAL = "specialization.hit", "specialization.miss", "execution_count"


def pretty(name: str) -> str:
    return name.replace("_", " ").lower()


def _load_metadata_from_source():
    def get_defines(filepath: Path, prefix: str = "SPEC_FAIL"):
        with open(SOURCE_DIR / filepath) as spec_src:
            defines = collections.defaultdict(list)
            start = "#define " + prefix + "_"
            for line in spec_src:
                line = line.strip()
                if not line.startswith(start):
                    continue
                line = line[len(start) :]
                name, val = line.split()
                defines[int(val.strip())].append(name.strip())
        return defines

    import opcode

    return {
        "_specialized_instructions": [
            op for op in opcode._specialized_opmap.keys() if "__" not in op  # type: ignore
        ],
        "_stats_defines": get_defines(
            Path("Include") / "cpython" / "pystats.h", "EVAL_CALL"
        ),
        "_defines": get_defines(Path("Python") / "specialize.c"),
    }


def load_raw_data(input: Path) -> RawData:
    if input.is_file():
        with open(input, "r") as fd:
            data = json.load(fd)

        data["_stats_defines"] = {int(k): v for k, v in data["_stats_defines"].items()}
        data["_defines"] = {int(k): v for k, v in data["_defines"].items()}

        return data

    elif input.is_dir():
        stats = collections.Counter[str]()

        for filename in input.iterdir():
            with open(filename) as fd:
                for line in fd:
                    try:
                        key, value = line.split(":")
                    except ValueError:
                        print(
                            f"Unparsable line: '{line.strip()}' in {filename}",
                            file=sys.stderr,
                        )
                        continue
                    stats[key.strip()] += int(value)
            stats["__nfiles__"] += 1

        data = dict(stats)
        data.update(_load_metadata_from_source())
        return data

    else:
        raise ValueError(f"{input:r} is not a file or directory path")


def save_raw_data(data: RawData, json_output: TextIO):
    json.dump(data, json_output)


class OpcodeStats:
    """
    Manages the data related to specific set of opcodes, e.g. tier1 (with prefix
    "opcode") or tier2 (with prefix "uops").
    """

    def __init__(self, data: dict[str, Any], defines, specialized_instructions):
        self._data = data
        self._defines = defines
        self._specialized_instructions = specialized_instructions

    def get_opcode_names(self) -> KeysView[str]:
        return self._data.keys()

    def get_pair_counts(self) -> dict[tuple[str, str], int]:
        pair_counts = {}
        for name_i, opcode_stat in self._data.items():
            for key, value in opcode_stat.items():
                if value and key.startswith("pair_count"):
                    name_j, _, _ = key[len("pair_count") + 1 :].partition("]")
                    pair_counts[(name_i, name_j)] = value
        return pair_counts

    def get_total_execution_count(self) -> int:
        return sum(x.get("execution_count", 0) for x in self._data.values())

    def get_execution_counts(self) -> dict[str, tuple[int, int]]:
        counts = {}
        for name, opcode_stat in self._data.items():
            if "execution_count" in opcode_stat:
                count = opcode_stat["execution_count"]
                miss = 0
                if "specializable" not in opcode_stat:
                    miss = opcode_stat.get("specialization.miss", 0)
                counts[name] = (count, miss)
        return counts

    @functools.cache
    def _get_pred_succ(
        self,
    ) -> tuple[dict[str, collections.Counter], dict[str, collections.Counter]]:
        pair_counts = self.get_pair_counts()

        predecessors: dict[str, collections.Counter] = collections.defaultdict(
            collections.Counter
        )
        successors: dict[str, collections.Counter] = collections.defaultdict(
            collections.Counter
        )
        for (first, second), count in pair_counts.items():
            if count:
                predecessors[second][first] = count
                successors[first][second] = count

        return predecessors, successors

    def get_predecessors(self, opcode: str) -> collections.Counter[str]:
        return self._get_pred_succ()[0][opcode]

    def get_successors(self, opcode: str) -> collections.Counter[str]:
        return self._get_pred_succ()[1][opcode]

    def _get_stats_for_opcode(self, opcode: str) -> dict[str, int]:
        return self._data[opcode]

    def get_specialization_total(self, opcode: str) -> int:
        family_stats = self._get_stats_for_opcode(opcode)
        return sum(family_stats.get(kind, 0) for kind in TOTAL)

    def get_specialization_counts(self, opcode: str) -> dict[str, int]:
        family_stats = self._get_stats_for_opcode(opcode)

        result = {}
        for key, value in sorted(family_stats.items()):
            if key.startswith("specialization."):
                label = key[len("specialization.") :]
                if label in ("success", "failure") or label.startswith("failure_kinds"):
                    continue
            elif key in (
                "execution_count",
                "specializable",
            ) or key.startswith("pair"):
                continue
            else:
                label = key
            result[label] = value

        return result

    def get_specialization_success_failure(self, opcode: str) -> dict[str, int]:
        family_stats = self._get_stats_for_opcode(opcode)
        result = {}
        for key in ("specialization.success", "specialization.failure"):
            label = key[len("specialization.") :]
            val = family_stats.get(key, 0)
            result[label] = val
        return result

    def get_specialization_failure_total(self, opcode: str) -> int:
        return self._get_stats_for_opcode(opcode).get("specialization.failure", 0)

    def get_specialization_failure_kinds(self, opcode: str) -> dict[str, int]:
        def kind_to_text(kind: int, opcode: str):
            if kind <= 8:
                return pretty(self._defines[kind][0])
            if opcode == "LOAD_SUPER_ATTR":
                opcode = "SUPER"
            elif opcode.endswith("ATTR"):
                opcode = "ATTR"
            elif opcode in ("FOR_ITER", "SEND"):
                opcode = "ITER"
            elif opcode.endswith("SUBSCR"):
                opcode = "SUBSCR"
            for name in self._defines[kind]:
                if name.startswith(opcode):
                    return pretty(name[len(opcode) + 1 :])
            return "kind " + str(kind)

        family_stats = self._get_stats_for_opcode(opcode)
        failure_kinds = [0] * 40
        for key in family_stats:
            if not key.startswith("specialization.failure_kind"):
                continue
            index = int(key[:-1].split("[")[1])
            failure_kinds[index] = family_stats[key]
        return {
            kind_to_text(index, opcode): value
            for (index, value) in enumerate(failure_kinds)
            if value
        }

    def is_specializable(self, opcode: str) -> bool:
        return "specializable" in self._get_stats_for_opcode(opcode)

    def get_specialized_total_counts(self) -> tuple[int, int, int]:
        basic = 0
        specialized_hits = 0
        specialized_misses = 0
        not_specialized = 0
        for opcode, opcode_stat in self._data.items():
            if "execution_count" not in opcode_stat:
                continue
            count = opcode_stat["execution_count"]
            if "specializable" in opcode_stat:
                not_specialized += count
            elif opcode in self._specialized_instructions:
                miss = opcode_stat.get("specialization.miss", 0)
                specialized_hits += count - miss
                specialized_misses += miss
            else:
                basic += count
        return basic, specialized_hits, specialized_misses, not_specialized

    def get_deferred_counts(self) -> dict[str, int]:
        return {
            opcode: opcode_stat.get("specialization.deferred", 0)
            for opcode, opcode_stat in self._data.items()
            if opcode != "RESUME"
        }

    def get_misses_counts(self) -> dict[str, int]:
        return {
            opcode: opcode_stat.get("specialization.miss", 0)
            for opcode, opcode_stat in self._data.items()
            if not self.is_specializable(opcode)
        }

    def get_opcode_counts(self) -> dict[str, int]:
        counts = {}
        for opcode, entry in self._data.items():
            count = entry.get("count", 0)
            if count:
                counts[opcode] = count
        return counts


class Stats:
    def __init__(self, data: RawData):
        self._data = data

    def get(self, key: str) -> int:
        return self._data.get(key, 0)

    @functools.cache
    def get_opcode_stats(self, prefix: str) -> OpcodeStats:
        opcode_stats = collections.defaultdict[str, dict](dict)
        for key, value in self._data.items():
            if not key.startswith(prefix):
                continue
            name, _, rest = key[len(prefix) + 1 :].partition("]")
            opcode_stats[name][rest.strip(".")] = value
        return OpcodeStats(
            opcode_stats,
            self._data["_defines"],
            self._data["_specialized_instructions"],
        )

    def get_call_stats(self) -> dict[str, int]:
        defines = self._data["_stats_defines"]
        result = {}
        for key, value in sorted(self._data.items()):
            if "Calls to" in key:
                result[key] = value
            elif key.startswith("Calls "):
                name, index = key[:-1].split("[")
                label = f"{name} ({pretty(defines[int(index)][0])})"
                result[label] = value

        for key, value in sorted(self._data.items()):
            if key.startswith("Frame"):
                result[key] = value

        return result

    def get_object_stats(self) -> dict[str, tuple[int, int]]:
        total_materializations = self._data.get("Object new values", 0)
        total_allocations = self._data.get("Object allocations", 0) + self._data.get(
            "Object allocations from freelist", 0
        )
        total_increfs = self._data.get(
            "Object interpreter increfs", 0
        ) + self._data.get("Object increfs", 0)
        total_decrefs = self._data.get(
            "Object interpreter decrefs", 0
        ) + self._data.get("Object decrefs", 0)

        result = {}
        for key, value in self._data.items():
            if key.startswith("Object"):
                if "materialize" in key:
                    den = total_materializations
                elif "allocations" in key:
                    den = total_allocations
                elif "increfs" in key:
                    den = total_increfs
                elif "decrefs" in key:
                    den = total_decrefs
                else:
                    den = None
                label = key[6:].strip()
                label = label[0].upper() + label[1:]
                result[label] = (value, den)
        return result

    def get_gc_stats(self) -> list[dict[str, int]]:
        gc_stats: list[dict[str, int]] = []
        for key, value in self._data.items():
            if not key.startswith("GC"):
                continue
            n, _, rest = key[3:].partition("]")
            name = rest.strip()
            gen_n = int(n)
            while len(gc_stats) <= gen_n:
                gc_stats.append({})
            gc_stats[gen_n][name] = value
        return gc_stats

    def get_optimization_stats(self) -> dict[str, tuple[int, int | None]]:
        if "Optimization attempts" not in self._data:
            return {}

        attempts = self._data["Optimization attempts"]
        created = self._data["Optimization traces created"]
        executed = self._data["Optimization traces executed"]
        uops = self._data["Optimization uops executed"]
        trace_stack_overflow = self._data["Optimization trace stack overflow"]
        trace_stack_underflow = self._data["Optimization trace stack underflow"]
        trace_too_long = self._data["Optimization trace too long"]
        trace_too_short = self._data["Optimization trace too short"]
        inner_loop = self._data["Optimization inner loop"]
        recursive_call = self._data["Optimization recursive call"]

        return {
            "Optimization attempts": (attempts, None),
            "Traces created": (created, attempts),
            "Trace stack overflow": (trace_stack_overflow, attempts),
            "Trace stack underflow": (trace_stack_underflow, attempts),
            "Trace too long": (trace_too_long, attempts),
            "Trace too short": (trace_too_short, attempts),
            "Inner loop found": (inner_loop, attempts),
            "Recursive call": (recursive_call, attempts),
            "Traces executed": (executed, None),
            "Uops executed": (uops, executed),
        }

    def get_histogram(self, prefix: str) -> list[tuple[int, int]]:
        rows = []
        for k, v in self._data.items():
            match = re.match(f"{prefix}\\[([0-9]+)\\]", k)
            if match is not None:
                entry = int(match.groups()[0])
                rows.append((entry, v))
        rows.sort()
        return rows


class Count(int):
    def markdown(self) -> str:
        return format(self, ",d")


class Ratio:
    def __init__(self, num: int, den: int | None, percentage: bool = True):
        self.num = num
        self.den = den
        self.percentage = percentage
        if den == 0 and num != 0:
            raise ValueError("Invalid denominator")

    def __float__(self):
        if self.den == 0:
            return 0.0
        elif self.den is None:
            return self.num
        else:
            return self.num / self.den

    def markdown(self) -> str:
        if self.den == 0 or self.den is None:
            return ""
        elif self.percentage:
            return f"{self.num / self.den:,.01%}"
        else:
            return f"{self.num / self.den:,.02f}"


class DiffRatio(Ratio):
    def __init__(self, base: int | str, head: int | str):
        if isinstance(base, str) or isinstance(head, str):
            super().__init__(0, 0)
        else:
            super().__init__(head - base, base)


class JoinMode(enum.Enum):
    # Join using the first column as a key
    SIMPLE = 0
    # Join using the first column as a key, and indicate the change in the
    # second column of each input table as a new column
    CHANGE = 1
    # Join using the first column as a key, indicating the change in the second
    # column of each input table as a ne column, and omit all other columns
    CHANGE_ONE_COLUMN = 2


class Table:
    """
    A Table defines how to convert a set of Stats into a specific set of rows
    displaying some aspect of the data.
    """

    def __init__(
        self,
        column_names: Columns,
        calc_rows: RowCalculator,
        join_mode: JoinMode = JoinMode.SIMPLE,
    ):
        self.columns = column_names
        self.calc_rows = calc_rows
        self.join_mode = join_mode

    def join_row(self, key: str, row_a: tuple, row_b: tuple) -> tuple:
        match self.join_mode:
            case JoinMode.SIMPLE:
                return (key, *row_a, *row_b)
            case JoinMode.CHANGE:
                return (key, *row_a, *row_b, DiffRatio(row_a[0], row_b[0]))
            case JoinMode.CHANGE_ONE_COLUMN:
                return (key, row_a[0], row_b[0], DiffRatio(row_a[0], row_b[0]))

    def join_columns(self, columns: Columns) -> Columns:
        match self.join_mode:
            case JoinMode.SIMPLE:
                return (
                    columns[0],
                    *("Base " + x for x in columns[1:]),
                    *("Head " + x for x in columns[1:]),
                )
            case JoinMode.CHANGE:
                return (
                    columns[0],
                    *("Base " + x for x in columns[1:]),
                    *("Head " + x for x in columns[1:]),
                ) + ("Change:",)
            case JoinMode.CHANGE_ONE_COLUMN:
                return (
                    columns[0],
                    "Base " + columns[1],
                    "Head " + columns[1],
                    "Change:",
                )

    def join_tables(self, rows_a: Rows, rows_b: Rows) -> tuple[Columns, Rows]:
        ncols = len(self.columns)

        default = ("",) * (ncols - 1)
        data_a = {x[0]: x[1:] for x in rows_a}
        data_b = {x[0]: x[1:] for x in rows_b}

        if len(data_a) != len(rows_a) or len(data_b) != len(rows_b):
            raise ValueError("Duplicate keys")

        # To preserve ordering, use A's keys as is and then add any in B that
        # aren't in A
        keys = list(data_a.keys()) + [k for k in data_b.keys() if k not in data_a]
        rows = [
            self.join_row(k, data_a.get(k, default), data_b.get(k, default))
            for k in keys
        ]
        if self.join_mode in (JoinMode.CHANGE, JoinMode.CHANGE_ONE_COLUMN):
            rows.sort(key=lambda row: abs(float(row[-1])), reverse=True)

        columns = self.join_columns(self.columns)
        return columns, rows

    def get_table(
        self, base_stats: Stats, head_stats: Stats | None = None
    ) -> tuple[Columns, Rows]:
        if head_stats is None:
            rows = self.calc_rows(base_stats)
            return self.columns, rows
        else:
            rows_a = self.calc_rows(base_stats)
            rows_b = self.calc_rows(head_stats)
            cols, rows = self.join_tables(rows_a, rows_b)
            return cols, rows


class Section:
    """
    A Section defines a section of the output document.
    """

    def __init__(
        self,
        title: str = "",
        summary: str = "",
        part_iter=None,
        comparative: bool = True,
    ):
        self.title = title
        if not summary:
            self.summary = title.lower()
        else:
            self.summary = summary
        if part_iter is None:
            part_iter = []
        if isinstance(part_iter, list):

            def iter_parts(base_stats: Stats, head_stats: Stats | None):
                yield from part_iter

            self.part_iter = iter_parts
        else:
            self.part_iter = part_iter
        self.comparative = comparative


def calc_execution_count_table(prefix: str) -> RowCalculator:
    def calc(stats: Stats) -> Rows:
        opcode_stats = stats.get_opcode_stats(prefix)
        counts = opcode_stats.get_execution_counts()
        total = opcode_stats.get_total_execution_count()
        cumulative = 0
        rows: Rows = []
        for opcode, (count, miss) in sorted(
            counts.items(), key=itemgetter(1), reverse=True
        ):
            cumulative += count
            if miss:
                miss_val = Ratio(miss, count)
            else:
                miss_val = None
            rows.append(
                (
                    opcode,
                    Count(count),
                    Ratio(count, total),
                    Ratio(cumulative, total),
                    miss_val,
                )
            )
        return rows

    return calc


def execution_count_section() -> Section:
    return Section(
        "Execution counts",
        "execution counts for all instructions",
        [
            Table(
                ("Name", "Count:", "Self:", "Cumulative:", "Miss ratio:"),
                calc_execution_count_table("opcode"),
                join_mode=JoinMode.CHANGE_ONE_COLUMN,
            )
        ],
    )


def pair_count_section() -> Section:
    def calc_pair_count_table(stats: Stats) -> Rows:
        opcode_stats = stats.get_opcode_stats("opcode")
        pair_counts = opcode_stats.get_pair_counts()
        total = opcode_stats.get_total_execution_count()

        cumulative = 0
        rows: Rows = []
        for (opcode_i, opcode_j), count in itertools.islice(
            sorted(pair_counts.items(), key=itemgetter(1), reverse=True), 100
        ):
            cumulative += count
            rows.append(
                (
                    f"{opcode_i} {opcode_j}",
                    Count(count),
                    Ratio(count, total),
                    Ratio(cumulative, total),
                )
            )
        return rows

    return Section(
        "Pair counts",
        "Pair counts for top 100 pairs",
        [
            Table(
                ("Pair", "Count:", "Self:", "Cumulative:"),
                calc_pair_count_table,
            )
        ],
        comparative=False,
    )


def pre_succ_pairs_section() -> Section:
    def iter_pre_succ_pairs_tables(base_stats: Stats, head_stats: Stats | None = None):
        assert head_stats is None

        opcode_stats = base_stats.get_opcode_stats("opcode")

        for opcode in opcode_stats.get_opcode_names():
            predecessors = opcode_stats.get_predecessors(opcode)
            successors = opcode_stats.get_successors(opcode)
            predecessors_total = predecessors.total()
            successors_total = successors.total()
            if predecessors_total == 0 and successors_total == 0:
                continue
            pred_rows = [
                (pred, Count(count), Ratio(count, predecessors_total))
                for (pred, count) in predecessors.most_common(5)
            ]
            succ_rows = [
                (succ, Count(count), Ratio(count, successors_total))
                for (succ, count) in successors.most_common(5)
            ]

            yield Section(
                opcode,
                f"Successors and predecessors for {opcode}",
                [
                    Table(
                        ("Predecessors", "Count:", "Percentage:"),
                        lambda *_: pred_rows,  # type: ignore
                    ),
                    Table(
                        ("Successors", "Count:", "Percentage:"),
                        lambda *_: succ_rows,  # type: ignore
                    ),
                ],
            )

    return Section(
        "Predecessor/Successor Pairs",
        "Top 5 predecessors and successors of each opcode",
        iter_pre_succ_pairs_tables,
        comparative=False,
    )


def specialization_section() -> Section:
    def calc_specialization_table(opcode: str) -> RowCalculator:
        def calc(stats: Stats) -> Rows:
            opcode_stats = stats.get_opcode_stats("opcode")
            total = opcode_stats.get_specialization_total(opcode)
            specialization_counts = opcode_stats.get_specialization_counts(opcode)

            return [
                (
                    f"{label:>12}",
                    Count(count),
                    Ratio(count, total),
                )
                for label, count in specialization_counts.items()
            ]

        return calc

    def calc_specialization_success_failure_table(name: str) -> RowCalculator:
        def calc(stats: Stats) -> Rows:
            values = stats.get_opcode_stats(
                "opcode"
            ).get_specialization_success_failure(name)
            total = sum(values.values())
            if total:
                return [
                    (label.capitalize(), Count(val), Ratio(val, total))
                    for label, val in values.items()
                ]
            else:
                return []

        return calc

    def calc_specialization_failure_kind_table(name: str) -> RowCalculator:
        def calc(stats: Stats) -> Rows:
            opcode_stats = stats.get_opcode_stats("opcode")
            failures = opcode_stats.get_specialization_failure_kinds(name)
            total = opcode_stats.get_specialization_failure_total(name)

            return sorted(
                [
                    (label, Count(value), Ratio(value, total))
                    for label, value in failures.items()
                    if value
                ],
                key=itemgetter(1),
                reverse=True,
            )

        return calc

    def iter_specialization_tables(base_stats: Stats, head_stats: Stats | None = None):
        opcode_base_stats = base_stats.get_opcode_stats("opcode")
        names = opcode_base_stats.get_opcode_names()
        if head_stats is not None:
            opcode_head_stats = head_stats.get_opcode_stats("opcode")
            names &= opcode_head_stats.get_opcode_names()  # type: ignore
        else:
            opcode_head_stats = None

        for opcode in sorted(names):
            if not opcode_base_stats.is_specializable(opcode):
                continue
            if opcode_base_stats.get_specialization_total(opcode) == 0 and (
                opcode_head_stats is None
                or opcode_head_stats.get_specialization_total(opcode) == 0
            ):
                continue
            yield Section(
                opcode,
                f"specialization stats for {opcode} family",
                [
                    Table(
                        ("Kind", "Count:", "Ratio:"),
                        calc_specialization_table(opcode),
                        JoinMode.CHANGE,
                    ),
                    Table(
                        ("", "Count:", "Ratio:"),
                        calc_specialization_success_failure_table(opcode),
                        JoinMode.CHANGE,
                    ),
                    Table(
                        ("Failure kind", "Count:", "Ratio:"),
                        calc_specialization_failure_kind_table(opcode),
                        JoinMode.CHANGE,
                    ),
                ],
            )

    return Section(
        "Specialization stats",
        "specialization stats by family",
        iter_specialization_tables,
    )


def specialization_effectiveness_section() -> Section:
    def calc_specialization_effectiveness_table(stats: Stats) -> Rows:
        opcode_stats = stats.get_opcode_stats("opcode")
        total = opcode_stats.get_total_execution_count()

        (
            basic,
            specialized_hits,
            specialized_misses,
            not_specialized,
        ) = opcode_stats.get_specialized_total_counts()

        return [
            ("Basic", Count(basic), Ratio(basic, total)),
            (
                "Not specialized",
                Count(not_specialized),
                Ratio(not_specialized, total),
            ),
            (
                "Specialized hits",
                Count(specialized_hits),
                Ratio(specialized_hits, total),
            ),
            (
                "Specialized misses",
                Count(specialized_misses),
                Ratio(specialized_misses, total),
            ),
        ]

    def calc_deferred_by_table(stats: Stats) -> Rows:
        opcode_stats = stats.get_opcode_stats("opcode")
        deferred_counts = opcode_stats.get_deferred_counts()
        total = sum(deferred_counts.values())
        if total == 0:
            return []

        return [
            (name, Count(value), Ratio(value, total))
            for name, value in sorted(
                deferred_counts.items(), key=itemgetter(1), reverse=True
            )[:10]
        ]

    def calc_misses_by_table(stats: Stats) -> Rows:
        opcode_stats = stats.get_opcode_stats("opcode")
        misses_counts = opcode_stats.get_misses_counts()
        total = sum(misses_counts.values())
        if total == 0:
            return []

        return [
            (name, Count(value), Ratio(value, total))
            for name, value in sorted(
                misses_counts.items(), key=itemgetter(1), reverse=True
            )[:10]
        ]

    return Section(
        "Specialization effectiveness",
        "",
        [
            Table(
                ("Instructions", "Count:", "Ratio:"),
                calc_specialization_effectiveness_table,
                JoinMode.CHANGE,
            ),
            Section(
                "Deferred by instruction",
                "",
                [
                    Table(
                        ("Name", "Count:", "Ratio:"),
                        calc_deferred_by_table,
                        JoinMode.CHANGE,
                    )
                ],
            ),
            Section(
                "Misses by instruction",
                "",
                [
                    Table(
                        ("Name", "Count:", "Ratio:"),
                        calc_misses_by_table,
                        JoinMode.CHANGE,
                    )
                ],
            ),
        ],
    )


def call_stats_section() -> Section:
    def calc_call_stats_table(stats: Stats) -> Rows:
        call_stats = stats.get_call_stats()
        total = sum(v for k, v in call_stats.items() if "Calls to" in k)
        return [
            (key, Count(value), Ratio(value, total))
            for key, value in call_stats.items()
        ]

    return Section(
        "Call stats",
        "Inlined calls and frame stats",
        [
            Table(
                ("", "Count:", "Ratio:"),
                calc_call_stats_table,
                JoinMode.CHANGE,
            )
        ],
    )


def object_stats_section() -> Section:
    def calc_object_stats_table(stats: Stats) -> Rows:
        object_stats = stats.get_object_stats()
        return [
            (label, Count(value), Ratio(value, den))
            for label, (value, den) in object_stats.items()
        ]

    return Section(
        "Object stats",
        "allocations, frees and dict materializatons",
        [
            Table(
                ("", "Count:", "Ratio:"),
                calc_object_stats_table,
                JoinMode.CHANGE,
            )
        ],
    )


def gc_stats_section() -> Section:
    def calc_gc_stats(stats: Stats) -> Rows:
        gc_stats = stats.get_gc_stats()

        return [
            (
                Count(i),
                Count(gen["collections"]),
                Count(gen["objects collected"]),
                Count(gen["object visits"]),
            )
            for (i, gen) in enumerate(gc_stats)
        ]

    return Section(
        "GC stats",
        "GC collections and effectiveness",
        [
            Table(
                ("Generation:", "Collections:", "Objects collected:", "Object visits:"),
                calc_gc_stats,
            )
        ],
    )


def optimization_section() -> Section:
    def calc_optimization_table(stats: Stats) -> Rows:
        optimization_stats = stats.get_optimization_stats()

        return [
            (
                label,
                Count(value),
                Ratio(value, den, percentage=label != "Uops executed"),
            )
            for label, (value, den) in optimization_stats.items()
        ]

    def calc_histogram_table(key: str, den: str) -> RowCalculator:
        def calc(stats: Stats) -> Rows:
            histogram = stats.get_histogram(key)
            denominator = stats.get(den)

            rows: Rows = []
            last_non_zero = 0
            for k, v in histogram:
                if v != 0:
                    last_non_zero = len(rows)
                rows.append(
                    (
                        f"<= {k:,d}",
                        Count(v),
                        Ratio(v, denominator),
                    )
                )
            # Don't include any zero entries at the end
            rows = rows[: last_non_zero + 1]
            return rows

        return calc

    def calc_unsupported_opcodes_table(stats: Stats) -> Rows:
        unsupported_opcodes = stats.get_opcode_stats("unsupported_opcode")
        return sorted(
            [
                (opcode, Count(count))
                for opcode, count in unsupported_opcodes.get_opcode_counts().items()
            ],
            key=itemgetter(1),
            reverse=True,
        )

    def iter_optimization_tables(base_stats: Stats, head_stats: Stats | None = None):
        if not base_stats.get_optimization_stats() or (
            head_stats is not None and not head_stats.get_optimization_stats()
        ):
            return

        yield Table(("", "Count:", "Ratio:"), calc_optimization_table, JoinMode.CHANGE)
        for name, den in [
            ("Trace length", "Optimization traces created"),
            ("Optimized trace length", "Optimization traces created"),
            ("Trace run length", "Optimization traces executed"),
        ]:
            yield Section(
                f"{name} histogram",
                "",
                [
                    Table(
                        ("Range", "Count:", "Ratio:"),
                        calc_histogram_table(name, den),
                        JoinMode.CHANGE,
                    )
                ],
            )
        yield Section(
            "Uop execution stats",
            "",
            [
                Table(
                    ("Name", "Count:", "Self:", "Cumulative:", "Miss ratio:"),
                    calc_execution_count_table("uops"),
                    JoinMode.CHANGE_ONE_COLUMN,
                )
            ],
        )
        yield Section(
            "Unsupported opcodes",
            "",
            [
                Table(
                    ("Opcode", "Count:"),
                    calc_unsupported_opcodes_table,
                    JoinMode.CHANGE,
                )
            ],
        )

    return Section(
        "Optimization (Tier 2) stats",
        "statistics about the Tier 2 optimizer",
        iter_optimization_tables,
    )


def meta_stats_section() -> Section:
    def calc_rows(stats: Stats) -> Rows:
        return [("Number of data files", Count(stats.get("__nfiles__")))]

    return Section(
        "Meta stats",
        "Meta statistics",
        [Table(("", "Count:"), calc_rows, JoinMode.CHANGE)],
    )


LAYOUT = [
    execution_count_section(),
    pair_count_section(),
    pre_succ_pairs_section(),
    specialization_section(),
    specialization_effectiveness_section(),
    call_stats_section(),
    object_stats_section(),
    gc_stats_section(),
    optimization_section(),
    meta_stats_section(),
]


def output_markdown(
    out: TextIO,
    obj: Section | Table | list,
    base_stats: Stats,
    head_stats: Stats | None = None,
    level: int = 2,
) -> None:
    def to_markdown(x):
        if hasattr(x, "markdown"):
            return x.markdown()
        elif isinstance(x, str):
            return x
        elif x is None:
            return ""
        else:
            raise TypeError(f"Can't convert {x} to markdown")

    match obj:
        case Section():
            if obj.title:
                print("#" * level, obj.title, file=out)
                print(file=out)
                print("<details>", file=out)
                print("<summary>", obj.summary, "</summary>", file=out)
                print(file=out)
            if head_stats is not None and obj.comparative is False:
                print("Not included in comparative output.\n")
            else:
                for part in obj.part_iter(base_stats, head_stats):
                    output_markdown(out, part, base_stats, head_stats, level=level + 1)
            print(file=out)
            if obj.title:
                print("</details>", file=out)
                print(file=out)

        case Table():
            header, rows = obj.get_table(base_stats, head_stats)
            if len(rows) == 0:
                return

            width = len(header)
            header_line = "|"
            under_line = "|"
            for item in header:
                under = "---"
                if item.endswith(":"):
                    item = item[:-1]
                    under += ":"
                header_line += item + " | "
                under_line += under + "|"
            print(header_line, file=out)
            print(under_line, file=out)
            for row in rows:
                if len(row) != width:
                    raise ValueError(
                        "Wrong number of elements in row '" + str(row) + "'"
                    )
                print("|", " | ".join(to_markdown(i) for i in row), "|", file=out)
            print(file=out)

        case list():
            for part in obj:
                output_markdown(out, part, base_stats, head_stats, level=level)

            print("---", file=out)
            print("Stats gathered on:", date.today(), file=out)


def output_stats(inputs: list[Path], json_output=TextIO | None):
    match len(inputs):
        case 1:
            data = load_raw_data(Path(inputs[0]))
            if json_output is not None:
                save_raw_data(data, json_output)  # type: ignore
            stats = Stats(data)
            output_markdown(sys.stdout, LAYOUT, stats)
        case 2:
            if json_output is not None:
                raise ValueError(
                    "Can not output to JSON when there are multiple inputs"
                )
            base_data = load_raw_data(Path(inputs[0]))
            head_data = load_raw_data(Path(inputs[1]))
            base_stats = Stats(base_data)
            head_stats = Stats(head_data)
            output_markdown(sys.stdout, LAYOUT, base_stats, head_stats)


def main():
    parser = argparse.ArgumentParser(description="Summarize pystats results")

    parser.add_argument(
        "inputs",
        nargs="*",
        type=str,
        default=[DEFAULT_DIR],
        help=f"""
        Input source(s).
        For each entry, if a .json file, the output provided by --json-output from a previous run;
        if a directory, a directory containing raw pystats .txt files.
        If one source is provided, its stats are printed.
        If two sources are provided, comparative stats are printed.
        Default is {DEFAULT_DIR}.
        """,
    )

    parser.add_argument(
        "--json-output",
        nargs="?",
        type=argparse.FileType("w"),
        help="Output complete raw results to the given JSON file.",
    )

    args = parser.parse_args()

    if len(args.inputs) > 2:
        raise ValueError("0-2 arguments may be provided.")

    output_stats(args.inputs, json_output=args.json_output)


if __name__ == "__main__":
    main()
