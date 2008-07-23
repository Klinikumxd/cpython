"""
TestCases for testing the locking sub-system.
"""

import time

import unittest
from test_all import verbose, have_threads, get_new_environment_path, get_new_database_path

if have_threads :
    from threading import Thread, currentThread

try:
    # For Pythons w/distutils pybsddb
    from bsddb3 import db
except ImportError:
    # For Python 2.3
    from bsddb import db

try:
    from bsddb3 import test_support
except ImportError:
    from test import test_support


#----------------------------------------------------------------------

class LockingTestCase(unittest.TestCase):
    import sys
    if sys.version_info[:3] < (2, 4, 0):
        def assertTrue(self, expr, msg=None):
            self.failUnless(expr,msg=msg)


    def setUp(self):
        self.homeDir = get_new_environment_path()
        self.env = db.DBEnv()
        self.env.open(self.homeDir, db.DB_THREAD | db.DB_INIT_MPOOL |
                                    db.DB_INIT_LOCK | db.DB_CREATE)


    def tearDown(self):
        self.env.close()
        test_support.rmtree(self.homeDir)


    def test01_simple(self):
        if verbose:
            print '\n', '-=' * 30
            print "Running %s.test01_simple..." % self.__class__.__name__

        anID = self.env.lock_id()
        if verbose:
            print "locker ID: %s" % anID
        lock = self.env.lock_get(anID, "some locked thing", db.DB_LOCK_WRITE)
        if verbose:
            print "Aquired lock: %s" % lock
        self.env.lock_put(lock)
        if verbose:
            print "Released lock: %s" % lock
        self.env.lock_id_free(anID)


    def test02_threaded(self):
        if verbose:
            print '\n', '-=' * 30
            print "Running %s.test02_threaded..." % self.__class__.__name__

        threads = []
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_WRITE,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_READ,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_READ,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_WRITE,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_READ,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_READ,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_WRITE,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_WRITE,)))
        threads.append(Thread(target = self.theThread,
                              args=(db.DB_LOCK_WRITE,)))

        for t in threads:
            t.setDaemon(True)
            t.start()
        for t in threads:
            t.join()

    def test03_lock_timeout(self):
        self.env.set_timeout(0, db.DB_SET_LOCK_TIMEOUT)
        self.env.set_timeout(0, db.DB_SET_TXN_TIMEOUT)
        self.env.set_timeout(123456, db.DB_SET_LOCK_TIMEOUT)
        self.env.set_timeout(7890123, db.DB_SET_TXN_TIMEOUT)

        def deadlock_detection() :
            while not deadlock_detection.end :
                deadlock_detection.count = \
                    self.env.lock_detect(db.DB_LOCK_EXPIRE)
                if deadlock_detection.count :
                    while not deadlock_detection.end :
                        pass
                    break
                time.sleep(0.01)

        deadlock_detection.end=False
        deadlock_detection.count=0
        t=Thread(target=deadlock_detection)
        t.setDaemon(True)
        t.start()
        self.env.set_timeout(100000, db.DB_SET_LOCK_TIMEOUT)
        anID = self.env.lock_id()
        anID2 = self.env.lock_id()
        self.assertNotEqual(anID, anID2)
        lock = self.env.lock_get(anID, "shared lock", db.DB_LOCK_WRITE)
        start_time=time.time()
        self.assertRaises(db.DBLockNotGrantedError,
                self.env.lock_get,anID2, "shared lock", db.DB_LOCK_READ)
        end_time=time.time()
        deadlock_detection.end=True
        self.assertTrue((end_time-start_time) >= 0.1)
        self.env.lock_put(lock)
        t.join()

        self.env.lock_id_free(anID)
        self.env.lock_id_free(anID2)

        if db.version() >= (4,6):
            self.assertTrue(deadlock_detection.count>0)

    def theThread(self, lockType):
        name = currentThread().getName()
        if lockType ==  db.DB_LOCK_WRITE:
            lt = "write"
        else:
            lt = "read"

        anID = self.env.lock_id()
        if verbose:
            print "%s: locker ID: %s" % (name, anID)

        for i in xrange(1000) :
            lock = self.env.lock_get(anID, "some locked thing", lockType)
            if verbose:
                print "%s: Aquired %s lock: %s" % (name, lt, lock)

            self.env.lock_put(lock)
            if verbose:
                print "%s: Released %s lock: %s" % (name, lt, lock)

        self.env.lock_id_free(anID)


#----------------------------------------------------------------------

def test_suite():
    suite = unittest.TestSuite()

    if have_threads:
        suite.addTest(unittest.makeSuite(LockingTestCase))
    else:
        suite.addTest(unittest.makeSuite(LockingTestCase, 'test01'))

    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')
