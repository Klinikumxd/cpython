from pathlib import Path
from typing import TextIO

from analyzer import (
    Instruction,
    Uop,
    Properties,
    StackItem,
)
from cwriter import CWriter
from typing import Callable, Mapping, TextIO, Iterator, Tuple
from lexer import Token
from stack import Stack


ROOT = Path(__file__).parent.parent.parent
DEFAULT_INPUT = (ROOT / "Python/bytecodes.c").absolute().as_posix()


def root_relative_path(filename: str) -> str:
    try:
        return Path(filename).absolute().relative_to(ROOT).as_posix()
    except ValueError:
        # Not relative to root, just return original path.
        return filename


def type_and_null(var: StackItem) -> Tuple[str, str]:
    if var.type:
        return var.type, "NULL"
    elif var.is_array():
        return "_PyStackRef *", "NULL"
    else:
        return "_PyStackRef", "PyStackRef_NULL"


def write_header(
    generator: str, sources: list[str], outfile: TextIO, comment: str = "//"
) -> None:
    outfile.write(
        f"""{comment} This file is generated by {root_relative_path(generator)}
{comment} from:
{comment}   {", ".join(root_relative_path(src) for src in sources)}
{comment} Do not edit!
"""
    )


def emit_to(out: CWriter, tkn_iter: Iterator[Token], end: str) -> None:
    parens = 0
    for tkn in tkn_iter:
        if tkn.kind == end and parens == 0:
            return
        if tkn.kind == "LPAREN":
            parens += 1
        if tkn.kind == "RPAREN":
            parens -= 1
        out.emit(tkn)


def replace_deopt(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    unused: Stack,
    inst: Instruction | None,
) -> None:
    out.emit_at("DEOPT_IF", tkn)
    out.emit(next(tkn_iter))
    emit_to(out, tkn_iter, "RPAREN")
    next(tkn_iter)  # Semi colon
    out.emit(", ")
    assert inst is not None
    assert inst.family is not None
    out.emit(inst.family.name)
    out.emit(");\n")


def replace_error(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
) -> None:
    out.emit_at("if ", tkn)
    out.emit(next(tkn_iter))
    emit_to(out, tkn_iter, "COMMA")
    label = next(tkn_iter).text
    next(tkn_iter)  # RPAREN
    next(tkn_iter)  # Semi colon
    out.emit(") ")
    c_offset = stack.peek_offset.to_c()
    try:
        offset = -int(c_offset)
        close = ";\n"
    except ValueError:
        offset = None
        out.emit(f"{{ stack_pointer += {c_offset}; ")
        close = "; }\n"
    out.emit("goto ")
    if offset:
        out.emit(f"pop_{offset}_")
    out.emit(label)
    out.emit(close)


def replace_error_no_pop(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
) -> None:
    next(tkn_iter)  # LPAREN
    next(tkn_iter)  # RPAREN
    next(tkn_iter)  # Semi colon
    out.emit_at("goto error;", tkn)


def replace_decrefs(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
) -> None:
    next(tkn_iter)
    next(tkn_iter)
    next(tkn_iter)
    out.emit_at("", tkn)
    for var in uop.stack.inputs:
        if var.name == "unused" or var.name == "null" or var.peek:
            continue
        if var.size:
            out.emit(f"for (int _i = {var.size}; --_i >= 0;) {{\n")
            out.emit(f"PyStackRef_CLOSE({var.name}[_i]);\n")
            out.emit("}\n")
        elif var.condition:
            if var.condition == "1":
                out.emit(f"PyStackRef_CLOSE({var.name});\n")
            elif var.condition != "0":
                out.emit(f"PyStackRef_XCLOSE({var.name});\n")
        else:
            out.emit(f"PyStackRef_CLOSE({var.name});\n")


def replace_sync_sp(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
) -> None:
    next(tkn_iter)
    next(tkn_iter)
    next(tkn_iter)
    stack.flush(out)


def replace_check_eval_breaker(
    out: CWriter,
    tkn: Token,
    tkn_iter: Iterator[Token],
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
) -> None:
    next(tkn_iter)
    next(tkn_iter)
    next(tkn_iter)
    if not uop.properties.ends_with_eval_breaker:
        out.emit_at("CHECK_EVAL_BREAKER();", tkn)


REPLACEMENT_FUNCTIONS = {
    "EXIT_IF": replace_deopt,
    "DEOPT_IF": replace_deopt,
    "ERROR_IF": replace_error,
    "ERROR_NO_POP": replace_error_no_pop,
    "DECREF_INPUTS": replace_decrefs,
    "CHECK_EVAL_BREAKER": replace_check_eval_breaker,
    "SYNC_SP": replace_sync_sp,
}

ReplacementFunctionType = Callable[
    [CWriter, Token, Iterator[Token], Uop, Stack, Instruction | None], None
]


def emit_tokens(
    out: CWriter,
    uop: Uop,
    stack: Stack,
    inst: Instruction | None,
    replacement_functions: Mapping[
        str, ReplacementFunctionType
    ] = REPLACEMENT_FUNCTIONS,
) -> None:
    tkns = uop.body[1:-1]
    if not tkns:
        return
    tkn_iter = iter(tkns)
    out.start_line()
    for tkn in tkn_iter:
        if tkn.kind == "IDENTIFIER" and tkn.text in replacement_functions:
            replacement_functions[tkn.text](out, tkn, tkn_iter, uop, stack, inst)
        else:
            out.emit(tkn)


def cflags(p: Properties) -> str:
    flags: list[str] = []
    if p.oparg:
        flags.append("HAS_ARG_FLAG")
    if p.uses_co_consts:
        flags.append("HAS_CONST_FLAG")
    if p.uses_co_names:
        flags.append("HAS_NAME_FLAG")
    if p.jumps:
        flags.append("HAS_JUMP_FLAG")
    if p.has_free:
        flags.append("HAS_FREE_FLAG")
    if p.uses_locals:
        flags.append("HAS_LOCAL_FLAG")
    if p.eval_breaker:
        flags.append("HAS_EVAL_BREAK_FLAG")
    if p.deopts:
        flags.append("HAS_DEOPT_FLAG")
    if p.side_exit:
        flags.append("HAS_EXIT_FLAG")
    if not p.infallible:
        flags.append("HAS_ERROR_FLAG")
    if p.error_without_pop:
        flags.append("HAS_ERROR_NO_POP_FLAG")
    if p.escapes:
        flags.append("HAS_ESCAPES_FLAG")
    if p.pure:
        flags.append("HAS_PURE_FLAG")
    if p.oparg_and_1:
        flags.append("HAS_OPARG_AND_1_FLAG")
    if flags:
        return " | ".join(flags)
    else:
        return "0"
