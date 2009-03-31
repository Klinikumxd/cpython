from test import support
import unittest

# Skip test if nis module does not exist.
nis = support.import_module('nis')

raise unittest.SkipTest("test_nis hangs on Solaris")

class NisTests(unittest.TestCase):
    def test_maps(self):
        try:
            maps = nis.maps()
        except nis.error as msg:
            # NIS is probably not active, so this test isn't useful
            if support.verbose:
                print("Test Skipped:", msg)
            # Can't raise SkipTest as regrtest only recognizes the exception
            #   import time.
            return
        try:
            # On some systems, this map is only accessible to the
            # super user
            maps.remove("passwd.adjunct.byname")
        except ValueError:
            pass

        done = 0
        for nismap in maps:
            mapping = nis.cat(nismap)
            for k, v in mapping.items():
                if not k:
                    continue
                if nis.match(k, nismap) != v:
                    self.fail("NIS match failed for key `%s' in map `%s'" % (k, nismap))
                else:
                    # just test the one key, otherwise this test could take a
                    # very long time
                    done = 1
                    break
            if done:
                break

def test_main():
    support.run_unittest(NisTests)

if __name__ == '__main__':
    test_main()
