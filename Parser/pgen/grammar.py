import collections


class Grammar:
    """Pgen parsing tables class.

    The instance variables are as follows:

    symbol2number -- a dict mapping symbol names to numbers.  Symbol
                     numbers are always 256 or higher, to distinguish
                     them from token numbers, which are between 0 and
                     255 (inclusive).

    number2symbol -- a dict mapping numbers to symbol names;
                     these two are each other's inverse.

    states        -- a list of DFAs, where each DFA is a list of
                     states, each state is a list of arcs, and each
                     arc is a (i, j) pair where i is a label and j is
                     a state number.  The DFA number is the index into
                     this list.  (This name is slightly confusing.)
                     Final states are represented by a special arc of
                     the form (0, j) where j is its own state number.

    dfas          -- a dict mapping symbol numbers to (DFA, first)
                     pairs, where DFA is an item from the states list
                     above, and first is a set of tokens that can
                     begin this grammar rule.

    labels        -- a list of (x, y) pairs where x is either a token
                     number or a symbol number, and y is either None
                     or a string; the strings are keywords.  The label
                     number is the index in this list; label numbers
                     are used to mark state transitions (arcs) in the
                     DFAs.

    start         -- the number of the grammar's start symbol.

    keywords      -- a dict mapping keyword strings to arc labels.

    tokens        -- a dict mapping token numbers to arc labels.

    """

    def __init__(self):
        self.symbol2number = collections.OrderedDict()
        self.number2symbol = collections.OrderedDict()
        self.states = []
        self.dfas = collections.OrderedDict()
        self.labels = [(0, "EMPTY")]
        self.keywords = collections.OrderedDict()
        self.tokens = collections.OrderedDict()
        self.symbol2label = collections.OrderedDict()
        self.start = 256

    def produce_graminit_h(self, writer):
        writer("/* Generated by Parser/pgen */\n\n")
        for number, symbol in self.number2symbol.items():
            writer("#define {} {}\n".format(symbol, number))

    def produce_graminit_c(self, writer):
        writer("/* Generated by Parser/pgen */\n\n")

        writer('#include "grammar.h"\n')
        writer("grammar _PyParser_Grammar;\n")

        self.print_dfas(writer)
        self.print_labels(writer)

        writer("grammar _PyParser_Grammar = {\n")
        writer("    {n_dfas},\n".format(n_dfas=len(self.dfas)))
        writer("    dfas,\n")
        writer("    {{{n_labels}, labels}},\n".format(n_labels=len(self.labels)))
        writer("    {start_number}\n".format(start_number=self.start))
        writer("};\n")

    def print_labels(self, writer):
        writer(
            "static const label labels[{n_labels}] = {{\n".format(n_labels=len(self.labels))
        )
        for label, name in self.labels:
            label_name = '"{}"'.format(name) if name is not None else 0
            writer(
                '    {{{label}, {label_name}}},\n'.format(
                    label=label, label_name=label_name
                )
            )
        writer("};\n")

    def print_dfas(self, writer):
        self.print_states(writer)
        writer("static const dfa dfas[{}] = {{\n".format(len(self.dfas)))
        for dfaindex, dfa_elem in enumerate(self.dfas.items()):
            symbol, (dfa, first_sets) = dfa_elem
            writer(
                '    {{{dfa_symbol}, "{symbol_name}", '.format(
                    dfa_symbol=symbol, symbol_name=self.number2symbol[symbol]
                )
                + "{n_states}, states_{dfa_index},\n".format(
                    n_states=len(dfa), dfa_index=dfaindex
                )
                + '     "'
            )

            bitset = bytearray((len(self.labels) >> 3) + 1)
            for token in first_sets:
                bitset[token >> 3] |= 1 << (token & 7)
            for byte in bitset:
                writer("\\%03o" % (byte & 0xFF))
            writer('"},\n')
        writer("};\n")

    def print_states(self, write):
        for dfaindex, dfa in enumerate(self.states):
            self.print_arcs(write, dfaindex, dfa)
            write(
                "static state states_{dfa_index}[{n_states}] = {{\n".format(
                    dfa_index=dfaindex, n_states=len(dfa)
                )
            )
            for stateindex, state in enumerate(dfa):
                narcs = len(state)
                write(
                    "    {{{n_arcs}, arcs_{dfa_index}_{state_index}}},\n".format(
                        n_arcs=narcs, dfa_index=dfaindex, state_index=stateindex
                    )
                )
            write("};\n")

    def print_arcs(self, write, dfaindex, states):
        for stateindex, state in enumerate(states):
            narcs = len(state)
            write(
                "static const arc arcs_{dfa_index}_{state_index}[{n_arcs}] = {{\n".format(
                    dfa_index=dfaindex, state_index=stateindex, n_arcs=narcs
                )
            )
            for a, b in state:
                write(
                    "    {{{from_label}, {to_state}}},\n".format(
                        from_label=a, to_state=b
                    )
                )
            write("};\n")
