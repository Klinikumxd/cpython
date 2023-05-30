import re
import sys
import unittest

from test import support
from test.support import import_helper
from test.support.script_helper import assert_python_failure
from test.support.testcase import ExceptionIsLikeMixin

from .test_misc import decode_stderr

# Skip this test if the _testcapi module isn't available.
_testcapi = import_helper.import_module('_testcapi')

class Test_Exceptions(unittest.TestCase):

    def test_exception(self):
        raised_exception = ValueError("5")
        new_exc = TypeError("TEST")
        try:
            raise raised_exception
        except ValueError as e:
            orig_sys_exception = sys.exception()
            orig_exception = _testcapi.set_exception(new_exc)
            new_sys_exception = sys.exception()
            new_exception = _testcapi.set_exception(orig_exception)
            reset_sys_exception = sys.exception()

            self.assertEqual(orig_exception, e)

            self.assertEqual(orig_exception, raised_exception)
            self.assertEqual(orig_sys_exception, orig_exception)
            self.assertEqual(reset_sys_exception, orig_exception)
            self.assertEqual(new_exception, new_exc)
            self.assertEqual(new_sys_exception, new_exception)
        else:
            self.fail("Exception not raised")

    def test_exc_info(self):
        raised_exception = ValueError("5")
        new_exc = TypeError("TEST")
        try:
            raise raised_exception
        except ValueError as e:
            tb = e.__traceback__
            orig_sys_exc_info = sys.exc_info()
            orig_exc_info = _testcapi.set_exc_info(new_exc.__class__, new_exc, None)
            new_sys_exc_info = sys.exc_info()
            new_exc_info = _testcapi.set_exc_info(*orig_exc_info)
            reset_sys_exc_info = sys.exc_info()

            self.assertEqual(orig_exc_info[1], e)

            self.assertSequenceEqual(orig_exc_info, (raised_exception.__class__, raised_exception, tb))
            self.assertSequenceEqual(orig_sys_exc_info, orig_exc_info)
            self.assertSequenceEqual(reset_sys_exc_info, orig_exc_info)
            self.assertSequenceEqual(new_exc_info, (new_exc.__class__, new_exc, None))
            self.assertSequenceEqual(new_sys_exc_info, new_exc_info)
        else:
            self.assertTrue(False)


class Test_FatalError(unittest.TestCase):

    def check_fatal_error(self, code, expected, not_expected=()):
        with support.SuppressCrashReport():
            rc, out, err = assert_python_failure('-sSI', '-c', code)

        err = decode_stderr(err)
        self.assertIn('Fatal Python error: _testcapi_fatal_error_impl: MESSAGE\n',
                      err)

        match = re.search(r'^Extension modules:(.*) \(total: ([0-9]+)\)$',
                          err, re.MULTILINE)
        if not match:
            self.fail(f"Cannot find 'Extension modules:' in {err!r}")
        modules = set(match.group(1).strip().split(', '))
        total = int(match.group(2))

        for name in expected:
            self.assertIn(name, modules)
        for name in not_expected:
            self.assertNotIn(name, modules)
        self.assertEqual(len(modules), total)

    @support.requires_subprocess()
    def test_fatal_error(self):
        # By default, stdlib extension modules are ignored,
        # but not test modules.
        expected = ('_testcapi',)
        not_expected = ('sys',)
        code = 'import _testcapi, sys; _testcapi.fatal_error(b"MESSAGE")'
        self.check_fatal_error(code, expected, not_expected)

        # Mark _testcapi as stdlib module, but not sys
        expected = ('sys',)
        not_expected = ('_testcapi',)
        code = """if True:
            import _testcapi, sys
            sys.stdlib_module_names = frozenset({"_testcapi"})
            _testcapi.fatal_error(b"MESSAGE")
        """
        self.check_fatal_error(code, expected)


class Test_ErrSetAndRestore(unittest.TestCase):

    def test_err_set_raised(self):
        with self.assertRaises(ValueError):
            _testcapi.err_set_raised(ValueError())
        v = ValueError()
        try:
            _testcapi.err_set_raised(v)
        except ValueError as ex:
            self.assertIs(v, ex)

    def test_err_restore(self):
        with self.assertRaises(ValueError):
            _testcapi.err_restore(ValueError)
        with self.assertRaises(ValueError):
            _testcapi.err_restore(ValueError, 1)
        with self.assertRaises(ValueError):
            _testcapi.err_restore(ValueError, 1, None)
        with self.assertRaises(ValueError):
            _testcapi.err_restore(ValueError, ValueError())
        try:
            _testcapi.err_restore(KeyError, "hi")
        except KeyError as k:
            self.assertEqual("hi", k.args[0])
        try:
            1/0
        except Exception as e:
            tb = e.__traceback__
        with self.assertRaises(ValueError):
            _testcapi.err_restore(ValueError, 1, tb)
        with self.assertRaises(TypeError):
            _testcapi.err_restore(ValueError, 1, 0)
        try:
            _testcapi.err_restore(ValueError, 1, tb)
        except ValueError as v:
            self.assertEqual(1, v.args[0])
            self.assertIs(tb, v.__traceback__.tb_next)

    def test_set_object(self):

        # new exception as obj is not an exception
        with self.assertRaises(ValueError) as e:
            _testcapi.exc_set_object(ValueError, 42)
        self.assertEqual(e.exception.args, (42,))

        # wraps the exception because unrelated types
        with self.assertRaises(ValueError) as e:
            _testcapi.exc_set_object(ValueError, TypeError(1,2,3))
        wrapped = e.exception.args[0]
        self.assertIsInstance(wrapped, TypeError)
        self.assertEqual(wrapped.args, (1, 2, 3))

        # is superclass, so does not wrap
        with self.assertRaises(PermissionError) as e:
            _testcapi.exc_set_object(OSError, PermissionError(24))
        self.assertEqual(e.exception.args, (24,))

        class Meta(type):
            def __subclasscheck__(cls, sub):
                1/0

        class Broken(Exception, metaclass=Meta):
            pass

        with self.assertRaises(ZeroDivisionError) as e:
            _testcapi.exc_set_object(Broken, Broken())

    def test_set_object_and_fetch(self):
        class Broken(Exception):
            def __init__(self, *arg):
                raise ValueError("Broken __init__")

        exc = _testcapi.exc_set_object_fetch(Broken, 'abcd')
        self.assertIsInstance(exc, ValueError)
        self.assertEqual(exc.__notes__[0],
                         "Normalization failed: type=Broken args='abcd'")

        class BadArg:
            def __repr__(self):
                raise TypeError('Broken arg type')

        exc = _testcapi.exc_set_object_fetch(Broken, BadArg())
        self.assertIsInstance(exc, ValueError)
        self.assertEqual(exc.__notes__[0],
                         'Normalization failed: type=Broken args=<unknown>')


class Test_PyUnstable_Exc_PrepReraiseStar(ExceptionIsLikeMixin, unittest.TestCase):

    def setUp(self):
        super().setUp()
        try:
            raise ExceptionGroup("eg", [TypeError('bad type'), ValueError(42)])
        except ExceptionGroup as e:
            self.orig = e

    def test_invalid_args(self):
        with self.assertRaisesRegex(TypeError, "orig must be an exception"):
            _testcapi.unstable_exc_prep_reraise_star(42, [None])

        with self.assertRaisesRegex(TypeError, "excs must be a list"):
            _testcapi.unstable_exc_prep_reraise_star(self.orig, 42)

        with self.assertRaisesRegex(TypeError, "not an exception"):
            _testcapi.unstable_exc_prep_reraise_star(self.orig, [TypeError(42), 42])

        with self.assertRaisesRegex(ValueError, "orig must be a raised exception"):
            _testcapi.unstable_exc_prep_reraise_star(ValueError(42), [TypeError(42)])

        with self.assertRaisesRegex(ValueError, "orig must be a raised exception"):
            _testcapi.unstable_exc_prep_reraise_star(ExceptionGroup("eg", [ValueError(42)]),
                                                     [TypeError(42)])


    def test_nothing_to_reraise(self):
        self.assertEqual(
            _testcapi.unstable_exc_prep_reraise_star(self.orig, [None]), None)

        try:
            raise ValueError(42)
        except ValueError as e:
            orig = e
        self.assertEqual(
            _testcapi.unstable_exc_prep_reraise_star(orig, [None]), None)

    def test_reraise_orig(self):
        orig = self.orig
        res = _testcapi.unstable_exc_prep_reraise_star(orig, [orig])
        self.assertExceptionIsLike(res, orig)

    def test_raise_orig_parts(self):
        orig = self.orig
        match, rest = orig.split(TypeError)

        test_cases = [
            ([match, rest], orig),
            ([rest, match], orig),
            ([match], match),
            ([rest], rest),
            ([], None),
        ]

        for input, expected in test_cases:
            with self.subTest(input=input):
                res = _testcapi.unstable_exc_prep_reraise_star(orig, input)
                self.assertExceptionIsLike(res, expected)


    def test_raise_with_new_exceptions(self):
        orig = self.orig

        match, rest = orig.split(TypeError)
        new1 = OSError('bad file')
        new2 = RuntimeError('bad runtime')

        test_cases = [
            ([new1, match, rest], ExceptionGroup("", [new1, orig])),
            ([match, new1, rest], ExceptionGroup("", [new1, orig])),
            ([match, rest, new1], ExceptionGroup("", [new1, orig])),

            ([new1, new2, match, rest], ExceptionGroup("", [new1, new2, orig])),
            ([new1, match, new2, rest], ExceptionGroup("", [new1, new2, orig])),
            ([new2, rest, match, new1], ExceptionGroup("", [new2, new1, orig])),
            ([rest, new2, match, new1], ExceptionGroup("", [new2, new1, orig])),


            ([new1, new2, rest], ExceptionGroup("", [new1, new2, rest])),
            ([new1, match, new2], ExceptionGroup("", [new1, new2, match])),
            ([rest, new2, new1], ExceptionGroup("", [new2, new1, rest])),
            ([new1, new2], ExceptionGroup("", [new1, new2])),
            ([new2, new1], ExceptionGroup("", [new2, new1])),
        ]

        for (input, expected) in test_cases:
            with self.subTest(input=input):
                res = _testcapi.unstable_exc_prep_reraise_star(orig, input)
                self.assertExceptionIsLike(res, expected)


if __name__ == "__main__":
    unittest.main()
