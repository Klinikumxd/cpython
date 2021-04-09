#! /usr/bin/env python3

"""
This script should be called *manually* when we want to upgrade SSLError
`library` and `reason` mnemonics to a more recent OpenSSL version.

It takes two arguments:
- the path to the OpenSSL source tree (e.g. git checkout)
- the path to the header file to be generated Modules/_ssl_data_{version}.h
- error codes are version specific
"""

import argparse
import datetime
import operator
import os
import re
import sys


parser = argparse.ArgumentParser(
    description="Generate ssl_data.h from OpenSSL sources"
)
parser.add_argument("srcdir", help="OpenSSL source directory")
parser.add_argument(
    "output", nargs="?", type=argparse.FileType("w"), default=sys.stdout
)


def _file_search(fname, pat):
    with open(fname, encoding="utf-8") as f:
        for line in f:
            match = pat.search(line)
            if match is not None:
                yield match


def parse_err_h(args):
    """Parse err codes, e.g. ERR_LIB_X509: 11"""
    pat = re.compile(r"#\s*define\W+ERR_LIB_(\w+)\s+(\d+)")
    lib2errnum = {}
    for match in _file_search(args.err_h, pat):
        libname, num = match.groups()
        lib2errnum[libname] = int(num)

    return lib2errnum


def parse_openssl_error_text(args):
    """Parse error reasons, X509_R_AKID_MISMATCH"""
    # ignore backslash line continuation for now
    pat = re.compile(r"^((\w+?)_R_(\w+)):(\d+):")
    for match in _file_search(args.errtxt, pat):
        reason, libname, errname, num = match.groups()
        if "_F_" in reason:
            # ignore function codes
            continue
        num = int(num)
        yield reason, libname, errname, num


def parse_extra_reasons(args):
    """Parse extra reasons from openssl.ec"""
    pat = re.compile(r"^R\s+((\w+)_R_(\w+))\s+(\d+)")
    for match in _file_search(args.errcodes, pat):
        reason, libname, errname, num = match.groups()
        num = int(num)
        yield reason, libname, errname, num


def gen_library_codes(args):
    """Generate table short libname to numeric code"""
    yield "static struct py_ssl_library_code library_codes[] = {"
    for libname in sorted(args.lib2errnum):
        yield f"#ifdef ERR_LIB_{libname}"
        yield f'    {{"{libname}", ERR_LIB_{libname}}},'
        yield "#endif"
    yield "    { NULL }"
    yield "};"
    yield ""


def gen_error_codes(args):
    """Generate error code table for error reasons"""
    yield "static struct py_ssl_error_code error_codes[] = {"
    for reason, libname, errname, num in args.reasons:
        yield f"  #ifdef {reason}"
        yield f'    {{"{errname}", ERR_LIB_{libname}, {reason}}},'
        yield "  #else"
        yield f'    {{"{errname}", {args.lib2errnum[libname]}, {num}}},'
        yield "  #endif"

    yield "    { NULL }"
    yield "};"
    yield ""


def main():
    args = parser.parse_args()

    args.err_h = os.path.join(args.srcdir, "include", "openssl", "err.h")
    if not os.path.isfile(args.err_h):
        # Fall back to infile for OpenSSL 3.0.0
        args.err_h += ".in"
    args.errcodes = os.path.join(args.srcdir, "crypto", "err", "openssl.ec")
    args.errtxt = os.path.join(args.srcdir, "crypto", "err", "openssl.txt")

    if not os.path.isfile(args.errtxt):
        parser.error(f"File {args.errtxt} not found in srcdir\n.")

    # {X509: 11, ...}
    args.lib2errnum = parse_err_h(args)

    # [('X509_R_AKID_MISMATCH', 'X509', 'AKID_MISMATCH', 110), ...]
    reasons = []
    reasons.extend(parse_openssl_error_text(args))
    reasons.extend(parse_extra_reasons(args))
    # sort by libname, numeric error code
    args.reasons = sorted(reasons, key=operator.itemgetter(0, 3))

    lines = [
        "/* File generated by Tools/ssl/make_ssl_data.py */"
        f"/* Generated on {datetime.datetime.utcnow().isoformat()} */"
    ]
    lines.extend(gen_library_codes(args))
    lines.append("")
    lines.extend(gen_error_codes(args))

    for line in lines:
        args.output.write(line + "\n")


if __name__ == "__main__":
    main()
