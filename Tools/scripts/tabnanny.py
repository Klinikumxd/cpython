#! /usr/bin/env python

"""The Tab Nanny despises ambiguous indentation.  She knows no mercy."""

# Released to the public domain, by Tim Peters, 15 April 1998.

__version__ = "5"

import os
import sys
import getopt
import tokenize

verbose = 0

def errprint(*args):
    sep = ""
    for arg in args:
        sys.stderr.write(sep + str(arg))
        sep = " "
    sys.stderr.write("\n")

def main():
    global verbose
    try:
        opts, args = getopt.getopt(sys.argv[1:], "v")
    except getopt.error, msg:
        errprint(msg)
        return
    for o, a in opts:
        if o == '-v':
            verbose = verbose + 1
    if not args:
        errprint("Usage:", sys.argv[0], "[-v] file_or_directory ...")
        return
    for arg in args:
        check(arg)

class NannyNag:
    def __init__(self, lineno, msg, line):
        self.lineno, self.msg, self.line = lineno, msg, line
    def get_lineno(self):
        return self.lineno
    def get_msg(self):
        return self.msg
    def get_line(self):
        return self.line

def check(file):
    if os.path.isdir(file) and not os.path.islink(file):
        if verbose:
            print "%s: listing directory" % `file`
        names = os.listdir(file)
        for name in names:
            fullname = os.path.join(file, name)
            if (os.path.isdir(fullname) and
                not os.path.islink(fullname) or
                os.path.normcase(name[-3:]) == ".py"):
                check(fullname)
        return

    try:
        f = open(file)
    except IOError, msg:
        errprint("%s: I/O Error: %s" % (`file`, str(msg)))
        return

    if verbose > 1:
        print "checking", `file`, "..."

    reset_globals()
    try:
        tokenize.tokenize(f.readline, tokeneater)

    except tokenize.TokenError, msg:
        errprint("%s: Token Error: %s" % (`file`, str(msg)))
        return

    except NannyNag, nag:
        badline = nag.get_lineno()
        line = nag.get_line()
        if verbose:
            print "%s: *** Line %d: trouble in tab city! ***" % (
                `file`, badline)
            print "offending line:", `line`
            print nag.get_msg()
        else:
            print file, badline, `line`
        return

    if verbose:
        print "%s: Clean bill of health." % `file`

class Whitespace:
    # the characters used for space and tab
    S, T = ' \t'

    # members:
    #   raw
    #       the original string
    #   n
    #       the number of leading whitespace characters in raw
    #   nt
    #       the number of tabs in raw[:n]
    #   norm
    #       the normal form as a pair (count, trailing), where:
    #       count
    #           a tuple such that raw[:n] contains count[i]
    #           instances of S * i + T
    #       trailing
    #           the number of trailing spaces in raw[:n]
    #       It's A Theorem that m.indent_level(t) ==
    #       n.indent_level(t) for all t >= 1 iff m.norm == n.norm.
    #   is_simple
    #       true iff raw[:n] is of the form (T*)(S*)

    def __init__(self, ws):
        self.raw  = ws
        S, T = Whitespace.S, Whitespace.T
        count = []
        b = n = nt = 0
        for ch in self.raw:
            if ch == S:
                n = n + 1
                b = b + 1
            elif ch == T:
                n = n + 1
                nt = nt + 1
                if b >= len(count):
                    count = count + [0] * (b - len(count) + 1)
                count[b] = count[b] + 1
                b = 0
            else:
                break
        self.n    = n
        self.nt   = nt
        self.norm = tuple(count), b
        self.is_simple = len(count) <= 1

    # return length of longest contiguous run of spaces (whether or not
    # preceding a tab)
    def longest_run_of_spaces(self):
        count, trailing = self.norm
        return max(len(count)-1, trailing)

    def indent_level(self, tabsize):
        # count, il = self.norm
        # for i in range(len(count)):
        #    if count[i]:
        #        il = il + (i/tabsize + 1)*tabsize * count[i]
        # return il

        # quicker:
        # il = trailing + sum (i/ts + 1)*ts*count[i] =
        # trailing + ts * sum (i/ts + 1)*count[i] =
        # trailing + ts * sum i/ts*count[i] + count[i] =
        # trailing + ts * [(sum i/ts*count[i]) + (sum count[i])] =
        # trailing + ts * [(sum i/ts*count[i]) + num_tabs]
        # and note that i/ts*count[i] is 0 when i < ts

        count, trailing = self.norm
        il = 0
        for i in range(tabsize, len(count)):
            il = il + i/tabsize * count[i]
        return trailing + tabsize * (il + self.nt)

    # return true iff self.indent_level(t) == other.indent_level(t)
    # for all t >= 1
    def equal(self, other):
        return self.norm == other.norm

    # return a list of tuples (ts, i1, i2) such that
    # i1 == self.indent_level(ts) != other.indent_level(ts) == i2.
    # Intended to be used after not self.equal(other) is known, in which
    # case it will return at least one witnessing tab size.
    def not_equal_witness(self, other):
        n = max(self.longest_run_of_spaces(),
                other.longest_run_of_spaces()) + 1
        a = []
        for ts in range(1, n+1):
            if self.indent_level(ts) != other.indent_level(ts):
                a.append( (ts,
                           self.indent_level(ts),
                           other.indent_level(ts)) )
        return a

    # Return true iff self.indent_level(t) < other.indent_level(t)
    # for all t >= 1.
    # The algorithm is due to Vincent Broman.
    # Easy to prove it's correct.
    # XXXpost that.
    # Trivial to prove n is sharp (consider T vs ST).
    # Unknown whether there's a faster general way.  I suspected so at
    # first, but no longer.
    # For the special (but common!) case where M and N are both of the
    # form (T*)(S*), M.less(N) iff M.len() < N.len() and
    # M.num_tabs() <= N.num_tabs(). Proof is easy but kinda long-winded.
    # XXXwrite that up.
    # Note that M is of the form (T*)(S*) iff len(M.norm[0]) <= 1.
    def less(self, other):
        if self.n >= other.n:
            return 0
        if self.is_simple and other.is_simple:
            return self.nt <= other.nt
        n = max(self.longest_run_of_spaces(),
                other.longest_run_of_spaces()) + 1
        # the self.n >= other.n test already did it for ts=1
        for ts in range(2, n+1):
            if self.indent_level(ts) >= other.indent_level(ts):
                return 0
        return 1

    # return a list of tuples (ts, i1, i2) such that
    # i1 == self.indent_level(ts) >= other.indent_level(ts) == i2.
    # Intended to be used after not self.less(other) is known, in which
    # case it will return at least one witnessing tab size.
    def not_less_witness(self, other):
        n = max(self.longest_run_of_spaces(),
                other.longest_run_of_spaces()) + 1
        a = []
        for ts in range(1, n+1):
            if self.indent_level(ts) >= other.indent_level(ts):
                a.append( (ts,
                           self.indent_level(ts),
                           other.indent_level(ts)) )
        return a

def format_witnesses(w):
    import string
    firsts = map(lambda tup: str(tup[0]), w)
    prefix = "at tab size"
    if len(w) > 1:
        prefix = prefix + "s"
    return prefix + " " + string.join(firsts, ', ')

# The collection of globals, the reset_globals() function, and the
# tokeneater() function, depend on which version of tokenize is
# in use.

if hasattr(tokenize, 'NL'):
 # take advantage of Guido's patch!

 indents = []
 check_equal = 0

 def reset_globals():
     global indents, check_equal
     check_equal = 0
     indents = [Whitespace("")]

 def tokeneater(type, token, start, end, line,
                INDENT=tokenize.INDENT,
                DEDENT=tokenize.DEDENT,
                NEWLINE=tokenize.NEWLINE,
                JUNK=(tokenize.COMMENT, tokenize.NL) ):
     global indents, check_equal

     if type == NEWLINE:
         # a program statement, or ENDMARKER, will eventually follow,
         # after some (possibly empty) run of tokens of the form
         #     (NL | COMMENT)* (INDENT | DEDENT+)?
         # If an INDENT appears, setting check_equal is wrong, and will
         # be undone when we see the INDENT.
         check_equal = 1

     elif type == INDENT:
         check_equal = 0
         thisguy = Whitespace(token)
         if not indents[-1].less(thisguy):
             witness = indents[-1].not_less_witness(thisguy)
             msg = "indent not greater e.g. " + format_witnesses(witness)
             raise NannyNag(start[0], msg, line)
         indents.append(thisguy)

     elif type == DEDENT:
         # there's nothing we need to check here!  what's important is
         # that when the run of DEDENTs ends, the indentation of the
         # program statement (or ENDMARKER) that triggered the run is
         # equal to what's left at the top of the indents stack

         # Ouch!  This assert triggers if the last line of the source
         # is indented *and* lacks a newline -- then DEDENTs pop out
         # of thin air.
         # assert check_equal  # else no earlier NEWLINE, or an earlier INDENT
         check_equal = 1

         del indents[-1]

     elif check_equal and type not in JUNK:
         # this is the first "real token" following a NEWLINE, so it
         # must be the first token of the next program statement, or an
         # ENDMARKER; the "line" argument exposes the leading whitespace
         # for this statement; in the case of ENDMARKER, line is an empty
         # string, so will properly match the empty string with which the
         # "indents" stack was seeded
         check_equal = 0
         thisguy = Whitespace(line)
         if not indents[-1].equal(thisguy):
             witness = indents[-1].not_equal_witness(thisguy)
             msg = "indent not equal e.g. " + format_witnesses(witness)
             raise NannyNag(start[0], msg, line)

else:
 # unpatched version of tokenize

 nesting_level = 0
 indents = []
 check_equal = 0

 def reset_globals():
     global nesting_level, indents, check_equal
     nesting_level = check_equal = 0
     indents = [Whitespace("")]

 def tokeneater(type, token, start, end, line,
                INDENT=tokenize.INDENT,
                DEDENT=tokenize.DEDENT,
                NEWLINE=tokenize.NEWLINE,
                COMMENT=tokenize.COMMENT,
                OP=tokenize.OP):
     global nesting_level, indents, check_equal

     if type == INDENT:
         check_equal = 0
         thisguy = Whitespace(token)
         if not indents[-1].less(thisguy):
             witness = indents[-1].not_less_witness(thisguy)
             msg = "indent not greater e.g. " + format_witnesses(witness)
             raise NannyNag(start[0], msg, line)
         indents.append(thisguy)

     elif type == DEDENT:
         del indents[-1]

     elif type == NEWLINE:
         if nesting_level == 0:
             check_equal = 1

     elif type == COMMENT:
         pass

     elif check_equal:
         check_equal = 0
         thisguy = Whitespace(line)
         if not indents[-1].equal(thisguy):
             witness = indents[-1].not_equal_witness(thisguy)
             msg = "indent not equal e.g. " + format_witnesses(witness)
             raise NannyNag(start[0], msg, line)

     if type == OP and token in ('{', '[', '('):
         nesting_level = nesting_level + 1

     elif type == OP and token in ('}', ']', ')'):
         if nesting_level == 0:
             raise NannyNag(start[0],
                            "unbalanced bracket '" + token + "'",
                            line)
         nesting_level = nesting_level - 1

if __name__ == '__main__':
    main()

