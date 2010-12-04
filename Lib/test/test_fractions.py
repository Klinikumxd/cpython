"""Tests for Lib/fractions.py."""

from decimal import Decimal
from test.support import run_unittest, requires_IEEE_754
import math
import numbers
import operator
import fractions
import unittest
from copy import copy, deepcopy
from pickle import dumps, loads
F = fractions.Fraction
gcd = fractions.gcd

class DummyFloat(object):
    """Dummy float class for testing comparisons with Fractions"""

    def __init__(self, value):
        if not isinstance(value, float):
            raise TypeError("DummyFloat can only be initialized from float")
        self.value = value

    def _richcmp(self, other, op):
        if isinstance(other, numbers.Rational):
            return op(F.from_float(self.value), other)
        elif isinstance(other, DummyFloat):
            return op(self.value, other.value)
        else:
            return NotImplemented

    def __eq__(self, other): return self._richcmp(other, operator.eq)
    def __le__(self, other): return self._richcmp(other, operator.le)
    def __lt__(self, other): return self._richcmp(other, operator.lt)
    def __ge__(self, other): return self._richcmp(other, operator.ge)
    def __gt__(self, other): return self._richcmp(other, operator.gt)

    # shouldn't be calling __float__ at all when doing comparisons
    def __float__(self):
        assert False, "__float__ should not be invoked for comparisons"

    # same goes for subtraction
    def __sub__(self, other):
        assert False, "__sub__ should not be invoked for comparisons"
    __rsub__ = __sub__


class DummyRational(object):
    """Test comparison of Fraction with a naive rational implementation."""

    def __init__(self, num, den):
        g = gcd(num, den)
        self.num = num // g
        self.den = den // g

    def __eq__(self, other):
        if isinstance(other, fractions.Fraction):
            return (self.num == other._numerator and
                    self.den == other._denominator)
        else:
            return NotImplemented

    def __lt__(self, other):
        return(self.num * other._denominator < self.den * other._numerator)

    def __gt__(self, other):
        return(self.num * other._denominator > self.den * other._numerator)

    def __le__(self, other):
        return(self.num * other._denominator <= self.den * other._numerator)

    def __ge__(self, other):
        return(self.num * other._denominator >= self.den * other._numerator)

    # this class is for testing comparisons; conversion to float
    # should never be used for a comparison, since it loses accuracy
    def __float__(self):
        assert False, "__float__ should not be invoked"

class GcdTest(unittest.TestCase):

    def testMisc(self):
        self.assertEqual(0, gcd(0, 0))
        self.assertEqual(1, gcd(1, 0))
        self.assertEqual(-1, gcd(-1, 0))
        self.assertEqual(1, gcd(0, 1))
        self.assertEqual(-1, gcd(0, -1))
        self.assertEqual(1, gcd(7, 1))
        self.assertEqual(-1, gcd(7, -1))
        self.assertEqual(1, gcd(-23, 15))
        self.assertEqual(12, gcd(120, 84))
        self.assertEqual(-12, gcd(84, -120))


def _components(r):
    return (r.numerator, r.denominator)


class FractionTest(unittest.TestCase):

    def assertTypedEquals(self, expected, actual):
        """Asserts that both the types and values are the same."""
        self.assertEqual(type(expected), type(actual))
        self.assertEqual(expected, actual)

    def assertRaisesMessage(self, exc_type, message,
                            callable, *args, **kwargs):
        """Asserts that callable(*args, **kwargs) raises exc_type(message)."""
        try:
            callable(*args, **kwargs)
        except exc_type as e:
            self.assertEqual(message, str(e))
        else:
            self.fail("%s not raised" % exc_type.__name__)

    def testInit(self):
        self.assertEqual((0, 1), _components(F()))
        self.assertEqual((7, 1), _components(F(7)))
        self.assertEqual((7, 3), _components(F(F(7, 3))))

        self.assertEqual((-1, 1), _components(F(-1, 1)))
        self.assertEqual((-1, 1), _components(F(1, -1)))
        self.assertEqual((1, 1), _components(F(-2, -2)))
        self.assertEqual((1, 2), _components(F(5, 10)))
        self.assertEqual((7, 15), _components(F(7, 15)))
        self.assertEqual((10**23, 1), _components(F(10**23)))

        self.assertEqual((3, 77), _components(F(F(3, 7), 11)))
        self.assertEqual((-9, 5), _components(F(2, F(-10, 9))))
        self.assertEqual((2486, 2485), _components(F(F(22, 7), F(355, 113))))

        self.assertRaisesMessage(ZeroDivisionError, "Fraction(12, 0)",
                                 F, 12, 0)
        self.assertRaises(TypeError, F, 1.5 + 3j)

        self.assertRaises(TypeError, F, "3/2", 3)
        self.assertRaises(TypeError, F, 3, 0j)
        self.assertRaises(TypeError, F, 3, 1j)

    @requires_IEEE_754
    def testInitFromFloat(self):
        self.assertEqual((5, 2), _components(F(2.5)))
        self.assertEqual((0, 1), _components(F(-0.0)))
        self.assertEqual((3602879701896397, 36028797018963968),
                         _components(F(0.1)))
        self.assertRaises(TypeError, F, float('nan'))
        self.assertRaises(TypeError, F, float('inf'))
        self.assertRaises(TypeError, F, float('-inf'))

    def testInitFromDecimal(self):
        self.assertEqual((11, 10),
                         _components(F(Decimal('1.1'))))
        self.assertEqual((7, 200),
                         _components(F(Decimal('3.5e-2'))))
        self.assertEqual((0, 1),
                         _components(F(Decimal('.000e20'))))
        self.assertRaises(TypeError, F, Decimal('nan'))
        self.assertRaises(TypeError, F, Decimal('snan'))
        self.assertRaises(TypeError, F, Decimal('inf'))
        self.assertRaises(TypeError, F, Decimal('-inf'))

    def testFromString(self):
        self.assertEqual((5, 1), _components(F("5")))
        self.assertEqual((3, 2), _components(F("3/2")))
        self.assertEqual((3, 2), _components(F(" \n  +3/2")))
        self.assertEqual((-3, 2), _components(F("-3/2  ")))
        self.assertEqual((13, 2), _components(F("    013/02 \n  ")))
        self.assertEqual((16, 5), _components(F(" 3.2 ")))
        self.assertEqual((-16, 5), _components(F(" -3.2 ")))
        self.assertEqual((-3, 1), _components(F(" -3. ")))
        self.assertEqual((3, 5), _components(F(" .6 ")))
        self.assertEqual((1, 3125), _components(F("32.e-5")))
        self.assertEqual((1000000, 1), _components(F("1E+06")))
        self.assertEqual((-12300, 1), _components(F("-1.23e4")))
        self.assertEqual((0, 1), _components(F(" .0e+0\t")))
        self.assertEqual((0, 1), _components(F("-0.000e0")))

        self.assertRaisesMessage(
            ZeroDivisionError, "Fraction(3, 0)",
            F, "3/0")
        self.assertRaisesMessage(
            ValueError, "Invalid literal for Fraction: '3/'",
            F, "3/")
        self.assertRaisesMessage(
            ValueError, "Invalid literal for Fraction: '/2'",
            F, "/2")
        self.assertRaisesMessage(
            ValueError, "Invalid literal for Fraction: '3 /2'",
            F, "3 /2")
        self.assertRaisesMessage(
            # Denominators don't need a sign.
            ValueError, "Invalid literal for Fraction: '3/+2'",
            F, "3/+2")
        self.assertRaisesMessage(
            # Imitate float's parsing.
            ValueError, "Invalid literal for Fraction: '+ 3/2'",
            F, "+ 3/2")
        self.assertRaisesMessage(
            # Avoid treating '.' as a regex special character.
            ValueError, "Invalid literal for Fraction: '3a2'",
            F, "3a2")
        self.assertRaisesMessage(
            # Don't accept combinations of decimals and rationals.
            ValueError, "Invalid literal for Fraction: '3/7.2'",
            F, "3/7.2")
        self.assertRaisesMessage(
            # Don't accept combinations of decimals and rationals.
            ValueError, "Invalid literal for Fraction: '3.2/7'",
            F, "3.2/7")
        self.assertRaisesMessage(
            # Allow 3. and .3, but not .
            ValueError, "Invalid literal for Fraction: '.'",
            F, ".")

    def testImmutable(self):
        r = F(7, 3)
        r.__init__(2, 15)
        self.assertEqual((7, 3), _components(r))

        self.assertRaises(AttributeError, setattr, r, 'numerator', 12)
        self.assertRaises(AttributeError, setattr, r, 'denominator', 6)
        self.assertEqual((7, 3), _components(r))

        # But if you _really_ need to:
        r._numerator = 4
        r._denominator = 2
        self.assertEqual((4, 2), _components(r))
        # Which breaks some important operations:
        self.assertNotEqual(F(4, 2), r)

    def testFromFloat(self):
        self.assertRaises(TypeError, F.from_float, 3+4j)
        self.assertEqual((10, 1), _components(F.from_float(10)))
        bigint = 1234567890123456789
        self.assertEqual((bigint, 1), _components(F.from_float(bigint)))
        self.assertEqual((0, 1), _components(F.from_float(-0.0)))
        self.assertEqual((10, 1), _components(F.from_float(10.0)))
        self.assertEqual((-5, 2), _components(F.from_float(-2.5)))
        self.assertEqual((99999999999999991611392, 1),
                         _components(F.from_float(1e23)))
        self.assertEqual(float(10**23), float(F.from_float(1e23)))
        self.assertEqual((3602879701896397, 1125899906842624),
                         _components(F.from_float(3.2)))
        self.assertEqual(3.2, float(F.from_float(3.2)))

        inf = 1e1000
        nan = inf - inf
        self.assertRaisesMessage(
            TypeError, "Cannot convert inf to Fraction.",
            F.from_float, inf)
        self.assertRaisesMessage(
            TypeError, "Cannot convert -inf to Fraction.",
            F.from_float, -inf)
        self.assertRaisesMessage(
            TypeError, "Cannot convert nan to Fraction.",
            F.from_float, nan)

    def testFromDecimal(self):
        self.assertRaises(TypeError, F.from_decimal, 3+4j)
        self.assertEqual(F(10, 1), F.from_decimal(10))
        self.assertEqual(F(0), F.from_decimal(Decimal("-0")))
        self.assertEqual(F(5, 10), F.from_decimal(Decimal("0.5")))
        self.assertEqual(F(5, 1000), F.from_decimal(Decimal("5e-3")))
        self.assertEqual(F(5000), F.from_decimal(Decimal("5e3")))
        self.assertEqual(1 - F(1, 10**30),
                         F.from_decimal(Decimal("0." + "9" * 30)))

        self.assertRaisesMessage(
            TypeError, "Cannot convert Infinity to Fraction.",
            F.from_decimal, Decimal("inf"))
        self.assertRaisesMessage(
            TypeError, "Cannot convert -Infinity to Fraction.",
            F.from_decimal, Decimal("-inf"))
        self.assertRaisesMessage(
            TypeError, "Cannot convert NaN to Fraction.",
            F.from_decimal, Decimal("nan"))
        self.assertRaisesMessage(
            TypeError, "Cannot convert sNaN to Fraction.",
            F.from_decimal, Decimal("snan"))

    def testLimitDenominator(self):
        rpi = F('3.1415926535897932')
        self.assertEqual(rpi.limit_denominator(10000), F(355, 113))
        self.assertEqual(-rpi.limit_denominator(10000), F(-355, 113))
        self.assertEqual(rpi.limit_denominator(113), F(355, 113))
        self.assertEqual(rpi.limit_denominator(112), F(333, 106))
        self.assertEqual(F(201, 200).limit_denominator(100), F(1))
        self.assertEqual(F(201, 200).limit_denominator(101), F(102, 101))
        self.assertEqual(F(0).limit_denominator(10000), F(0))

    def testConversions(self):
        self.assertTypedEquals(-1, math.trunc(F(-11, 10)))
        self.assertTypedEquals(-2, math.floor(F(-11, 10)))
        self.assertTypedEquals(-1, math.ceil(F(-11, 10)))
        self.assertTypedEquals(-1, math.ceil(F(-10, 10)))
        self.assertTypedEquals(-1, int(F(-11, 10)))
        self.assertTypedEquals(0, round(F(-1, 10)))
        self.assertTypedEquals(0, round(F(-5, 10)))
        self.assertTypedEquals(-2, round(F(-15, 10)))
        self.assertTypedEquals(-1, round(F(-7, 10)))

        self.assertEqual(False, bool(F(0, 1)))
        self.assertEqual(True, bool(F(3, 2)))
        self.assertTypedEquals(0.1, float(F(1, 10)))

        # Check that __float__ isn't implemented by converting the
        # numerator and denominator to float before dividing.
        self.assertRaises(OverflowError, float, int('2'*400+'7'))
        self.assertAlmostEqual(2.0/3,
                               float(F(int('2'*400+'7'), int('3'*400+'1'))))

        self.assertTypedEquals(0.1+0j, complex(F(1,10)))

    def testRound(self):
        self.assertTypedEquals(F(-200), round(F(-150), -2))
        self.assertTypedEquals(F(-200), round(F(-250), -2))
        self.assertTypedEquals(F(30), round(F(26), -1))
        self.assertTypedEquals(F(-2, 10), round(F(-15, 100), 1))
        self.assertTypedEquals(F(-2, 10), round(F(-25, 100), 1))


    def testArithmetic(self):
        self.assertEqual(F(1, 2), F(1, 10) + F(2, 5))
        self.assertEqual(F(-3, 10), F(1, 10) - F(2, 5))
        self.assertEqual(F(1, 25), F(1, 10) * F(2, 5))
        self.assertEqual(F(1, 4), F(1, 10) / F(2, 5))
        self.assertTypedEquals(2, F(9, 10) // F(2, 5))
        self.assertTypedEquals(10**23, F(10**23, 1) // F(1))
        self.assertEqual(F(2, 3), F(-7, 3) % F(3, 2))
        self.assertEqual(F(8, 27), F(2, 3) ** F(3))
        self.assertEqual(F(27, 8), F(2, 3) ** F(-3))
        self.assertTypedEquals(2.0, F(4) ** F(1, 2))
        z = pow(F(-1), F(1, 2))
        self.assertAlmostEqual(z.real, 0)
        self.assertEqual(z.imag, 1)

    def testMixedArithmetic(self):
        self.assertTypedEquals(F(11, 10), F(1, 10) + 1)
        self.assertTypedEquals(1.1, F(1, 10) + 1.0)
        self.assertTypedEquals(1.1 + 0j, F(1, 10) + (1.0 + 0j))
        self.assertTypedEquals(F(11, 10), 1 + F(1, 10))
        self.assertTypedEquals(1.1, 1.0 + F(1, 10))
        self.assertTypedEquals(1.1 + 0j, (1.0 + 0j) + F(1, 10))

        self.assertTypedEquals(F(-9, 10), F(1, 10) - 1)
        self.assertTypedEquals(-0.9, F(1, 10) - 1.0)
        self.assertTypedEquals(-0.9 + 0j, F(1, 10) - (1.0 + 0j))
        self.assertTypedEquals(F(9, 10), 1 - F(1, 10))
        self.assertTypedEquals(0.9, 1.0 - F(1, 10))
        self.assertTypedEquals(0.9 + 0j, (1.0 + 0j) - F(1, 10))

        self.assertTypedEquals(F(1, 10), F(1, 10) * 1)
        self.assertTypedEquals(0.1, F(1, 10) * 1.0)
        self.assertTypedEquals(0.1 + 0j, F(1, 10) * (1.0 + 0j))
        self.assertTypedEquals(F(1, 10), 1 * F(1, 10))
        self.assertTypedEquals(0.1, 1.0 * F(1, 10))
        self.assertTypedEquals(0.1 + 0j, (1.0 + 0j) * F(1, 10))

        self.assertTypedEquals(F(1, 10), F(1, 10) / 1)
        self.assertTypedEquals(0.1, F(1, 10) / 1.0)
        self.assertTypedEquals(0.1 + 0j, F(1, 10) / (1.0 + 0j))
        self.assertTypedEquals(F(10, 1), 1 / F(1, 10))
        self.assertTypedEquals(10.0, 1.0 / F(1, 10))
        self.assertTypedEquals(10.0 + 0j, (1.0 + 0j) / F(1, 10))

        self.assertTypedEquals(0, F(1, 10) // 1)
        self.assertTypedEquals(0, F(1, 10) // 1.0)
        self.assertTypedEquals(10, 1 // F(1, 10))
        self.assertTypedEquals(10**23, 10**22 // F(1, 10))
        self.assertTypedEquals(10, 1.0 // F(1, 10))

        self.assertTypedEquals(F(1, 10), F(1, 10) % 1)
        self.assertTypedEquals(0.1, F(1, 10) % 1.0)
        self.assertTypedEquals(F(0, 1), 1 % F(1, 10))
        self.assertTypedEquals(0.0, 1.0 % F(1, 10))

        # No need for divmod since we don't override it.

        # ** has more interesting conversion rules.
        self.assertTypedEquals(F(100, 1), F(1, 10) ** -2)
        self.assertTypedEquals(F(100, 1), F(10, 1) ** 2)
        self.assertTypedEquals(0.1, F(1, 10) ** 1.0)
        self.assertTypedEquals(0.1 + 0j, F(1, 10) ** (1.0 + 0j))
        self.assertTypedEquals(4 , 2 ** F(2, 1))
        z = pow(-1, F(1, 2))
        self.assertAlmostEqual(0, z.real)
        self.assertEqual(1, z.imag)
        self.assertTypedEquals(F(1, 4) , 2 ** F(-2, 1))
        self.assertTypedEquals(2.0 , 4 ** F(1, 2))
        self.assertTypedEquals(0.25, 2.0 ** F(-2, 1))
        self.assertTypedEquals(1.0 + 0j, (1.0 + 0j) ** F(1, 10))

    def testMixingWithDecimal(self):
        # Decimal refuses mixed arithmetic (but not mixed comparisons)
        self.assertRaisesMessage(
            TypeError,
            "unsupported operand type(s) for +: 'Fraction' and 'Decimal'",
            operator.add, F(3,11), Decimal('3.1415926'))

    def testComparisons(self):
        self.assertTrue(F(1, 2) < F(2, 3))
        self.assertFalse(F(1, 2) < F(1, 2))
        self.assertTrue(F(1, 2) <= F(2, 3))
        self.assertTrue(F(1, 2) <= F(1, 2))
        self.assertFalse(F(2, 3) <= F(1, 2))
        self.assertTrue(F(1, 2) == F(1, 2))
        self.assertFalse(F(1, 2) == F(1, 3))
        self.assertFalse(F(1, 2) != F(1, 2))
        self.assertTrue(F(1, 2) != F(1, 3))

    def testComparisonsDummyRational(self):
        self.assertTrue(F(1, 2) == DummyRational(1, 2))
        self.assertTrue(DummyRational(1, 2) == F(1, 2))
        self.assertFalse(F(1, 2) == DummyRational(3, 4))
        self.assertFalse(DummyRational(3, 4) == F(1, 2))

        self.assertTrue(F(1, 2) < DummyRational(3, 4))
        self.assertFalse(F(1, 2) < DummyRational(1, 2))
        self.assertFalse(F(1, 2) < DummyRational(1, 7))
        self.assertFalse(F(1, 2) > DummyRational(3, 4))
        self.assertFalse(F(1, 2) > DummyRational(1, 2))
        self.assertTrue(F(1, 2) > DummyRational(1, 7))
        self.assertTrue(F(1, 2) <= DummyRational(3, 4))
        self.assertTrue(F(1, 2) <= DummyRational(1, 2))
        self.assertFalse(F(1, 2) <= DummyRational(1, 7))
        self.assertFalse(F(1, 2) >= DummyRational(3, 4))
        self.assertTrue(F(1, 2) >= DummyRational(1, 2))
        self.assertTrue(F(1, 2) >= DummyRational(1, 7))

        self.assertTrue(DummyRational(1, 2) < F(3, 4))
        self.assertFalse(DummyRational(1, 2) < F(1, 2))
        self.assertFalse(DummyRational(1, 2) < F(1, 7))
        self.assertFalse(DummyRational(1, 2) > F(3, 4))
        self.assertFalse(DummyRational(1, 2) > F(1, 2))
        self.assertTrue(DummyRational(1, 2) > F(1, 7))
        self.assertTrue(DummyRational(1, 2) <= F(3, 4))
        self.assertTrue(DummyRational(1, 2) <= F(1, 2))
        self.assertFalse(DummyRational(1, 2) <= F(1, 7))
        self.assertFalse(DummyRational(1, 2) >= F(3, 4))
        self.assertTrue(DummyRational(1, 2) >= F(1, 2))
        self.assertTrue(DummyRational(1, 2) >= F(1, 7))

    def testComparisonsDummyFloat(self):
        x = DummyFloat(1./3.)
        y = F(1, 3)
        self.assertTrue(x != y)
        self.assertTrue(x < y or x > y)
        self.assertFalse(x == y)
        self.assertFalse(x <= y and x >= y)
        self.assertTrue(y != x)
        self.assertTrue(y < x or y > x)
        self.assertFalse(y == x)
        self.assertFalse(y <= x and y >= x)

    def testMixedLess(self):
        self.assertTrue(2 < F(5, 2))
        self.assertFalse(2 < F(4, 2))
        self.assertTrue(F(5, 2) < 3)
        self.assertFalse(F(4, 2) < 2)

        self.assertTrue(F(1, 2) < 0.6)
        self.assertFalse(F(1, 2) < 0.4)
        self.assertTrue(0.4 < F(1, 2))
        self.assertFalse(0.5 < F(1, 2))

        self.assertFalse(float('inf') < F(1, 2))
        self.assertTrue(float('-inf') < F(0, 10))
        self.assertFalse(float('nan') < F(-3, 7))
        self.assertTrue(F(1, 2) < float('inf'))
        self.assertFalse(F(17, 12) < float('-inf'))
        self.assertFalse(F(144, -89) < float('nan'))

    def testMixedLessEqual(self):
        self.assertTrue(0.5 <= F(1, 2))
        self.assertFalse(0.6 <= F(1, 2))
        self.assertTrue(F(1, 2) <= 0.5)
        self.assertFalse(F(1, 2) <= 0.4)
        self.assertTrue(2 <= F(4, 2))
        self.assertFalse(2 <= F(3, 2))
        self.assertTrue(F(4, 2) <= 2)
        self.assertFalse(F(5, 2) <= 2)

        self.assertFalse(float('inf') <= F(1, 2))
        self.assertTrue(float('-inf') <= F(0, 10))
        self.assertFalse(float('nan') <= F(-3, 7))
        self.assertTrue(F(1, 2) <= float('inf'))
        self.assertFalse(F(17, 12) <= float('-inf'))
        self.assertFalse(F(144, -89) <= float('nan'))

    def testBigFloatComparisons(self):
        # Because 10**23 can't be represented exactly as a float:
        self.assertFalse(F(10**23) == float(10**23))
        # The first test demonstrates why these are important.
        self.assertFalse(1e23 < float(F(math.trunc(1e23) + 1)))
        self.assertTrue(1e23 < F(math.trunc(1e23) + 1))
        self.assertFalse(1e23 <= F(math.trunc(1e23) - 1))
        self.assertTrue(1e23 > F(math.trunc(1e23) - 1))
        self.assertFalse(1e23 >= F(math.trunc(1e23) + 1))

    def testBigComplexComparisons(self):
        self.assertFalse(F(10**23) == complex(10**23))
        self.assertRaises(TypeError, operator.gt, F(10**23), complex(10**23))
        self.assertRaises(TypeError, operator.le, F(10**23), complex(10**23))

        x = F(3, 8)
        z = complex(0.375, 0.0)
        w = complex(0.375, 0.2)
        self.assertTrue(x == z)
        self.assertFalse(x != z)
        self.assertFalse(x == w)
        self.assertTrue(x != w)
        for op in operator.lt, operator.le, operator.gt, operator.ge:
            self.assertRaises(TypeError, op, x, z)
            self.assertRaises(TypeError, op, z, x)
            self.assertRaises(TypeError, op, x, w)
            self.assertRaises(TypeError, op, w, x)

    def testMixedEqual(self):
        self.assertTrue(0.5 == F(1, 2))
        self.assertFalse(0.6 == F(1, 2))
        self.assertTrue(F(1, 2) == 0.5)
        self.assertFalse(F(1, 2) == 0.4)
        self.assertTrue(2 == F(4, 2))
        self.assertFalse(2 == F(3, 2))
        self.assertTrue(F(4, 2) == 2)
        self.assertFalse(F(5, 2) == 2)
        self.assertFalse(F(5, 2) == float('nan'))
        self.assertFalse(float('nan') == F(3, 7))
        self.assertFalse(F(5, 2) == float('inf'))
        self.assertFalse(float('-inf') == F(2, 5))

    def testStringification(self):
        self.assertEqual("Fraction(7, 3)", repr(F(7, 3)))
        self.assertEqual("Fraction(6283185307, 2000000000)",
                         repr(F('3.1415926535')))
        self.assertEqual("Fraction(-1, 100000000000000000000)",
                         repr(F(1, -10**20)))
        self.assertEqual("7/3", str(F(7, 3)))
        self.assertEqual("7", str(F(7, 1)))

    def testHash(self):
        self.assertEqual(hash(2.5), hash(F(5, 2)))
        self.assertEqual(hash(10**50), hash(F(10**50)))
        self.assertNotEqual(hash(float(10**23)), hash(F(10**23)))
        # Check that __hash__ produces the same value as hash(), for
        # consistency with int and Decimal.  (See issue #10356.)
        self.assertEqual(hash(F(-1)), F(-1).__hash__())

    def testApproximatePi(self):
        # Algorithm borrowed from
        # http://docs.python.org/lib/decimal-recipes.html
        three = F(3)
        lasts, t, s, n, na, d, da = 0, three, 3, 1, 0, 0, 24
        while abs(s - lasts) > F(1, 10**9):
            lasts = s
            n, na = n+na, na+8
            d, da = d+da, da+32
            t = (t * n) / d
            s += t
        self.assertAlmostEqual(math.pi, s)

    def testApproximateCos1(self):
        # Algorithm borrowed from
        # http://docs.python.org/lib/decimal-recipes.html
        x = F(1)
        i, lasts, s, fact, num, sign = 0, 0, F(1), 1, 1, 1
        while abs(s - lasts) > F(1, 10**9):
            lasts = s
            i += 2
            fact *= i * (i-1)
            num *= x * x
            sign *= -1
            s += num / fact * sign
        self.assertAlmostEqual(math.cos(1), s)

    def test_copy_deepcopy_pickle(self):
        r = F(13, 7)
        self.assertEqual(r, loads(dumps(r)))
        self.assertEqual(id(r), id(copy(r)))
        self.assertEqual(id(r), id(deepcopy(r)))

    def test_slots(self):
        # Issue 4998
        r = F(13, 7)
        self.assertRaises(AttributeError, setattr, r, 'a', 10)

def test_main():
    run_unittest(FractionTest, GcdTest)

if __name__ == '__main__':
    test_main()
