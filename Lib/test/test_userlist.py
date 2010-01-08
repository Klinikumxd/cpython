# Check every path through every method of UserList

from UserList import UserList
from test import test_support, list_tests
import warnings

class UserListTest(list_tests.CommonTest):
    type2test = UserList

    def test_getslice(self):
        super(UserListTest, self).test_getslice()
        l = [0, 1, 2, 3, 4]
        u = self.type2test(l)
        for i in range(-3, 6):
            self.assertEqual(u[:i], l[:i])
            self.assertEqual(u[i:], l[i:])
            for j in xrange(-3, 6):
                self.assertEqual(u[i:j], l[i:j])

    def test_add_specials(self):
        u = UserList("spam")
        u2 = u + "eggs"
        self.assertEqual(u2, list("spameggs"))

    def test_radd_specials(self):
        u = UserList("eggs")
        u2 = "spam" + u
        self.assertEqual(u2, list("spameggs"))
        u2 = u.__radd__(UserList("spam"))
        self.assertEqual(u2, list("spameggs"))

    def test_iadd(self):
        super(UserListTest, self).test_iadd()
        u = [0, 1]
        u += UserList([0, 1])
        self.assertEqual(u, [0, 1, 0, 1])

    def test_mixedcmp(self):
        u = self.type2test([0, 1])
        self.assertEqual(u, [0, 1])
        self.assertNotEqual(u, [0])
        self.assertNotEqual(u, [0, 2])

    def test_mixedadd(self):
        u = self.type2test([0, 1])
        self.assertEqual(u + [], u)
        self.assertEqual(u + [2], [0, 1, 2])

    def test_getitemoverwriteiter(self):
        # Verify that __getitem__ overrides *are* recognized by __iter__
        class T(self.type2test):
            def __getitem__(self, key):
                return str(key) + '!!!'
        self.assertEqual(iter(T((1,2))).next(), "0!!!")

def test_main():
    with warnings.catch_warnings():
        # Silence Py3k warnings
        warnings.filterwarnings("ignore", ".+slice__ has been removed",
                                DeprecationWarning)
        test_support.run_unittest(UserListTest)

if __name__ == "__main__":
    test_main()
