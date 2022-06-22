#! /usr/bin/env python3
# This script generates Lib/re/_casefix.py.

import collections
import sys
import unicodedata

def update_file(file, content):
    try:
        with open(file, 'r', encoding='utf-8') as fobj:
            if fobj.read() == content:
                return False
    except (OSError, ValueError):
        pass
    with open(file, 'w', encoding='utf-8') as fobj:
        fobj.write(content)
    return True

re_casefix_template = """\
# Auto-generated by Tools/scripts/generate_re_casefix.py.

# Maps the code of lowercased character to codes of different lowercased
# characters which have the same uppercase.
_EXTRA_CASES = {
%s
}
"""

def uname(i):
    return unicodedata.name(chr(i), r'U+%04X' % i)

class hexint(int):
    def __repr__(self):
        return '%#06x' % self

def alpha(i):
    c = chr(i)
    return c if c.isalpha() else ascii(c)[1:-1]


def main(outfile='Lib/re/_casefix.py'):
    # Find sets of characters which have the same uppercase.
    equivalent_chars = collections.defaultdict(str)
    for c in map(chr, range(sys.maxunicode + 1)):
        equivalent_chars[c.upper()] += c
    equivalent_chars = [t for t in equivalent_chars.values() if len(t) > 1]

    # List of codes of lowercased characters which have the same uppercase.
    equivalent_lower_codes = [sorted(t)
                              for s in equivalent_chars
                              for t in [set(ord(c.lower()) for c in s)]
                              if len(t) > 1]

    bad_codes = []
    for t in equivalent_lower_codes:
        for i in t:
            if i > 0xffff:
                bad_codes.extend(t)
                try:
                    bad_codes.append(ord(chr(i).upper()))
                except (ValueError, TypeError):
                    pass
                break
    if bad_codes:
        print('Case-insensitive matching may not work correctly for character:',
              file=sys.stderr)
        for i in sorted(bad_codes):
            print("  '%s' (U+%04x, %s)" % (alpha(i), i, uname(i)),
                  file=sys.stderr)
        sys.exit(1)

    mapping = {i: tuple(j for j in t if i != j)
               for t in equivalent_lower_codes
               for i in t}

    items = []
    for i, t in sorted(mapping.items()):
        items.append('    # %s: %s' % (
            uname(i),
            ', '.join(map(uname, t)),
        ))
        items.append("    %r: %r, # '%s': '%s'" % (
            hexint(i),
            tuple(map(hexint, t)),
            alpha(i),
            ''.join(map(alpha, t)),
        ))

    update_file(outfile, re_casefix_template % '\n'.join(items))


if __name__ == '__main__':
    import sys
    main(*sys.argv[1:])
