"""Unittests for test.script_helper.  Who tests the test helper?"""

import subprocess
import sys
from test import script_helper
import unittest
from unittest import mock


class TestScriptHelper(unittest.TestCase):
    def test_assert_python_expect_success(self):
        t = script_helper._assert_python(True, '-c', 'import sys; sys.exit(0)')
        self.assertEqual(0, t[0], 'return code was not 0')

    def test_assert_python_expect_failure(self):
        # I didn't import the sys module so this child will fail.
        rc, out, err = script_helper._assert_python(False, '-c', 'sys.exit(0)')
        self.assertNotEqual(0, rc, 'return code should not be 0')

    def test_assert_python_raises_expect_success(self):
        # I didn't import the sys module so this child will fail.
        with self.assertRaises(AssertionError) as error_context:
            script_helper._assert_python(True, '-c', 'sys.exit(0)')
        error_msg = str(error_context.exception)
        self.assertIn('command line was:', error_msg)
        self.assertIn('sys.exit(0)', error_msg, msg='unexpected command line')

    def test_assert_python_raises_expect_failure(self):
        with self.assertRaises(AssertionError) as error_context:
            script_helper._assert_python(False, '-c', 'import sys; sys.exit(0)')
        error_msg = str(error_context.exception)
        self.assertIn('Process return code is 0,', error_msg)
        self.assertIn('import sys; sys.exit(0)', error_msg,
                      msg='unexpected command line.')


class TestScriptHelperEnvironment(unittest.TestCase):
    """Code coverage for _interpreter_requires_environment()."""

    def setUp(self):
        self.assertTrue(
                hasattr(script_helper, '__cached_interp_requires_environment'))
        # Reset the private cached state.
        script_helper.__dict__['__cached_interp_requires_environment'] = None

    def tearDown(self):
        # Reset the private cached state.
        script_helper.__dict__['__cached_interp_requires_environment'] = None

    @mock.patch('subprocess.check_call')
    def test_interpreter_requires_environment_true(self, mock_check_call):
        mock_check_call.side_effect = subprocess.CalledProcessError('', '')
        self.assertTrue(script_helper._interpreter_requires_environment())
        self.assertTrue(script_helper._interpreter_requires_environment())
        self.assertEqual(1, mock_check_call.call_count)

    @mock.patch('subprocess.check_call')
    def test_interpreter_requires_environment_false(self, mock_check_call):
        # The mocked subprocess.check_call fakes a no-error process.
        script_helper._interpreter_requires_environment()
        self.assertFalse(script_helper._interpreter_requires_environment())
        self.assertEqual(1, mock_check_call.call_count)

    @mock.patch('subprocess.check_call')
    def test_interpreter_requires_environment_details(self, mock_check_call):
        script_helper._interpreter_requires_environment()
        self.assertFalse(script_helper._interpreter_requires_environment())
        self.assertFalse(script_helper._interpreter_requires_environment())
        self.assertEqual(1, mock_check_call.call_count)
        check_call_command = mock_check_call.call_args[0][0]
        self.assertEqual(sys.executable, check_call_command[0])
        self.assertIn('-E', check_call_command)


if __name__ == '__main__':
    unittest.main()
