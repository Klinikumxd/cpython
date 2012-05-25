"""Header value parser implementing various email-related RFC parsing rules.

The parsing methods defined in this module implement various email related
parsing rules.  Principal among them is RFC 5322, which is the followon
to RFC 2822 and primarily a clarification of the former.  It also implements
RFC 2047 encoded word decoding.

RFC 5322 goes to considerable trouble to maintain backward compatibility with
RFC 822 in the parse phase, while cleaning up the structure on the generation
phase.  This parser supports correct RFC 5322 generation by tagging white space
as folding white space only when folding is allowed in the non-obsolete rule
sets.  Actually, the parser is even more generous when accepting input than RFC
5322 mandates, following the spirit of Postel's Law, which RFC 5322 encourages.
Where possible deviations from the standard are annotated on the 'defects'
attribute of tokens that deviate.

The general structure of the parser follows RFC 5322, and uses its terminology
where there is a direct correspondence.  Where the implementation requires a
somewhat different structure than that used by the formal grammar, new terms
that mimic the closest existing terms are used.  Thus, it really helps to have
a copy of RFC 5322 handy when studying this code.

Input to the parser is a string that has already been unfolded according to
RFC 5322 rules.  According to the RFC this unfolding is the very first step, and
this parser leaves the unfolding step to a higher level message parser, which
will have already detected the line breaks that need unfolding while
determining the beginning and end of each header.

The output of the parser is a TokenList object, which is a list subclass.  A
TokenList is a recursive data structure.  The terminal nodes of the structure
are Terminal objects, which are subclasses of str.  These do not correspond
directly to terminal objects in the formal grammar, but are instead more
practical higher level combinations of true terminals.

All TokenList and Terminal objects have a 'value' attribute, which produces the
semantically meaningful value of that part of the parse subtree.  The value of
all whitespace tokens (no matter how many sub-tokens they may contain) is a
single space, as per the RFC rules.  This includes 'CFWS', which is herein
included in the general class of whitespace tokens.  There is one exception to
the rule that whitespace tokens are collapsed into single spaces in values: in
the value of a 'bare-quoted-string' (a quoted-string with no leading or
trailing whitespace), any whitespace that appeared between the quotation marks
is preserved in the returned value.  Note that in all Terminal strings quoted
pairs are turned into their unquoted values.

All TokenList and Terminal objects also have a string value, which attempts to
be a "canonical" representation of the RFC-compliant form of the substring that
produced the parsed subtree, including minimal use of quoted pair quoting.
Whitespace runs are not collapsed.

Comment tokens also have a 'content' attribute providing the string found
between the parens (including any nested comments) with whitespace preserved.

All TokenList and Terminal objects have a 'defects' attribute which is a
possibly empty list all of the defects found while creating the token.  Defects
may appear on any token in the tree, and a composite list of all defects in the
subtree is available through the 'all_defects' attribute of any node.  (For
Terminal notes x.defects == x.all_defects.)

Each object in a parse tree is called a 'token', and each has a 'token_type'
attribute that gives the name from the RFC 5322 grammar that it represents.
Not all RFC 5322 nodes are produced, and there is one non-RFC 5322 node that
may be produced: 'ptext'.  A 'ptext' is a string of printable ascii characters.
It is returned in place of lists of (ctext/quoted-pair) and
(qtext/quoted-pair).

XXX: provide complete list of token types.
"""

import re
from email import _encoded_words as _ew
from email import errors
from email import utils

#
# Useful constants and functions
#

WSP = set(' \t')
CFWS_LEADER = WSP | set('(')
SPECIALS = set(r'()<>@,:;.\"[]')
ATOM_ENDS = SPECIALS | WSP
DOT_ATOM_ENDS = ATOM_ENDS - set('.')
# '.', '"', and '(' do not end phrases in order to support obs-phrase
PHRASE_ENDS = SPECIALS - set('."(')

def quote_string(value):
    return '"'+str(value).replace('\\', '\\\\').replace('"', r'\"')+'"'

#
# Accumulator for header folding
#

class _Folded:

    def __init__(self, maxlen, policy):
        self.maxlen = maxlen
        self.policy = policy
        self.lastlen = 0
        self.stickyspace = None
        self.firstline = True
        self.done = []
        self.current = []

    def newline(self):
        self.done.extend(self.current)
        self.done.append(self.policy.linesep)
        self.current.clear()
        self.lastlen = 0

    def finalize(self):
        if self.current:
            self.newline()

    def __str__(self):
        return ''.join(self.done)

    def append(self, stoken):
        self.current.append(stoken)

    def append_if_fits(self, token, stoken=None):
        if stoken is None:
            stoken = str(token)
        l = len(stoken)
        if self.stickyspace is not None:
            stickyspace_len = len(self.stickyspace)
            if self.lastlen + stickyspace_len + l <= self.maxlen:
                self.current.append(self.stickyspace)
                self.lastlen += stickyspace_len
                self.current.append(stoken)
                self.lastlen += l
                self.stickyspace = None
                self.firstline = False
                return True
            if token.has_fws:
                ws = token.pop_leading_fws()
                if ws is not None:
                    self.stickyspace += str(ws)
                    stickyspace_len += len(ws)
                token._fold(self)
                return True
            if stickyspace_len and l + 1 <= self.maxlen:
                margin = self.maxlen - l
                if 0 < margin < stickyspace_len:
                    trim = stickyspace_len - margin
                    self.current.append(self.stickyspace[:trim])
                    self.stickyspace = self.stickyspace[trim:]
                    stickyspace_len = trim
                self.newline()
                self.current.append(self.stickyspace)
                self.current.append(stoken)
                self.lastlen = l + stickyspace_len
                self.stickyspace = None
                self.firstline = False
                return True
            if not self.firstline:
                self.newline()
            self.current.append(self.stickyspace)
            self.current.append(stoken)
            self.stickyspace = None
            self.firstline = False
            return True
        if self.lastlen + l <= self.maxlen:
            self.current.append(stoken)
            self.lastlen += l
            return True
        if l < self.maxlen:
            self.newline()
            self.current.append(stoken)
            self.lastlen = l
            return True
        return False

#
# TokenList and its subclasses
#

class TokenList(list):

    token_type = None

    def __init__(self, *args, **kw):
        super().__init__(*args, **kw)
        self.defects = []

    def __str__(self):
        return ''.join(str(x) for x in self)

    def __repr__(self):
        return '{}({})'.format(self.__class__.__name__,
                             super().__repr__())

    @property
    def value(self):
        return ''.join(x.value for x in self if x.value)

    @property
    def all_defects(self):
        return sum((x.all_defects for x in self), self.defects)

    #
    # Folding API
    #
    # parts():
    #
    # return a list of objects that constitute the "higher level syntactic
    # objects" specified by the RFC as the best places to fold a header line.
    # The returned objects must include leading folding white space, even if
    # this means mutating the underlying parse tree of the object.  Each object
    # is only responsible for returning *its* parts, and should not drill down
    # to any lower level except as required to meet the leading folding white
    # space constraint.
    #
    # _fold(folded):
    #
    #   folded: the result accumulator.  This is an instance of _Folded.
    #       (XXX: I haven't finished factoring this out yet, the folding code
    #       pretty much uses this as a state object.) When the folded.current
    #       contains as much text as will fit, the _fold method should call
    #       folded.newline.
    #  folded.lastlen: the current length of the test stored in folded.current.
    #  folded.maxlen: The maximum number of characters that may appear on a
    #       folded line.  Differs from the policy setting in that "no limit" is
    #       represented by +inf, which means it can be used in the trivially
    #       logical fashion in comparisons.
    #
    # Currently no subclasses implement parts, and I think this will remain
    # true.  A subclass only needs to implement _fold when the generic version
    # isn't sufficient.  _fold will need to be implemented primarily when it is
    # possible for encoded words to appear in the specialized token-list, since
    # there is no generic algorithm that can know where exactly the encoded
    # words are allowed.  A _fold implementation is responsible for filling
    # lines in the same general way that the top level _fold does. It may, and
    # should, call the _fold method of sub-objects in a similar fashion to that
    # of the top level _fold.
    #
    # XXX: I'm hoping it will be possible to factor the existing code further
    # to reduce redundancy and make the logic clearer.

    @property
    def parts(self):
        klass = self.__class__
        this = []
        for token in self:
            if token.startswith_fws():
                if this:
                    yield this[0] if len(this)==1 else klass(this)
                    this.clear()
            end_ws = token.pop_trailing_ws()
            this.append(token)
            if end_ws:
                yield klass(this)
                this = [end_ws]
        if this:
            yield this[0] if len(this)==1 else klass(this)

    def startswith_fws(self):
        return self[0].startswith_fws()

    def pop_leading_fws(self):
        if self[0].token_type == 'fws':
            return self.pop(0)
        return self[0].pop_leading_fws()

    def pop_trailing_ws(self):
        if self[-1].token_type == 'cfws':
            return self.pop(-1)
        return self[-1].pop_trailing_ws()

    @property
    def has_fws(self):
        for part in self:
            if part.has_fws:
                return True
        return False

    def has_leading_comment(self):
        return self[0].has_leading_comment()

    @property
    def comments(self):
        comments = []
        for token in self:
            comments.extend(token.comments)
        return comments

    def fold(self, *, policy):
        # max_line_length 0/None means no limit, ie: infinitely long.
        maxlen = policy.max_line_length or float("+inf")
        folded = _Folded(maxlen, policy)
        self._fold(folded)
        folded.finalize()
        return str(folded)

    def as_encoded_word(self, charset):
        # This works only for things returned by 'parts', which include
        # the leading fws, if any, that should be used.
        res = []
        ws = self.pop_leading_fws()
        if ws:
            res.append(ws)
        trailer = self.pop(-1) if self[-1].token_type=='fws' else ''
        res.append(_ew.encode(str(self), charset))
        res.append(trailer)
        return ''.join(res)

    def cte_encode(self, charset, policy):
        res = []
        for part in self:
            res.append(part.cte_encode(charset, policy))
        return ''.join(res)

    def _fold(self, folded):
        for part in self.parts:
            tstr = str(part)
            tlen = len(tstr)
            try:
                str(part).encode('us-ascii')
            except UnicodeEncodeError:
                if any(isinstance(x, errors.UndecodableBytesDefect)
                        for x in part.all_defects):
                    charset = 'unknown-8bit'
                else:
                    # XXX: this should be a policy setting
                    charset = 'utf-8'
                tstr = part.cte_encode(charset, folded.policy)
                tlen = len(tstr)
            if folded.append_if_fits(part, tstr):
                continue
            # Peel off the leading whitespace if any and make it sticky, to
            # avoid infinite recursion.
            ws = part.pop_leading_fws()
            if ws is not None:
                # Peel off the leading whitespace and make it sticky, to
                # avoid infinite recursion.
                folded.stickyspace = str(part.pop(0))
                if folded.append_if_fits(part):
                    continue
            if part.has_fws:
                part._fold(folded)
                continue
            # There are no fold points in this one; it is too long for a single
            # line and can't be split...we just have to put it on its own line.
            folded.append(tstr)
            folded.newline()

    def pprint(self, indent=''):
        print('\n'.join(self._pp(indent='')))

    def ppstr(self, indent=''):
        return '\n'.join(self._pp(indent=''))

    def _pp(self, indent=''):
        yield '{}{}/{}('.format(
            indent,
            self.__class__.__name__,
            self.token_type)
        for token in self:
            for line in token._pp(indent+'    '):
                yield line
        if self.defects:
            extra = ' Defects: {}'.format(self.defects)
        else:
            extra = ''
        yield '{}){}'.format(indent, extra)


class WhiteSpaceTokenList(TokenList):

    @property
    def value(self):
        return ' '

    @property
    def comments(self):
        return [x.content for x in self if x.token_type=='comment']


class UnstructuredTokenList(TokenList):

    token_type = 'unstructured'

    def _fold(self, folded):
        if any(x.token_type=='encoded-word' for x in self):
            return self._fold_encoded(folded)
        # Here we can have either a pure ASCII string that may or may not
        # have surrogateescape encoded bytes, or a unicode string.
        last_ew = None
        for part in self.parts:
            tstr = str(part)
            is_ew = False
            try:
                str(part).encode('us-ascii')
            except UnicodeEncodeError:
                if any(isinstance(x, errors.UndecodableBytesDefect)
                       for x in part.all_defects):
                    charset = 'unknown-8bit'
                else:
                    charset = 'utf-8'
                if last_ew is not None:
                    # We've already done an EW, combine this one with it
                    # if there's room.
                    chunk = get_unstructured(
                        ''.join(folded.current[last_ew:]+[tstr])).as_encoded_word(charset)
                    oldlastlen = sum(len(x) for x in folded.current[:last_ew])
                    schunk = str(chunk)
                    lchunk = len(schunk)
                    if oldlastlen + lchunk <= folded.maxlen:
                        del folded.current[last_ew:]
                        folded.append(schunk)
                        folded.lastlen = oldlastlen + lchunk
                        continue
                tstr = part.as_encoded_word(charset)
                is_ew = True
            if folded.append_if_fits(part, tstr):
                if is_ew:
                    last_ew = len(folded.current) - 1
                continue
            if is_ew or last_ew:
                # It's too big to fit on the line, but since we've
                # got encoded words we can use encoded word folding.
                part._fold_as_ew(folded)
                continue
            # Peel off the leading whitespace if any and make it sticky, to
            # avoid infinite recursion.
            ws = part.pop_leading_fws()
            if ws is not None:
                folded.stickyspace = str(ws)
                if folded.append_if_fits(part):
                    continue
            if part.has_fws:
                part.fold(folded)
                continue
            # It can't be split...we just have to put it on its own line.
            folded.append(tstr)
            folded.newline()
            last_ew = None

    def cte_encode(self, charset, policy):
        res = []
        last_ew = None
        for part in self:
            spart = str(part)
            try:
                spart.encode('us-ascii')
                res.append(spart)
            except UnicodeEncodeError:
                if last_ew is None:
                    res.append(part.cte_encode(charset, policy))
                    last_ew = len(res)
                else:
                    tl = get_unstructured(''.join(res[last_ew:] + [spart]))
                    res.append(tl.as_encoded_word())
        return ''.join(res)


class Phrase(TokenList):

    token_type = 'phrase'

    def _fold(self, folded):
        # As with Unstructured, we can have pure ASCII with or without
        # surrogateescape encoded bytes, or we could have unicode.  But this
        # case is more complicated, since we have to deal with the various
        # sub-token types and how they can be composed in the face of
        # unicode-that-needs-CTE-encoding, and the fact that if a token a
        # comment that becomes a barrier across which we can't compose encoded
        # words.
        last_ew = None
        for part in self.parts:
            tstr = str(part)
            tlen = len(tstr)
            has_ew = False
            try:
                str(part).encode('us-ascii')
            except UnicodeEncodeError:
                if any(isinstance(x, errors.UndecodableBytesDefect)
                        for x in part.all_defects):
                    charset = 'unknown-8bit'
                else:
                    charset = 'utf-8'
                if last_ew is not None and not part.has_leading_comment():
                    # We've already done an EW, let's see if we can combine
                    # this one with it.  The last_ew logic ensures that all we
                    # have at this point is atoms, no comments or quoted
                    # strings.  So we can treat the text between the last
                    # encoded word and the content of this token as
                    # unstructured text, and things will work correctly.  But
                    # we have to strip off any trailing comment on this token
                    # first, and if it is a quoted string we have to pull out
                    # the content (we're encoding it, so it no longer needs to
                    # be quoted).
                    if part[-1].token_type == 'cfws' and part.comments:
                        remainder = part.pop(-1)
                    else:
                        remainder = ''
                    for i, token in enumerate(part):
                        if token.token_type == 'bare-quoted-string':
                            part[i] = UnstructuredTokenList(token[:])
                    chunk = get_unstructured(
                        ''.join(folded.current[last_ew:]+[tstr])).as_encoded_word(charset)
                    schunk = str(chunk)
                    lchunk = len(schunk)
                    if last_ew + lchunk <= folded.maxlen:
                        del folded.current[last_ew:]
                        folded.append(schunk)
                        folded.lastlen = sum(len(x) for x in folded.current)
                        continue
                tstr = part.as_encoded_word(charset)
                tlen = len(tstr)
                has_ew = True
            if folded.append_if_fits(part, tstr):
                if has_ew and not part.comments:
                    last_ew = len(folded.current) - 1
                elif part.comments or part.token_type == 'quoted-string':
                    # If a comment is involved we can't combine EWs.  And if a
                    # quoted string is involved, it's not worth the effort to
                    # try to combine them.
                    last_ew = None
                continue
            part._fold(folded)

    def cte_encode(self, charset, policy):
        res = []
        last_ew = None
        is_ew = False
        for part in self:
            spart = str(part)
            try:
                spart.encode('us-ascii')
                res.append(spart)
            except UnicodeEncodeError:
                is_ew = True
                if last_ew is None:
                    if not part.comments:
                        last_ew = len(res)
                    res.append(part.cte_encode(charset, policy))
                elif not part.has_leading_comment():
                    if part[-1].token_type == 'cfws' and part.comments:
                        remainder = part.pop(-1)
                    else:
                        remainder = ''
                    for i, token in enumerate(part):
                        if token.token_type == 'bare-quoted-string':
                            part[i] = UnstructuredTokenList(token[:])
                    tl = get_unstructured(''.join(res[last_ew:] + [spart]))
                    res[last_ew:] = [tl.as_encoded_word(charset)]
            if part.comments or (not is_ew and part.token_type == 'quoted-string'):
                last_ew = None
        return ''.join(res)

class Word(TokenList):

    token_type = 'word'


class CFWSList(WhiteSpaceTokenList):

    token_type = 'cfws'

    def has_leading_comment(self):
        return bool(self.comments)


class Atom(TokenList):

    token_type = 'atom'


class EncodedWord(TokenList):

    token_type = 'encoded-word'
    cte = None
    charset = None
    lang = None

    @property
    def encoded(self):
        if self.cte is not None:
            return self.cte
        _ew.encode(str(self), self.charset)



class QuotedString(TokenList):

    token_type = 'quoted-string'

    @property
    def content(self):
        for x in self:
            if x.token_type == 'bare-quoted-string':
                return x.value

    @property
    def quoted_value(self):
        res = []
        for x in self:
            if x.token_type == 'bare-quoted-string':
                res.append(str(x))
            else:
                res.append(x.value)
        return ''.join(res)


class BareQuotedString(QuotedString):

    token_type = 'bare-quoted-string'

    def __str__(self):
        return quote_string(''.join(self))

    @property
    def value(self):
        return ''.join(str(x) for x in self)


class Comment(WhiteSpaceTokenList):

    token_type = 'comment'

    def __str__(self):
        return ''.join(sum([
                            ["("],
                            [self.quote(x) for x in self],
                            [")"],
                            ], []))

    def quote(self, value):
        if value.token_type == 'comment':
            return str(value)
        return str(value).replace('\\', '\\\\').replace(
                                  '(', '\(').replace(
                                  ')', '\)')

    @property
    def content(self):
        return ''.join(str(x) for x in self)

    @property
    def comments(self):
        return [self.content]

class AddressList(TokenList):

    token_type = 'address-list'

    @property
    def addresses(self):
        return [x for x in self if x.token_type=='address']

    @property
    def mailboxes(self):
        return sum((x.mailboxes
                    for x in self if x.token_type=='address'), [])

    @property
    def all_mailboxes(self):
        return sum((x.all_mailboxes
                    for x in self if x.token_type=='address'), [])


class Address(TokenList):

    token_type = 'address'

    @property
    def display_name(self):
        if self[0].token_type == 'group':
            return self[0].display_name

    @property
    def mailboxes(self):
        if self[0].token_type == 'mailbox':
            return [self[0]]
        elif self[0].token_type == 'invalid-mailbox':
            return []
        return self[0].mailboxes

    @property
    def all_mailboxes(self):
        if self[0].token_type == 'mailbox':
            return [self[0]]
        elif self[0].token_type == 'invalid-mailbox':
            return [self[0]]
        return self[0].all_mailboxes

class MailboxList(TokenList):

    token_type = 'mailbox-list'

    @property
    def mailboxes(self):
        return [x for x in self if x.token_type=='mailbox']

    @property
    def all_mailboxes(self):
        return [x for x in self
            if x.token_type in ('mailbox', 'invalid-mailbox')]


class GroupList(TokenList):

    token_type = 'group-list'

    @property
    def mailboxes(self):
        if not self or self[0].token_type != 'mailbox-list':
            return []
        return self[0].mailboxes

    @property
    def all_mailboxes(self):
        if not self or self[0].token_type != 'mailbox-list':
            return []
        return self[0].all_mailboxes


class Group(TokenList):

    token_type = "group"

    @property
    def mailboxes(self):
        if self[2].token_type != 'group-list':
            return []
        return self[2].mailboxes

    @property
    def all_mailboxes(self):
        if self[2].token_type != 'group-list':
            return []
        return self[2].all_mailboxes

    @property
    def display_name(self):
        return self[0].display_name


class NameAddr(TokenList):

    token_type = 'name-addr'

    @property
    def display_name(self):
        if len(self) == 1:
            return None
        return self[0].display_name

    @property
    def local_part(self):
        return self[-1].local_part

    @property
    def domain(self):
        return self[-1].domain

    @property
    def route(self):
        return self[-1].route

    @property
    def addr_spec(self):
        return self[-1].addr_spec


class AngleAddr(TokenList):

    token_type = 'angle-addr'

    @property
    def local_part(self):
        for x in self:
            if x.token_type == 'addr-spec':
                return x.local_part

    @property
    def domain(self):
        for x in self:
            if x.token_type == 'addr-spec':
                return x.domain

    @property
    def route(self):
        for x in self:
            if x.token_type == 'obs-route':
                return x.domains

    @property
    def addr_spec(self):
        for x in self:
            if x.token_type == 'addr-spec':
                return x.addr_spec


class ObsRoute(TokenList):

    token_type = 'obs-route'

    @property
    def domains(self):
        return [x.domain for x in self if x.token_type == 'domain']


class Mailbox(TokenList):

    token_type = 'mailbox'

    @property
    def display_name(self):
        if self[0].token_type == 'name-addr':
            return self[0].display_name

    @property
    def local_part(self):
        return self[0].local_part

    @property
    def domain(self):
        return self[0].domain

    @property
    def route(self):
        if self[0].token_type == 'name-addr':
            return self[0].route

    @property
    def addr_spec(self):
        return self[0].addr_spec


class InvalidMailbox(TokenList):

    token_type = 'invalid-mailbox'

    @property
    def display_name(self):
        return None

    local_part = domain = route = addr_spec = display_name


class Domain(TokenList):

    token_type = 'domain'

    @property
    def domain(self):
        return ''.join(super().value.split())


class DotAtom(TokenList):

    token_type = 'dot-atom'


class DotAtomText(TokenList):

    token_type = 'dot-atom-text'


class AddrSpec(TokenList):

    token_type = 'addr-spec'

    @property
    def local_part(self):
        return self[0].local_part

    @property
    def domain(self):
        if len(self) < 3:
            return None
        return self[-1].domain

    @property
    def value(self):
        if len(self) < 3:
            return self[0].value
        return self[0].value.rstrip()+self[1].value+self[2].value.lstrip()

    @property
    def addr_spec(self):
        nameset = set(self.local_part)
        if len(nameset) > len(nameset-DOT_ATOM_ENDS):
            lp = quote_string(self.local_part)
        else:
            lp = self.local_part
        if self.domain is not None:
            return lp + '@' + self.domain
        return lp


class ObsLocalPart(TokenList):

    token_type = 'obs-local-part'


class DisplayName(Phrase):

    token_type = 'display-name'

    @property
    def display_name(self):
        res = TokenList(self)
        if res[0].token_type == 'cfws':
            res.pop(0)
        else:
            if res[0][0].token_type == 'cfws':
                res[0] = TokenList(res[0][1:])
        if res[-1].token_type == 'cfws':
            res.pop()
        else:
            if res[-1][-1].token_type == 'cfws':
                res[-1] = TokenList(res[-1][:-1])
        return res.value

    @property
    def value(self):
        quote = False
        if self.defects:
            quote = True
        else:
            for x in self:
                if x.token_type == 'quoted-string':
                    quote = True
        if quote:
            pre = post = ''
            if self[0].token_type=='cfws' or self[0][0].token_type=='cfws':
                pre = ' '
            if self[-1].token_type=='cfws' or self[-1][-1].token_type=='cfws':
                post = ' '
            return pre+quote_string(self.display_name)+post
        else:
            return super().value


class LocalPart(TokenList):

    token_type = 'local-part'

    @property
    def value(self):
        if self[0].token_type == "quoted-string":
            return self[0].quoted_value
        else:
            return self[0].value

    @property
    def local_part(self):
        # Strip whitespace from front, back, and around dots.
        res = [DOT]
        last = DOT
        last_is_tl = False
        for tok in self[0] + [DOT]:
            if tok.token_type == 'cfws':
                continue
            if (last_is_tl and tok.token_type == 'dot' and
                    last[-1].token_type == 'cfws'):
                res[-1] = TokenList(last[:-1])
            is_tl = isinstance(tok, TokenList)
            if (is_tl and last.token_type == 'dot' and
                    tok[0].token_type == 'cfws'):
                res.append(TokenList(tok[1:]))
            else:
                res.append(tok)
            last = res[-1]
            last_is_tl = is_tl
        res = TokenList(res[1:-1])
        return res.value


class DomainLiteral(TokenList):

    token_type = 'domain-literal'

    @property
    def domain(self):
        return ''.join(super().value.split())

    @property
    def ip(self):
        for x in self:
            if x.token_type == 'ptext':
                return x.value


class HeaderLabel(TokenList):

    token_type = 'header-label'


class Header(TokenList):

    token_type = 'header'

    def _fold(self, folded):
        folded.append(str(self.pop(0)))
        folded.lastlen = len(folded.current[0])
        # The first line of the header is different from all others: we don't
        # want to start a new object on a new line if it has any fold points in
        # it that would allow part of it to be on the first header line.
        # Further, if the first fold point would fit on the new line, we want
        # to do that, but if it doesn't we want to put it on the first line.
        # Folded supports this via the stickyspace attribute.  If this
        # attribute is not None, it does the special handling.
        folded.stickyspace = str(self.pop(0)) if self[0].token_type == 'cfws' else ''
        rest = self.pop(0)
        if self:
            raise ValueError("Malformed Header token list")
        rest._fold(folded)


#
# Terminal classes and instances
#

class Terminal(str):

    def __new__(cls, value, token_type):
        self = super().__new__(cls, value)
        self.token_type = token_type
        self.defects = []
        return self

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, super().__repr__())

    @property
    def all_defects(self):
        return list(self.defects)

    def _pp(self, indent=''):
        return ["{}{}/{}({}){}".format(
            indent,
            self.__class__.__name__,
            self.token_type,
            super().__repr__(),
            '' if not self.defects else ' {}'.format(self.defects),
            )]

    def cte_encode(self, charset, policy):
        value = str(self)
        try:
            value.encode('us-ascii')
            return value
        except UnicodeEncodeError:
            return _ew.encode(value, charset)

    def pop_trailing_ws(self):
        # This terminates the recursion.
        return None

    def pop_leading_fws(self):
        # This terminates the recursion.
        return None

    @property
    def comments(self):
        return []

    def has_leading_comment(self):
        return False

    def __getnewargs__(self):
        return(str(self), self.token_type)


class WhiteSpaceTerminal(Terminal):

    @property
    def value(self):
        return ' '

    def startswith_fws(self):
        return True

    has_fws = True


class ValueTerminal(Terminal):

    @property
    def value(self):
        return self

    def startswith_fws(self):
        return False

    has_fws = False

    def as_encoded_word(self, charset):
        return _ew.encode(str(self), charset)


class EWWhiteSpaceTerminal(WhiteSpaceTerminal):

    @property
    def value(self):
        return ''

    @property
    def encoded(self):
        return self[:]

    def __str__(self):
        return ''

    has_fws = True


# XXX these need to become classes and used as instances so
# that a program can't change them in a parse tree and screw
# up other parse trees.  Maybe should have  tests for that, too.
DOT = ValueTerminal('.', 'dot')
ListSeparator = ValueTerminal(',', 'list-separator')
RouteComponentMarker = ValueTerminal('@', 'route-component-marker')

#
# Parser
#

"""Parse strings according to RFC822/2047/2822/5322 rules.

This is a stateless parser.  Each get_XXX function accepts a string and
returns either a Terminal or a TokenList representing the RFC object named
by the method and a string containing the remaining unparsed characters
from the input.  Thus a parser method consumes the next syntactic construct
of a given type and returns a token representing the construct plus the
unparsed remainder of the input string.

For example, if the first element of a structured header is a 'phrase',
then:

    phrase, value = get_phrase(value)

returns the complete phrase from the start of the string value, plus any
characters left in the string after the phrase is removed.

"""

_wsp_splitter = re.compile(r'([{}]+)'.format(''.join(WSP))).split
_non_atom_end_matcher = re.compile(r"[^{}]+".format(
    ''.join(ATOM_ENDS).replace('\\','\\\\').replace(']','\]'))).match
_non_printable_finder = re.compile(r"[\x00-\x20\x7F]").findall

def _validate_xtext(xtext):
    """If input token contains ASCII non-printables, register a defect."""

    non_printables = _non_printable_finder(xtext)
    if non_printables:
        xtext.defects.append(errors.NonPrintableDefect(non_printables))
    if utils._has_surrogates(xtext):
        xtext.defects.append(errors.UndecodableBytesDefect(
            "Non-ASCII characters found in header token"))

def _get_ptext_to_endchars(value, endchars):
    """Scan printables/quoted-pairs until endchars and return unquoted ptext.

    This function turns a run of qcontent, ccontent-without-comments, or
    dtext-with-quoted-printables into a single string by unquoting any
    quoted printables.  It returns the string, the remaining value, and
    a flag that is True iff there were any quoted printables decoded.

    """
    fragment, *remainder = _wsp_splitter(value, 1)
    vchars = []
    escape = False
    had_qp = False
    for pos in range(len(fragment)):
        if fragment[pos] == '\\':
            if escape:
                escape = False
                had_qp = True
            else:
                escape = True
                continue
        if escape:
            escape = False
        elif fragment[pos] in endchars:
            break
        vchars.append(fragment[pos])
    else:
        pos = pos + 1
    return ''.join(vchars), ''.join([fragment[pos:]] + remainder), had_qp

def _decode_ew_run(value):
    """ Decode a run of RFC2047 encoded words.

        _decode_ew_run(value) -> (text, value, defects)

    Scans the supplied value for a run of tokens that look like they are RFC
    2047 encoded words, decodes those words into text according to RFC 2047
    rules (whitespace between encoded words is discarded), and returns the text
    and the remaining value (including any leading whitespace on the remaining
    value), as well as a list of any defects encountered while decoding.  The
    input value may not have any leading whitespace.

    """
    res = []
    defects = []
    last_ws = ''
    while value:
        try:
            tok, ws, value = _wsp_splitter(value, 1)
        except ValueError:
            tok, ws, value = value, '', ''
        if not (tok.startswith('=?') and tok.endswith('?=')):
            return ''.join(res), last_ws + tok + ws + value, defects
        text, charset, lang, new_defects = _ew.decode(tok)
        res.append(text)
        defects.extend(new_defects)
        last_ws = ws
    return ''.join(res), last_ws, defects

def get_fws(value):
    """FWS = 1*WSP

    This isn't the RFC definition.  We're using fws to represent tokens where
    folding can be done, but when we are parsing the *un*folding has already
    been done so we don't need to watch out for CRLF.

    """
    newvalue = value.lstrip()
    fws = WhiteSpaceTerminal(value[:len(value)-len(newvalue)], 'fws')
    return fws, newvalue

def get_encoded_word(value):
    """ encoded-word = "=?" charset "?" encoding "?" encoded-text "?="

    """
    ew = EncodedWord()
    if not value.startswith('=?'):
        raise errors.HeaderParseError(
            "expected encoded word but found {}".format(value))
    tok, *remainder = value[2:].split('?=', 1)
    if tok == value[2:]:
        raise errors.HeaderParseError(
            "expected encoded word but found {}".format(value))
    remstr = ''.join(remainder)
    if remstr[:2].isdigit():
        rest, *remainder = remstr.split('?=', 1)
        tok = tok + '?=' + rest
    if len(tok.split()) > 1:
        ew.defects.append(errors.InvalidHeaderDefect(
            "whitespace inside encoded word"))
    ew.cte = value
    value = ''.join(remainder)
    try:
        text, charset, lang, defects = _ew.decode('=?' + tok + '?=')
    except ValueError:
        raise errors.HeaderParseError(
            "encoded word format invalid: '{}'".format(ew.cte))
    ew.charset = charset
    ew.lang = lang
    ew.defects.extend(defects)
    while text:
        if text[0] in WSP:
            token, text = get_fws(text)
            ew.append(token)
            continue
        chars, *remainder = _wsp_splitter(text, 1)
        vtext = ValueTerminal(chars, 'vtext')
        _validate_xtext(vtext)
        ew.append(vtext)
        text = ''.join(remainder)
    return ew, value

def get_unstructured(value):
    """unstructured = (*([FWS] vchar) *WSP) / obs-unstruct
       obs-unstruct = *((*LF *CR *(obs-utext) *LF *CR)) / FWS)
       obs-utext = %d0 / obs-NO-WS-CTL / LF / CR

       obs-NO-WS-CTL is control characters except WSP/CR/LF.

    So, basically, we have printable runs, plus control characters or nulls in
    the obsolete syntax, separated by whitespace.  Since RFC 2047 uses the
    obsolete syntax in its specification, but requires whitespace on either
    side of the encoded words, I can see no reason to need to separate the
    non-printable-non-whitespace from the printable runs if they occur, so we
    parse this into xtext tokens separated by WSP tokens.

    Because an 'unstructured' value must by definition constitute the entire
    value, this 'get' routine does not return a remaining value, only the
    parsed TokenList.

    """
    # XXX: but what about bare CR and LF?  They might signal the start or
    # end of an encoded word.  YAGNI for now, since out current parsers
    # will never send us strings with bard CR or LF.

    unstructured = UnstructuredTokenList()
    while value:
        if value[0] in WSP:
            token, value = get_fws(value)
            unstructured.append(token)
            continue
        if value.startswith('=?'):
            try:
                token, value = get_encoded_word(value)
            except errors.HeaderParseError:
                pass
            else:
                have_ws = True
                if len(unstructured) > 0:
                    if unstructured[-1].token_type != 'fws':
                        unstructured.defects.append(errors.InvalidHeaderDefect(
                            "missing whitespace before encoded word"))
                        have_ws = False
                if have_ws and len(unstructured) > 1:
                    if unstructured[-2].token_type == 'encoded-word':
                        unstructured[-1] = EWWhiteSpaceTerminal(
                            unstructured[-1], 'fws')
                unstructured.append(token)
                continue
        tok, *remainder = _wsp_splitter(value, 1)
        vtext = ValueTerminal(tok, 'vtext')
        _validate_xtext(vtext)
        unstructured.append(vtext)
        value = ''.join(remainder)
    return unstructured

def get_qp_ctext(value):
    """ctext = <printable ascii except \ ( )>

    This is not the RFC ctext, since we are handling nested comments in comment
    and unquoting quoted-pairs here.  We allow anything except the '()'
    characters, but if we find any ASCII other than the RFC defined printable
    ASCII an NonPrintableDefect is added to the token's defects list.  Since
    quoted pairs are converted to their unquoted values, what is returned is
    a 'ptext' token.  In this case it is a WhiteSpaceTerminal, so it's value
    is ' '.

    """
    ptext, value, _ = _get_ptext_to_endchars(value, '()')
    ptext = WhiteSpaceTerminal(ptext, 'ptext')
    _validate_xtext(ptext)
    return ptext, value

def get_qcontent(value):
    """qcontent = qtext / quoted-pair

    We allow anything except the DQUOTE character, but if we find any ASCII
    other than the RFC defined printable ASCII an NonPrintableDefect is
    added to the token's defects list.  Any quoted pairs are converted to their
    unquoted values, so what is returned is a 'ptext' token.  In this case it
    is a ValueTerminal.

    """
    ptext, value, _ = _get_ptext_to_endchars(value, '"')
    ptext = ValueTerminal(ptext, 'ptext')
    _validate_xtext(ptext)
    return ptext, value

def get_atext(value):
    """atext = <matches _atext_matcher>

    We allow any non-ATOM_ENDS in atext, but add an InvalidATextDefect to
    the token's defects list if we find non-atext characters.
    """
    m = _non_atom_end_matcher(value)
    if not m:
        raise errors.HeaderParseError(
            "expected atext but found '{}'".format(value))
    atext = m.group()
    value = value[len(atext):]
    atext = ValueTerminal(atext, 'atext')
    _validate_xtext(atext)
    return atext, value

def get_bare_quoted_string(value):
    """bare-quoted-string = DQUOTE *([FWS] qcontent) [FWS] DQUOTE

    A quoted-string without the leading or trailing white space.  Its
    value is the text between the quote marks, with whitespace
    preserved and quoted pairs decoded.
    """
    if value[0] != '"':
        raise errors.HeaderParseError(
            "expected '\"' but found '{}'".format(value))
    bare_quoted_string = BareQuotedString()
    value = value[1:]
    while value and value[0] != '"':
        if value[0] in WSP:
            token, value = get_fws(value)
        else:
            token, value = get_qcontent(value)
        bare_quoted_string.append(token)
    if not value:
        bare_quoted_string.defects.append(errors.InvalidHeaderDefect(
            "end of header inside quoted string"))
        return bare_quoted_string, value
    return bare_quoted_string, value[1:]

def get_comment(value):
    """comment = "(" *([FWS] ccontent) [FWS] ")"
       ccontent = ctext / quoted-pair / comment

    We handle nested comments here, and quoted-pair in our qp-ctext routine.
    """
    if value and value[0] != '(':
        raise errors.HeaderParseError(
            "expected '(' but found '{}'".format(value))
    comment = Comment()
    value = value[1:]
    while value and value[0] != ")":
        if value[0] in WSP:
            token, value = get_fws(value)
        elif value[0] == '(':
            token, value = get_comment(value)
        else:
            token, value = get_qp_ctext(value)
        comment.append(token)
    if not value:
        comment.defects.append(errors.InvalidHeaderDefect(
            "end of header inside comment"))
        return comment, value
    return comment, value[1:]

def get_cfws(value):
    """CFWS = (1*([FWS] comment) [FWS]) / FWS

    """
    cfws = CFWSList()
    while value and value[0] in CFWS_LEADER:
        if value[0] in WSP:
            token, value = get_fws(value)
        else:
            token, value = get_comment(value)
        cfws.append(token)
    return cfws, value

def get_quoted_string(value):
    """quoted-string = [CFWS] <bare-quoted-string> [CFWS]

    'bare-quoted-string' is an intermediate class defined by this
    parser and not by the RFC grammar.  It is the quoted string
    without any attached CFWS.
    """
    quoted_string = QuotedString()
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        quoted_string.append(token)
    token, value = get_bare_quoted_string(value)
    quoted_string.append(token)
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        quoted_string.append(token)
    return quoted_string, value

def get_atom(value):
    """atom = [CFWS] 1*atext [CFWS]

    """
    atom = Atom()
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        atom.append(token)
    if value and value[0] in ATOM_ENDS:
        raise errors.HeaderParseError(
            "expected atom but found '{}'".format(value))
    token, value = get_atext(value)
    atom.append(token)
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        atom.append(token)
    return atom, value

def get_dot_atom_text(value):
    """ dot-text = 1*atext *("." 1*atext)

    """
    dot_atom_text = DotAtomText()
    if not value or value[0] in ATOM_ENDS:
        raise errors.HeaderParseError("expected atom at a start of "
            "dot-atom-text but found '{}'".format(value))
    while value and value[0] not in ATOM_ENDS:
        token, value = get_atext(value)
        dot_atom_text.append(token)
        if value and value[0] == '.':
            dot_atom_text.append(DOT)
            value = value[1:]
    if dot_atom_text[-1] is DOT:
        raise errors.HeaderParseError("expected atom at end of dot-atom-text "
            "but found '{}'".format('.'+value))
    return dot_atom_text, value

def get_dot_atom(value):
    """ dot-atom = [CFWS] dot-atom-text [CFWS]

    """
    dot_atom = DotAtom()
    if value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        dot_atom.append(token)
    token, value = get_dot_atom_text(value)
    dot_atom.append(token)
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        dot_atom.append(token)
    return dot_atom, value

def get_word(value):
    """word = atom / quoted-string

    Either atom or quoted-string may start with CFWS.  We have to peel off this
    CFWS first to determine which type of word to parse.  Afterward we splice
    the leading CFWS, if any, into the parsed sub-token.

    If neither an atom or a quoted-string is found before the next special, a
    HeaderParseError is raised.

    The token returned is either an Atom or a QuotedString, as appropriate.
    This means the 'word' level of the formal grammar is not represented in the
    parse tree; this is because having that extra layer when manipulating the
    parse tree is more confusing than it is helpful.

    """
    if value[0] in CFWS_LEADER:
        leader, value = get_cfws(value)
    else:
        leader = None
    if value[0]=='"':
        token, value = get_quoted_string(value)
    elif value[0] in SPECIALS:
        raise errors.HeaderParseError("Expected 'atom' or 'quoted-string' "
                                      "but found '{}'".format(value))
    else:
        token, value = get_atom(value)
    if leader is not None:
        token[:0] = [leader]
    return token, value

def get_phrase(value):
    """ phrase = 1*word / obs-phrase
        obs-phrase = word *(word / "." / CFWS)

    This means a phrase can be a sequence of words, periods, and CFWS in any
    order as long as it starts with at least one word.  If anything other than
    words is detected, an ObsoleteHeaderDefect is added to the token's defect
    list.  We also accept a phrase that starts with CFWS followed by a dot;
    this is registered as an InvalidHeaderDefect, since it is not supported by
    even the obsolete grammar.

    """
    phrase = Phrase()
    try:
        token, value = get_word(value)
        phrase.append(token)
    except errors.HeaderParseError:
        phrase.defects.append(errors.InvalidHeaderDefect(
            "phrase does not start with word"))
    while value and value[0] not in PHRASE_ENDS:
        if value[0]=='.':
            phrase.append(DOT)
            phrase.defects.append(errors.ObsoleteHeaderDefect(
                "period in 'phrase'"))
            value = value[1:]
        else:
            try:
                token, value = get_word(value)
            except errors.HeaderParseError:
                if value[0] in CFWS_LEADER:
                    token, value = get_cfws(value)
                    phrase.defects.append(errors.ObsoleteHeaderDefect(
                        "comment found without atom"))
                else:
                    raise
            phrase.append(token)
    return phrase, value

def get_local_part(value):
    """ local-part = dot-atom / quoted-string / obs-local-part

    """
    local_part = LocalPart()
    leader = None
    if value[0] in CFWS_LEADER:
        leader, value = get_cfws(value)
    if not value:
        raise errors.HeaderParseError(
            "expected local-part but found '{}'".format(value))
    try:
        token, value = get_dot_atom(value)
    except errors.HeaderParseError:
        try:
            token, value = get_word(value)
        except errors.HeaderParseError:
            if value[0] != '\\' and value[0] in PHRASE_ENDS:
                raise
            token = TokenList()
    if leader is not None:
        token[:0] = [leader]
    local_part.append(token)
    if value and (value[0]=='\\' or value[0] not in PHRASE_ENDS):
        obs_local_part, value = get_obs_local_part(str(local_part) + value)
        if obs_local_part.token_type == 'invalid-obs-local-part':
            local_part.defects.append(errors.InvalidHeaderDefect(
                "local-part is not dot-atom, quoted-string, or obs-local-part"))
        else:
            local_part.defects.append(errors.ObsoleteHeaderDefect(
                "local-part is not a dot-atom (contains CFWS)"))
        local_part[0] = obs_local_part
    try:
        local_part.value.encode('ascii')
    except UnicodeEncodeError:
        local_part.defects.append(errors.NonASCIILocalPartDefect(
                "local-part contains non-ASCII characters)"))
    return local_part, value

def get_obs_local_part(value):
    """ obs-local-part = word *("." word)
    """
    obs_local_part = ObsLocalPart()
    last_non_ws_was_dot = False
    while value and (value[0]=='\\' or value[0] not in PHRASE_ENDS):
        if value[0] == '.':
            if last_non_ws_was_dot:
                obs_local_part.defects.append(errors.InvalidHeaderDefect(
                    "invalid repeated '.'"))
            obs_local_part.append(DOT)
            last_non_ws_was_dot = True
            value = value[1:]
            continue
        elif value[0]=='\\':
            obs_local_part.append(ValueTerminal(value[0],
                                                'misplaced-special'))
            value = value[1:]
            obs_local_part.defects.append(errors.InvalidHeaderDefect(
                "'\\' character outside of quoted-string/ccontent"))
            last_non_ws_was_dot = False
            continue
        if obs_local_part and obs_local_part[-1].token_type != 'dot':
            obs_local_part.defects.append(errors.InvalidHeaderDefect(
                "missing '.' between words"))
        try:
            token, value = get_word(value)
            last_non_ws_was_dot = False
        except errors.HeaderParseError:
            if value[0] not in CFWS_LEADER:
                raise
            token, value = get_cfws(value)
        obs_local_part.append(token)
    if (obs_local_part[0].token_type == 'dot' or
            obs_local_part[0].token_type=='cfws' and
            obs_local_part[1].token_type=='dot'):
        obs_local_part.defects.append(errors.InvalidHeaderDefect(
            "Invalid leading '.' in local part"))
    if (obs_local_part[-1].token_type == 'dot' or
            obs_local_part[-1].token_type=='cfws' and
            obs_local_part[-2].token_type=='dot'):
        obs_local_part.defects.append(errors.InvalidHeaderDefect(
            "Invalid trailing '.' in local part"))
    if obs_local_part.defects:
        obs_local_part.token_type = 'invalid-obs-local-part'
    return obs_local_part, value

def get_dtext(value):
    """ dtext = <printable ascii except \ [ ]> / obs-dtext
        obs-dtext = obs-NO-WS-CTL / quoted-pair

    We allow anything except the excluded characters, but but if we find any
    ASCII other than the RFC defined printable ASCII an NonPrintableDefect is
    added to the token's defects list.  Quoted pairs are converted to their
    unquoted values, so what is returned is a ptext token, in this case a
    ValueTerminal.  If there were quoted-printables, an ObsoleteHeaderDefect is
    added to the returned token's defect list.

    """
    ptext, value, had_qp = _get_ptext_to_endchars(value, '[]')
    ptext = ValueTerminal(ptext, 'ptext')
    if had_qp:
        ptext.defects.append(errors.ObsoleteHeaderDefect(
            "quoted printable found in domain-literal"))
    _validate_xtext(ptext)
    return ptext, value

def _check_for_early_dl_end(value, domain_literal):
    if value:
        return False
    domain_literal.append(errors.InvalidHeaderDefect(
        "end of input inside domain-literal"))
    domain_literal.append(ValueTerminal(']', 'domain-literal-end'))
    return True

def get_domain_literal(value):
    """ domain-literal = [CFWS] "[" *([FWS] dtext) [FWS] "]" [CFWS]

    """
    domain_literal = DomainLiteral()
    if value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        domain_literal.append(token)
    if not value:
        raise errors.HeaderParseError("expected domain-literal")
    if value[0] != '[':
        raise errors.HeaderParseError("expected '[' at start of domain-literal "
                "but found '{}'".format(value))
    value = value[1:]
    if _check_for_early_dl_end(value, domain_literal):
        return domain_literal, value
    domain_literal.append(ValueTerminal('[', 'domain-literal-start'))
    if value[0] in WSP:
        token, value = get_fws(value)
        domain_literal.append(token)
    token, value = get_dtext(value)
    domain_literal.append(token)
    if _check_for_early_dl_end(value, domain_literal):
        return domain_literal, value
    if value[0] in WSP:
        token, value = get_fws(value)
        domain_literal.append(token)
    if _check_for_early_dl_end(value, domain_literal):
        return domain_literal, value
    if value[0] != ']':
        raise errors.HeaderParseError("expected ']' at end of domain-literal "
                "but found '{}'".format(value))
    domain_literal.append(ValueTerminal(']', 'domain-literal-end'))
    value = value[1:]
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        domain_literal.append(token)
    return domain_literal, value

def get_domain(value):
    """ domain = dot-atom / domain-literal / obs-domain
        obs-domain = atom *("." atom))

    """
    domain = Domain()
    leader = None
    if value[0] in CFWS_LEADER:
        leader, value = get_cfws(value)
    if not value:
        raise errors.HeaderParseError(
            "expected domain but found '{}'".format(value))
    if value[0] == '[':
        token, value = get_domain_literal(value)
        if leader is not None:
            token[:0] = [leader]
        domain.append(token)
        return domain, value
    try:
        token, value = get_dot_atom(value)
    except errors.HeaderParseError:
        token, value = get_atom(value)
    if leader is not None:
        token[:0] = [leader]
    domain.append(token)
    if value and value[0] == '.':
        domain.defects.append(errors.ObsoleteHeaderDefect(
            "domain is not a dot-atom (contains CFWS)"))
        if domain[0].token_type == 'dot-atom':
            domain[:] = domain[0]
        while value and value[0] == '.':
            domain.append(DOT)
            token, value = get_atom(value[1:])
            domain.append(token)
    return domain, value

def get_addr_spec(value):
    """ addr-spec = local-part "@" domain

    """
    addr_spec = AddrSpec()
    token, value = get_local_part(value)
    addr_spec.append(token)
    if not value or value[0] != '@':
        addr_spec.defects.append(errors.InvalidHeaderDefect(
            "add-spec local part with no domain"))
        return addr_spec, value
    addr_spec.append(ValueTerminal('@', 'address-at-symbol'))
    token, value = get_domain(value[1:])
    addr_spec.append(token)
    return addr_spec, value

def get_obs_route(value):
    """ obs-route = obs-domain-list ":"
        obs-domain-list = *(CFWS / ",") "@" domain *("," [CFWS] ["@" domain])

        Returns an obs-route token with the appropriate sub-tokens (that is,
        there is no obs-domain-list in the parse tree).
    """
    obs_route = ObsRoute()
    while value and (value[0]==',' or value[0] in CFWS_LEADER):
        if value[0] in CFWS_LEADER:
            token, value = get_cfws(value)
            obs_route.append(token)
        elif value[0] == ',':
            obs_route.append(ListSeparator)
            value = value[1:]
    if not value or value[0] != '@':
        raise errors.HeaderParseError(
            "expected obs-route domain but found '{}'".format(value))
    obs_route.append(RouteComponentMarker)
    token, value = get_domain(value[1:])
    obs_route.append(token)
    while value and value[0]==',':
        obs_route.append(ListSeparator)
        value = value[1:]
        if not value:
            break
        if value[0] in CFWS_LEADER:
            token, value = get_cfws(value)
            obs_route.append(token)
        if value[0] == '@':
            obs_route.append(RouteComponentMarker)
            token, value = get_domain(value[1:])
            obs_route.append(token)
    if not value:
        raise errors.HeaderParseError("end of header while parsing obs-route")
    if value[0] != ':':
        raise errors.HeaderParseError( "expected ':' marking end of "
            "obs-route but found '{}'".format(value))
    obs_route.append(ValueTerminal(':', 'end-of-obs-route-marker'))
    return obs_route, value[1:]

def get_angle_addr(value):
    """ angle-addr = [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
        obs-angle-addr = [CFWS] "<" obs-route addr-spec ">" [CFWS]

    """
    angle_addr = AngleAddr()
    if value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        angle_addr.append(token)
    if not value or value[0] != '<':
        raise errors.HeaderParseError(
            "expected angle-addr but found '{}'".format(value))
    angle_addr.append(ValueTerminal('<', 'angle-addr-start'))
    value = value[1:]
    try:
        token, value = get_addr_spec(value)
    except errors.HeaderParseError:
        try:
            token, value = get_obs_route(value)
            angle_addr.defects.append(errors.ObsoleteHeaderDefect(
                "obsolete route specification in angle-addr"))
        except errors.HeaderParseError:
            raise errors.HeaderParseError(
                "expected addr-spec or but found '{}'".format(value))
        angle_addr.append(token)
        token, value = get_addr_spec(value)
    angle_addr.append(token)
    if value and value[0] == '>':
        value = value[1:]
    else:
        angle_addr.defects.append(errors.InvalidHeaderDefect(
            "missing trailing '>' on angle-addr"))
    angle_addr.append(ValueTerminal('>', 'angle-addr-end'))
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        angle_addr.append(token)
    return angle_addr, value

def get_display_name(value):
    """ display-name = phrase

    Because this is simply a name-rule, we don't return a display-name
    token containing a phrase, but rather a display-name token with
    the content of the phrase.

    """
    display_name = DisplayName()
    token, value = get_phrase(value)
    display_name.extend(token[:])
    display_name.defects = token.defects[:]
    return display_name, value


def get_name_addr(value):
    """ name-addr = [display-name] angle-addr

    """
    name_addr = NameAddr()
    # Both the optional display name and the angle-addr can start with cfws.
    leader = None
    if value[0] in CFWS_LEADER:
        leader, value = get_cfws(value)
        if not value:
            raise errors.HeaderParseError(
                "expected name-addr but found '{}'".format(leader))
    if value[0] != '<':
        if value[0] in PHRASE_ENDS:
            raise errors.HeaderParseError(
                "expected name-addr but found '{}'".format(value))
        token, value = get_display_name(value)
        if not value:
            raise errors.HeaderParseError(
                "expected name-addr but found '{}'".format(token))
        if leader is not None:
            token[0][:0] = [leader]
            leader = None
        name_addr.append(token)
    token, value = get_angle_addr(value)
    if leader is not None:
        token[:0] = [leader]
    name_addr.append(token)
    return name_addr, value

def get_mailbox(value):
    """ mailbox = name-addr / addr-spec

    """
    # The only way to figure out if we are dealing with a name-addr or an
    # addr-spec is to try parsing each one.
    mailbox = Mailbox()
    try:
        token, value = get_name_addr(value)
    except errors.HeaderParseError:
        try:
            token, value = get_addr_spec(value)
        except errors.HeaderParseError:
            raise errors.HeaderParseError(
                "expected mailbox but found '{}'".format(value))
    if any(isinstance(x, errors.InvalidHeaderDefect)
                       for x in token.all_defects):
        mailbox.token_type = 'invalid-mailbox'
    mailbox.append(token)
    return mailbox, value

def get_invalid_mailbox(value, endchars):
    """ Read everything up to one of the chars in endchars.

    This is outside the formal grammar.  The InvalidMailbox TokenList that is
    returned acts like a Mailbox, but the data attributes are None.

    """
    invalid_mailbox = InvalidMailbox()
    while value and value[0] not in endchars:
        if value[0] in PHRASE_ENDS:
            invalid_mailbox.append(ValueTerminal(value[0],
                                                 'misplaced-special'))
            value = value[1:]
        else:
            token, value = get_phrase(value)
            invalid_mailbox.append(token)
    return invalid_mailbox, value

def get_mailbox_list(value):
    """ mailbox-list = (mailbox *("," mailbox)) / obs-mbox-list
        obs-mbox-list = *([CFWS] ",") mailbox *("," [mailbox / CFWS])

    For this routine we go outside the formal grammar in order to improve error
    handling.  We recognize the end of the mailbox list only at the end of the
    value or at a ';' (the group terminator).  This is so that we can turn
    invalid mailboxes into InvalidMailbox tokens and continue parsing any
    remaining valid mailboxes.  We also allow all mailbox entries to be null,
    and this condition is handled appropriately at a higher level.

    """
    mailbox_list = MailboxList()
    while value and value[0] != ';':
        try:
            token, value = get_mailbox(value)
            mailbox_list.append(token)
        except errors.HeaderParseError:
            leader = None
            if value[0] in CFWS_LEADER:
                leader, value = get_cfws(value)
                if not value or value[0] in ',;':
                    mailbox_list.append(leader)
                    mailbox_list.defects.append(errors.ObsoleteHeaderDefect(
                        "empty element in mailbox-list"))
                else:
                    token, value = get_invalid_mailbox(value, ',;')
                    if leader is not None:
                        token[:0] = [leader]
                    mailbox_list.append(token)
                    mailbox_list.defects.append(errors.InvalidHeaderDefect(
                        "invalid mailbox in mailbox-list"))
            elif value[0] == ',':
                mailbox_list.defects.append(errors.ObsoleteHeaderDefect(
                    "empty element in mailbox-list"))
            else:
                token, value = get_invalid_mailbox(value, ',;')
                if leader is not None:
                    token[:0] = [leader]
                mailbox_list.append(token)
                mailbox_list.defects.append(errors.InvalidHeaderDefect(
                    "invalid mailbox in mailbox-list"))
        if value and value[0] not in ',;':
            # Crap after mailbox; treat it as an invalid mailbox.
            # The mailbox info will still be available.
            mailbox = mailbox_list[-1]
            mailbox.token_type = 'invalid-mailbox'
            token, value = get_invalid_mailbox(value, ',;')
            mailbox.extend(token)
            mailbox_list.defects.append(errors.InvalidHeaderDefect(
                "invalid mailbox in mailbox-list"))
        if value and value[0] == ',':
            mailbox_list.append(ListSeparator)
            value = value[1:]
    return mailbox_list, value


def get_group_list(value):
    """ group-list = mailbox-list / CFWS / obs-group-list
        obs-group-list = 1*([CFWS] ",") [CFWS]

    """
    group_list = GroupList()
    if not value:
        group_list.defects.append(errors.InvalidHeaderDefect(
            "end of header before group-list"))
        return group_list, value
    leader = None
    if value and value[0] in CFWS_LEADER:
        leader, value = get_cfws(value)
        if not value:
            # This should never happen in email parsing, since CFWS-only is a
            # legal alternative to group-list in a group, which is the only
            # place group-list appears.
            group_list.defects.append(errors.InvalidHeaderDefect(
                "end of header in group-list"))
            group_list.append(leader)
            return group_list, value
        if value[0] == ';':
            group_list.append(leader)
            return group_list, value
    token, value = get_mailbox_list(value)
    if len(token.all_mailboxes)==0:
        if leader is not None:
            group_list.append(leader)
        group_list.extend(token)
        group_list.defects.append(errors.ObsoleteHeaderDefect(
            "group-list with empty entries"))
        return group_list, value
    if leader is not None:
        token[:0] = [leader]
    group_list.append(token)
    return group_list, value

def get_group(value):
    """ group = display-name ":" [group-list] ";" [CFWS]

    """
    group = Group()
    token, value = get_display_name(value)
    if not value or value[0] != ':':
        raise errors.HeaderParseError("expected ':' at end of group "
            "display name but found '{}'".format(value))
    group.append(token)
    group.append(ValueTerminal(':', 'group-display-name-terminator'))
    value = value[1:]
    if value and value[0] == ';':
        group.append(ValueTerminal(';', 'group-terminator'))
        return group, value[1:]
    token, value = get_group_list(value)
    group.append(token)
    if not value:
        group.defects.append(errors.InvalidHeaderDefect(
            "end of header in group"))
    if value[0] != ';':
        raise errors.HeaderParseError(
            "expected ';' at end of group but found {}".format(value))
    group.append(ValueTerminal(';', 'group-terminator'))
    value = value[1:]
    if value and value[0] in CFWS_LEADER:
        token, value = get_cfws(value)
        group.append(token)
    return group, value

def get_address(value):
    """ address = mailbox / group

    Note that counter-intuitively, an address can be either a single address or
    a list of addresses (a group).  This is why the returned Address object has
    a 'mailboxes' attribute which treats a single address as a list of length
    one.  When you need to differentiate between to two cases, extract the single
    element, which is either a mailbox or a group token.

    """
    # The formal grammar isn't very helpful when parsing an address.  mailbox
    # and group, especially when allowing for obsolete forms, start off very
    # similarly.  It is only when you reach one of @, <, or : that you know
    # what you've got.  So, we try each one in turn, starting with the more
    # likely of the two.  We could perhaps make this more efficient by looking
    # for a phrase and then branching based on the next character, but that
    # would be a premature optimization.
    address = Address()
    try:
        token, value = get_group(value)
    except errors.HeaderParseError:
        try:
            token, value = get_mailbox(value)
        except errors.HeaderParseError:
            raise errors.HeaderParseError(
                "expected address but found '{}'".format(value))
    address.append(token)
    return address, value

def get_address_list(value):
    """ address_list = (address *("," address)) / obs-addr-list
        obs-addr-list = *([CFWS] ",") address *("," [address / CFWS])

    We depart from the formal grammar here by continuing to parse until the end
    of the input, assuming the input to be entirely composed of an
    address-list.  This is always true in email parsing, and allows us
    to skip invalid addresses to parse additional valid ones.

    """
    address_list = AddressList()
    while value:
        try:
            token, value = get_address(value)
            address_list.append(token)
        except errors.HeaderParseError as err:
            leader = None
            if value[0] in CFWS_LEADER:
                leader, value = get_cfws(value)
                if not value or value[0] == ',':
                    address_list.append(leader)
                    address_list.defects.append(errors.ObsoleteHeaderDefect(
                        "address-list entry with no content"))
                else:
                    token, value = get_invalid_mailbox(value, ',')
                    if leader is not None:
                        token[:0] = [leader]
                    address_list.append(Address([token]))
                    address_list.defects.append(errors.InvalidHeaderDefect(
                        "invalid address in address-list"))
            elif value[0] == ',':
                address_list.defects.append(errors.ObsoleteHeaderDefect(
                    "empty element in address-list"))
            else:
                token, value = get_invalid_mailbox(value, ',')
                if leader is not None:
                    token[:0] = [leader]
                address_list.append(Address([token]))
                address_list.defects.append(errors.InvalidHeaderDefect(
                    "invalid address in address-list"))
        if value and value[0] != ',':
            # Crap after address; treat it as an invalid mailbox.
            # The mailbox info will still be available.
            mailbox = address_list[-1][0]
            mailbox.token_type = 'invalid-mailbox'
            token, value = get_invalid_mailbox(value, ',')
            mailbox.extend(token)
            address_list.defects.append(errors.InvalidHeaderDefect(
                "invalid address in address-list"))
        if value:  # Must be a , at this point.
            address_list.append(ValueTerminal(',', 'list-separator'))
            value = value[1:]
    return address_list, value
