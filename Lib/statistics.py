"""
Basic statistics module.

This module provides functions for calculating statistics of data, including
averages, variance, and standard deviation.

Calculating averages
--------------------

==================  =============================================
Function            Description
==================  =============================================
mean                Arithmetic mean (average) of data.
harmonic_mean       Harmonic mean of data.
median              Median (middle value) of data.
median_low          Low median of data.
median_high         High median of data.
median_grouped      Median, or 50th percentile, of grouped data.
mode                Mode (most common value) of data.
==================  =============================================

Calculate the arithmetic mean ("the average") of data:

>>> mean([-1.0, 2.5, 3.25, 5.75])
2.625


Calculate the standard median of discrete data:

>>> median([2, 3, 4, 5])
3.5


Calculate the median, or 50th percentile, of data grouped into class intervals
centred on the data values provided. E.g. if your data points are rounded to
the nearest whole number:

>>> median_grouped([2, 2, 3, 3, 3, 4])  #doctest: +ELLIPSIS
2.8333333333...

This should be interpreted in this way: you have two data points in the class
interval 1.5-2.5, three data points in the class interval 2.5-3.5, and one in
the class interval 3.5-4.5. The median of these data points is 2.8333...


Calculating variability or spread
---------------------------------

==================  =============================================
Function            Description
==================  =============================================
pvariance           Population variance of data.
variance            Sample variance of data.
pstdev              Population standard deviation of data.
stdev               Sample standard deviation of data.
==================  =============================================

Calculate the standard deviation of sample data:

>>> stdev([2.5, 3.25, 5.5, 11.25, 11.75])  #doctest: +ELLIPSIS
4.38961843444...

If you have previously calculated the mean, you can pass it as the optional
second argument to the four "spread" functions to avoid recalculating it:

>>> data = [1, 2, 2, 4, 4, 4, 5, 6]
>>> mu = mean(data)
>>> pvariance(data, mu)
2.5


Exceptions
----------

A single exception is defined: StatisticsError is a subclass of ValueError.

"""

__all__ = [ 'StatisticsError',
            'pstdev', 'pvariance', 'stdev', 'variance',
            'median',  'median_low', 'median_high', 'median_grouped',
            'mean', 'mode', 'harmonic_mean',
          ]

import collections
import decimal
import math
import numbers

from fractions import Fraction
from decimal import Decimal
from itertools import groupby, chain
from bisect import bisect_left, bisect_right



# === Exceptions ===

class StatisticsError(ValueError):
    pass


# === Private utilities ===

def _sum(data, start=0):
    """_sum(data [, start]) -> (type, sum, count)

    Return a high-precision sum of the given numeric data as a fraction,
    together with the type to be converted to and the count of items.

    If optional argument ``start`` is given, it is added to the total.
    If ``data`` is empty, ``start`` (defaulting to 0) is returned.


    Examples
    --------

    >>> _sum([3, 2.25, 4.5, -0.5, 1.0], 0.75)
    (<class 'float'>, Fraction(11, 1), 5)

    Some sources of round-off error will be avoided:

    # Built-in sum returns zero.
    >>> _sum([1e50, 1, -1e50] * 1000)
    (<class 'float'>, Fraction(1000, 1), 3000)

    Fractions and Decimals are also supported:

    >>> from fractions import Fraction as F
    >>> _sum([F(2, 3), F(7, 5), F(1, 4), F(5, 6)])
    (<class 'fractions.Fraction'>, Fraction(63, 20), 4)

    >>> from decimal import Decimal as D
    >>> data = [D("0.1375"), D("0.2108"), D("0.3061"), D("0.0419")]
    >>> _sum(data)
    (<class 'decimal.Decimal'>, Fraction(6963, 10000), 4)

    Mixed types are currently treated as an error, except that int is
    allowed.
    """
    count = 0
    n, d = _exact_ratio(start)
    partials = {d: n}
    partials_get = partials.get
    T = _coerce(int, type(start))
    for typ, values in groupby(data, type):
        T = _coerce(T, typ)  # or raise TypeError
        for n,d in map(_exact_ratio, values):
            count += 1
            partials[d] = partials_get(d, 0) + n
    if None in partials:
        # The sum will be a NAN or INF. We can ignore all the finite
        # partials, and just look at this special one.
        total = partials[None]
        assert not _isfinite(total)
    else:
        # Sum all the partial sums using builtin sum.
        # FIXME is this faster if we sum them in order of the denominator?
        total = sum(Fraction(n, d) for d, n in sorted(partials.items()))
    return (T, total, count)


def _isfinite(x):
    try:
        return x.is_finite()  # Likely a Decimal.
    except AttributeError:
        return math.isfinite(x)  # Coerces to float first.


def _coerce(T, S):
    """Coerce types T and S to a common type, or raise TypeError.

    Coercion rules are currently an implementation detail. See the CoerceTest
    test class in test_statistics for details.
    """
    # See http://bugs.python.org/issue24068.
    assert T is not bool, "initial type T is bool"
    # If the types are the same, no need to coerce anything. Put this
    # first, so that the usual case (no coercion needed) happens as soon
    # as possible.
    if T is S:  return T
    # Mixed int & other coerce to the other type.
    if S is int or S is bool:  return T
    if T is int:  return S
    # If one is a (strict) subclass of the other, coerce to the subclass.
    if issubclass(S, T):  return S
    if issubclass(T, S):  return T
    # Ints coerce to the other type.
    if issubclass(T, int):  return S
    if issubclass(S, int):  return T
    # Mixed fraction & float coerces to float (or float subclass).
    if issubclass(T, Fraction) and issubclass(S, float):
        return S
    if issubclass(T, float) and issubclass(S, Fraction):
        return T
    # Any other combination is disallowed.
    msg = "don't know how to coerce %s and %s"
    raise TypeError(msg % (T.__name__, S.__name__))


def _exact_ratio(x):
    """Return Real number x to exact (numerator, denominator) pair.

    >>> _exact_ratio(0.25)
    (1, 4)

    x is expected to be an int, Fraction, Decimal or float.
    """
    try:
        # Optimise the common case of floats. We expect that the most often
        # used numeric type will be builtin floats, so try to make this as
        # fast as possible.
        if type(x) is float or type(x) is Decimal:
            return x.as_integer_ratio()
        try:
            # x may be an int, Fraction, or Integral ABC.
            return (x.numerator, x.denominator)
        except AttributeError:
            try:
                # x may be a float or Decimal subclass.
                return x.as_integer_ratio()
            except AttributeError:
                # Just give up?
                pass
    except (OverflowError, ValueError):
        # float NAN or INF.
        assert not _isfinite(x)
        return (x, None)
    msg = "can't convert type '{}' to numerator/denominator"
    raise TypeError(msg.format(type(x).__name__))


def _convert(value, T):
    """Convert value to given numeric type T."""
    if type(value) is T:
        # This covers the cases where T is Fraction, or where value is
        # a NAN or INF (Decimal or float).
        return value
    if issubclass(T, int) and value.denominator != 1:
        T = float
    try:
        # FIXME: what do we do if this overflows?
        return T(value)
    except TypeError:
        if issubclass(T, Decimal):
            return T(value.numerator)/T(value.denominator)
        else:
            raise


def _counts(data):
    # Generate a table of sorted (value, frequency) pairs.
    table = collections.Counter(iter(data)).most_common()
    if not table:
        return table
    # Extract the values with the highest frequency.
    maxfreq = table[0][1]
    for i in range(1, len(table)):
        if table[i][1] != maxfreq:
            table = table[:i]
            break
    return table


def _find_lteq(a, x):
    'Locate the leftmost value exactly equal to x'
    i = bisect_left(a, x)
    if i != len(a) and a[i] == x:
        return i
    raise ValueError


def _find_rteq(a, l, x):
    'Locate the rightmost value exactly equal to x'
    i = bisect_right(a, x, lo=l)
    if i != (len(a)+1) and a[i-1] == x:
        return i-1
    raise ValueError


def _fail_neg(values, errmsg='negative value'):
    """Iterate over values, failing if any are less than zero."""
    for x in values:
        if x < 0:
            raise StatisticsError(errmsg)
        yield x


class _nroot_NS:
    """Hands off! Don't touch!

    Everything inside this namespace (class) is an even-more-private
    implementation detail of the private _nth_root function.
    """
    # This class exists only to be used as a namespace, for convenience
    # of being able to keep the related functions together, and to
    # collapse the group in an editor. If this were C# or C++, I would
    # use a Namespace, but the closest Python has is a class.
    #
    # FIXME possibly move this out into a separate module?
    # That feels like overkill, and may encourage people to treat it as
    # a public feature.
    def __init__(self):
        raise TypeError('namespace only, do not instantiate')

    def nth_root(x, n):
        """Return the positive nth root of numeric x.

        This may be more accurate than ** or pow():

        >>> math.pow(1000, 1.0/3)  #doctest:+SKIP
        9.999999999999998

        >>> _nth_root(1000, 3)
        10.0
        >>> _nth_root(11**5, 5)
        11.0
        >>> _nth_root(2, 12)
        1.0594630943592953

        """
        if not isinstance(n, int):
            raise TypeError('degree n must be an int')
        if n < 2:
            raise ValueError('degree n must be 2 or more')
        if isinstance(x, decimal.Decimal):
            return _nroot_NS.decimal_nroot(x, n)
        elif isinstance(x, numbers.Real):
            return _nroot_NS.float_nroot(x, n)
        else:
            raise TypeError('expected a number, got %s') % type(x).__name__

    def float_nroot(x, n):
        """Handle nth root of Reals, treated as a float."""
        assert isinstance(n, int) and n > 1
        if x < 0:
            if n%2 == 0:
                raise ValueError('domain error: even root of negative number')
            else:
                return -_nroot_NS.nroot(-x, n)
        elif x == 0:
            return math.copysign(0.0, x)
        elif x > 0:
            try:
                isinfinity = math.isinf(x)
            except OverflowError:
                return _nroot_NS.bignum_nroot(x, n)
            else:
                if isinfinity:
                    return float('inf')
                else:
                    return _nroot_NS.nroot(x, n)
        else:
            assert math.isnan(x)
            return float('nan')

    def nroot(x, n):
        """Calculate x**(1/n), then improve the answer."""
        # This uses math.pow() to calculate an initial guess for the root,
        # then uses the iterated nroot algorithm to improve it.
        #
        # By my testing, about 8% of the time the iterated algorithm ends
        # up converging to a result which is less accurate than the initial
        # guess. [FIXME: is this still true?] In that case, we use the
        # guess instead of the "improved" value. This way, we're never
        # less accurate than math.pow().
        r1 = math.pow(x, 1.0/n)
        eps1 = abs(r1**n - x)
        if eps1 == 0.0:
            # r1 is the exact root, so we're done. By my testing, this
            # occurs about 80% of the time for x < 1 and 30% of the
            # time for x > 1.
            return r1
        else:
            try:
                r2 = _nroot_NS.iterated_nroot(x, n, r1)
            except RuntimeError:
                return r1
            else:
                eps2 = abs(r2**n - x)
                if eps1 < eps2:
                    return r1
                return r2

    def iterated_nroot(a, n, g):
        """Return the nth root of a, starting with guess g.

        This is a special case of Newton's Method.
        https://en.wikipedia.org/wiki/Nth_root_algorithm
        """
        np = n - 1
        def iterate(r):
            try:
                return (np*r + a/math.pow(r, np))/n
            except OverflowError:
                # If r is large enough, r**np may overflow. If that
                # happens, r**-np will be small, but not necessarily zero.
                return (np*r + a*math.pow(r, -np))/n
        # With a good guess, such as g = a**(1/n), this will converge in
        # only a few iterations. However a poor guess can take thousands
        # of iterations to converge, if at all. We guard against poor
        # guesses by setting an upper limit to the number of iterations.
        r1 = g
        r2 = iterate(g)
        for i in range(1000):
            if r1 == r2:
                break
            # Use Floyd's cycle-finding algorithm to avoid being trapped
            # in a cycle.
            # https://en.wikipedia.org/wiki/Cycle_detection#Tortoise_and_hare
            r1 = iterate(r1)
            r2 = iterate(iterate(r2))
        else:
            # If the guess is particularly bad, the above may fail to
            # converge in any reasonable time.
            raise RuntimeError('nth-root failed to converge')
        return r2

    def decimal_nroot(x, n):
        """Handle nth root of Decimals."""
        assert isinstance(x, decimal.Decimal)
        assert isinstance(n, int)
        if x.is_snan():
            # Signalling NANs always raise.
            raise decimal.InvalidOperation('nth-root of snan')
        if x.is_qnan():
            # Quiet NANs only raise if the context is set to raise,
            # otherwise return a NAN.
            ctx = decimal.getcontext()
            if ctx.traps[decimal.InvalidOperation]:
                raise decimal.InvalidOperation('nth-root of nan')
            else:
                # Preserve the input NAN.
                return x
        if x.is_infinite():
            return x
        # FIXME this hasn't had the extensive testing of the float
        # version _iterated_nroot so there's possibly some buggy
        # corner cases buried in here. Can it overflow? Fail to
        # converge or get trapped in a cycle? Converge to a less
        # accurate root?
        np = n - 1
        def iterate(r):
            return (np*r + x/r**np)/n
        r0 = x**(decimal.Decimal(1)/n)
        assert isinstance(r0, decimal.Decimal)
        r1 = iterate(r0)
        while True:
            if r1 == r0:
                return r1
            r0, r1 = r1, iterate(r1)

    def bignum_nroot(x, n):
        """Return the nth root of a positive huge number."""
        assert x > 0
        # I state without proof that ⁿ√x ≈ ⁿ√2·ⁿ√(x//2)
        # and that for sufficiently big x the error is acceptible.
        # We now halve x until it is small enough to get the root.
        m = 0
        while True:
            x //= 2
            m += 1
            try:
                y = float(x)
            except OverflowError:
                continue
            break
        a = _nroot_NS.nroot(y, n)
        # At this point, we want the nth-root of 2**m, or 2**(m/n).
        # We can write that as 2**(q + r/n) = 2**q * ⁿ√2**r where q = m//n.
        q, r = divmod(m, n)
        b = 2**q * _nroot_NS.nroot(2**r, n)
        return a * b


# This is the (private) function for calculating nth roots:
_nth_root = _nroot_NS.nth_root
assert type(_nth_root) is type(lambda: None)


def _product(values):
    """Return product of values as (exponent, mantissa)."""
    errmsg = 'mixed Decimal and float is not supported'
    prod = 1
    for x in values:
        if isinstance(x, float):
            break
        prod *= x
    else:
        return (0, prod)
    if isinstance(prod, Decimal):
        raise TypeError(errmsg)
    # Since floats can overflow easily, we calculate the product as a
    # sort of poor-man's BigFloat. Given that:
    #
    #   x = 2**p * m  # p == power or exponent (scale), m = mantissa
    #
    # we can calculate the product of two (or more) x values as:
    #
    #   x1*x2 = 2**p1*m1 * 2**p2*m2 = 2**(p1+p2)*(m1*m2)
    #
    mant, scale = 1, 0  #math.frexp(prod)  # FIXME
    for y in chain([x], values):
        if isinstance(y, Decimal):
            raise TypeError(errmsg)
        m1, e1 = math.frexp(y)
        m2, e2 = math.frexp(mant)
        scale += (e1 + e2)
        mant = m1*m2
    return (scale, mant)


# === Measures of central tendency (averages) ===

def mean(data):
    """Return the sample arithmetic mean of data.

    >>> mean([1, 2, 3, 4, 4])
    2.8

    >>> from fractions import Fraction as F
    >>> mean([F(3, 7), F(1, 21), F(5, 3), F(1, 3)])
    Fraction(13, 21)

    >>> from decimal import Decimal as D
    >>> mean([D("0.5"), D("0.75"), D("0.625"), D("0.375")])
    Decimal('0.5625')

    If ``data`` is empty, StatisticsError will be raised.
    """
    if iter(data) is data:
        data = list(data)
    n = len(data)
    if n < 1:
        raise StatisticsError('mean requires at least one data point')
    T, total, count = _sum(data)
    assert count == n
    return _convert(total/n, T)


def geometric_mean(data):
    """Return the geometric mean of data.

    The geometric mean is appropriate when averaging quantities which
    are multiplied together rather than added, for example growth rates.
    Suppose an investment grows by 10% in the first year, falls by 5% in
    the second, then grows by 12% in the third, what is the average rate
    of growth over the three years?

    >>> geometric_mean([1.10, 0.95, 1.12])
    1.0538483123382172

    giving an average growth of 5.385%. Using the arithmetic mean will
    give approximately 5.667%, which is too high.

    ``StatisticsError`` will be raised if ``data`` is empty, or any
    element is less than zero.
    """
    if iter(data) is data:
        data = list(data)
    errmsg = 'geometric mean does not support negative values'
    n = len(data)
    if n < 1:
        raise StatisticsError('geometric_mean requires at least one data point')
    elif n == 1:
        x = data[0]
        if isinstance(g, (numbers.Real, Decimal)):
            if x < 0:
                raise StatisticsError(errmsg)
            return x
        else:
            raise TypeError('unsupported type')
    else:
        scale, prod = _product(_fail_neg(data, errmsg))
        r = _nth_root(prod, n)
        if scale:
            p, q = divmod(scale, n)
            s = 2**p * _nth_root(2**q, n)
        else:
            s = 1
        return s*r


def harmonic_mean(data):
    """Return the harmonic mean of data.

    The harmonic mean, sometimes called the subcontrary mean, is the
    reciprocal of the arithmetic mean of the reciprocals of the data,
    and is often appropriate when averaging quantities which are rates
    or ratios, for example speeds. Example:

    Suppose an investor purchases an equal value of shares in each of
    three companies, with P/E (price/earning) ratios of 2.5, 3 and 10.
    What is the average P/E ratio for the investor's portfolio?

    >>> harmonic_mean([2.5, 3, 10])  # For an equal investment portfolio.
    3.6

    Using the arithmetic mean would give an average of about 5.167, which
    is too high.

    If ``data`` is empty, or any element is less than zero,
    ``harmonic_mean`` will raise ``StatisticsError``.
    """
    # For a justification for using harmonic mean for P/E ratios, see
    # http://fixthepitch.pellucid.com/comps-analysis-the-missing-harmony-of-summary-statistics/
    # http://papers.ssrn.com/sol3/papers.cfm?abstract_id=2621087
    if iter(data) is data:
        data = list(data)
    errmsg = 'harmonic mean does not support negative values'
    n = len(data)
    if n < 1:
        raise StatisticsError('harmonic_mean requires at least one data point')
    elif n == 1:
        x = data[0]
        if isinstance(x, (numbers.Real, Decimal)):
            if x < 0:
                raise StatisticsError(errmsg)
            return x
        else:
            raise TypeError('unsupported type')
    try:
        T, total, count = _sum(1/x for x in _fail_neg(data, errmsg))
    except ZeroDivisionError:
        return 0
    assert count == n
    return _convert(n/total, T)


# FIXME: investigate ways to calculate medians without sorting? Quickselect?
def median(data):
    """Return the median (middle value) of numeric data.

    When the number of data points is odd, return the middle data point.
    When the number of data points is even, the median is interpolated by
    taking the average of the two middle values:

    >>> median([1, 3, 5])
    3
    >>> median([1, 3, 5, 7])
    4.0

    """
    data = sorted(data)
    n = len(data)
    if n == 0:
        raise StatisticsError("no median for empty data")
    if n%2 == 1:
        return data[n//2]
    else:
        i = n//2
        return (data[i - 1] + data[i])/2


def median_low(data):
    """Return the low median of numeric data.

    When the number of data points is odd, the middle value is returned.
    When it is even, the smaller of the two middle values is returned.

    >>> median_low([1, 3, 5])
    3
    >>> median_low([1, 3, 5, 7])
    3

    """
    data = sorted(data)
    n = len(data)
    if n == 0:
        raise StatisticsError("no median for empty data")
    if n%2 == 1:
        return data[n//2]
    else:
        return data[n//2 - 1]


def median_high(data):
    """Return the high median of data.

    When the number of data points is odd, the middle value is returned.
    When it is even, the larger of the two middle values is returned.

    >>> median_high([1, 3, 5])
    3
    >>> median_high([1, 3, 5, 7])
    5

    """
    data = sorted(data)
    n = len(data)
    if n == 0:
        raise StatisticsError("no median for empty data")
    return data[n//2]


def median_grouped(data, interval=1):
    """Return the 50th percentile (median) of grouped continuous data.

    >>> median_grouped([1, 2, 2, 3, 4, 4, 4, 4, 4, 5])
    3.7
    >>> median_grouped([52, 52, 53, 54])
    52.5

    This calculates the median as the 50th percentile, and should be
    used when your data is continuous and grouped. In the above example,
    the values 1, 2, 3, etc. actually represent the midpoint of classes
    0.5-1.5, 1.5-2.5, 2.5-3.5, etc. The middle value falls somewhere in
    class 3.5-4.5, and interpolation is used to estimate it.

    Optional argument ``interval`` represents the class interval, and
    defaults to 1. Changing the class interval naturally will change the
    interpolated 50th percentile value:

    >>> median_grouped([1, 3, 3, 5, 7], interval=1)
    3.25
    >>> median_grouped([1, 3, 3, 5, 7], interval=2)
    3.5

    This function does not check whether the data points are at least
    ``interval`` apart.
    """
    data = sorted(data)
    n = len(data)
    if n == 0:
        raise StatisticsError("no median for empty data")
    elif n == 1:
        return data[0]
    # Find the value at the midpoint. Remember this corresponds to the
    # centre of the class interval.
    x = data[n//2]
    for obj in (x, interval):
        if isinstance(obj, (str, bytes)):
            raise TypeError('expected number but got %r' % obj)
    try:
        L = x - interval/2  # The lower limit of the median interval.
    except TypeError:
        # Mixed type. For now we just coerce to float.
        L = float(x) - float(interval)/2

    # Uses bisection search to search for x in data with log(n) time complexity
    # Find the position of leftmost occurrence of x in data
    l1 = _find_lteq(data, x)
    # Find the position of rightmost occurrence of x in data[l1...len(data)]
    # Assuming always l1 <= l2
    l2 = _find_rteq(data, l1, x)
    cf = l1
    f = l2 - l1 + 1
    return L + interval*(n/2 - cf)/f


def mode(data):
    """Return the most common data point from discrete or nominal data.

    ``mode`` assumes discrete data, and returns a single value. This is the
    standard treatment of the mode as commonly taught in schools:

    >>> mode([1, 1, 2, 3, 3, 3, 3, 4])
    3

    This also works with nominal (non-numeric) data:

    >>> mode(["red", "blue", "blue", "red", "green", "red", "red"])
    'red'

    If there is not exactly one most common value, ``mode`` will raise
    StatisticsError.
    """
    # Generate a table of sorted (value, frequency) pairs.
    table = _counts(data)
    if len(table) == 1:
        return table[0][0]
    elif table:
        raise StatisticsError(
                'no unique mode; found %d equally common values' % len(table)
                )
    else:
        raise StatisticsError('no mode for empty data')


# === Measures of spread ===

# See http://mathworld.wolfram.com/Variance.html
#     http://mathworld.wolfram.com/SampleVariance.html
#     http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
#
# Under no circumstances use the so-called "computational formula for
# variance", as that is only suitable for hand calculations with a small
# amount of low-precision data. It has terrible numeric properties.
#
# See a comparison of three computational methods here:
# http://www.johndcook.com/blog/2008/09/26/comparing-three-methods-of-computing-standard-deviation/

def _ss(data, c=None):
    """Return sum of square deviations of sequence data.

    If ``c`` is None, the mean is calculated in one pass, and the deviations
    from the mean are calculated in a second pass. Otherwise, deviations are
    calculated from ``c`` as given. Use the second case with care, as it can
    lead to garbage results.
    """
    if c is None:
        c = mean(data)
    T, total, count = _sum((x-c)**2 for x in data)
    # The following sum should mathematically equal zero, but due to rounding
    # error may not.
    U, total2, count2 = _sum((x-c) for x in data)
    assert T == U and count == count2
    total -=  total2**2/len(data)
    assert not total < 0, 'negative sum of square deviations: %f' % total
    return (T, total)


def variance(data, xbar=None):
    """Return the sample variance of data.

    data should be an iterable of Real-valued numbers, with at least two
    values. The optional argument xbar, if given, should be the mean of
    the data. If it is missing or None, the mean is automatically calculated.

    Use this function when your data is a sample from a population. To
    calculate the variance from the entire population, see ``pvariance``.

    Examples:

    >>> data = [2.75, 1.75, 1.25, 0.25, 0.5, 1.25, 3.5]
    >>> variance(data)
    1.3720238095238095

    If you have already calculated the mean of your data, you can pass it as
    the optional second argument ``xbar`` to avoid recalculating it:

    >>> m = mean(data)
    >>> variance(data, m)
    1.3720238095238095

    This function does not check that ``xbar`` is actually the mean of
    ``data``. Giving arbitrary values for ``xbar`` may lead to invalid or
    impossible results.

    Decimals and Fractions are supported:

    >>> from decimal import Decimal as D
    >>> variance([D("27.5"), D("30.25"), D("30.25"), D("34.5"), D("41.75")])
    Decimal('31.01875')

    >>> from fractions import Fraction as F
    >>> variance([F(1, 6), F(1, 2), F(5, 3)])
    Fraction(67, 108)

    """
    if iter(data) is data:
        data = list(data)
    n = len(data)
    if n < 2:
        raise StatisticsError('variance requires at least two data points')
    T, ss = _ss(data, xbar)
    return _convert(ss/(n-1), T)


def pvariance(data, mu=None):
    """Return the population variance of ``data``.

    data should be an iterable of Real-valued numbers, with at least one
    value. The optional argument mu, if given, should be the mean of
    the data. If it is missing or None, the mean is automatically calculated.

    Use this function to calculate the variance from the entire population.
    To estimate the variance from a sample, the ``variance`` function is
    usually a better choice.

    Examples:

    >>> data = [0.0, 0.25, 0.25, 1.25, 1.5, 1.75, 2.75, 3.25]
    >>> pvariance(data)
    1.25

    If you have already calculated the mean of the data, you can pass it as
    the optional second argument to avoid recalculating it:

    >>> mu = mean(data)
    >>> pvariance(data, mu)
    1.25

    This function does not check that ``mu`` is actually the mean of ``data``.
    Giving arbitrary values for ``mu`` may lead to invalid or impossible
    results.

    Decimals and Fractions are supported:

    >>> from decimal import Decimal as D
    >>> pvariance([D("27.5"), D("30.25"), D("30.25"), D("34.5"), D("41.75")])
    Decimal('24.815')

    >>> from fractions import Fraction as F
    >>> pvariance([F(1, 4), F(5, 4), F(1, 2)])
    Fraction(13, 72)

    """
    if iter(data) is data:
        data = list(data)
    n = len(data)
    if n < 1:
        raise StatisticsError('pvariance requires at least one data point')
    T, ss = _ss(data, mu)
    return _convert(ss/n, T)


def stdev(data, xbar=None):
    """Return the square root of the sample variance.

    See ``variance`` for arguments and other details.

    >>> stdev([1.5, 2.5, 2.5, 2.75, 3.25, 4.75])
    1.0810874155219827

    """
    var = variance(data, xbar)
    try:
        return var.sqrt()
    except AttributeError:
        return math.sqrt(var)


def pstdev(data, mu=None):
    """Return the square root of the population variance.

    See ``pvariance`` for arguments and other details.

    >>> pstdev([1.5, 2.5, 2.5, 2.75, 3.25, 4.75])
    0.986893273527251

    """
    var = pvariance(data, mu)
    try:
        return var.sqrt()
    except AttributeError:
        return math.sqrt(var)
