#! /usr/bin/env python3

"""RFC 3548: Base16, Base32, Base64 Data Encodings"""

# Modified 04-Oct-1995 by Jack Jansen to use binascii module
# Modified 30-Dec-2003 by Barry Warsaw to add full RFC 3548 support
# Modified 22-May-2007 by Guido van Rossum to use bytes everywhere

import re
import struct
import binascii


__all__ = [
    # Legacy interface exports traditional RFC 1521 Base64 encodings
    'encode', 'decode', 'encodebytes', 'decodebytes',
    # Generalized interface for other encodings
    'b64encode', 'b64decode', 'b32encode', 'b32decode',
    'b16encode', 'b16decode',
    # Standard Base64 encoding
    'standard_b64encode', 'standard_b64decode',
    # Some common Base64 alternatives.  As referenced by RFC 3458, see thread
    # starting at:
    #
    # http://zgp.org/pipermail/p2p-hackers/2001-September/000316.html
    'urlsafe_b64encode', 'urlsafe_b64decode',
    ]


bytes_types = (bytes, bytearray)  # Types acceptable as binary data

def _bytes_from_decode_data(s):
    if isinstance(s, str):
        try:
            return s.encode('ascii')
        except UnicodeEncodeError:
            raise ValueError('string argument should contain only ASCII characters')
    if isinstance(s, bytes_types):
        return s
    try:
        return memoryview(s).tobytes()
    except TypeError:
        raise TypeError("argument should be a bytes-like object or ASCII "
                        "string, not %r" % s.__class__.__name__) from None


# Base64 encoding/decoding uses binascii

def b64encode(s, altchars=None):
    """Encode a byte string using Base64.

    s is the byte string to encode.  Optional altchars must be a byte
    string of length 2 which specifies an alternative alphabet for the
    '+' and '/' characters.  This allows an application to
    e.g. generate url or filesystem safe Base64 strings.

    The encoded byte string is returned.
    """
    # Strip off the trailing newline
    encoded = binascii.b2a_base64(s)[:-1]
    if altchars is not None:
        assert len(altchars) == 2, repr(altchars)
        return encoded.translate(bytes.maketrans(b'+/', altchars))
    return encoded


def b64decode(s, altchars=None, validate=False):
    """Decode a Base64 encoded byte string.

    s is the byte string to decode.  Optional altchars must be a
    string of length 2 which specifies the alternative alphabet used
    instead of the '+' and '/' characters.

    The decoded string is returned.  A binascii.Error is raised if s is
    incorrectly padded.

    If validate is False (the default), non-base64-alphabet characters are
    discarded prior to the padding check.  If validate is True,
    non-base64-alphabet characters in the input result in a binascii.Error.
    """
    s = _bytes_from_decode_data(s)
    if altchars is not None:
        altchars = _bytes_from_decode_data(altchars)
        assert len(altchars) == 2, repr(altchars)
        s = s.translate(bytes.maketrans(altchars, b'+/'))
    if validate and not re.match(b'^[A-Za-z0-9+/]*={0,2}$', s):
        raise binascii.Error('Non-base64 digit found')
    return binascii.a2b_base64(s)


def standard_b64encode(s):
    """Encode a byte string using the standard Base64 alphabet.

    s is the byte string to encode.  The encoded byte string is returned.
    """
    return b64encode(s)

def standard_b64decode(s):
    """Decode a byte string encoded with the standard Base64 alphabet.

    s is the byte string to decode.  The decoded byte string is
    returned.  binascii.Error is raised if the input is incorrectly
    padded or if there are non-alphabet characters present in the
    input.
    """
    return b64decode(s)


_urlsafe_encode_translation = bytes.maketrans(b'+/', b'-_')
_urlsafe_decode_translation = bytes.maketrans(b'-_', b'+/')

def urlsafe_b64encode(s):
    """Encode a byte string using a url-safe Base64 alphabet.

    s is the byte string to encode.  The encoded byte string is
    returned.  The alphabet uses '-' instead of '+' and '_' instead of
    '/'.
    """
    return b64encode(s).translate(_urlsafe_encode_translation)

def urlsafe_b64decode(s):
    """Decode a byte string encoded with the standard Base64 alphabet.

    s is the byte string to decode.  The decoded byte string is
    returned.  binascii.Error is raised if the input is incorrectly
    padded or if there are non-alphabet characters present in the
    input.

    The alphabet uses '-' instead of '+' and '_' instead of '/'.
    """
    s = _bytes_from_decode_data(s)
    s = s.translate(_urlsafe_decode_translation)
    return b64decode(s)



# Base32 encoding/decoding must be done in Python
_b32alphabet = b'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567'
_b32tab = [bytes([i]) for i in _b32alphabet]
_b32tab2 = [a + b for a in _b32tab for b in _b32tab]
_b32rev = {v: k for k, v in enumerate(_b32alphabet)}

def b32encode(s):
    """Encode a byte string using Base32.

    s is the byte string to encode.  The encoded byte string is returned.
    """
    if not isinstance(s, bytes_types):
        s = memoryview(s).tobytes()
    leftover = len(s) % 5
    # Pad the last quantum with zero bits if necessary
    if leftover:
        s = s + bytes(5 - leftover)  # Don't use += !
    encoded = bytearray()
    from_bytes = int.from_bytes
    b32tab2 = _b32tab2
    for i in range(0, len(s), 5):
        c = from_bytes(s[i: i + 5], 'big')
        encoded += (b32tab2[c >> 30] +           # bits 1 - 10
                    b32tab2[(c >> 20) & 0x3ff] + # bits 11 - 20
                    b32tab2[(c >> 10) & 0x3ff] + # bits 21 - 30
                    b32tab2[c & 0x3ff]           # bits 31 - 40
                   )
    # Adjust for any leftover partial quanta
    if leftover == 1:
        encoded[-6:] = b'======'
    elif leftover == 2:
        encoded[-4:] = b'===='
    elif leftover == 3:
        encoded[-3:] = b'==='
    elif leftover == 4:
        encoded[-1:] = b'='
    return bytes(encoded)

def b32decode(s, casefold=False, map01=None):
    """Decode a Base32 encoded byte string.

    s is the byte string to decode.  Optional casefold is a flag
    specifying whether a lowercase alphabet is acceptable as input.
    For security purposes, the default is False.

    RFC 3548 allows for optional mapping of the digit 0 (zero) to the
    letter O (oh), and for optional mapping of the digit 1 (one) to
    either the letter I (eye) or letter L (el).  The optional argument
    map01 when not None, specifies which letter the digit 1 should be
    mapped to (when map01 is not None, the digit 0 is always mapped to
    the letter O).  For security purposes the default is None, so that
    0 and 1 are not allowed in the input.

    The decoded byte string is returned.  binascii.Error is raised if
    the input is incorrectly padded or if there are non-alphabet
    characters present in the input.
    """
    s = _bytes_from_decode_data(s)
    if len(s) % 8:
        raise binascii.Error('Incorrect padding')
    # Handle section 2.4 zero and one mapping.  The flag map01 will be either
    # False, or the character to map the digit 1 (one) to.  It should be
    # either L (el) or I (eye).
    if map01 is not None:
        map01 = _bytes_from_decode_data(map01)
        assert len(map01) == 1, repr(map01)
        s = s.translate(bytes.maketrans(b'01', b'O' + map01))
    if casefold:
        s = s.upper()
    # Strip off pad characters from the right.  We need to count the pad
    # characters because this will tell us how many null bytes to remove from
    # the end of the decoded string.
    l = len(s)
    s = s.rstrip(b'=')
    padchars = l - len(s)
    # Now decode the full quanta
    decoded = bytearray()
    b32rev = _b32rev
    for i in range(0, len(s), 8):
        quanta = s[i: i + 8]
        acc = 0
        try:
            for c in quanta:
                acc = (acc << 5) + b32rev[c]
        except KeyError:
            raise binascii.Error('Non-base32 digit found') from None
        decoded += acc.to_bytes(5, 'big')
    # Process the last, partial quanta
    if padchars:
        acc <<= 5 * padchars
        last = acc.to_bytes(5, 'big')
        if padchars == 1:
            decoded[-5:] = last[:-1]
        elif padchars == 3:
            decoded[-5:] = last[:-2]
        elif padchars == 4:
            decoded[-5:] = last[:-3]
        elif padchars == 6:
            decoded[-5:] = last[:-4]
        else:
            raise binascii.Error('Incorrect padding')
    return bytes(decoded)



# RFC 3548, Base 16 Alphabet specifies uppercase, but hexlify() returns
# lowercase.  The RFC also recommends against accepting input case
# insensitively.
def b16encode(s):
    """Encode a byte string using Base16.

    s is the byte string to encode.  The encoded byte string is returned.
    """
    return binascii.hexlify(s).upper()


def b16decode(s, casefold=False):
    """Decode a Base16 encoded byte string.

    s is the byte string to decode.  Optional casefold is a flag
    specifying whether a lowercase alphabet is acceptable as input.
    For security purposes, the default is False.

    The decoded byte string is returned.  binascii.Error is raised if
    s were incorrectly padded or if there are non-alphabet characters
    present in the string.
    """
    s = _bytes_from_decode_data(s)
    if casefold:
        s = s.upper()
    if re.search(b'[^0-9A-F]', s):
        raise binascii.Error('Non-base16 digit found')
    return binascii.unhexlify(s)



# Legacy interface.  This code could be cleaned up since I don't believe
# binascii has any line length limitations.  It just doesn't seem worth it
# though.  The files should be opened in binary mode.

MAXLINESIZE = 76 # Excluding the CRLF
MAXBINSIZE = (MAXLINESIZE//4)*3

def encode(input, output):
    """Encode a file; input and output are binary files."""
    while True:
        s = input.read(MAXBINSIZE)
        if not s:
            break
        while len(s) < MAXBINSIZE:
            ns = input.read(MAXBINSIZE-len(s))
            if not ns:
                break
            s += ns
        line = binascii.b2a_base64(s)
        output.write(line)


def decode(input, output):
    """Decode a file; input and output are binary files."""
    while True:
        line = input.readline()
        if not line:
            break
        s = binascii.a2b_base64(line)
        output.write(s)

def _input_type_check(s):
    try:
        m = memoryview(s)
    except TypeError as err:
        msg = "expected bytes-like object, not %s" % s.__class__.__name__
        raise TypeError(msg) from err
    if m.format not in ('c', 'b', 'B'):
        msg = ("expected single byte elements, not %r from %s" %
                                          (m.format, s.__class__.__name__))
        raise TypeError(msg)
    if m.ndim != 1:
        msg = ("expected 1-D data, not %d-D data from %s" %
                                          (m.ndim, s.__class__.__name__))
        raise TypeError(msg)


def encodebytes(s):
    """Encode a bytestring into a bytestring containing multiple lines
    of base-64 data."""
    _input_type_check(s)
    pieces = []
    for i in range(0, len(s), MAXBINSIZE):
        chunk = s[i : i + MAXBINSIZE]
        pieces.append(binascii.b2a_base64(chunk))
    return b"".join(pieces)

def encodestring(s):
    """Legacy alias of encodebytes()."""
    import warnings
    warnings.warn("encodestring() is a deprecated alias, use encodebytes()",
                  DeprecationWarning, 2)
    return encodebytes(s)


def decodebytes(s):
    """Decode a bytestring of base-64 data into a bytestring."""
    _input_type_check(s)
    return binascii.a2b_base64(s)

def decodestring(s):
    """Legacy alias of decodebytes()."""
    import warnings
    warnings.warn("decodestring() is a deprecated alias, use decodebytes()",
                  DeprecationWarning, 2)
    return decodebytes(s)


# Usable as a script...
def main():
    """Small main program"""
    import sys, getopt
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'deut')
    except getopt.error as msg:
        sys.stdout = sys.stderr
        print(msg)
        print("""usage: %s [-d|-e|-u|-t] [file|-]
        -d, -u: decode
        -e: encode (default)
        -t: encode and decode string 'Aladdin:open sesame'"""%sys.argv[0])
        sys.exit(2)
    func = encode
    for o, a in opts:
        if o == '-e': func = encode
        if o == '-d': func = decode
        if o == '-u': func = decode
        if o == '-t': test(); return
    if args and args[0] != '-':
        with open(args[0], 'rb') as f:
            func(f, sys.stdout.buffer)
    else:
        func(sys.stdin.buffer, sys.stdout.buffer)


def test():
    s0 = b"Aladdin:open sesame"
    print(repr(s0))
    s1 = encodebytes(s0)
    print(repr(s1))
    s2 = decodebytes(s1)
    print(repr(s2))
    assert s0 == s2


if __name__ == '__main__':
    main()
