#!/usr/bin/env python
# -*- mode: python -*-
# $Id$

# Re test suite and benchmark suite v1.5a2

# The 3 possible outcomes for each pattern
[SUCCEED, FAIL, SYNTAX_ERROR] = range(3)

# Benchmark suite (needs expansion)
#
# The benchmark suite does not test correctness, just speed.  The
# first element of each tuple is the regex pattern; the second is a 
# string to match it against.  The benchmarking code will embed the
# second string inside several sizes of padding, to test how regex 
# matching performs on large strings.

benchmarks = [
    ('Python', 'Python'),		# Simple text literal
    ('.*Python', 'Python'),		# Bad text literal
    ('.*Python.*', 'Python'),		# Worse text literal
    ('.*(Python)', 'Python'),		# Bad text literal with grouping

    ('(Python|Perl|Tcl', 'Perl'),	# Alternation
    ('(Python|Perl|Tcl)', 'Perl'),	# Grouped alternation
    ('(Python)\\1', 'PythonPython'),	# Backreference
    ('([0a-z][a-z]*,)+', 'a5,b7,c9,'),	# Disable the fastmap optimization
    ('([a-z][a-z0-9]*,)+', 'a5,b7,c9,') # A few sets
]

# Test suite (for verifying correctness)
#
# The test suite is a list of 5- or 3-tuples.  The 5 parts of a
# complete tuple are:
# element 0: a string containing the pattern
#         1: the string to match against the pattern
#         2: the expected result (SUCCEED, FAIL, SYNTAX_ERROR)
#         3: a string that will be eval()'ed to produce a test string.
#            This is an arbitrary Python expression; the available
#            variables are "found" (the whole match), and "g1", "g2", ...
#            up to "g10" contain the contents of each group, or the
#            string 'None' if the group wasn't given a value.
#         4: The expected result of evaluating the expression.
#            If the two don't match, an error is reported.
#
# If the regex isn't expected to work, the latter two elements can be omitted.

tests = [
    ('abc', 'abc', SUCCEED, 'found', 'abc'),
    ('abc', 'xbc', FAIL),
    ('abc', 'axc', FAIL),
    ('abc', 'abx', FAIL),
    ('abc', 'xabcy', SUCCEED, 'found', 'abc'),
    ('abc', 'ababc', SUCCEED, 'found', 'abc'),
    ('ab*c', 'abc', SUCCEED, 'found', 'abc'),
    ('ab*bc', 'abc', SUCCEED, 'found', 'abc'),
    ('ab*bc', 'abbc', SUCCEED, 'found', 'abbc'),
    ('ab*bc', 'abbbbc', SUCCEED, 'found', 'abbbbc'),
    ('ab+bc', 'abbc', SUCCEED, 'found', 'abbc'),
    ('ab+bc', 'abc', FAIL),
    ('ab+bc', 'abq', FAIL),
    ('ab+bc', 'abbbbc', SUCCEED, 'found', 'abbbbc'),
    ('ab?bc', 'abbc', SUCCEED, 'found', 'abbc'),
    ('ab?bc', 'abc', SUCCEED, 'found', 'abc'),
    ('ab?bc', 'abbbbc', FAIL),
    ('ab?c', 'abc', SUCCEED, 'found', 'abc'),
    ('^abc$', 'abc', SUCCEED, 'found', 'abc'),
    ('^abc$', 'abcc', FAIL),
    ('^abc', 'abcc', SUCCEED, 'found', 'abc'),
    ('^abc$', 'aabc', FAIL),
    ('abc$', 'aabc', SUCCEED, 'found', 'abc'),
    ('^', 'abc', SUCCEED, 'found+"-"', '-'),
    ('$', 'abc', SUCCEED, 'found+"-"', '-'),
    ('a.c', 'abc', SUCCEED, 'found', 'abc'),
    ('a.c', 'axc', SUCCEED, 'found', 'axc'),
    ('a.*c', 'axyzc', SUCCEED, 'found', 'axyzc'),
    ('a.*c', 'axyzd', FAIL),
    ('a[bc]d', 'abc', FAIL),
    ('a[bc]d', 'abd', SUCCEED, 'found', 'abd'),
    ('a[b-d]e', 'abd', FAIL),
    ('a[b-d]e', 'ace', SUCCEED, 'found', 'ace'),
    ('a[b-d]', 'aac', SUCCEED, 'found', 'ac'),

# In 're', the way to put a '-' in a set is to escape it: '[\-]'
#    ('a[-b]', 'a-', SUCCEED, 'found', 'a-'),

    ('a[-b]', 'a-', SYNTAX_ERROR),
    ('a[\\-b]', 'a-', SUCCEED, 'found', 'a-'),
    ('a[b-]', 'a-', SYNTAX_ERROR),
    ('a[]b', '-', SYNTAX_ERROR),
    ('a[', '-', SYNTAX_ERROR),
    ('a\\', '-', SYNTAX_ERROR),
    ('abc)', '-', SYNTAX_ERROR),
    ('(abc', '-', SYNTAX_ERROR),
    ('a]', 'a]', SUCCEED, 'found', 'a]'),
    
# In 're', the way to put a ']' in a set is to escape it: '[\]]'
#    ('a[]]b', 'a]b', SUCCEED, 'found', 'a]b'),
    
    ('a[\]]b', 'a]b', SUCCEED, 'found', 'a]b'),
    ('a[^bc]d', 'aed', SUCCEED, 'found', 'aed'),
    ('a[^bc]d', 'abd', FAIL),
    ('a[^\\-b]c', 'adc', SUCCEED, 'found', 'adc'),
    ('a[^\\-b]c', 'a-c', FAIL),
    ('a[^\\]b]c', 'a]c', FAIL),
    ('a[^\\]b]c', 'adc', SUCCEED, 'found', 'adc'),
    ('\\ba\\b', 'a-', SUCCEED, '"-"', '-'),
    ('\\ba\\b', '-a', SUCCEED, '"-"', '-'),
    ('\\ba\\b', '-a-', SUCCEED, '"-"', '-'),
    ('\\by\\b', 'xy', FAIL),
    ('\\by\\b', 'yz', FAIL),
    ('\\by\\b', 'xyz', FAIL),
    ('ab|cd', 'abc', SUCCEED, 'found', 'ab'),
    ('ab|cd', 'abcd', SUCCEED, 'found', 'ab'),
    ('()ef', 'def', SUCCEED, 'found+"-"+g1', 'ef-'),
    ('$b', 'b', FAIL),
    
# This is an error in 1.5
#    ('a(b', 'a(b', SUCCEED, 'found+"-"+g1', 'a(b-None'),
    
    ('a\\(*b', 'ab', SUCCEED, 'found', 'ab'),
    ('a\\(*b', 'a((b', SUCCEED, 'found', 'a((b'),
    ('a\\\\b', 'a\\b', SUCCEED, 'found', 'a\\b'),
    ('((a))', 'abc', SUCCEED, 'found+"-"+g1+"-"+g2', 'a-a-a'),
    ('(a)b(c)', 'abc', SUCCEED, 'found+"-"+g1+"-"+g2', 'abc-a-c'),
    ('a+b+c', 'aabbabc', SUCCEED, 'found', 'abc'),
    ('(a+|b)*', 'ab', SUCCEED, 'found+"-"+g1', 'ab-b'),
    ('(a+|b)+', 'ab', SUCCEED, 'found+"-"+g1', 'ab-b'),
    ('(a+|b)?', 'ab', SUCCEED, 'found+"-"+g1', 'a-a'),
    (')(', '-', SYNTAX_ERROR),
    ('[^ab]*', 'cde', SUCCEED, 'found', 'cde'),
    ('abc', '', FAIL),
    ('a*', '', SUCCEED, 'found', ''),
    ('a|b|c|d|e', 'e', SUCCEED, 'found', 'e'),
    ('(a|b|c|d|e)f', 'ef', SUCCEED, 'found+"-"+g1', 'ef-e'),
    ('abcd*efg', 'abcdefg', SUCCEED, 'found', 'abcdefg'),
    ('ab*', 'xabyabbbz', SUCCEED, 'found', 'ab'),
    ('ab*', 'xayabbbz', SUCCEED, 'found', 'a'),
    ('(ab|cd)e', 'abcde', SUCCEED, 'found+"-"+g1', 'cde-cd'),
    ('[abhgefdc]ij', 'hij', SUCCEED, 'found', 'hij'),
    ('^(ab|cd)e', 'abcde', FAIL,
     'xg1y', 'xy'),
    ('(abc|)ef', 'abcdef', SUCCEED, 'found+"-"+g1', 'ef-'),
    ('(a|b)c*d', 'abcd', SUCCEED, 'found+"-"+g1', 'bcd-b'),
    ('(ab|ab*)bc', 'abc', SUCCEED, 'found+"-"+g1', 'abc-a'),
    ('a([bc]*)c*', 'abc', SUCCEED, 'found+"-"+g1', 'abc-bc'),
    ('a([bc]*)(c*d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-bc-d'),
    ('a([bc]+)(c*d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-bc-d'),
    ('a([bc]*)(c+d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-b-cd'),
    ('a[bcd]*dcdcde', 'adcdcde', SUCCEED, 'found', 'adcdcde'),
    ('a[bcd]+dcdcde', 'adcdcde', FAIL),
    ('(ab|a)b*c', 'abc', SUCCEED, 'found+"-"+g1', 'abc-ab'),
    ('((a)(b)c)(d)', 'abcd', SUCCEED, 'g1+"-"+g2+"-"+g3+"-"+g4', 'abc-a-b-d'),
    ('[a-zA-Z_][a-zA-Z0-9_]*', 'alpha', SUCCEED, 'found', 'alpha'),
    ('^a(bc+|b[eh])g|.h$', 'abh', SUCCEED, 'found+"-"+g1', 'bh-None'),
    ('(bc+d$|ef*g.|h?i(j|k))', 'effgz', SUCCEED, 'found+"-"+g1+"-"+g2', 'effgz-effgz-None'),
    ('(bc+d$|ef*g.|h?i(j|k))', 'ij', SUCCEED, 'found+"-"+g1+"-"+g2', 'ij-ij-j'),
    ('(bc+d$|ef*g.|h?i(j|k))', 'effg', FAIL),
    ('(bc+d$|ef*g.|h?i(j|k))', 'bcdd', FAIL),
    ('(bc+d$|ef*g.|h?i(j|k))', 'reffgz', SUCCEED, 'found+"-"+g1+"-"+g2', 'effgz-effgz-None'),
    ('(((((((((a)))))))))', 'a', SUCCEED, 'found', 'a'),
    ('multiple words of text', 'uh-uh', FAIL),
    ('multiple words', 'multiple words, yeah', SUCCEED, 'found', 'multiple words'),
    ('(.*)c(.*)', 'abcde', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcde-ab-de'),
    ('\\((.*), (.*)\\)', '(a, b)', SUCCEED, 'g2+"-"+g1', 'b-a'),
    ('[k]', 'ab', FAIL),
    ('a[-]?c', 'ac', SUCCEED, 'found', 'ac'),
    ('(abc)\\1', 'abcabc', SUCCEED, 'g1', 'abc'),
    ('([a-c]*)\\1', 'abcabc', SUCCEED, 'g1', 'abc'),
    ('^(.+)?B', 'AB', SUCCEED, 'g1', 'A'),
    ('(a+).\\1$', 'aaaaa', SUCCEED, 'found+"-"+g1', 'aaaaa-aa'),
    ('^(a+).\\1$', 'aaaa', FAIL),
    ('(abc)\\1', 'abcabc', SUCCEED, 'found+"-"+g1', 'abcabc-abc'),
    ('([a-c]+)\\1', 'abcabc', SUCCEED, 'found+"-"+g1', 'abcabc-abc'),
    ('(a)\\1', 'aa', SUCCEED, 'found+"-"+g1', 'aa-a'),
    ('(a+)\\1', 'aa', SUCCEED, 'found+"-"+g1', 'aa-a'),
    ('(a+)+\\1', 'aa', SUCCEED, 'found+"-"+g1', 'aa-a'),
    ('(a).+\\1', 'aba', SUCCEED, 'found+"-"+g1', 'aba-a'),
    ('(a)ba*\\1', 'aba', SUCCEED, 'found+"-"+g1', 'aba-a'),
    ('(aa|a)a\\1$', 'aaa', SUCCEED, 'found+"-"+g1', 'aaa-a'),
    ('(a|aa)a\\1$', 'aaa', SUCCEED, 'found+"-"+g1', 'aaa-a'),
    ('(a+)a\\1$', 'aaa', SUCCEED, 'found+"-"+g1', 'aaa-a'),
    ('([abc]*)\\1', 'abcabc', SUCCEED, 'found+"-"+g1', 'abcabc-abc'),
    ('(a)(b)c|ab', 'ab', SUCCEED, 'found+"-"+g1+"-"+g2', 'ab-None-None'),
    ('(a)+x', 'aaax', SUCCEED, 'found+"-"+g1', 'aaax-a'),
    ('([ac])+x', 'aacx', SUCCEED, 'found+"-"+g1', 'aacx-c'),
    ('([^/]*/)*sub1/', 'd:msgs/tdir/sub1/trial/away.cpp', SUCCEED, 'found+"-"+g1', 'd:msgs/tdir/sub1/-tdir/'),
    ('([^.]*)\\.([^:]*):[T ]+(.*)', 'track1.title:TBlah blah blah', SUCCEED, 'found+"-"+g1+"-"+g2+"-"+g3', 'track1.title:TBlah blah blah-track1-title-Blah blah blah'),
    ('([^N]*N)+', 'abNNxyzN', SUCCEED, 'found+"-"+g1', 'abNNxyzN-xyzN'),
    ('([^N]*N)+', 'abNNxyz', SUCCEED, 'found+"-"+g1', 'abNN-N'),
    ('([abc]*)x', 'abcx', SUCCEED, 'found+"-"+g1', 'abcx-abc'),
    ('([abc]*)x', 'abc', FAIL),
    ('([xyz]*)x', 'abcx', SUCCEED, 'found+"-"+g1', 'x-'),
    ('(a)+b|aac', 'aac', SUCCEED, 'found+"-"+g1', 'aac-None'),

    # Test symbolic groups

    ('(?P<i d>aaa)a', 'aaaa', SYNTAX_ERROR),
    ('(?P<id>aaa)a', 'aaaa', SUCCEED, 'found+"-"+id', 'aaaa-aaa'),
    ('(?P<id>aa)(?P=id)', 'aaaa', SUCCEED, 'found+"-"+id', 'aaaa-aa'),
    ('(?P<id>aa)(?P=xd)', 'aaaa', SYNTAX_ERROR),

    # Test octal escapes/memory references

    ('\\1', 'a', SYNTAX_ERROR),
    ('\\09', chr(0) + '9', SUCCEED, 'found', chr(0) + '9'),
    ('\\141', 'a', SUCCEED, 'found', 'a'),
    ('(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)(k)(l)\\119', 'abcdefghijklk9', SUCCEED, 'found+"-"+g11', 'abcdefghijklk9-k')
    
    ]
