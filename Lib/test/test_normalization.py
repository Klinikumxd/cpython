from test.test_support import verbose, TestFailed, TestSkipped, verify
import sys
import os
from unicodedata import normalize

TESTDATAFILE = "NormalizationTest.txt"
skip_expected = not os.path.exists(TESTDATAFILE)

class RangeError:
    pass

def NFC(str):
    return normalize("NFC", str)

def NFKC(str):
    return normalize("NFKC", str)

def NFD(str):
    return normalize("NFD", str)

def NFKD(str):
    return normalize("NFKD", str)

def unistr(data):
    data = [int(x, 16) for x in data.split(" ")]
    for x in data:
        if x > sys.maxunicode:
            raise RangeError
    return u"".join([unichr(x) for x in data])

def test_main():
    if skip_expected:
        raise TestSkipped(TESTDATAFILE + " not found, download from " +
                    "http://www.unicode.org/Public/UNIDATA/" + TESTDATAFILE)

    data = open(TESTDATAFILE).readlines()

    part1_data = {}
    for line in data:
        if '#' in line:
            line = line.split('#')[0]
        line = line.strip()
        if not line:
            continue
        if line.startswith("@Part"):
            part = line
            continue
        try:
            c1,c2,c3,c4,c5 = [unistr(x) for x in line.split(';')[:-1]]
        except RangeError:
            # Skip unsupported characters
            continue

        if verbose:
            print line

        # Perform tests
        verify(c2 ==  NFC(c1) ==  NFC(c2) ==  NFC(c3), line)
        verify(c4 ==  NFC(c4) ==  NFC(c5), line)
        verify(c3 ==  NFD(c1) ==  NFD(c2) ==  NFD(c3), line)
        verify(c5 ==  NFD(c4) ==  NFD(c5), line)
        verify(c4 == NFKC(c1) == NFKC(c2) == NFKC(c3) == NFKC(c4) == NFKC(c5),
               line)
        verify(c5 == NFKD(c1) == NFKD(c2) == NFKD(c3) == NFKD(c4) == NFKD(c5),
               line)

        # Record part 1 data
        if part == "@Part1":
            part1_data[c1] = 1

    # Perform tests for all other data
    for c in range(sys.maxunicode+1):
        X = unichr(c)
        if X in part1_data:
            continue
        assert X == NFC(X) == NFD(X) == NFKC(X) == NFKD(X), c

if __name__ == "__main__":
    test_main()
