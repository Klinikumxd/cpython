# UserString is a wrapper around the native builtin string type.
# UserString instances should behave similar to builtin string objects.

import unittest
from test import string_tests

from collections import UserString

class UserStringTest(
    string_tests.CommonTest,
    string_tests.MixinStrUnicodeUserStringTest,
    unittest.TestCase
    ):

    type2test = UserString

    # Overwrite the three testing methods, because UserString
    # can't cope with arguments propagated to UserString
    # (and we don't test with subclasses)
    def checkequal(self, result, object, methodname, *args, **kwargs):
        result = self.fixtype(result)
        object = self.fixtype(object)
        # we don't fix the arguments, because UserString can't cope with it
        realresult = getattr(object, methodname)(*args, **kwargs)
        self.assertEqual(
            result,
            realresult
        )

    def checkraises(self, exc, obj, methodname, *args):
        obj = self.fixtype(obj)
        # we don't fix the arguments, because UserString can't cope with it
        with self.assertRaises(exc) as cm:
            getattr(obj, methodname)(*args)
        self.assertNotEqual(str(cm.exception), '')

    def checkcall(self, object, methodname, *args):
        object = self.fixtype(object)
        # we don't fix the arguments, because UserString can't cope with it
        getattr(object, methodname)(*args)

    def test_rmod(self):
        class ustr2(UserString):
            pass

        class ustr3(ustr2):
            def __rmod__(self, other):
                return super().__rmod__(other)

        fmt2 = ustr2('value is %s')
        str3 = ustr3('TEST')
        self.assertEqual(fmt2 % str3, 'value is TEST')


if __name__ == "__main__":
    unittest.main()
