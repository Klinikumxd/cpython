#
# Secret Labs' Regular Expression Engine
# $Id$
#
# various symbols used by the regular expression engine.
# run this script to update the _sre include files!
#
# Copyright (c) 1998-2000 by Secret Labs AB.  All rights reserved.
#
# This code can only be used for 1.6 alpha testing.  All other use
# require explicit permission from Secret Labs AB.
#
# Portions of this engine have been developed in cooperation with
# CNRI.  Hewlett-Packard provided funding for 1.6 integration and
# other compatibility work.
#

# operators

FAILURE = "failure"
SUCCESS = "success"

ANY = "any"
ASSERT = "assert"
AT = "at"
BRANCH = "branch"
CALL = "call"
CATEGORY = "category"
GROUP = "group"
GROUP_IGNORE = "group_ignore"
IN = "in"
IN_IGNORE = "in_ignore"
JUMP = "jump"
LITERAL = "literal"
LITERAL_IGNORE = "literal_ignore"
MARK = "mark"
MAX_REPEAT = "max_repeat"
MAX_REPEAT_ONE = "max_repeat_one"
MAX_UNTIL = "max_until"
MIN_REPEAT = "min_repeat"
MIN_UNTIL = "min_until"
NEGATE = "negate"
NOT_LITERAL = "not_literal"
NOT_LITERAL_IGNORE = "not_literal_ignore"
RANGE = "range"
REPEAT = "repeat"
SUBPATTERN = "subpattern"

# positions
AT_BEGINNING = "at_beginning"
AT_BOUNDARY = "at_boundary"
AT_NON_BOUNDARY = "at_non_boundary"
AT_END = "at_end"

# categories

CATEGORY_DIGIT = "category_digit"
CATEGORY_NOT_DIGIT = "category_not_digit"
CATEGORY_SPACE = "category_space"
CATEGORY_NOT_SPACE = "category_not_space"
CATEGORY_WORD = "category_word"
CATEGORY_NOT_WORD = "category_not_word"

CODES = [

    # failure=0 success=1 (just because it looks better that way :-)
    FAILURE, SUCCESS,

    ANY,
    ASSERT,
    AT,
    BRANCH,
    CALL,
    CATEGORY,
    GROUP, GROUP_IGNORE,
    IN, IN_IGNORE,
    JUMP,
    LITERAL, LITERAL_IGNORE,
    MARK,
    MAX_REPEAT, MAX_UNTIL,
    MAX_REPEAT_ONE,
    MIN_REPEAT, MIN_UNTIL,
    NOT_LITERAL, NOT_LITERAL_IGNORE,
    NEGATE,
    RANGE,
    REPEAT

]

# convert to dictionary
c = {}
i = 0
for code in CODES:
    c[code] = i
    i = i + 1
CODES = c

# replacement operations for "ignore case" mode
MAP_IGNORE = {
    GROUP: GROUP_IGNORE,
    IN: IN_IGNORE,
    LITERAL: LITERAL_IGNORE,
    NOT_LITERAL: NOT_LITERAL_IGNORE
}

POSITIONS = {
    AT_BEGINNING: ord("a"),
    AT_BOUNDARY: ord("b"),
    AT_NON_BOUNDARY: ord("B"),
    AT_END: ord("z"),
}

CATEGORIES = {
    CATEGORY_DIGIT: ord("d"),
    CATEGORY_NOT_DIGIT: ord("D"),
    CATEGORY_SPACE: ord("s"),
    CATEGORY_NOT_SPACE: ord("S"),
    CATEGORY_WORD: ord("w"),
    CATEGORY_NOT_WORD: ord("W"),
}

if __name__ == "__main__":
    import string
    items = CODES.items()
    items.sort(lambda a, b: cmp(a[1], b[1]))
    f = open("sre_constants.h", "w")
    f.write("/* generated by sre_constants.py */\n")
    for k, v in items:
        f.write("#define SRE_OP_" + string.upper(k) + " " + str(v) + "\n")
    f.close()
    print "done"
