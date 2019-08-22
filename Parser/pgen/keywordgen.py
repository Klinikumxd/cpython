"""Generate Lib/keyword.py from the Grammar and Tokens files using pgen"""

import argparse

from .pgen import ParserGenerator

TEMPLATE = r'''
"""Keywords (from "Grammar/Grammar")

This file is automatically generated; please don't muck it up!

To update the symbols in this file, 'cd' to the top directory of
the python source tree and run:

    python3 -m Parser.pgen.keywordgen Grammar/Grammar \
                                      Grammar/Tokens \
                                      Lib/keyword.py

Alternatively, you can run 'make regen-keyword'.
"""

__all__ = ["iskeyword", "kwlist"]

kwlist = [
    {keywords}
]

iskeyword = frozenset(kwlist).__contains__
'''.lstrip()

EXTRA_KEYWORDS = ["async", "await"]


def main():
    parser = argparse.ArgumentParser(
        description="Generate the Lib/keywords.py " "file from the grammar."
    )
    parser.add_argument(
        "grammar", type=str, help="The file with the grammar definition in EBNF format"
    )
    parser.add_argument("tokens", type=str, help="The file with the token definitions")
    parser.add_argument(
        "keyword_file",
        type=argparse.FileType("w"),
        help="The path to write the keyword definitions",
    )
    args = parser.parse_args()
    p = ParserGenerator(args.grammar, args.tokens)
    grammar = p.make_grammar()

    with args.keyword_file as thefile:
        all_keywords = sorted(list(grammar.keywords) + EXTRA_KEYWORDS)

        keywords = ",\n    ".join(map(repr, all_keywords))
        thefile.write(TEMPLATE.format(keywords=keywords))


if __name__ == "__main__":
    main()
