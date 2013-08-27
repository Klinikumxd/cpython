from test.support import run_unittest, requires_IEEE_754
from test.test_math import parse_testfile, test_file
import unittest
import cmath, math
from cmath import phase, polar, rect, pi
import sysconfig

INF = float('inf')
NAN = float('nan')

complex_zeros = [complex(x, y) for x in [0.0, -0.0] for y in [0.0, -0.0]]
complex_infinities = [complex(x, y) for x, y in [
        (INF, 0.0),  # 1st quadrant
        (INF, 2.3),
        (INF, INF),
        (2.3, INF),
        (0.0, INF),
        (-0.0, INF), # 2nd quadrant
        (-2.3, INF),
        (-INF, INF),
        (-INF, 2.3),
        (-INF, 0.0),
        (-INF, -0.0), # 3rd quadrant
        (-INF, -2.3),
        (-INF, -INF),
        (-2.3, -INF),
        (-0.0, -INF),
        (0.0, -INF), # 4th quadrant
        (2.3, -INF),
        (INF, -INF),
        (INF, -2.3),
        (INF, -0.0)
        ]]
complex_nans = [complex(x, y) for x, y in [
        (NAN, -INF),
        (NAN, -2.3),
        (NAN, -0.0),
        (NAN, 0.0),
        (NAN, 2.3),
        (NAN, INF),
        (-INF, NAN),
        (-2.3, NAN),
        (-0.0, NAN),
        (0.0, NAN),
        (2.3, NAN),
        (INF, NAN)
        ]]

class CMathTests(unittest.TestCase):
    # list of all functions in cmath
    test_functions = [getattr(cmath, fname) for fname in [
            'acos', 'acosh', 'asin', 'asinh', 'atan', 'atanh',
            'cos', 'cosh', 'exp', 'log', 'log10', 'sin', 'sinh',
            'sqrt', 'tan', 'tanh']]
    # test first and second arguments independently for 2-argument log
    test_functions.append(lambda x : cmath.log(x, 1729. + 0j))
    test_functions.append(lambda x : cmath.log(14.-27j, x))

    def setUp(self):
        self.test_values = open(test_file)

    def tearDown(self):
        self.test_values.close()

    def assertFloatIdentical(self, x, y):
        """Fail unless floats x and y are identical, in the sense that:
        (1) both x and y are nans, or
        (2) both x and y are infinities, with the same sign, or
        (3) both x and y are zeros, with the same sign, or
        (4) x and y are both finite and nonzero, and x == y

        """
        msg = 'floats {!r} and {!r} are not identical'

        if math.isnan(x) or math.isnan(y):
            if math.isnan(x) and math.isnan(y):
                return
        elif x == y:
            if x != 0.0:
                return
            # both zero; check that signs match
            elif math.copysign(1.0, x) == math.copysign(1.0, y):
                return
            else:
                msg += ': zeros have different signs'
        self.fail(msg.format(x, y))

    def assertComplexIdentical(self, x, y):
        """Fail unless complex numbers x and y have equal values and signs.

        In particular, if x and y both have real (or imaginary) part
        zero, but the zeros have different signs, this test will fail.

        """
        self.assertFloatIdentical(x.real, y.real)
        self.assertFloatIdentical(x.imag, y.imag)

    def rAssertAlmostEqual(self, a, b, rel_err = 2e-15, abs_err = 5e-323,
                           msg=None):
        """Fail if the two floating-point numbers are not almost equal.

        Determine whether floating-point values a and b are equal to within
        a (small) rounding error.  The default values for rel_err and
        abs_err are chosen to be suitable for platforms where a float is
        represented by an IEEE 754 double.  They allow an error of between
        9 and 19 ulps.
        """

        # special values testing
        if math.isnan(a):
            if math.isnan(b):
                return
            self.fail(msg or '{!r} should be nan'.format(b))

        if math.isinf(a):
            if a == b:
                return
            self.fail(msg or 'finite result where infinity expected: '
                      'expected {!r}, got {!r}'.format(a, b))

        # if both a and b are zero, check whether they have the same sign
        # (in theory there are examples where it would be legitimate for a
        # and b to have opposite signs; in practice these hardly ever
        # occur).
        if not a and not b:
            if math.copysign(1., a) != math.copysign(1., b):
                self.fail(msg or 'zero has wrong sign: expected {!r}, '
                          'got {!r}'.format(a, b))

        # if a-b overflows, or b is infinite, return False.  Again, in
        # theory there are examples where a is within a few ulps of the
        # max representable float, and then b could legitimately be
        # infinite.  In practice these examples are rare.
        try:
            absolute_error = abs(b-a)
        except OverflowError:
            pass
        else:
            # test passes if either the absolute error or the relative
            # error is sufficiently small.  The defaults amount to an
            # error of between 9 ulps and 19 ulps on an IEEE-754 compliant
            # machine.
            if absolute_error <= max(abs_err, rel_err * abs(a)):
                return
        self.fail(msg or
                  '{!r} and {!r} are not sufficiently close'.format(a, b))

    def test_constants(self):
        e_expected = 2.71828182845904523536
        pi_expected = 3.14159265358979323846
        self.assertAlmostEqual(cmath.pi, pi_expected, places=9,
            msg="cmath.pi is {}; should be {}".format(cmath.pi, pi_expected))
        self.assertAlmostEqual(cmath.e, e_expected, places=9,
            msg="cmath.e is {}; should be {}".format(cmath.e, e_expected))

    def test_user_object(self):
        # Test automatic calling of __complex__ and __float__ by cmath
        # functions

        # some random values to use as test values; we avoid values
        # for which any of the functions in cmath is undefined
        # (i.e. 0., 1., -1., 1j, -1j) or would cause overflow
        cx_arg = 4.419414439 + 1.497100113j
        flt_arg = -6.131677725

        # a variety of non-complex numbers, used to check that
        # non-complex return values from __complex__ give an error
        non_complexes = ["not complex", 1, 5, 2., None,
                         object(), NotImplemented]

        # Now we introduce a variety of classes whose instances might
        # end up being passed to the cmath functions

        # usual case: new-style class implementing __complex__
        class MyComplex(object):
            def __init__(self, value):
                self.value = value
            def __complex__(self):
                return self.value

        # old-style class implementing __complex__
        class MyComplexOS:
            def __init__(self, value):
                self.value = value
            def __complex__(self):
                return self.value

        # classes for which __complex__ raises an exception
        class SomeException(Exception):
            pass
        class MyComplexException(object):
            def __complex__(self):
                raise SomeException
        class MyComplexExceptionOS:
            def __complex__(self):
                raise SomeException

        # some classes not providing __float__ or __complex__
        class NeitherComplexNorFloat(object):
            pass
        class NeitherComplexNorFloatOS:
            pass
        class MyInt(object):
            def __int__(self): return 2
            def __index__(self): return 2
        class MyIntOS:
            def __int__(self): return 2
            def __index__(self): return 2

        # other possible combinations of __float__ and __complex__
        # that should work
        class FloatAndComplex(object):
            def __float__(self):
                return flt_arg
            def __complex__(self):
                return cx_arg
        class FloatAndComplexOS:
            def __float__(self):
                return flt_arg
            def __complex__(self):
                return cx_arg
        class JustFloat(object):
            def __float__(self):
                return flt_arg
        class JustFloatOS:
            def __float__(self):
                return flt_arg

        for f in self.test_functions:
            # usual usage
            self.assertEqual(f(MyComplex(cx_arg)), f(cx_arg))
            self.assertEqual(f(MyComplexOS(cx_arg)), f(cx_arg))
            # other combinations of __float__ and __complex__
            self.assertEqual(f(FloatAndComplex()), f(cx_arg))
            self.assertEqual(f(FloatAndComplexOS()), f(cx_arg))
            self.assertEqual(f(JustFloat()), f(flt_arg))
            self.assertEqual(f(JustFloatOS()), f(flt_arg))
            # TypeError should be raised for classes not providing
            # either __complex__ or __float__, even if they provide
            # __int__ or __index__.  An old-style class
            # currently raises AttributeError instead of a TypeError;
            # this could be considered a bug.
            self.assertRaises(TypeError, f, NeitherComplexNorFloat())
            self.assertRaises(TypeError, f, MyInt())
            self.assertRaises(Exception, f, NeitherComplexNorFloatOS())
            self.assertRaises(Exception, f, MyIntOS())
            # non-complex return value from __complex__ -> TypeError
            for bad_complex in non_complexes:
                self.assertRaises(TypeError, f, MyComplex(bad_complex))
                self.assertRaises(TypeError, f, MyComplexOS(bad_complex))
            # exceptions in __complex__ should be propagated correctly
            self.assertRaises(SomeException, f, MyComplexException())
            self.assertRaises(SomeException, f, MyComplexExceptionOS())

    def test_input_type(self):
        # ints should be acceptable inputs to all cmath
        # functions, by virtue of providing a __float__ method
        for f in self.test_functions:
            for arg in [2, 2.]:
                self.assertEqual(f(arg), f(arg.__float__()))

        # but strings should give a TypeError
        for f in self.test_functions:
            for arg in ["a", "long_string", "0", "1j", ""]:
                self.assertRaises(TypeError, f, arg)

    def test_cmath_matches_math(self):
        # check that corresponding cmath and math functions are equal
        # for floats in the appropriate range

        # test_values in (0, 1)
        test_values = [0.01, 0.1, 0.2, 0.5, 0.9, 0.99]

        # test_values for functions defined on [-1., 1.]
        unit_interval = test_values + [-x for x in test_values] + \
            [0., 1., -1.]

        # test_values for log, log10, sqrt
        positive = test_values + [1.] + [1./x for x in test_values]
        nonnegative = [0.] + positive

        # test_values for functions defined on the whole real line
        real_line = [0.] + positive + [-x for x in positive]

        test_functions = {
            'acos' : unit_interval,
            'asin' : unit_interval,
            'atan' : real_line,
            'cos' : real_line,
            'cosh' : real_line,
            'exp' : real_line,
            'log' : positive,
            'log10' : positive,
            'sin' : real_line,
            'sinh' : real_line,
            'sqrt' : nonnegative,
            'tan' : real_line,
            'tanh' : real_line}

        for fn, values in test_functions.items():
            float_fn = getattr(math, fn)
            complex_fn = getattr(cmath, fn)
            for v in values:
                z = complex_fn(v)
                self.rAssertAlmostEqual(float_fn(v), z.real)
                self.assertEqual(0., z.imag)

        # test two-argument version of log with various bases
        for base in [0.5, 2., 10.]:
            for v in positive:
                z = cmath.log(v, base)
                self.rAssertAlmostEqual(math.log(v, base), z.real)
                self.assertEqual(0., z.imag)

    @requires_IEEE_754
    def test_specific_values(self):
        def rect_complex(z):
            """Wrapped version of rect that accepts a complex number instead of
            two float arguments."""
            return cmath.rect(z.real, z.imag)

        def polar_complex(z):
            """Wrapped version of polar that returns a complex number instead of
            two floats."""
            return complex(*polar(z))

        for id, fn, ar, ai, er, ei, flags in parse_testfile(test_file):
            arg = complex(ar, ai)
            expected = complex(er, ei)
            if fn == 'rect':
                function = rect_complex
            elif fn == 'polar':
                function = polar_complex
            else:
                function = getattr(cmath, fn)
            if 'divide-by-zero' in flags or 'invalid' in flags:
                try:
                    actual = function(arg)
                except ValueError:
                    continue
                else:
                    self.fail('ValueError not raised in test '
                          '{}: {}(complex({!r}, {!r}))'.format(id, fn, ar, ai))

            if 'overflow' in flags:
                try:
                    actual = function(arg)
                except OverflowError:
                    continue
                else:
                    self.fail('OverflowError not raised in test '
                          '{}: {}(complex({!r}, {!r}))'.format(id, fn, ar, ai))

            actual = function(arg)

            if 'ignore-real-sign' in flags:
                actual = complex(abs(actual.real), actual.imag)
                expected = complex(abs(expected.real), expected.imag)
            if 'ignore-imag-sign' in flags:
                actual = complex(actual.real, abs(actual.imag))
                expected = complex(expected.real, abs(expected.imag))

            # for the real part of the log function, we allow an
            # absolute error of up to 2e-15.
            if fn in ('log', 'log10'):
                real_abs_err = 2e-15
            else:
                real_abs_err = 5e-323

            error_message = (
                '{}: {}(complex({!r}, {!r}))\n'
                'Expected: complex({!r}, {!r})\n'
                'Received: complex({!r}, {!r})\n'
                'Received value insufficiently close to expected value.'
                ).format(id, fn, ar, ai,
                     expected.real, expected.imag,
                     actual.real, actual.imag)
            self.rAssertAlmostEqual(expected.real, actual.real,
                                        abs_err=real_abs_err,
                                        msg=error_message)
            self.rAssertAlmostEqual(expected.imag, actual.imag,
                                        msg=error_message)

    def assertCISEqual(self, a, b):
        eps = 1E-7
        if abs(a[0] - b[0]) > eps or abs(a[1] - b[1]) > eps:
            self.fail((a ,b))

    def test_polar(self):
        self.assertCISEqual(polar(0), (0., 0.))
        self.assertCISEqual(polar(1.), (1., 0.))
        self.assertCISEqual(polar(-1.), (1., pi))
        self.assertCISEqual(polar(1j), (1., pi/2))
        self.assertCISEqual(polar(-1j), (1., -pi/2))

    def test_phase(self):
        self.assertAlmostEqual(phase(0), 0.)
        self.assertAlmostEqual(phase(1.), 0.)
        self.assertAlmostEqual(phase(-1.), pi)
        self.assertAlmostEqual(phase(-1.+1E-300j), pi)
        self.assertAlmostEqual(phase(-1.-1E-300j), -pi)
        self.assertAlmostEqual(phase(1j), pi/2)
        self.assertAlmostEqual(phase(-1j), -pi/2)

        # zeros
        self.assertEqual(phase(complex(0.0, 0.0)), 0.0)
        self.assertEqual(phase(complex(0.0, -0.0)), -0.0)
        self.assertEqual(phase(complex(-0.0, 0.0)), pi)
        self.assertEqual(phase(complex(-0.0, -0.0)), -pi)

        # infinities
        self.assertAlmostEqual(phase(complex(-INF, -0.0)), -pi)
        self.assertAlmostEqual(phase(complex(-INF, -2.3)), -pi)
        self.assertAlmostEqual(phase(complex(-INF, -INF)), -0.75*pi)
        self.assertAlmostEqual(phase(complex(-2.3, -INF)), -pi/2)
        self.assertAlmostEqual(phase(complex(-0.0, -INF)), -pi/2)
        self.assertAlmostEqual(phase(complex(0.0, -INF)), -pi/2)
        self.assertAlmostEqual(phase(complex(2.3, -INF)), -pi/2)
        self.assertAlmostEqual(phase(complex(INF, -INF)), -pi/4)
        self.assertEqual(phase(complex(INF, -2.3)), -0.0)
        self.assertEqual(phase(complex(INF, -0.0)), -0.0)
        self.assertEqual(phase(complex(INF, 0.0)), 0.0)
        self.assertEqual(phase(complex(INF, 2.3)), 0.0)
        self.assertAlmostEqual(phase(complex(INF, INF)), pi/4)
        self.assertAlmostEqual(phase(complex(2.3, INF)), pi/2)
        self.assertAlmostEqual(phase(complex(0.0, INF)), pi/2)
        self.assertAlmostEqual(phase(complex(-0.0, INF)), pi/2)
        self.assertAlmostEqual(phase(complex(-2.3, INF)), pi/2)
        self.assertAlmostEqual(phase(complex(-INF, INF)), 0.75*pi)
        self.assertAlmostEqual(phase(complex(-INF, 2.3)), pi)
        self.assertAlmostEqual(phase(complex(-INF, 0.0)), pi)

        # real or imaginary part NaN
        for z in complex_nans:
            self.assertTrue(math.isnan(phase(z)))

    def test_abs(self):
        # zeros
        for z in complex_zeros:
            self.assertEqual(abs(z), 0.0)

        # infinities
        for z in complex_infinities:
            self.assertEqual(abs(z), INF)

        # real or imaginary part NaN
        self.assertEqual(abs(complex(NAN, -INF)), INF)
        self.assertTrue(math.isnan(abs(complex(NAN, -2.3))))
        self.assertTrue(math.isnan(abs(complex(NAN, -0.0))))
        self.assertTrue(math.isnan(abs(complex(NAN, 0.0))))
        self.assertTrue(math.isnan(abs(complex(NAN, 2.3))))
        self.assertEqual(abs(complex(NAN, INF)), INF)
        self.assertEqual(abs(complex(-INF, NAN)), INF)
        self.assertTrue(math.isnan(abs(complex(-2.3, NAN))))
        self.assertTrue(math.isnan(abs(complex(-0.0, NAN))))
        self.assertTrue(math.isnan(abs(complex(0.0, NAN))))
        self.assertTrue(math.isnan(abs(complex(2.3, NAN))))
        self.assertEqual(abs(complex(INF, NAN)), INF)
        self.assertTrue(math.isnan(abs(complex(NAN, NAN))))


    @requires_IEEE_754
    def test_abs_overflows(self):
        # result overflows
        self.assertRaises(OverflowError, abs, complex(1.4e308, 1.4e308))

    def assertCEqual(self, a, b):
        eps = 1E-7
        if abs(a.real - b[0]) > eps or abs(a.imag - b[1]) > eps:
            self.fail((a ,b))

    def test_rect(self):
        self.assertCEqual(rect(0, 0), (0, 0))
        self.assertCEqual(rect(1, 0), (1., 0))
        self.assertCEqual(rect(1, -pi), (-1., 0))
        self.assertCEqual(rect(1, pi/2), (0, 1.))
        self.assertCEqual(rect(1, -pi/2), (0, -1.))

    def test_isfinite(self):
        real_vals = [float('-inf'), -2.3, -0.0,
                     0.0, 2.3, float('inf'), float('nan')]
        for x in real_vals:
            for y in real_vals:
                z = complex(x, y)
                self.assertEqual(cmath.isfinite(z),
                                  math.isfinite(x) and math.isfinite(y))

    def test_isnan(self):
        self.assertFalse(cmath.isnan(1))
        self.assertFalse(cmath.isnan(1j))
        self.assertFalse(cmath.isnan(INF))
        self.assertTrue(cmath.isnan(NAN))
        self.assertTrue(cmath.isnan(complex(NAN, 0)))
        self.assertTrue(cmath.isnan(complex(0, NAN)))
        self.assertTrue(cmath.isnan(complex(NAN, NAN)))
        self.assertTrue(cmath.isnan(complex(NAN, INF)))
        self.assertTrue(cmath.isnan(complex(INF, NAN)))

    def test_isinf(self):
        self.assertFalse(cmath.isinf(1))
        self.assertFalse(cmath.isinf(1j))
        self.assertFalse(cmath.isinf(NAN))
        self.assertTrue(cmath.isinf(INF))
        self.assertTrue(cmath.isinf(complex(INF, 0)))
        self.assertTrue(cmath.isinf(complex(0, INF)))
        self.assertTrue(cmath.isinf(complex(INF, INF)))
        self.assertTrue(cmath.isinf(complex(NAN, INF)))
        self.assertTrue(cmath.isinf(complex(INF, NAN)))

    @requires_IEEE_754
    @unittest.skipIf(sysconfig.get_config_var('TANH_PRESERVES_ZERO_SIGN') == 0,
                     "system tanh() function doesn't copy the sign")
    def testTanhSign(self):
        for z in complex_zeros:
            self.assertComplexIdentical(cmath.tanh(z), z)

    # The algorithm used for atan and atanh makes use of the system
    # log1p function; If that system function doesn't respect the sign
    # of zero, then atan and atanh will also have difficulties with
    # the sign of complex zeros.
    @requires_IEEE_754
    def testAtanSign(self):
        for z in complex_zeros:
            self.assertComplexIdentical(cmath.atan(z), z)

    @requires_IEEE_754
    def testAtanhSign(self):
        for z in complex_zeros:
            self.assertComplexIdentical(cmath.atanh(z), z)


def test_main():
    run_unittest(CMathTests)

if __name__ == "__main__":
    test_main()
