"""A module to test whether doctest recognizes some 2.2 features,
like static and class methods.

>>> print 'yup'  # 1
yup
"""

import test_support

# XXX The class docstring is skipped.
class C(object):
    """Class C.

    >>> print C()  # 2
    42
    """

    def __init__(self):
        """C.__init__.

        >>> print C() # 3
        42
        """

    def __str__(self):
        """
        >>> print C() # 4
        42
        """
        return "42"

    # XXX The class docstring is skipped.
    class D(object):
        """A nested D class.

        >>> print "In D!"   # 5
        In D!
        """

        def nested(self):
            """
            >>> print 3 # 6
            3
            """

    def getx(self):
        """
        >>> c = C()    # 7
        >>> c.x = 12   # 8
        >>> print c.x  # 9
        -12
        """
        return -self._x

    def setx(self, value):
        """
        >>> c = C()     # 10
        >>> c.x = 12    # 11
        >>> print c.x   # 12
        -12
        """
        self._x = value

    x = property(getx, setx, doc="""\
        >>> c = C()    # 13
        >>> c.x = 12   # 14
        >>> print c.x  # 15
        -12
        """)

    def statm():
        """
        A static method.

        >>> print C.statm()    # 16
        666
        >>> print C().statm()  # 17
        666
        """
        return 666

    statm = staticmethod(statm)

    def clsm(cls, val):
        """
        A class method.

        >>> print C.clsm(22)    # 18
        22
        >>> print C().clsm(22)  # 19
        22
        """
        return 22

    clsm = classmethod(clsm)

def test_main():
    import test_doctest2
    # XXX 2 class docstrings are skipped.
    # EXPECTED = 19
    EXPECTED = 17
    f, t = test_support.run_doctest(test_doctest2)
    if t != EXPECTED:
        raise test_support.TestFailed("expected %d tests to run, not %d" %
                                      (EXPECTED, t))

# Pollute the namespace with a bunch of imported functions and classes,
# to make sure they don't get tested.
from doctest import *

if __name__ == '__main__':
    test_main()
