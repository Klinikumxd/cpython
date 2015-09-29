import os
import platform
import random
import re
import sys
import sysconfig
import tempfile
import textwrap
from test.libregrtest.runtest import (
    findtests, runtest,
    STDTESTS, NOTTESTS, PASSED, FAILED, ENV_CHANGED, SKIPPED, RESOURCE_DENIED)
from test.libregrtest.cmdline import _parse_args
from test.libregrtest.setup import setup_python
from test import support
try:
    import gc
except ImportError:
    gc = None


# When tests are run from the Python build directory, it is best practice
# to keep the test files in a subfolder.  This eases the cleanup of leftover
# files using the "make distclean" command.
if sysconfig.is_python_build():
    TEMPDIR = os.path.join(sysconfig.get_config_var('srcdir'), 'build')
else:
    TEMPDIR = tempfile.gettempdir()
TEMPDIR = os.path.abspath(TEMPDIR)


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
    single, randomize, findleaks, use_resources, trace, coverdir,
    print_slow, and random_seed) allow programmers calling main()
    directly to set the values that would normally be set by flags
    on the command line.
    """
    def __init__(self):
        # Namespace of command line options
        self.ns = None

        # tests
        self.tests = []
        self.selected = []

        # test results
        self.good = []
        self.bad = []
        self.skipped = []
        self.resource_denieds = []
        self.environment_changed = []
        self.interrupted = False

        # used by --slow
        self.test_times = []

        # used by --coverage, trace.Trace instance
        self.tracer = None

        # used by --findleaks, store for gc.garbage
        self.found_garbage = []

        # used to display the progress bar "[ 3/100]"
        self.test_count = ''
        self.test_count_width = 1

        # used by --single
        self.next_single_test = None
        self.next_single_filename = None

    def accumulate_result(self, test, result):
        ok, test_time = result
        self.test_times.append((test_time, test))
        if ok == PASSED:
            self.good.append(test)
        elif ok == FAILED:
            self.bad.append(test)
        elif ok == ENV_CHANGED:
            self.environment_changed.append(test)
        elif ok == SKIPPED:
            self.skipped.append(test)
        elif ok == RESOURCE_DENIED:
            self.skipped.append(test)
            self.resource_denieds.append(test)

    def display_progress(self, test_index, test):
        if self.ns.quiet:
            return
        fmt = "[{1:{0}}{2}/{3}] {4}" if self.bad else "[{1:{0}}{2}] {4}"
        print(fmt.format(self.test_count_width, test_index,
                         self.test_count, len(self.bad), test),
              flush=True)

    def parse_args(self, kwargs):
        ns = _parse_args(sys.argv[1:], **kwargs)

        if ns.threshold is not None and gc is None:
            print('No GC available, ignore --threshold.')
            ns.threshold = None

        if ns.findleaks:
            if gc is not None:
                # Uncomment the line below to report garbage that is not
                # freeable by reference counting alone.  By default only
                # garbage that is not collectable by the GC is reported.
                pass
                #gc.set_debug(gc.DEBUG_SAVEALL)
            else:
                print('No GC available, disabling --findleaks')
                ns.findleaks = False

        # Strip .py extensions.
        removepy(ns.args)

        return ns

    def find_tests(self, tests):
        self.tests = tests

        if self.ns.single:
            self.next_single_filename = os.path.join(TEMPDIR, 'pynexttest')
            try:
                with open(self.next_single_filename, 'r') as fp:
                    next_test = fp.read().strip()
                    self.tests = [next_test]
            except OSError:
                pass

        if self.ns.fromfile:
            self.tests = []
            with open(os.path.join(support.SAVEDCWD, self.ns.fromfile)) as fp:
                count_pat = re.compile(r'\[\s*\d+/\s*\d+\]')
                for line in fp:
                    line = count_pat.sub('', line)
                    guts = line.split() # assuming no test has whitespace in its name
                    if guts and not guts[0].startswith('#'):
                        self.tests.extend(guts)

        removepy(self.tests)

        stdtests = STDTESTS[:]
        nottests = NOTTESTS.copy()
        if self.ns.exclude:
            for arg in self.ns.args:
                if arg in stdtests:
                    stdtests.remove(arg)
                nottests.add(arg)
            self.ns.args = []

        # For a partial run, we do not need to clutter the output.
        if (self.ns.verbose
            or self.ns.header
            or not (self.ns.quiet or self.ns.single
                    or self.tests or self.ns.args)):
            # Print basic platform information
            print("==", platform.python_implementation(), *sys.version.split())
            print("==  ", platform.platform(aliased=True),
                          "%s-endian" % sys.byteorder)
            print("==  ", "hash algorithm:", sys.hash_info.algorithm,
                  "64bit" if sys.maxsize > 2**32 else "32bit")
            print("==  ", os.getcwd())
            print("Testing with flags:", sys.flags)

        # if testdir is set, then we are not running the python tests suite, so
        # don't add default tests to be executed or skipped (pass empty values)
        if self.ns.testdir:
            alltests = findtests(self.ns.testdir, list(), set())
        else:
            alltests = findtests(self.ns.testdir, stdtests, nottests)

        self.selected = self.tests or self.ns.args or alltests
        if self.ns.single:
            self.selected = self.selected[:1]
            try:
                pos = alltests.index(self.selected[0])
                self.next_single_test = alltests[pos + 1]
            except IndexError:
                pass

        # Remove all the selected tests that precede start if it's set.
        if self.ns.start:
            try:
                del self.selected[:self.selected.index(self.ns.start)]
            except ValueError:
                print("Couldn't find starting test (%s), using all tests"
                      % self.ns.start)

        if self.ns.randomize:
            if self.ns.random_seed is None:
                self.ns.random_seed = random.randrange(10000000)
            random.seed(self.ns.random_seed)
            print("Using random seed", self.ns.random_seed)
            random.shuffle(self.selected)

    def display_result(self):
        if self.interrupted:
            # print a newline after ^C
            print()
            print("Test suite interrupted by signal SIGINT.")
            executed = set(self.good) | set(self.bad) | set(self.skipped)
            omitted = set(self.selected) - executed
            print(count(len(omitted), "test"), "omitted:")
            printlist(omitted)

        if self.good and not self.ns.quiet:
            if (not self.bad
                and not self.skipped
                and not self.interrupted
                and len(self.good) > 1):
                print("All", end=' ')
            print(count(len(self.good), "test"), "OK.")

        if self.ns.print_slow:
            self.test_times.sort(reverse=True)
            print("10 slowest tests:")
            for time, test in self.test_times[:10]:
                print("%s: %.1fs" % (test, time))

        if self.bad:
            print(count(len(self.bad), "test"), "failed:")
            printlist(self.bad)

        if self.environment_changed:
            print("{} altered the execution environment:".format(
                     count(len(self.environment_changed), "test")))
            printlist(self.environment_changed)

        if self.skipped and not self.ns.quiet:
            print(count(len(self.skipped), "test"), "skipped:")
            printlist(self.skipped)

        if self.ns.verbose2 and self.bad:
            print("Re-running failed tests in verbose mode")
            for test in self.bad[:]:
                print("Re-running test %r in verbose mode" % test, flush=True)
                try:
                    self.ns.verbose = True
                    ok = runtest(test, True, self.ns.quiet, self.ns.huntrleaks,
                                 timeout=self.ns.timeout)
                except KeyboardInterrupt:
                    # print a newline separate from the ^C
                    print()
                    break
                else:
                    if ok[0] in {PASSED, ENV_CHANGED, SKIPPED, RESOURCE_DENIED}:
                        self.bad.remove(test)
            else:
                if self.bad:
                    print(count(len(self.bad), 'test'), "failed again:")
                    printlist(self.bad)

    def run_test(self, test):
        result = runtest(test,
                         self.ns.verbose,
                         self.ns.quiet,
                         self.ns.huntrleaks,
                         output_on_failure=self.ns.verbose3,
                         timeout=self.ns.timeout,
                         failfast=self.ns.failfast,
                         match_tests=self.ns.match_tests)
        self.accumulate_result(test, result)

    def run_tests_sequential(self):
        if self.ns.trace:
            import trace
            self.tracer = trace.Trace(ignoredirs=[sys.base_prefix,
                                                  sys.base_exec_prefix,
                                                  tempfile.gettempdir()],
                                      trace=False, count=True)

        save_modules = sys.modules.keys()

        for test_index, test in enumerate(self.tests, 1):
            self.display_progress(test_index, test)
            if self.tracer:
                # If we're tracing code coverage, then we don't exit with status
                # if on a false return value from main.
                cmd = 'self.run_test(test)'
                self.tracer.runctx(cmd, globals=globals(), locals=vars())
            else:
                try:
                    self.run_test(test)
                except KeyboardInterrupt:
                    self.interrupted = True
                    break

            if self.ns.findleaks:
                gc.collect()
                if gc.garbage:
                    print("Warning: test created", len(gc.garbage), end=' ')
                    print("uncollectable object(s).")
                    # move the uncollectable objects somewhere so we don't see
                    # them again
                    self.found_garbage.extend(gc.garbage)
                    del gc.garbage[:]

            # Unload the newly imported modules (best effort finalization)
            for module in sys.modules.keys():
                if module not in save_modules and module.startswith("test."):
                    support.unload(module)

    def run_tests(self):
        support.verbose = self.ns.verbose   # Tell tests to be moderately quiet
        support.use_resources = self.ns.use_resources

        if self.ns.forever:
            def test_forever(tests):
                while True:
                    for test in tests:
                        yield test
                        if self.bad:
                            return
            self.tests = test_forever(list(self.selected))
            self.test_count = ''
            self.test_count_width = 3
        else:
            self.tests = iter(self.selected)
            self.test_count = '/{}'.format(len(self.selected))
            self.test_count_width = len(self.test_count) - 1

        if self.ns.use_mp:
            from test.libregrtest.runtest_mp import run_tests_multiprocess
            run_tests_multiprocess(self)
        else:
            self.run_tests_sequential()

    def finalize(self):
        if self.next_single_filename:
            if self.next_single_test:
                with open(self.next_single_filename, 'w') as fp:
                    fp.write(self.next_single_test + '\n')
            else:
                os.unlink(self.next_single_filename)

        if self.tracer:
            r = self.tracer.results()
            r.write_results(show_missing=True, summary=True,
                            coverdir=self.ns.coverdir)

        if self.ns.runleaks:
            os.system("leaks %d" % os.getpid())

    def main(self, tests=None, **kwargs):
        self.ns = self.parse_args(kwargs)

        setup_python(self.ns)

        if self.ns.slaveargs is not None:
            from test.libregrtest.runtest_mp import run_tests_slave
            run_tests_slave(self.ns.slaveargs)
        if self.ns.wait:
            input("Press any key to continue...")

        self.find_tests(tests)
        self.run_tests()

        self.display_result()
        self.finalize()
        sys.exit(len(self.bad) > 0 or self.interrupted)


def removepy(names):
    if not names:
        return
    for idx, name in enumerate(names):
        basename, ext = os.path.splitext(name)
        if ext == '.py':
            names[idx] = basename


def count(n, word):
    if n == 1:
        return "%d %s" % (n, word)
    else:
        return "%d %ss" % (n, word)


def printlist(x, width=70, indent=4):
    """Print the elements of iterable x to stdout.

    Optional arg width (default 70) is the maximum line length.
    Optional arg indent (default 4) is the number of blanks with which to
    begin each line.
    """

    blanks = ' ' * indent
    # Print the sorted list: 'x' may be a '--random' list or a set()
    print(textwrap.fill(' '.join(str(elt) for elt in sorted(x)), width,
                        initial_indent=blanks, subsequent_indent=blanks))


def main(tests=None, **kwargs):
    Regrtest().main(tests=tests, **kwargs)


def main_in_temp_cwd():
    """Run main() in a temporary working directory."""
    if sysconfig.is_python_build():
        try:
            os.mkdir(TEMPDIR)
        except FileExistsError:
            pass

    # Define a writable temp dir that will be used as cwd while running
    # the tests. The name of the dir includes the pid to allow parallel
    # testing (see the -j option).
    test_cwd = 'test_python_{}'.format(os.getpid())
    test_cwd = os.path.join(TEMPDIR, test_cwd)

    # Run the tests in a context manager that temporarily changes the CWD to a
    # temporary and writable directory.  If it's not possible to create or
    # change the CWD, the original CWD will be used.  The original CWD is
    # available from support.SAVEDCWD.
    with support.temp_cwd(test_cwd, quiet=True):
        main()
