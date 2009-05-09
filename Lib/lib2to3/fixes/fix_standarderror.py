# Copyright 2007 Google, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""Fixer for StandardError -> Exception."""

# Local imports
from .. import fixer_base
from ..fixer_util import Name


class FixStandarderror(fixer_base.BaseFix):

    PATTERN = """
              'StandardError'
              """

    def transform(self, node, results):
        return Name(u"Exception", prefix=node.get_prefix())
