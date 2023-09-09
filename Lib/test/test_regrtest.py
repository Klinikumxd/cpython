"""
Tests of regrtest.py.

Note: test_regrtest cannot be run twice in parallel.
"""

import contextlib
import dataclasses
import glob
import io
import locale
import os.path
import platform
import re
import subprocess
import sys
import sysconfig
import tempfile
import textwrap
import unittest
from test import libregrtest
from test import support
from test.support import os_helper, TestStats
from test.libregrtest import utils, setup
from test.libregrtest.runtest import normalize_test_name

if not support.has_subprocess_support:
    raise unittest.SkipTest("test module requires subprocess")

ROOT_DIR = os.path.join(os.path.dirname(__file__), '..', '..')
ROOT_DIR = os.path.abspath(os.path.normpath(ROOT_DIR))
LOG_PREFIX = r'[0-9]+:[0-9]+:[0-9]+ (?:load avg: [0-9]+\.[0-9]{2} )?'

EXITCODE_BAD_TEST = 2
EXITCODE_ENV_CHANGED = 3
EXITCODE_NO_TESTS_RAN = 4
EXITCODE_RERUN_FAIL = 5
EXITCODE_INTERRUPTED = 130

TEST_INTERRUPTED = textwrap.dedent("""
    from signal import SIGINT, raise_signal
    try:
        raise_signal(SIGINT)
    except ImportError:
        import os
        os.kill(os.getpid(), SIGINT)
    """)


class ParseArgsTestCase(unittest.TestCase):
    """
    Test regrtest's argument parsing, function _parse_args().
    """

    def checkError(self, args, msg):
        with support.captured_stderr() as err, self.assertRaises(SystemExit):
            libregrtest._parse_args(args)
        self.assertIn(msg, err.getvalue())

    def test_help(self):
        for opt in '-h', '--help':
            with self.subTest(opt=opt):
                with support.captured_stdout() as out, \
                     self.assertRaises(SystemExit):
                    libregrtest._parse_args([opt])
                self.assertIn('Run Python regression tests.', out.getvalue())

    def test_timeout(self):
        ns = libregrtest._parse_args(['--timeout', '4.2'])
        self.assertEqual(ns.timeout, 4.2)
        self.checkError(['--timeout'], 'expected one argument')
        self.checkError(['--timeout', 'foo'], 'invalid float value')

    def test_wait(self):
        ns = libregrtest._parse_args(['--wait'])
        self.assertTrue(ns.wait)

    def test_worker_json(self):
        ns = libregrtest._parse_args(['--worker-json', '[[], {}]'])
        self.assertEqual(ns.worker_json, '[[], {}]')
        self.checkError(['--worker-json'], 'expected one argument')

    def test_start(self):
        for opt in '-S', '--start':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'foo'])
                self.assertEqual(ns.start, 'foo')
                self.checkError([opt], 'expected one argument')

    def test_verbose(self):
        ns = libregrtest._parse_args(['-v'])
        self.assertEqual(ns.verbose, 1)
        ns = libregrtest._parse_args(['-vvv'])
        self.assertEqual(ns.verbose, 3)
        ns = libregrtest._parse_args(['--verbose'])
        self.assertEqual(ns.verbose, 1)
        ns = libregrtest._parse_args(['--verbose'] * 3)
        self.assertEqual(ns.verbose, 3)
        ns = libregrtest._parse_args([])
        self.assertEqual(ns.verbose, 0)

    def test_rerun(self):
        for opt in '-w', '--rerun', '--verbose2':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.rerun)

    def test_verbose3(self):
        for opt in '-W', '--verbose3':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.verbose3)

    def test_quiet(self):
        for opt in '-q', '--quiet':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.quiet)
                self.assertEqual(ns.verbose, 0)

    def test_slowest(self):
        for opt in '-o', '--slowest':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.print_slow)

    def test_header(self):
        ns = libregrtest._parse_args(['--header'])
        self.assertTrue(ns.header)

        ns = libregrtest._parse_args(['--verbose'])
        self.assertTrue(ns.header)

    def test_randomize(self):
        for opt in '-r', '--randomize':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.randomize)

    def test_randseed(self):
        ns = libregrtest._parse_args(['--randseed', '12345'])
        self.assertEqual(ns.random_seed, 12345)
        self.assertTrue(ns.randomize)
        self.checkError(['--randseed'], 'expected one argument')
        self.checkError(['--randseed', 'foo'], 'invalid int value')

    def test_fromfile(self):
        for opt in '-f', '--fromfile':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'foo'])
                self.assertEqual(ns.fromfile, 'foo')
                self.checkError([opt], 'expected one argument')
                self.checkError([opt, 'foo', '-s'], "don't go together")

    def test_exclude(self):
        for opt in '-x', '--exclude':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.exclude)

    def test_single(self):
        for opt in '-s', '--single':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.single)
                self.checkError([opt, '-f', 'foo'], "don't go together")

    def test_ignore(self):
        for opt in '-i', '--ignore':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'pattern'])
                self.assertEqual(ns.ignore_tests, ['pattern'])
                self.checkError([opt], 'expected one argument')

        self.addCleanup(os_helper.unlink, os_helper.TESTFN)
        with open(os_helper.TESTFN, "w") as fp:
            print('matchfile1', file=fp)
            print('matchfile2', file=fp)

        filename = os.path.abspath(os_helper.TESTFN)
        ns = libregrtest._parse_args(['-m', 'match',
                                      '--ignorefile', filename])
        self.assertEqual(ns.ignore_tests,
                         ['matchfile1', 'matchfile2'])

    def test_match(self):
        for opt in '-m', '--match':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'pattern'])
                self.assertEqual(ns.match_tests, ['pattern'])
                self.checkError([opt], 'expected one argument')

        ns = libregrtest._parse_args(['-m', 'pattern1',
                                      '-m', 'pattern2'])
        self.assertEqual(ns.match_tests, ['pattern1', 'pattern2'])

        self.addCleanup(os_helper.unlink, os_helper.TESTFN)
        with open(os_helper.TESTFN, "w") as fp:
            print('matchfile1', file=fp)
            print('matchfile2', file=fp)

        filename = os.path.abspath(os_helper.TESTFN)
        ns = libregrtest._parse_args(['-m', 'match',
                                      '--matchfile', filename])
        self.assertEqual(ns.match_tests,
                         ['match', 'matchfile1', 'matchfile2'])

    def test_failfast(self):
        for opt in '-G', '--failfast':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, '-v'])
                self.assertTrue(ns.failfast)
                ns = libregrtest._parse_args([opt, '-W'])
                self.assertTrue(ns.failfast)
                self.checkError([opt], '-G/--failfast needs either -v or -W')

    def test_use(self):
        for opt in '-u', '--use':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'gui,network'])
                self.assertEqual(ns.use_resources, ['gui', 'network'])

                ns = libregrtest._parse_args([opt, 'gui,none,network'])
                self.assertEqual(ns.use_resources, ['network'])

                expected = list(libregrtest.ALL_RESOURCES)
                expected.remove('gui')
                ns = libregrtest._parse_args([opt, 'all,-gui'])
                self.assertEqual(ns.use_resources, expected)
                self.checkError([opt], 'expected one argument')
                self.checkError([opt, 'foo'], 'invalid resource')

                # all + a resource not part of "all"
                ns = libregrtest._parse_args([opt, 'all,tzdata'])
                self.assertEqual(ns.use_resources,
                                 list(libregrtest.ALL_RESOURCES) + ['tzdata'])

                # test another resource which is not part of "all"
                ns = libregrtest._parse_args([opt, 'extralargefile'])
                self.assertEqual(ns.use_resources, ['extralargefile'])

    def test_memlimit(self):
        for opt in '-M', '--memlimit':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, '4G'])
                self.assertEqual(ns.memlimit, '4G')
                self.checkError([opt], 'expected one argument')

    def test_testdir(self):
        ns = libregrtest._parse_args(['--testdir', 'foo'])
        self.assertEqual(ns.testdir, os.path.join(os_helper.SAVEDCWD, 'foo'))
        self.checkError(['--testdir'], 'expected one argument')

    def test_runleaks(self):
        for opt in '-L', '--runleaks':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.runleaks)

    def test_huntrleaks(self):
        for opt in '-R', '--huntrleaks':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, ':'])
                self.assertEqual(ns.huntrleaks, (5, 4, 'reflog.txt'))
                ns = libregrtest._parse_args([opt, '6:'])
                self.assertEqual(ns.huntrleaks, (6, 4, 'reflog.txt'))
                ns = libregrtest._parse_args([opt, ':3'])
                self.assertEqual(ns.huntrleaks, (5, 3, 'reflog.txt'))
                ns = libregrtest._parse_args([opt, '6:3:leaks.log'])
                self.assertEqual(ns.huntrleaks, (6, 3, 'leaks.log'))
                self.checkError([opt], 'expected one argument')
                self.checkError([opt, '6'],
                                'needs 2 or 3 colon-separated arguments')
                self.checkError([opt, 'foo:'], 'invalid huntrleaks value')
                self.checkError([opt, '6:foo'], 'invalid huntrleaks value')

    def test_multiprocess(self):
        for opt in '-j', '--multiprocess':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, '2'])
                self.assertEqual(ns.use_mp, 2)
                self.checkError([opt], 'expected one argument')
                self.checkError([opt, 'foo'], 'invalid int value')
                self.checkError([opt, '2', '-T'], "don't go together")
                self.checkError([opt, '0', '-T'], "don't go together")

    def test_coverage(self):
        for opt in '-T', '--coverage':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.trace)

    def test_coverdir(self):
        for opt in '-D', '--coverdir':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, 'foo'])
                self.assertEqual(ns.coverdir,
                                 os.path.join(os_helper.SAVEDCWD, 'foo'))
                self.checkError([opt], 'expected one argument')

    def test_nocoverdir(self):
        for opt in '-N', '--nocoverdir':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertIsNone(ns.coverdir)

    def test_threshold(self):
        for opt in '-t', '--threshold':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt, '1000'])
                self.assertEqual(ns.threshold, 1000)
                self.checkError([opt], 'expected one argument')
                self.checkError([opt, 'foo'], 'invalid int value')

    def test_nowindows(self):
        for opt in '-n', '--nowindows':
            with self.subTest(opt=opt):
                with contextlib.redirect_stderr(io.StringIO()) as stderr:
                    ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.nowindows)
                err = stderr.getvalue()
                self.assertIn('the --nowindows (-n) option is deprecated', err)

    def test_forever(self):
        for opt in '-F', '--forever':
            with self.subTest(opt=opt):
                ns = libregrtest._parse_args([opt])
                self.assertTrue(ns.forever)

    def test_unrecognized_argument(self):
        self.checkError(['--xxx'], 'usage:')

    def test_long_option__partial(self):
        ns = libregrtest._parse_args(['--qui'])
        self.assertTrue(ns.quiet)
        self.assertEqual(ns.verbose, 0)

    def test_two_options(self):
        ns = libregrtest._parse_args(['--quiet', '--exclude'])
        self.assertTrue(ns.quiet)
        self.assertEqual(ns.verbose, 0)
        self.assertTrue(ns.exclude)

    def test_option_with_empty_string_value(self):
        ns = libregrtest._parse_args(['--start', ''])
        self.assertEqual(ns.start, '')

    def test_arg(self):
        ns = libregrtest._parse_args(['foo'])
        self.assertEqual(ns.args, ['foo'])

    def test_option_and_arg(self):
        ns = libregrtest._parse_args(['--quiet', 'foo'])
        self.assertTrue(ns.quiet)
        self.assertEqual(ns.verbose, 0)
        self.assertEqual(ns.args, ['foo'])

    def test_arg_option_arg(self):
        ns = libregrtest._parse_args(['test_unaryop', '-v', 'test_binop'])
        self.assertEqual(ns.verbose, 1)
        self.assertEqual(ns.args, ['test_unaryop', 'test_binop'])

    def test_unknown_option(self):
        self.checkError(['--unknown-option'],
                        'unrecognized arguments: --unknown-option')


@dataclasses.dataclass(slots=True)
class Rerun:
    name: str
    match: str | None
    success: bool


class BaseTestCase(unittest.TestCase):
    TEST_UNIQUE_ID = 1
    TESTNAME_PREFIX = 'test_regrtest_'
    TESTNAME_REGEX = r'test_[a-zA-Z0-9_]+'

    def setUp(self):
        self.testdir = os.path.realpath(os.path.dirname(__file__))

        self.tmptestdir = tempfile.mkdtemp()
        self.addCleanup(os_helper.rmtree, self.tmptestdir)

    def create_test(self, name=None, code=None):
        if not name:
            name = 'noop%s' % BaseTestCase.TEST_UNIQUE_ID
            BaseTestCase.TEST_UNIQUE_ID += 1

        if code is None:
            code = textwrap.dedent("""
                    import unittest

                    class Tests(unittest.TestCase):
                        def test_empty_test(self):
                            pass
                """)

        # test_regrtest cannot be run twice in parallel because
        # of setUp() and create_test()
        name = self.TESTNAME_PREFIX + name
        path = os.path.join(self.tmptestdir, name + '.py')

        self.addCleanup(os_helper.unlink, path)
        # Use 'x' mode to ensure that we do not override existing tests
        try:
            with open(path, 'x', encoding='utf-8') as fp:
                fp.write(code)
        except PermissionError as exc:
            if not sysconfig.is_python_build():
                self.skipTest("cannot write %s: %s" % (path, exc))
            raise
        return name

    def regex_search(self, regex, output):
        match = re.search(regex, output, re.MULTILINE)
        if not match:
            self.fail("%r not found in %r" % (regex, output))
        return match

    def check_line(self, output, regex, full=False):
        if full:
            regex += '\n'
        regex = re.compile(r'^' + regex, re.MULTILINE)
        self.assertRegex(output, regex)

    def parse_executed_tests(self, output):
        regex = (r'^%s\[ *[0-9]+(?:/ *[0-9]+)*\] (%s)'
                 % (LOG_PREFIX, self.TESTNAME_REGEX))
        parser = re.finditer(regex, output, re.MULTILINE)
        return list(match.group(1) for match in parser)

    def check_executed_tests(self, output, tests, skipped=(), failed=(),
                             env_changed=(), omitted=(),
                             rerun=None, run_no_tests=(),
                             resource_denied=(),
                             randomize=False, interrupted=False,
                             fail_env_changed=False,
                             *, stats, forever=False, filtered=False):
        if isinstance(tests, str):
            tests = [tests]
        if isinstance(skipped, str):
            skipped = [skipped]
        if isinstance(resource_denied, str):
            resource_denied = [resource_denied]
        if isinstance(failed, str):
            failed = [failed]
        if isinstance(env_changed, str):
            env_changed = [env_changed]
        if isinstance(omitted, str):
            omitted = [omitted]
        if isinstance(run_no_tests, str):
            run_no_tests = [run_no_tests]
        if isinstance(stats, int):
            stats = TestStats(stats)

        rerun_failed = []
        if rerun is not None:
            failed = [rerun.name]
            if not rerun.success:
                rerun_failed.append(rerun.name)

        executed = self.parse_executed_tests(output)
        total_tests = list(tests)
        if rerun is not None:
            total_tests.append(rerun.name)
        if randomize:
            self.assertEqual(set(executed), set(total_tests), output)
        else:
            self.assertEqual(executed, total_tests, output)

        def plural(count):
            return 's' if count != 1 else ''

        def list_regex(line_format, tests):
            count = len(tests)
            names = ' '.join(sorted(tests))
            regex = line_format % (count, plural(count))
            regex = r'%s:\n    %s$' % (regex, names)
            return regex

        if skipped:
            regex = list_regex('%s test%s skipped', skipped)
            self.check_line(output, regex)

        if resource_denied:
            regex = list_regex(r'%s test%s skipped \(resource denied\)', resource_denied)
            self.check_line(output, regex)

        if failed:
            regex = list_regex('%s test%s failed', failed)
            self.check_line(output, regex)

        if env_changed:
            regex = list_regex('%s test%s altered the execution environment',
                               env_changed)
            self.check_line(output, regex)

        if omitted:
            regex = list_regex('%s test%s omitted', omitted)
            self.check_line(output, regex)

        if rerun is not None:
            regex = list_regex('%s re-run test%s', [rerun.name])
            self.check_line(output, regex)
            regex = LOG_PREFIX + fr"Re-running 1 failed tests in verbose mode"
            self.check_line(output, regex)
            regex = fr"Re-running {rerun.name} in verbose mode"
            if rerun.match:
                regex = fr"{regex} \(matching: {rerun.match}\)"
            self.check_line(output, regex)

        if run_no_tests:
            regex = list_regex('%s test%s run no tests', run_no_tests)
            self.check_line(output, regex)

        good = (len(tests) - len(skipped) - len(resource_denied) - len(failed)
                - len(omitted) - len(env_changed) - len(run_no_tests))
        if good:
            regex = r'%s test%s OK\.' % (good, plural(good))
            if not skipped and not failed and (rerun is None or rerun.success) and good > 1:
                regex = 'All %s' % regex
            self.check_line(output, regex, full=True)

        if interrupted:
            self.check_line(output, 'Test suite interrupted by signal SIGINT.')

        # Total tests
        text = f'run={stats.tests_run:,}'
        if filtered:
            text = fr'{text} \(filtered\)'
        parts = [text]
        if stats.failures:
            parts.append(f'failures={stats.failures:,}')
        if stats.skipped:
            parts.append(f'skipped={stats.skipped:,}')
        line = fr'Total tests: {" ".join(parts)}'
        self.check_line(output, line, full=True)

        # Total test files
        run = len(total_tests) - len(resource_denied)
        if rerun is not None:
            total_failed = len(rerun_failed)
            total_rerun = 1
        else:
            total_failed = len(failed)
            total_rerun = 0
        if interrupted:
            run = 0
        text = f'run={run}'
        if not forever:
            text = f'{text}/{len(tests)}'
        if filtered:
            text = fr'{text} \(filtered\)'
        report = [text]
        for name, ntest in (
            ('failed', total_failed),
            ('env_changed', len(env_changed)),
            ('skipped', len(skipped)),
            ('resource_denied', len(resource_denied)),
            ('rerun', total_rerun),
            ('run_no_tests', len(run_no_tests)),
        ):
            if ntest:
                report.append(f'{name}={ntest}')
        line = fr'Total test files: {" ".join(report)}'
        self.check_line(output, line, full=True)

        # Result
        state = []
        if failed:
            state.append('FAILURE')
        elif fail_env_changed and env_changed:
            state.append('ENV CHANGED')
        if interrupted:
            state.append('INTERRUPTED')
        if not any((good, failed, interrupted, skipped,
                    env_changed, fail_env_changed)):
            state.append("NO TESTS RAN")
        elif not state:
            state.append('SUCCESS')
        state = ', '.join(state)
        if rerun is not None:
            new_state = 'SUCCESS' if rerun.success else 'FAILURE'
            state = 'FAILURE then ' + new_state
        self.check_line(output, f'Result: {state}', full=True)

    def parse_random_seed(self, output):
        match = self.regex_search(r'Using random seed ([0-9]+)', output)
        randseed = int(match.group(1))
        self.assertTrue(0 <= randseed <= 100_000_000, randseed)
        return randseed

    def run_command(self, args, input=None, exitcode=0, **kw):
        if not input:
            input = ''
        if 'stderr' not in kw:
            kw['stderr'] = subprocess.STDOUT
        proc = subprocess.run(args,
                              universal_newlines=True,
                              input=input,
                              stdout=subprocess.PIPE,
                              **kw)
        if proc.returncode != exitcode:
            msg = ("Command %s failed with exit code %s, but exit code %s expected!\n"
                   "\n"
                   "stdout:\n"
                   "---\n"
                   "%s\n"
                   "---\n"
                   % (str(args), proc.returncode, exitcode, proc.stdout))
            if proc.stderr:
                msg += ("\n"
                        "stderr:\n"
                        "---\n"
                        "%s"
                        "---\n"
                        % proc.stderr)
            self.fail(msg)
        return proc

    def run_python(self, args, **kw):
        extraargs = []
        if 'uops' in sys._xoptions:
            # Pass -X uops along
            extraargs.extend(['-X', 'uops'])
        args = [sys.executable, *extraargs, '-X', 'faulthandler', '-I', *args]
        proc = self.run_command(args, **kw)
        return proc.stdout


class CheckActualTests(BaseTestCase):
    def test_finds_expected_number_of_tests(self):
        """
        Check that regrtest appears to find the expected set of tests.
        """
        args = ['-Wd', '-E', '-bb', '-m', 'test.regrtest', '--list-tests']
        output = self.run_python(args)
        rough_number_of_tests_found = len(output.splitlines())
        actual_testsuite_glob = os.path.join(glob.escape(os.path.dirname(__file__)),
                                             'test*.py')
        rough_counted_test_py_files = len(glob.glob(actual_testsuite_glob))
        # We're not trying to duplicate test finding logic in here,
        # just give a rough estimate of how many there should be and
        # be near that.  This is a regression test to prevent mishaps
        # such as https://bugs.python.org/issue37667 in the future.
        # If you need to change the values in here during some
        # mythical future test suite reorganization, don't go
        # overboard with logic and keep that goal in mind.
        self.assertGreater(rough_number_of_tests_found,
                           rough_counted_test_py_files*9//10,
                           msg='Unexpectedly low number of tests found in:\n'
                           f'{", ".join(output.splitlines())}')


class ProgramsTestCase(BaseTestCase):
    """
    Test various ways to run the Python test suite. Use options close
    to options used on the buildbot.
    """

    NTEST = 4

    def setUp(self):
        super().setUp()

        # Create NTEST tests doing nothing
        self.tests = [self.create_test() for index in range(self.NTEST)]

        self.python_args = ['-Wd', '-E', '-bb']
        self.regrtest_args = ['-uall', '-rwW',
                              '--testdir=%s' % self.tmptestdir]
        self.regrtest_args.extend(('--timeout', '3600', '-j4'))
        if sys.platform == 'win32':
            self.regrtest_args.append('-n')

    def check_output(self, output):
        self.parse_random_seed(output)
        self.check_executed_tests(output, self.tests,
                                  randomize=True, stats=len(self.tests))

    def run_tests(self, args):
        output = self.run_python(args)
        self.check_output(output)

    def test_script_regrtest(self):
        # Lib/test/regrtest.py
        script = os.path.join(self.testdir, 'regrtest.py')

        args = [*self.python_args, script, *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def test_module_test(self):
        # -m test
        args = [*self.python_args, '-m', 'test',
                *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def test_module_regrtest(self):
        # -m test.regrtest
        args = [*self.python_args, '-m', 'test.regrtest',
                *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def test_module_autotest(self):
        # -m test.autotest
        args = [*self.python_args, '-m', 'test.autotest',
                *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def test_module_from_test_autotest(self):
        # from test import autotest
        code = 'from test import autotest'
        args = [*self.python_args, '-c', code,
                *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def test_script_autotest(self):
        # Lib/test/autotest.py
        script = os.path.join(self.testdir, 'autotest.py')
        args = [*self.python_args, script, *self.regrtest_args, *self.tests]
        self.run_tests(args)

    @unittest.skipUnless(sysconfig.is_python_build(),
                         'run_tests.py script is not installed')
    def test_tools_script_run_tests(self):
        # Tools/scripts/run_tests.py
        script = os.path.join(ROOT_DIR, 'Tools', 'scripts', 'run_tests.py')
        args = [script, *self.regrtest_args, *self.tests]
        self.run_tests(args)

    def run_batch(self, *args):
        proc = self.run_command(args)
        self.check_output(proc.stdout)

    @unittest.skipUnless(sysconfig.is_python_build(),
                         'test.bat script is not installed')
    @unittest.skipUnless(sys.platform == 'win32', 'Windows only')
    def test_tools_buildbot_test(self):
        # Tools\buildbot\test.bat
        script = os.path.join(ROOT_DIR, 'Tools', 'buildbot', 'test.bat')
        test_args = ['--testdir=%s' % self.tmptestdir]
        if platform.machine() == 'ARM64':
            test_args.append('-arm64') # ARM 64-bit build
        elif platform.machine() == 'ARM':
            test_args.append('-arm32')   # 32-bit ARM build
        elif platform.architecture()[0] == '64bit':
            test_args.append('-x64')   # 64-bit build
        if not support.Py_DEBUG:
            test_args.append('+d')     # Release build, use python.exe
        self.run_batch(script, *test_args, *self.tests)

    @unittest.skipUnless(sys.platform == 'win32', 'Windows only')
    def test_pcbuild_rt(self):
        # PCbuild\rt.bat
        script = os.path.join(ROOT_DIR, r'PCbuild\rt.bat')
        if not os.path.isfile(script):
            self.skipTest(f'File "{script}" does not exist')
        rt_args = ["-q"]             # Quick, don't run tests twice
        if platform.machine() == 'ARM64':
            rt_args.append('-arm64') # ARM 64-bit build
        elif platform.machine() == 'ARM':
            rt_args.append('-arm32')   # 32-bit ARM build
        elif platform.architecture()[0] == '64bit':
            rt_args.append('-x64')   # 64-bit build
        if support.Py_DEBUG:
            rt_args.append('-d')     # Debug build, use python_d.exe
        self.run_batch(script, *rt_args, *self.regrtest_args, *self.tests)


class ArgsTestCase(BaseTestCase):
    """
    Test arguments of the Python test suite.
    """

    def run_tests(self, *testargs, **kw):
        cmdargs = ['-m', 'test', '--testdir=%s' % self.tmptestdir, *testargs]
        return self.run_python(cmdargs, **kw)

    def test_success(self):
        code = textwrap.dedent("""
            import unittest

            class PassingTests(unittest.TestCase):
                def test_test1(self):
                    pass

                def test_test2(self):
                    pass

                def test_test3(self):
                    pass
        """)
        tests = [self.create_test(f'ok{i}', code=code) for i in range(1, 6)]

        output = self.run_tests(*tests)
        self.check_executed_tests(output, tests,
                                  stats=3 * len(tests))

    def test_skip(self):
        code = textwrap.dedent("""
            import unittest
            raise unittest.SkipTest("nope")
        """)
        test_ok = self.create_test('ok')
        test_skip = self.create_test('skip', code=code)
        tests = [test_ok, test_skip]

        output = self.run_tests(*tests)
        self.check_executed_tests(output, tests,
                                  skipped=[test_skip],
                                  stats=1)

    def test_failing_test(self):
        # test a failing test
        code = textwrap.dedent("""
            import unittest

            class FailingTest(unittest.TestCase):
                def test_failing(self):
                    self.fail("bug")
        """)
        test_ok = self.create_test('ok')
        test_failing = self.create_test('failing', code=code)
        tests = [test_ok, test_failing]

        output = self.run_tests(*tests, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, tests, failed=test_failing,
                                  stats=TestStats(2, 1))

    def test_resources(self):
        # test -u command line option
        tests = {}
        for resource in ('audio', 'network'):
            code = textwrap.dedent("""
                        from test import support; support.requires(%r)
                        import unittest
                        class PassingTest(unittest.TestCase):
                            def test_pass(self):
                                pass
                    """ % resource)

            tests[resource] = self.create_test(resource, code)
        test_names = sorted(tests.values())

        # -u all: 2 resources enabled
        output = self.run_tests('-u', 'all', *test_names)
        self.check_executed_tests(output, test_names, stats=2)

        # -u audio: 1 resource enabled
        output = self.run_tests('-uaudio', *test_names)
        self.check_executed_tests(output, test_names,
                                  resource_denied=tests['network'],
                                  stats=1)

        # no option: 0 resources enabled
        output = self.run_tests(*test_names, exitcode=EXITCODE_NO_TESTS_RAN)
        self.check_executed_tests(output, test_names,
                                  resource_denied=test_names,
                                  stats=0)

    def test_random(self):
        # test -r and --randseed command line option
        code = textwrap.dedent("""
            import random
            print("TESTRANDOM: %s" % random.randint(1, 1000))
        """)
        test = self.create_test('random', code)

        # first run to get the output with the random seed
        output = self.run_tests('-r', test, exitcode=EXITCODE_NO_TESTS_RAN)
        randseed = self.parse_random_seed(output)
        match = self.regex_search(r'TESTRANDOM: ([0-9]+)', output)
        test_random = int(match.group(1))

        # try to reproduce with the random seed
        output = self.run_tests('-r', '--randseed=%s' % randseed, test,
                                exitcode=EXITCODE_NO_TESTS_RAN)
        randseed2 = self.parse_random_seed(output)
        self.assertEqual(randseed2, randseed)

        match = self.regex_search(r'TESTRANDOM: ([0-9]+)', output)
        test_random2 = int(match.group(1))
        self.assertEqual(test_random2, test_random)

    def test_fromfile(self):
        # test --fromfile
        tests = [self.create_test() for index in range(5)]

        # Write the list of files using a format similar to regrtest output:
        # [1/2] test_1
        # [2/2] test_2
        filename = os_helper.TESTFN
        self.addCleanup(os_helper.unlink, filename)

        # test format '0:00:00 [2/7] test_opcodes -- test_grammar took 0 sec'
        with open(filename, "w") as fp:
            previous = None
            for index, name in enumerate(tests, 1):
                line = ("00:00:%02i [%s/%s] %s"
                        % (index, index, len(tests), name))
                if previous:
                    line += " -- %s took 0 sec" % previous
                print(line, file=fp)
                previous = name

        output = self.run_tests('--fromfile', filename)
        stats = len(tests)
        self.check_executed_tests(output, tests, stats=stats)

        # test format '[2/7] test_opcodes'
        with open(filename, "w") as fp:
            for index, name in enumerate(tests, 1):
                print("[%s/%s] %s" % (index, len(tests), name), file=fp)

        output = self.run_tests('--fromfile', filename)
        self.check_executed_tests(output, tests, stats=stats)

        # test format 'test_opcodes'
        with open(filename, "w") as fp:
            for name in tests:
                print(name, file=fp)

        output = self.run_tests('--fromfile', filename)
        self.check_executed_tests(output, tests, stats=stats)

        # test format 'Lib/test/test_opcodes.py'
        with open(filename, "w") as fp:
            for name in tests:
                print('Lib/test/%s.py' % name, file=fp)

        output = self.run_tests('--fromfile', filename)
        self.check_executed_tests(output, tests, stats=stats)

    def test_interrupted(self):
        code = TEST_INTERRUPTED
        test = self.create_test('sigint', code=code)
        output = self.run_tests(test, exitcode=EXITCODE_INTERRUPTED)
        self.check_executed_tests(output, test, omitted=test,
                                  interrupted=True, stats=0)

    def test_slowest(self):
        # test --slowest
        tests = [self.create_test() for index in range(3)]
        output = self.run_tests("--slowest", *tests)
        self.check_executed_tests(output, tests, stats=len(tests))
        regex = ('10 slowest tests:\n'
                 '(?:- %s: .*\n){%s}'
                 % (self.TESTNAME_REGEX, len(tests)))
        self.check_line(output, regex)

    def test_slowest_interrupted(self):
        # Issue #25373: test --slowest with an interrupted test
        code = TEST_INTERRUPTED
        test = self.create_test("sigint", code=code)

        for multiprocessing in (False, True):
            with self.subTest(multiprocessing=multiprocessing):
                if multiprocessing:
                    args = ("--slowest", "-j2", test)
                else:
                    args = ("--slowest", test)
                output = self.run_tests(*args, exitcode=EXITCODE_INTERRUPTED)
                self.check_executed_tests(output, test,
                                          omitted=test, interrupted=True,
                                          stats=0)

                regex = ('10 slowest tests:\n')
                self.check_line(output, regex)

    def test_coverage(self):
        # test --coverage
        test = self.create_test('coverage')
        output = self.run_tests("--coverage", test)
        self.check_executed_tests(output, [test], stats=1)
        regex = (r'lines +cov% +module +\(path\)\n'
                 r'(?: *[0-9]+ *[0-9]{1,2}% *[^ ]+ +\([^)]+\)+)+')
        self.check_line(output, regex)

    def test_wait(self):
        # test --wait
        test = self.create_test('wait')
        output = self.run_tests("--wait", test, input='key')
        self.check_line(output, 'Press any key to continue')

    def test_forever(self):
        # test --forever
        code = textwrap.dedent("""
            import builtins
            import unittest

            class ForeverTester(unittest.TestCase):
                def test_run(self):
                    # Store the state in the builtins module, because the test
                    # module is reload at each run
                    if 'RUN' in builtins.__dict__:
                        builtins.__dict__['RUN'] += 1
                        if builtins.__dict__['RUN'] >= 3:
                            self.fail("fail at the 3rd runs")
                    else:
                        builtins.__dict__['RUN'] = 1
        """)
        test = self.create_test('forever', code=code)

        # --forever
        output = self.run_tests('--forever', test, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [test]*3, failed=test,
                                  stats=TestStats(3, 1),
                                  forever=True)

        # --forever --rerun
        output = self.run_tests('--forever', '--rerun', test, exitcode=0)
        self.check_executed_tests(output, [test]*3,
                                  rerun=Rerun(test,
                                              match='test_run',
                                              success=True),
                                  stats=TestStats(4, 1),
                                  forever=True)

    def check_leak(self, code, what):
        test = self.create_test('huntrleaks', code=code)

        filename = 'reflog.txt'
        self.addCleanup(os_helper.unlink, filename)
        output = self.run_tests('--huntrleaks', '6:3:', test,
                                exitcode=EXITCODE_BAD_TEST,
                                stderr=subprocess.STDOUT)
        self.check_executed_tests(output, [test], failed=test, stats=1)

        line = 'beginning 9 repetitions\n123456789\n.........\n'
        self.check_line(output, re.escape(line))

        line2 = '%s leaked [1, 1, 1] %s, sum=3\n' % (test, what)
        self.assertIn(line2, output)

        with open(filename) as fp:
            reflog = fp.read()
            self.assertIn(line2, reflog)

    @unittest.skipUnless(support.Py_DEBUG, 'need a debug build')
    def test_huntrleaks(self):
        # test --huntrleaks
        code = textwrap.dedent("""
            import unittest

            GLOBAL_LIST = []

            class RefLeakTest(unittest.TestCase):
                def test_leak(self):
                    GLOBAL_LIST.append(object())
        """)
        self.check_leak(code, 'references')

    @unittest.skipUnless(support.Py_DEBUG, 'need a debug build')
    def test_huntrleaks_fd_leak(self):
        # test --huntrleaks for file descriptor leak
        code = textwrap.dedent("""
            import os
            import unittest

            class FDLeakTest(unittest.TestCase):
                def test_leak(self):
                    fd = os.open(__file__, os.O_RDONLY)
                    # bug: never close the file descriptor
        """)
        self.check_leak(code, 'file descriptors')

    def test_list_tests(self):
        # test --list-tests
        tests = [self.create_test() for i in range(5)]
        output = self.run_tests('--list-tests', *tests)
        self.assertEqual(output.rstrip().splitlines(),
                         tests)

    def test_list_cases(self):
        # test --list-cases
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_method1(self):
                    pass
                def test_method2(self):
                    pass
        """)
        testname = self.create_test(code=code)

        # Test --list-cases
        all_methods = ['%s.Tests.test_method1' % testname,
                       '%s.Tests.test_method2' % testname]
        output = self.run_tests('--list-cases', testname)
        self.assertEqual(output.splitlines(), all_methods)

        # Test --list-cases with --match
        all_methods = ['%s.Tests.test_method1' % testname]
        output = self.run_tests('--list-cases',
                                '-m', 'test_method1',
                                testname)
        self.assertEqual(output.splitlines(), all_methods)

    @support.cpython_only
    def test_crashed(self):
        # Any code which causes a crash
        code = 'import faulthandler; faulthandler._sigsegv()'
        crash_test = self.create_test(name="crash", code=code)

        tests = [crash_test]
        output = self.run_tests("-j2", *tests, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, tests, failed=crash_test,
                                  randomize=True, stats=0)

    def parse_methods(self, output):
        regex = re.compile("^(test[^ ]+).*ok$", flags=re.MULTILINE)
        return [match.group(1) for match in regex.finditer(output)]

    def test_ignorefile(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_method1(self):
                    pass
                def test_method2(self):
                    pass
                def test_method3(self):
                    pass
                def test_method4(self):
                    pass
        """)
        all_methods = ['test_method1', 'test_method2',
                       'test_method3', 'test_method4']
        testname = self.create_test(code=code)

        # only run a subset
        filename = os_helper.TESTFN
        self.addCleanup(os_helper.unlink, filename)

        subset = [
            # only ignore the method name
            'test_method1',
            # ignore the full identifier
            '%s.Tests.test_method3' % testname]
        with open(filename, "w") as fp:
            for name in subset:
                print(name, file=fp)

        output = self.run_tests("-v", "--ignorefile", filename, testname)
        methods = self.parse_methods(output)
        subset = ['test_method2', 'test_method4']
        self.assertEqual(methods, subset)

    def test_matchfile(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_method1(self):
                    pass
                def test_method2(self):
                    pass
                def test_method3(self):
                    pass
                def test_method4(self):
                    pass
        """)
        all_methods = ['test_method1', 'test_method2',
                       'test_method3', 'test_method4']
        testname = self.create_test(code=code)

        # by default, all methods should be run
        output = self.run_tests("-v", testname)
        methods = self.parse_methods(output)
        self.assertEqual(methods, all_methods)

        # only run a subset
        filename = os_helper.TESTFN
        self.addCleanup(os_helper.unlink, filename)

        subset = [
            # only match the method name
            'test_method1',
            # match the full identifier
            '%s.Tests.test_method3' % testname]
        with open(filename, "w") as fp:
            for name in subset:
                print(name, file=fp)

        output = self.run_tests("-v", "--matchfile", filename, testname)
        methods = self.parse_methods(output)
        subset = ['test_method1', 'test_method3']
        self.assertEqual(methods, subset)

    def test_env_changed(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_env_changed(self):
                    open("env_changed", "w").close()
        """)
        testname = self.create_test(code=code)

        # don't fail by default
        output = self.run_tests(testname)
        self.check_executed_tests(output, [testname],
                                  env_changed=testname, stats=1)

        # fail with --fail-env-changed
        output = self.run_tests("--fail-env-changed", testname,
                                exitcode=EXITCODE_ENV_CHANGED)
        self.check_executed_tests(output, [testname], env_changed=testname,
                                  fail_env_changed=True, stats=1)

    def test_rerun_fail(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_succeed(self):
                    return

                def test_fail_always(self):
                    # test that always fails
                    self.fail("bug")
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [testname],
                                  rerun=Rerun(testname,
                                              "test_fail_always",
                                              success=False),
                                  stats=TestStats(3, 2))

    def test_rerun_success(self):
        # FAILURE then SUCCESS
        marker_filename = os.path.abspath("regrtest_marker_filename")
        self.addCleanup(os_helper.unlink, marker_filename)
        self.assertFalse(os.path.exists(marker_filename))

        code = textwrap.dedent(f"""
            import os.path
            import unittest

            marker_filename = {marker_filename!r}

            class Tests(unittest.TestCase):
                def test_succeed(self):
                    return

                def test_fail_once(self):
                    if not os.path.exists(marker_filename):
                        open(marker_filename, "w").close()
                        self.fail("bug")
        """)
        testname = self.create_test(code=code)

        # FAILURE then SUCCESS => exit code 0
        output = self.run_tests("--rerun", testname, exitcode=0)
        self.check_executed_tests(output, [testname],
                                  rerun=Rerun(testname,
                                              match="test_fail_once",
                                              success=True),
                                  stats=TestStats(3, 1))
        os_helper.unlink(marker_filename)

        # with --fail-rerun, exit code EXITCODE_RERUN_FAIL
        # on "FAILURE then SUCCESS" state.
        output = self.run_tests("--rerun", "--fail-rerun", testname,
                                exitcode=EXITCODE_RERUN_FAIL)
        self.check_executed_tests(output, [testname],
                                  rerun=Rerun(testname,
                                              match="test_fail_once",
                                              success=True),
                                  stats=TestStats(3, 1))
        os_helper.unlink(marker_filename)

    def test_rerun_setup_class_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.TestCase):
                @classmethod
                def setUpClass(self):
                    raise RuntimeError('Fail')

                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match="ExampleTests",
                                              success=False),
                                  stats=0)

    def test_rerun_teardown_class_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.TestCase):
                @classmethod
                def tearDownClass(self):
                    raise RuntimeError('Fail')

                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match="ExampleTests",
                                              success=False),
                                  stats=2)

    def test_rerun_setup_module_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            def setUpModule():
                raise RuntimeError('Fail')

            class ExampleTests(unittest.TestCase):
                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match=None,
                                              success=False),
                                  stats=0)

    def test_rerun_teardown_module_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            def tearDownModule():
                raise RuntimeError('Fail')

            class ExampleTests(unittest.TestCase):
                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [testname],
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match=None,
                                              success=False),
                                  stats=2)

    def test_rerun_setup_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.TestCase):
                def setUp(self):
                    raise RuntimeError('Fail')

                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match="test_success",
                                              success=False),
                                  stats=2)

    def test_rerun_teardown_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.TestCase):
                def tearDown(self):
                    raise RuntimeError('Fail')

                def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match="test_success",
                                              success=False),
                                  stats=2)

    def test_rerun_async_setup_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.IsolatedAsyncioTestCase):
                async def asyncSetUp(self):
                    raise RuntimeError('Fail')

                async def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  rerun=Rerun(testname,
                                              match="test_success",
                                              success=False),
                                  stats=2)

    def test_rerun_async_teardown_hook_failure(self):
        # FAILURE then FAILURE
        code = textwrap.dedent("""
            import unittest

            class ExampleTests(unittest.IsolatedAsyncioTestCase):
                async def asyncTearDown(self):
                    raise RuntimeError('Fail')

                async def test_success(self):
                    return
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--rerun", testname, exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, testname,
                                  failed=[testname],
                                  rerun=Rerun(testname,
                                              match="test_success",
                                              success=False),
                                  stats=2)

    def test_no_tests_ran(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_bug(self):
                    pass
        """)
        testname = self.create_test(code=code)

        output = self.run_tests(testname, "-m", "nosuchtest",
                                exitcode=EXITCODE_NO_TESTS_RAN)
        self.check_executed_tests(output, [testname],
                                  run_no_tests=testname,
                                  stats=0, filtered=True)

    def test_no_tests_ran_skip(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_skipped(self):
                    self.skipTest("because")
        """)
        testname = self.create_test(code=code)

        output = self.run_tests(testname)
        self.check_executed_tests(output, [testname],
                                  stats=TestStats(1, skipped=1))

    def test_no_tests_ran_multiple_tests_nonexistent(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_bug(self):
                    pass
        """)
        testname = self.create_test(code=code)
        testname2 = self.create_test(code=code)

        output = self.run_tests(testname, testname2, "-m", "nosuchtest",
                                exitcode=EXITCODE_NO_TESTS_RAN)
        self.check_executed_tests(output, [testname, testname2],
                                  run_no_tests=[testname, testname2],
                                  stats=0, filtered=True)

    def test_no_test_ran_some_test_exist_some_not(self):
        code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_bug(self):
                    pass
        """)
        testname = self.create_test(code=code)
        other_code = textwrap.dedent("""
            import unittest

            class Tests(unittest.TestCase):
                def test_other_bug(self):
                    pass
        """)
        testname2 = self.create_test(code=other_code)

        output = self.run_tests(testname, testname2, "-m", "nosuchtest",
                                "-m", "test_other_bug", exitcode=0)
        self.check_executed_tests(output, [testname, testname2],
                                  run_no_tests=[testname],
                                  stats=1, filtered=True)

    @support.cpython_only
    def test_uncollectable(self):
        code = textwrap.dedent(r"""
            import _testcapi
            import gc
            import unittest

            @_testcapi.with_tp_del
            class Garbage:
                def __tp_del__(self):
                    pass

            class Tests(unittest.TestCase):
                def test_garbage(self):
                    # create an uncollectable object
                    obj = Garbage()
                    obj.ref_cycle = obj
                    obj = None
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--fail-env-changed", testname,
                                exitcode=EXITCODE_ENV_CHANGED)
        self.check_executed_tests(output, [testname],
                                  env_changed=[testname],
                                  fail_env_changed=True,
                                  stats=1)

    def test_multiprocessing_timeout(self):
        code = textwrap.dedent(r"""
            import time
            import unittest
            try:
                import faulthandler
            except ImportError:
                faulthandler = None

            class Tests(unittest.TestCase):
                # test hangs and so should be stopped by the timeout
                def test_sleep(self):
                    # we want to test regrtest multiprocessing timeout,
                    # not faulthandler timeout
                    if faulthandler is not None:
                        faulthandler.cancel_dump_traceback_later()

                    time.sleep(60 * 5)
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("-j2", "--timeout=1.0", testname,
                                exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [testname],
                                  failed=testname, stats=0)
        self.assertRegex(output,
                         re.compile('%s timed out' % testname, re.MULTILINE))

    def test_unraisable_exc(self):
        # --fail-env-changed must catch unraisable exception.
        # The exception must be displayed even if sys.stderr is redirected.
        code = textwrap.dedent(r"""
            import unittest
            import weakref
            from test.support import captured_stderr

            class MyObject:
                pass

            def weakref_callback(obj):
                raise Exception("weakref callback bug")

            class Tests(unittest.TestCase):
                def test_unraisable_exc(self):
                    obj = MyObject()
                    ref = weakref.ref(obj, weakref_callback)
                    with captured_stderr() as stderr:
                        # call weakref_callback() which logs
                        # an unraisable exception
                        obj = None
                    self.assertEqual(stderr.getvalue(), '')
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--fail-env-changed", "-v", testname,
                                exitcode=EXITCODE_ENV_CHANGED)
        self.check_executed_tests(output, [testname],
                                  env_changed=[testname],
                                  fail_env_changed=True,
                                  stats=1)
        self.assertIn("Warning -- Unraisable exception", output)
        self.assertIn("Exception: weakref callback bug", output)

    def test_threading_excepthook(self):
        # --fail-env-changed must catch uncaught thread exception.
        # The exception must be displayed even if sys.stderr is redirected.
        code = textwrap.dedent(r"""
            import threading
            import unittest
            from test.support import captured_stderr

            class MyObject:
                pass

            def func_bug():
                raise Exception("bug in thread")

            class Tests(unittest.TestCase):
                def test_threading_excepthook(self):
                    with captured_stderr() as stderr:
                        thread = threading.Thread(target=func_bug)
                        thread.start()
                        thread.join()
                    self.assertEqual(stderr.getvalue(), '')
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--fail-env-changed", "-v", testname,
                                exitcode=EXITCODE_ENV_CHANGED)
        self.check_executed_tests(output, [testname],
                                  env_changed=[testname],
                                  fail_env_changed=True,
                                  stats=1)
        self.assertIn("Warning -- Uncaught thread exception", output)
        self.assertIn("Exception: bug in thread", output)

    def test_print_warning(self):
        # bpo-45410: The order of messages must be preserved when -W and
        # support.print_warning() are used.
        code = textwrap.dedent(r"""
            import sys
            import unittest
            from test import support

            class MyObject:
                pass

            def func_bug():
                raise Exception("bug in thread")

            class Tests(unittest.TestCase):
                def test_print_warning(self):
                    print("msg1: stdout")
                    support.print_warning("msg2: print_warning")
                    # Fail with ENV CHANGED to see print_warning() log
                    support.environment_altered = True
        """)
        testname = self.create_test(code=code)

        # Expect an output like:
        #
        #   test_threading_excepthook (test.test_x.Tests) ... msg1: stdout
        #   Warning -- msg2: print_warning
        #   ok
        regex = (r"test_print_warning.*msg1: stdout\n"
                 r"Warning -- msg2: print_warning\n"
                 r"ok\n")
        for option in ("-v", "-W"):
            with self.subTest(option=option):
                cmd = ["--fail-env-changed", option, testname]
                output = self.run_tests(*cmd, exitcode=EXITCODE_ENV_CHANGED)
                self.check_executed_tests(output, [testname],
                                          env_changed=[testname],
                                          fail_env_changed=True,
                                          stats=1)
                self.assertRegex(output, regex)

    def test_unicode_guard_env(self):
        guard = os.environ.get(setup.UNICODE_GUARD_ENV)
        self.assertIsNotNone(guard, f"{setup.UNICODE_GUARD_ENV} not set")
        if guard.isascii():
            # Skip to signify that the env var value was changed by the user;
            # possibly to something ASCII to work around Unicode issues.
            self.skipTest("Modified guard")

    def test_cleanup(self):
        dirname = os.path.join(self.tmptestdir, "test_python_123")
        os.mkdir(dirname)
        filename = os.path.join(self.tmptestdir, "test_python_456")
        open(filename, "wb").close()
        names = [dirname, filename]

        cmdargs = ['-m', 'test',
                   '--tempdir=%s' % self.tmptestdir,
                   '--cleanup']
        self.run_python(cmdargs)

        for name in names:
            self.assertFalse(os.path.exists(name), name)

    @unittest.skipIf(support.is_wasi,
                     'checking temp files is not implemented on WASI')
    def test_leak_tmp_file(self):
        code = textwrap.dedent(r"""
            import os.path
            import tempfile
            import unittest

            class FileTests(unittest.TestCase):
                def test_leak_tmp_file(self):
                    filename = os.path.join(tempfile.gettempdir(), 'mytmpfile')
                    with open(filename, "wb") as fp:
                        fp.write(b'content')
        """)
        testnames = [self.create_test(code=code) for _ in range(3)]

        output = self.run_tests("--fail-env-changed", "-v", "-j2", *testnames,
                                exitcode=EXITCODE_ENV_CHANGED)
        self.check_executed_tests(output, testnames,
                                  env_changed=testnames,
                                  fail_env_changed=True,
                                  randomize=True,
                                  stats=len(testnames))
        for testname in testnames:
            self.assertIn(f"Warning -- {testname} leaked temporary "
                          f"files (1): mytmpfile",
                          output)

    def test_mp_decode_error(self):
        # gh-101634: If a worker stdout cannot be decoded, report a failed test
        # and a non-zero exit code.
        if sys.platform == 'win32':
            encoding = locale.getencoding()
        else:
            encoding = sys.stdout.encoding
            if encoding is None:
                encoding = sys.__stdout__.encoding
                if encoding is None:
                    self.skipTest(f"cannot get regrtest worker encoding")

        nonascii = b"byte:\xa0\xa9\xff\n"
        try:
            nonascii.decode(encoding)
        except UnicodeDecodeError:
            pass
        else:
            self.skipTest(f"{encoding} can decode non-ASCII bytes {nonascii!a}")

        code = textwrap.dedent(fr"""
            import sys
            # bytes which cannot be decoded from UTF-8
            nonascii = {nonascii!a}
            sys.stdout.buffer.write(nonascii)
            sys.stdout.buffer.flush()
        """)
        testname = self.create_test(code=code)

        output = self.run_tests("--fail-env-changed", "-v", "-j1", testname,
                                exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [testname],
                                  failed=[testname],
                                  randomize=True,
                                  stats=0)

    def test_doctest(self):
        code = textwrap.dedent(fr'''
            import doctest
            import sys
            from test import support

            def my_function():
                """
                Pass:

                >>> 1 + 1
                2

                Failure:

                >>> 2 + 3
                23
                >>> 1 + 1
                11

                Skipped test (ignored):

                >>> id(1.0)  # doctest: +SKIP
                7948648
                """

            def load_tests(loader, tests, pattern):
                tests.addTest(doctest.DocTestSuite())
                return tests
        ''')
        testname = self.create_test(code=code)

        output = self.run_tests("--fail-env-changed", "-v", "-j1", testname,
                                exitcode=EXITCODE_BAD_TEST)
        self.check_executed_tests(output, [testname],
                                  failed=[testname],
                                  randomize=True,
                                  stats=TestStats(1, 1, 0))


class TestUtils(unittest.TestCase):
    def test_format_duration(self):
        self.assertEqual(utils.format_duration(0),
                         '0 ms')
        self.assertEqual(utils.format_duration(1e-9),
                         '1 ms')
        self.assertEqual(utils.format_duration(10e-3),
                         '10 ms')
        self.assertEqual(utils.format_duration(1.5),
                         '1.5 sec')
        self.assertEqual(utils.format_duration(1),
                         '1.0 sec')
        self.assertEqual(utils.format_duration(2 * 60),
                         '2 min')
        self.assertEqual(utils.format_duration(2 * 60 + 1),
                         '2 min 1 sec')
        self.assertEqual(utils.format_duration(3 * 3600),
                         '3 hour')
        self.assertEqual(utils.format_duration(3 * 3600  + 2 * 60 + 1),
                         '3 hour 2 min')
        self.assertEqual(utils.format_duration(3 * 3600 + 1),
                         '3 hour 1 sec')

    def test_normalize_test_name(self):
        normalize = normalize_test_name
        self.assertEqual(normalize('test_access (test.test_os.FileTests.test_access)'),
                         'test_access')
        self.assertEqual(normalize('setUpClass (test.test_os.ChownFileTests)', is_error=True),
                         'ChownFileTests')
        self.assertEqual(normalize('test_success (test.test_bug.ExampleTests.test_success)', is_error=True),
                         'test_success')
        self.assertIsNone(normalize('setUpModule (test.test_x)', is_error=True))
        self.assertIsNone(normalize('tearDownModule (test.test_module)', is_error=True))


if __name__ == '__main__':
    unittest.main()
