import faulthandler
import locale
import os
import platform
import random
import re
import sys
import sysconfig
import tempfile
import time
import unittest
from test.libregrtest.cmdline import _parse_args, Namespace
from test.libregrtest.runtest import (
    findtests, split_test_packages, run_single_test, abs_module_name,
    PROGRESS_MIN_TIME, State, RunTests, TestResult, HuntRefleak,
    FilterTuple, FilterDict, TestList, StrPath, StrJSON, TestName)
from test.libregrtest.setup import setup_tests, setup_test_dir
from test.libregrtest.pgo import setup_pgo_tests
from test.libregrtest.utils import (strip_py_suffix, count, format_duration,
                                    printlist, get_build_info)
from test import support
from test.support import TestStats
from test.support import os_helper
from test.support import threading_helper


# bpo-38203: Maximum delay in seconds to exit Python (call Py_Finalize()).
# Used to protect against threading._shutdown() hang.
# Must be smaller than buildbot "1200 seconds without output" limit.
EXIT_TIMEOUT = 120.0

EXITCODE_BAD_TEST = 2
EXITCODE_ENV_CHANGED = 3
EXITCODE_NO_TESTS_RAN = 4
EXITCODE_RERUN_FAIL = 5
EXITCODE_INTERRUPTED = 130


class Regrtest:
    """Execute a test suite.

    This also parses command-line options and modifies its behavior
    accordingly.

    tests -- a list of strings containing test names (optional)
    testdir -- the directory in which to look for tests (optional)

    Users other than the Python test suite will certainly want to
    specify testdir; if it's omitted, the directory containing the
    Python test suite is searched for.

    If the tests argument is omitted, the tests listed on the
    command-line will be used.  If that's empty, too, then all *.py
    files beginning with test_ will be used.

    The other default arguments (verbose, quiet, exclude,
    single, randomize, use_resources, trace, coverdir,
    print_slow, and random_seed) allow programmers calling main()
    directly to set the values that would normally be set by flags
    on the command line.
    """
    def __init__(self, ns: Namespace):
        # Actions
        self.want_header: bool = ns.header
        self.want_list_tests: bool = ns.list_tests
        self.want_list_cases: bool = ns.list_cases
        self.want_wait: bool = ns.wait
        self.want_cleanup: bool = ns.cleanup
        self.want_rerun: bool = ns.rerun
        self.want_run_leaks: bool = ns.runleaks

        # Select tests
        if ns.match_tests:
            self.match_tests: FilterTuple = tuple(ns.match_tests)
        else:
            self.match_tests = None
        if ns.ignore_tests:
            self.ignore_tests: FilterTuple = tuple(ns.ignore_tests)
        else:
            self.ignore_tests = None
        self.exclude: bool = ns.exclude
        self.fromfile: StrPath | None = ns.fromfile
        self.starting_test: TestName | None = ns.start
        self.cmdline_args: TestList = ns.args

        # Workers
        if ns.use_mp is None:
            num_workers = 0  # run sequentially
        elif ns.use_mp <= 0:
            num_workers = -1  # use the number of CPUs
        else:
            num_workers = ns.use_mp
        self.num_workers: int = num_workers
        self.worker_json: StrJSON | None = ns.worker_json

        # Options to run tests
        self.fail_fast: bool = ns.failfast
        self.fail_env_changed: bool = ns.fail_env_changed
        self.fail_rerun: bool = ns.fail_rerun
        self.forever: bool = ns.forever
        self.randomize: bool = ns.randomize
        self.random_seed: int | None = ns.random_seed
        self.pgo: bool = ns.pgo
        self.pgo_extended: bool = ns.pgo_extended
        self.output_on_failure: bool = ns.verbose3
        self.timeout: float | None = ns.timeout
        self.verbose: bool = ns.verbose
        self.quiet: bool = ns.quiet
        if ns.huntrleaks:
            self.hunt_refleak: HuntRefleak = HuntRefleak(*ns.huntrleaks)
        else:
            self.hunt_refleak = None
        self.test_dir: StrPath | None = ns.testdir
        self.junit_filename: StrPath | None = ns.xmlpath
        self.memory_limit: str | None = ns.memlimit
        self.gc_threshold: int | None = ns.threshold
        self.use_resources: list[str] = ns.use_resources
        self.python_cmd: list[str] | None = ns.python
        self.coverage: bool = ns.trace
        self.coverage_dir: StrPath | None = ns.coverdir
        self.tmp_dir: StrPath | None = ns.tempdir

        # tests
        self.tests = []
        self.selected = []
        self.first_runtests: RunTests | None = None

        # test results
        self.good: TestList = []
        self.bad: TestList = []
        self.rerun_bad: TestList = []
        self.skipped: TestList = []
        self.resource_denied: TestList = []
        self.environment_changed: TestList = []
        self.run_no_tests: TestList = []
        self.rerun: TestList = []

        self.need_rerun: list[TestResult] = []
        self.first_state: str | None = None
        self.interrupted = False
        self.total_stats = TestStats()

        # used by --slowest
        self.test_times: list[tuple[float, TestName]] = []
        self.print_slowest: bool = ns.print_slow

        # used to display the progress bar "[ 3/100]"
        self.start_time = time.perf_counter()
        self.test_count_text = ''
        self.test_count_width = 1

        # used by --single
        self.single_test_run: bool = ns.single
        self.next_single_test: TestName | None = None
        self.next_single_filename: StrPath | None = None

        # used by --junit-xml
        self.testsuite_xml = None

        # misc
        self.win_load_tracker = None

    def get_executed(self):
        return (set(self.good) | set(self.bad) | set(self.skipped)
                | set(self.resource_denied) | set(self.environment_changed)
                | set(self.run_no_tests))

    def accumulate_result(self, result, rerun=False):
        test_name = result.test_name

        match result.state:
            case State.PASSED:
                self.good.append(test_name)
            case State.ENV_CHANGED:
                self.environment_changed.append(test_name)
            case State.SKIPPED:
                self.skipped.append(test_name)
            case State.RESOURCE_DENIED:
                self.resource_denied.append(test_name)
            case State.INTERRUPTED:
                self.interrupted = True
            case State.DID_NOT_RUN:
                self.run_no_tests.append(test_name)
            case _:
                if result.is_failed(self.fail_env_changed):
                    self.bad.append(test_name)
                    self.need_rerun.append(result)
                else:
                    raise ValueError(f"invalid test state: {result.state!r}")

        if result.has_meaningful_duration() and not rerun:
            self.test_times.append((result.duration, test_name))
        if result.stats is not None:
            self.total_stats.accumulate(result.stats)
        if rerun:
            self.rerun.append(test_name)

        xml_data = result.xml_data
        if xml_data:
            import xml.etree.ElementTree as ET
            for e in xml_data:
                try:
                    self.testsuite_xml.append(ET.fromstring(e))
                except ET.ParseError:
                    print(xml_data, file=sys.__stderr__)
                    raise

    def log(self, line=''):
        empty = not line

        # add the system load prefix: "load avg: 1.80 "
        load_avg = self.getloadavg()
        if load_avg is not None:
            line = f"load avg: {load_avg:.2f} {line}"

        # add the timestamp prefix:  "0:01:05 "
        test_time = time.perf_counter() - self.start_time

        mins, secs = divmod(int(test_time), 60)
        hours, mins = divmod(mins, 60)
        test_time = "%d:%02d:%02d" % (hours, mins, secs)

        line = f"{test_time} {line}"
        if empty:
            line = line[:-1]

        print(line, flush=True)

    def display_progress(self, test_index, text):
        if self.quiet:
            return

        # "[ 51/405/1] test_tcl passed"
        line = f"{test_index:{self.test_count_width}}{self.test_count_text}"
        fails = len(self.bad) + len(self.environment_changed)
        if fails and not self.pgo:
            line = f"{line}/{fails}"
        self.log(f"[{line}] {text}")

    def find_tests(self):
        if self.single_test_run:
            self.next_single_filename = os.path.join(self.tmp_dir, 'pynexttest')
            try:
                with open(self.next_single_filename, 'r') as fp:
                    next_test = fp.read().strip()
                    self.tests = [next_test]
            except OSError:
                pass

        if self.fromfile:
            self.tests = []
            # regex to match 'test_builtin' in line:
            # '0:00:00 [  4/400] test_builtin -- test_dict took 1 sec'
            regex = re.compile(r'\btest_[a-zA-Z0-9_]+\b')
            with open(os.path.join(os_helper.SAVEDCWD, self.fromfile)) as fp:
                for line in fp:
                    line = line.split('#', 1)[0]
                    line = line.strip()
                    match = regex.search(line)
                    if match is not None:
                        self.tests.append(match.group())

        strip_py_suffix(self.tests)

        if self.pgo:
            # add default PGO tests if no tests are specified
            setup_pgo_tests(self.cmdline_args, self.pgo_extended)

        exclude_tests = set()
        if self.exclude:
            for arg in self.cmdline_args:
                exclude_tests.add(arg)
            self.cmdline_args = []

        alltests = findtests(testdir=self.test_dir,
                             exclude=exclude_tests)

        if not self.fromfile:
            self.selected = self.tests or self.cmdline_args
            if self.selected:
                self.selected = split_test_packages(self.selected)
            else:
                self.selected = alltests
        else:
            self.selected = self.tests

        if self.single_test_run:
            self.selected = self.selected[:1]
            try:
                pos = alltests.index(self.selected[0])
                self.next_single_test = alltests[pos + 1]
            except IndexError:
                pass

        # Remove all the selected tests that precede start if it's set.
        if self.starting_test:
            try:
                del self.selected[:self.selected.index(self.starting_test)]
            except ValueError:
                print(f"Cannot find starting test: {self.starting_test}")
                sys.exit(1)

        if self.randomize:
            if self.random_seed is None:
                self.random_seed = random.randrange(100_000_000)
            random.seed(self.random_seed)
            random.shuffle(self.selected)

    @staticmethod
    def list_tests(tests: TestList):
        for name in tests:
            print(name)

    def _list_cases(self, suite):
        for test in suite:
            if isinstance(test, unittest.loader._FailedTest):
                continue
            if isinstance(test, unittest.TestSuite):
                self._list_cases(test)
            elif isinstance(test, unittest.TestCase):
                if support.match_test(test):
                    print(test.id())

    def list_cases(self):
        support.verbose = False
        support.set_match_tests(self.match_tests, self.ignore_tests)

        skipped = []
        for test_name in self.selected:
            module_name = abs_module_name(test_name, self.test_dir)
            try:
                suite = unittest.defaultTestLoader.loadTestsFromName(module_name)
                self._list_cases(suite)
            except unittest.SkipTest:
                skipped.append(test_name)

        if skipped:
            sys.stdout.flush()
            stderr = sys.stderr
            print(file=stderr)
            print(count(len(skipped), "test"), "skipped:", file=stderr)
            printlist(skipped, file=stderr)

    @staticmethod
    def get_rerun_match(rerun_list) -> FilterDict:
        rerun_match_tests = {}
        for result in rerun_list:
            match_tests = result.get_rerun_match_tests()
            # ignore empty match list
            if match_tests:
                rerun_match_tests[result.test_name] = match_tests
        return rerun_match_tests

    def _rerun_failed_tests(self, need_rerun, runtests: RunTests):
        # Configure the runner to re-run tests
        if self.num_workers == 0:
            self.num_workers = 1

        # Get tests to re-run
        tests = [result.test_name for result in need_rerun]
        match_tests_dict = self.get_rerun_match(need_rerun)

        # Clear previously failed tests
        self.rerun_bad.extend(self.bad)
        self.bad.clear()
        self.need_rerun.clear()

        # Re-run failed tests
        self.log(f"Re-running {len(tests)} failed tests in verbose mode in subprocesses")
        runtests = runtests.copy(
            tests=tuple(tests),
            rerun=True,
            verbose=True,
            forever=False,
            fail_fast=False,
            match_tests_dict=match_tests_dict,
            output_on_failure=False)
        self.set_tests(runtests)
        self._run_tests_mp(runtests, self.num_workers)
        return runtests

    def rerun_failed_tests(self, need_rerun, runtests: RunTests):
        if self.python_cmd:
            # Temp patch for https://github.com/python/cpython/issues/94052
            self.log(
                "Re-running failed tests is not supported with --python "
                "host runner option."
            )
            return

        self.first_state = self.get_tests_state()

        print()
        rerun_runtests = self._rerun_failed_tests(need_rerun, runtests)

        if self.bad:
            print(count(len(self.bad), 'test'), "failed again:")
            printlist(self.bad)

        self.display_result(rerun_runtests)

    def display_result(self, runtests):
        pgo = runtests.pgo

        # If running the test suite for PGO then no one cares about results.
        if pgo:
            return

        print()
        print("== Tests result: %s ==" % self.get_tests_state())

        if self.interrupted:
            print("Test suite interrupted by signal SIGINT.")

        omitted = set(self.selected) - self.get_executed()
        if omitted:
            print()
            print(count(len(omitted), "test"), "omitted:")
            printlist(omitted)

        if self.good and not self.quiet:
            print()
            if (not self.bad
                and not self.skipped
                and not self.interrupted
                and len(self.good) > 1):
                print("All", end=' ')
            print(count(len(self.good), "test"), "OK.")

        if self.print_slowest:
            self.test_times.sort(reverse=True)
            print()
            print("10 slowest tests:")
            for test_time, test in self.test_times[:10]:
                print("- %s: %s" % (test, format_duration(test_time)))

        if self.bad:
            print()
            print(count(len(self.bad), "test"), "failed:")
            printlist(self.bad)

        if self.environment_changed:
            print()
            print("{} altered the execution environment:".format(
                     count(len(self.environment_changed), "test")))
            printlist(self.environment_changed)

        if self.skipped and not self.quiet:
            print()
            print(count(len(self.skipped), "test"), "skipped:")
            printlist(self.skipped)

        if self.resource_denied and not self.quiet:
            print()
            print(count(len(self.resource_denied), "test"), "skipped (resource denied):")
            printlist(self.resource_denied)

        if self.rerun:
            print()
            print("%s:" % count(len(self.rerun), "re-run test"))
            printlist(self.rerun)

        if self.run_no_tests:
            print()
            print(count(len(self.run_no_tests), "test"), "run no tests:")
            printlist(self.run_no_tests)

    def run_test(self, test_name: TestName, runtests: RunTests, tracer):
        if tracer is not None:
            # If we're tracing code coverage, then we don't exit with status
            # if on a false return value from main.
            cmd = ('result = run_single_test(test_name, runtests)')
            namespace = dict(locals())
            tracer.runctx(cmd, globals=globals(), locals=namespace)
            result = namespace['result']
        else:
            result = run_single_test(test_name, runtests)

        self.accumulate_result(result)

        return result

    def run_tests_sequentially(self, runtests):
        if self.coverage:
            import trace
            tracer = trace.Trace(trace=False, count=True)
        else:
            tracer = None

        save_modules = sys.modules.keys()

        msg = "Run tests sequentially"
        if runtests.timeout:
            msg += " (timeout: %s)" % format_duration(runtests.timeout)
        self.log(msg)

        previous_test = None
        tests_iter = runtests.iter_tests()
        for test_index, test_name in enumerate(tests_iter, 1):
            start_time = time.perf_counter()

            text = test_name
            if previous_test:
                text = '%s -- %s' % (text, previous_test)
            self.display_progress(test_index, text)

            result = self.run_test(test_name, runtests, tracer)

            # Unload the newly imported modules (best effort finalization)
            for module in sys.modules.keys():
                if module not in save_modules and module.startswith("test."):
                    support.unload(module)

            if result.must_stop(self.fail_fast, self.fail_env_changed):
                break

            previous_test = str(result)
            test_time = time.perf_counter() - start_time
            if test_time >= PROGRESS_MIN_TIME:
                previous_test = "%s in %s" % (previous_test, format_duration(test_time))
            elif result.state == State.PASSED:
                # be quiet: say nothing if the test passed shortly
                previous_test = None

        if previous_test:
            print(previous_test)

        return tracer

    @staticmethod
    def display_header():
        # Print basic platform information
        print("==", platform.python_implementation(), *sys.version.split())
        print("==", platform.platform(aliased=True),
                      "%s-endian" % sys.byteorder)
        print("== Python build:", ' '.join(get_build_info()))
        print("== cwd:", os.getcwd())
        cpu_count = os.cpu_count()
        if cpu_count:
            print("== CPU count:", cpu_count)
        print("== encodings: locale=%s, FS=%s"
              % (locale.getencoding(), sys.getfilesystemencoding()))

        # This makes it easier to remember what to set in your local
        # environment when trying to reproduce a sanitizer failure.
        asan = support.check_sanitizer(address=True)
        msan = support.check_sanitizer(memory=True)
        ubsan = support.check_sanitizer(ub=True)
        sanitizers = []
        if asan:
            sanitizers.append("address")
        if msan:
            sanitizers.append("memory")
        if ubsan:
            sanitizers.append("undefined behavior")
        if not sanitizers:
            return

        print(f"== sanitizers: {', '.join(sanitizers)}")
        for sanitizer, env_var in (
            (asan, "ASAN_OPTIONS"),
            (msan, "MSAN_OPTIONS"),
            (ubsan, "UBSAN_OPTIONS"),
        ):
            options= os.environ.get(env_var)
            if sanitizer and options is not None:
                print(f"== {env_var}={options!r}")

    def no_tests_run(self):
        return not any((self.good, self.bad, self.skipped, self.interrupted,
                        self.environment_changed))

    def get_tests_state(self):
        result = []
        if self.bad:
            result.append("FAILURE")
        elif self.fail_env_changed and self.environment_changed:
            result.append("ENV CHANGED")
        elif self.no_tests_run():
            result.append("NO TESTS RAN")

        if self.interrupted:
            result.append("INTERRUPTED")

        if not result:
            result.append("SUCCESS")

        result = ', '.join(result)
        if self.first_state:
            result = '%s then %s' % (self.first_state, result)
        return result

    def _run_tests_mp(self, runtests: RunTests, num_workers: int) -> None:
        from test.libregrtest.runtest_mp import RunWorkers

        # If we're on windows and this is the parent runner (not a worker),
        # track the load average.
        if sys.platform == 'win32':
            from test.libregrtest.win_utils import WindowsLoadTracker

            try:
                self.win_load_tracker = WindowsLoadTracker()
            except PermissionError as error:
                # Standard accounts may not have access to the performance
                # counters.
                print(f'Failed to create WindowsLoadTracker: {error}')

        try:
            RunWorkers(self, runtests, num_workers).run()
        finally:
            if self.win_load_tracker is not None:
                self.win_load_tracker.close()
                self.win_load_tracker = None

    def set_tests(self, runtests: RunTests):
        self.tests = runtests.tests
        if runtests.forever:
            self.test_count_text = ''
            self.test_count_width = 3
        else:
            self.test_count_text = '/{}'.format(len(self.tests))
            self.test_count_width = len(self.test_count_text) - 1

    def run_tests(self, runtests: RunTests):
        self.first_runtests = runtests
        self.set_tests(runtests)
        if self.num_workers:
            self._run_tests_mp(runtests, self.num_workers)
            tracer = None
        else:
            tracer = self.run_tests_sequentially(runtests)
        return tracer

    def finalize_tests(self, tracer):
        if self.next_single_filename:
            if self.next_single_test:
                with open(self.next_single_filename, 'w') as fp:
                    fp.write(self.next_single_test + '\n')
            else:
                os.unlink(self.next_single_filename)

        if tracer is not None:
            results = tracer.results()
            results.write_results(show_missing=True, summary=True,
                                  coverdir=self.coverage_dir)

        if self.want_run_leaks:
            os.system("leaks %d" % os.getpid())

        self.save_xml_result()

    def display_summary(self):
        duration = time.perf_counter() - self.start_time
        filtered = bool(self.match_tests) or bool(self.ignore_tests)

        # Total duration
        print()
        print("Total duration: %s" % format_duration(duration))

        # Total tests
        total = self.total_stats
        text = f'run={total.tests_run:,}'
        if filtered:
            text = f"{text} (filtered)"
        stats = [text]
        if total.failures:
            stats.append(f'failures={total.failures:,}')
        if total.skipped:
            stats.append(f'skipped={total.skipped:,}')
        print(f"Total tests: {' '.join(stats)}")

        # Total test files
        all_tests = [self.good, self.bad, self.rerun,
                     self.skipped,
                     self.environment_changed, self.run_no_tests]
        run = sum(map(len, all_tests))
        text = f'run={run}'
        if not self.first_runtests.forever:
            ntest = len(self.first_runtests.tests)
            text = f"{text}/{ntest}"
        if filtered:
            text = f"{text} (filtered)"
        report = [text]
        for name, tests in (
            ('failed', self.bad),
            ('env_changed', self.environment_changed),
            ('skipped', self.skipped),
            ('resource_denied', self.resource_denied),
            ('rerun', self.rerun),
            ('run_no_tests', self.run_no_tests),
        ):
            if tests:
                report.append(f'{name}={len(tests)}')
        print(f"Total test files: {' '.join(report)}")

        # Result
        result = self.get_tests_state()
        print(f"Result: {result}")

    def save_xml_result(self):
        if not self.junit_filename and not self.testsuite_xml:
            return

        import xml.etree.ElementTree as ET
        root = ET.Element("testsuites")

        # Manually count the totals for the overall summary
        totals = {'tests': 0, 'errors': 0, 'failures': 0}
        for suite in self.testsuite_xml:
            root.append(suite)
            for k in totals:
                try:
                    totals[k] += int(suite.get(k, 0))
                except ValueError:
                    pass

        for k, v in totals.items():
            root.set(k, str(v))

        xmlpath = os.path.join(os_helper.SAVEDCWD, self.junit_filename)
        with open(xmlpath, 'wb') as f:
            for s in ET.tostringlist(root):
                f.write(s)

    @staticmethod
    def fix_umask():
        if support.is_emscripten:
            # Emscripten has default umask 0o777, which breaks some tests.
            # see https://github.com/emscripten-core/emscripten/issues/17269
            old_mask = os.umask(0)
            if old_mask == 0o777:
                os.umask(0o027)
            else:
                os.umask(old_mask)

    @staticmethod
    def select_temp_dir(tmp_dir):
        if tmp_dir:
            tmp_dir = os.path.expanduser(tmp_dir)
        else:
            # When tests are run from the Python build directory, it is best practice
            # to keep the test files in a subfolder.  This eases the cleanup of leftover
            # files using the "make distclean" command.
            if sysconfig.is_python_build():
                tmp_dir = sysconfig.get_config_var('abs_builddir')
                if tmp_dir is None:
                    # bpo-30284: On Windows, only srcdir is available. Using
                    # abs_builddir mostly matters on UNIX when building Python
                    # out of the source tree, especially when the source tree
                    # is read only.
                    tmp_dir = sysconfig.get_config_var('srcdir')
                tmp_dir = os.path.join(tmp_dir, 'build')
            else:
                tmp_dir = tempfile.gettempdir()

        return os.path.abspath(tmp_dir)

    def is_worker(self):
        return (self.worker_json is not None)

    @staticmethod
    def make_temp_dir(tmp_dir: StrPath, is_worker: bool):
        os.makedirs(tmp_dir, exist_ok=True)

        # Define a writable temp dir that will be used as cwd while running
        # the tests. The name of the dir includes the pid to allow parallel
        # testing (see the -j option).
        # Emscripten and WASI have stubbed getpid(), Emscripten has only
        # milisecond clock resolution. Use randint() instead.
        if sys.platform in {"emscripten", "wasi"}:
            nounce = random.randint(0, 1_000_000)
        else:
            nounce = os.getpid()

        if is_worker:
            work_dir = 'test_python_worker_{}'.format(nounce)
        else:
            work_dir = 'test_python_{}'.format(nounce)
        work_dir += os_helper.FS_NONASCII
        work_dir = os.path.join(tmp_dir, work_dir)
        return work_dir

    @staticmethod
    def cleanup_temp_dir(tmp_dir: StrPath):
        import glob

        path = os.path.join(glob.escape(tmp_dir), 'test_python_*')
        print("Cleanup %s directory" % tmp_dir)
        for name in glob.glob(path):
            if os.path.isdir(name):
                print("Remove directory: %s" % name)
                os_helper.rmtree(name)
            else:
                print("Remove file: %s" % name)
                os_helper.unlink(name)

    def main(self, tests: TestList | None = None):
        self.tests = tests

        if self.junit_filename:
            support.junit_xml_list = self.testsuite_xml = []

        strip_py_suffix(self.cmdline_args)

        self.tmp_dir = self.select_temp_dir(self.tmp_dir)

        self.fix_umask()

        if self.want_cleanup:
            self.cleanup_temp_dir(self.tmp_dir)
            sys.exit(0)

        work_dir = self.make_temp_dir(self.tmp_dir, self.is_worker())

        try:
            # Run the tests in a context manager that temporarily changes the
            # CWD to a temporary and writable directory. If it's not possible
            # to create or change the CWD, the original CWD will be used.
            # The original CWD is available from os_helper.SAVEDCWD.
            with os_helper.temp_cwd(work_dir, quiet=True):
                # When using multiprocessing, worker processes will use
                # work_dir as their parent temporary directory. So when the
                # main process exit, it removes also subdirectories of worker
                # processes.

                self._main()
        except SystemExit as exc:
            # bpo-38203: Python can hang at exit in Py_Finalize(), especially
            # on threading._shutdown() call: put a timeout
            if threading_helper.can_start_thread:
                faulthandler.dump_traceback_later(EXIT_TIMEOUT, exit=True)

            sys.exit(exc.code)

    def getloadavg(self):
        if self.win_load_tracker is not None:
            return self.win_load_tracker.getloadavg()

        if hasattr(os, 'getloadavg'):
            return os.getloadavg()[0]

        return None

    def get_exitcode(self):
        exitcode = 0
        if self.bad:
            exitcode = EXITCODE_BAD_TEST
        elif self.interrupted:
            exitcode = EXITCODE_INTERRUPTED
        elif self.fail_env_changed and self.environment_changed:
            exitcode = EXITCODE_ENV_CHANGED
        elif self.no_tests_run():
            exitcode = EXITCODE_NO_TESTS_RAN
        elif self.rerun and self.fail_rerun:
            exitcode = EXITCODE_RERUN_FAIL
        return exitcode

    def action_run_tests(self):
        if self.hunt_refleak and self.hunt_refleak.warmups < 3:
            msg = ("WARNING: Running tests with --huntrleaks/-R and "
                   "less than 3 warmup repetitions can give false positives!")
            print(msg, file=sys.stdout, flush=True)

        # For a partial run, we do not need to clutter the output.
        if (self.want_header
            or not(self.pgo or self.quiet or self.single_test_run
                   or self.tests or self.cmdline_args)):
            self.display_header()

        if self.randomize:
            print("Using random seed", self.random_seed)

        if self.num_workers < 0:
            # Use all cores + extras for tests that like to sleep
            self.num_workers = 2 + (os.cpu_count() or 1)

        runtests = RunTests(
            tuple(self.selected),
            fail_fast=self.fail_fast,
            match_tests=self.match_tests,
            ignore_tests=self.ignore_tests,
            forever=self.forever,
            pgo=self.pgo,
            pgo_extended=self.pgo_extended,
            output_on_failure=self.output_on_failure,
            timeout=self.timeout,
            verbose=self.verbose,
            quiet=self.quiet,
            hunt_refleak=self.hunt_refleak,
            test_dir=self.test_dir,
            junit_filename=self.junit_filename,
            memory_limit=self.memory_limit,
            gc_threshold=self.gc_threshold,
            use_resources=self.use_resources,
            python_cmd=self.python_cmd,
        )

        setup_tests(runtests)

        tracer = self.run_tests(runtests)
        self.display_result(runtests)

        need_rerun = self.need_rerun
        if self.want_rerun and need_rerun:
            self.rerun_failed_tests(need_rerun, runtests)

        self.display_summary()
        self.finalize_tests(tracer)

    def _main(self):
        if self.is_worker():
            from test.libregrtest.runtest_mp import worker_process
            worker_process(self.worker_json)
            return

        if self.want_wait:
            input("Press any key to continue...")

        setup_test_dir(self.test_dir)
        self.find_tests()

        exitcode = 0
        if self.want_list_tests:
            self.list_tests(self.selected)
        elif self.want_list_cases:
            self.list_cases()
        else:
            self.action_run_tests()
            exitcode = self.get_exitcode()

        sys.exit(exitcode)


def main(tests=None, **kwargs):
    """Run the Python suite."""
    ns = _parse_args(sys.argv[1:], **kwargs)
    Regrtest(ns).main(tests=tests)
