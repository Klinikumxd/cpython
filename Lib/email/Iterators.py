# Copyright (C) 2001,2002 Python Software Foundation
# Author: barry@zope.com (Barry Warsaw)

"""Various types of useful iterators and generators.
"""

try:
    from email._compat22 import body_line_iterator, typed_subpart_iterator
except SyntaxError:
    # Python 2.1 doesn't have generators
    from email._compat21 import body_line_iterator, typed_subpart_iterator



def _structure(msg, level=0):
    """A handy debugging aid"""
    tab = ' ' * (level * 4)
    print tab + msg.get('content-type', msg.get_default_type())
    if msg.is_multipart():
        for subpart in msg.get_payload():
            _structure(subpart, level+1)
