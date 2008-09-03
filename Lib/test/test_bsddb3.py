# Test driver for bsddb package.
"""
Run all test cases.
"""
import os
import sys
import tempfile
import time
import unittest
from test.support import requires, verbose, run_unittest, unlink, rmtree

# When running as a script instead of within the regrtest framework, skip the
# requires test, since it's obvious we want to run them.
verbose = False
if 'verbose' in sys.argv:
    verbose = True
    sys.argv.remove('verbose')

if 'silent' in sys.argv:  # take care of old flag, just in case
    verbose = False
    sys.argv.remove('silent')


class TimingCheck(unittest.TestCase):

    """This class is not a real test.  Its purpose is to print a message
    periodically when the test runs slowly.  This will prevent the buildbots
    from timing out on slow machines."""

    # How much time in seconds before printing a 'Still working' message.
    # Since this is run at most once between each test module, use a smaller
    # interval than other tests.
    _PRINT_WORKING_MSG_INTERVAL = 4 * 60

    # next_time is used as a global variable that survives each instance.
    # This is necessary since a new instance will be created for each test.
    next_time = time.time() + _PRINT_WORKING_MSG_INTERVAL

    def testCheckElapsedTime(self):
        # Print still working message since these tests can be really slow.
        now = time.time()
        if self.next_time <= now:
            TimingCheck.next_time = now + self._PRINT_WORKING_MSG_INTERVAL
            sys.__stdout__.write('  test_bsddb3 still working, be patient...\n')
            sys.__stdout__.flush()


# For invocation through regrtest
def test_main():
    from bsddb import db
    from bsddb.test import test_all

    # This must be improved...
    test_all.do_proxy_db_py3k(True)

    test_all.set_test_path_prefix(os.path.join(tempfile.gettempdir(),
                                 'z-test_bsddb3-%s' %
                                 os.getpid()))
    # Please leave this print in, having this show up in the buildbots
    # makes diagnosing problems a lot easier.
    # The decode is used to workaround this:
    # http://mail.python.org/pipermail/python-3000/2008-September/014709.html
    print(db.DB_VERSION_STRING.decode("iso8859-1"), file=sys.stderr)
    print('Test path prefix: ', test_all.get_test_path_prefix(), file=sys.stderr)
    try:
        run_unittest(test_all.suite(module_prefix='bsddb.test.',
                                    timing_check=TimingCheck))
    finally:
        # The only reason to remove db_home is in case if there is an old
        # one lying around.  This might be by a different user, so just
        # ignore errors.  We should always make a unique name now.
        try:
            test_all.remove_test_path_directory()
        except:
            pass

        # This must be improved...
        test_all.do_proxy_db_py3k(False)


if __name__ == '__main__':
    test_main()
