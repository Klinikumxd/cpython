# Copyright 2007 Google, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""Fixer that changes map(F, ...) into list(map(F, ...)) unless there
exists a 'from future_builtins import map' statement in the top-level
namespace.

As a special case, map(None, X) is changed into list(X).  (This is
necessary because the semantics are changed in this case -- the new
map(None, X) is equivalent to [(x,) for x in X].)

We avoid the transformation (except for the special case mentioned
above) if the map() call is directly contained in iter(<>), list(<>),
tuple(<>), sorted(<>), ...join(<>), or for V in <>:.

NOTE: This is still not correct if the original code was depending on
map(F, X, Y, ...) to go on until the longest argument is exhausted,
substituting None for missing values -- like zip(), it now stops as
soon as the shortest argument is exhausted.
"""

# Local imports
from ..pgen2 import token
from . import basefix
from .util import Name, Call, ListComp, in_special_context
from ..pygram import python_symbols as syms

class FixMap(basefix.ConditionalFix):

    PATTERN = """
    map_none=power<
        'map'
        trailer< '(' arglist< 'None' ',' arg=any [','] > ')' >
    >
    |
    map_lambda=power<
        'map'
        trailer<
            '('
            arglist<
                lambdef< 'lambda'
                         (fp=NAME | vfpdef< '(' fp=NAME ')'> ) ':' xp=any
                >
                ','
                it=any
            >
            ')'
        >
    >
    |
    power<
        'map'
        args=trailer< '(' [any] ')' >
    >
    """

    skip_on = 'future_builtins.map'

    def transform(self, node, results):
        if self.should_skip(node):
            return

        if node.parent.type == syms.simple_stmt:
            self.warning(node, "You should use a for loop here")
            new = node.clone()
            new.set_prefix("")
            new = Call(Name("list"), [new])
        elif "map_lambda" in results:
            new = ListComp(results.get("xp").clone(),
                           results.get("fp").clone(),
                           results.get("it").clone())
        else:
            if "map_none" in results:
                new = results["arg"].clone()
            else:
                if in_special_context(node):
                    return None
                new = node.clone()
            new.set_prefix("")
            new = Call(Name("list"), [new])
        new.set_prefix(node.get_prefix())
        return new
