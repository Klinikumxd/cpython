"""Class representing the list of files in a distribution.

The Manifest class can be used to:

 - read or write a MANIFEST file
 - read a template file and find out the file list
"""
# XXX todo: document + add tests
import re
import os
import fnmatch

from packaging import logger
from packaging.util import write_file, convert_path
from packaging.errors import (PackagingTemplateError,
                              PackagingInternalError)

__all__ = ['Manifest']

# a \ followed by some spaces + EOL
_COLLAPSE_PATTERN = re.compile('\\\w*\n', re.M)
_COMMENTED_LINE = re.compile('#.*?(?=\n)|\n(?=$)', re.M | re.S)


class Manifest(object):
    """A list of files built by on exploring the filesystem and filtered by
    applying various patterns to what we find there.
    """

    def __init__(self):
        self.allfiles = None
        self.files = []

    #
    # Public API
    #

    def findall(self, dir=os.curdir):
        self.allfiles = _findall(dir)

    def append(self, item):
        self.files.append(item)

    def extend(self, items):
        self.files.extend(items)

    def sort(self):
        # Not a strict lexical sort!
        self.files = [os.path.join(*path_tuple) for path_tuple in
                      sorted(os.path.split(path) for path in self.files)]

    def clear(self):
        """Clear all collected files."""
        self.files = []
        if self.allfiles is not None:
            self.allfiles = []

    def remove_duplicates(self):
        # Assumes list has been sorted!
        for i in range(len(self.files) - 1, 0, -1):
            if self.files[i] == self.files[i - 1]:
                del self.files[i]

    def read_template(self, path_or_file):
        """Read and parse a manifest template file.
        'path' can be a path or a file-like object.

        Updates the list accordingly.
        """
        if isinstance(path_or_file, str):
            f = open(path_or_file)
        else:
            f = path_or_file

        try:
            content = f.read()
            # first, let's unwrap collapsed lines
            content = _COLLAPSE_PATTERN.sub('', content)
            # next, let's remove commented lines and empty lines
            content = _COMMENTED_LINE.sub('', content)

            # now we have our cleaned up lines
            lines = [line.strip() for line in content.split('\n')]
        finally:
            f.close()

        for line in lines:
            if line == '':
                continue
            try:
                self._process_template_line(line)
            except PackagingTemplateError as msg:
                logger.warning("%s, %s", path_or_file, msg)

    def write(self, path):
        """Write the file list in 'self.filelist' (presumably as filled in
        by 'add_defaults()' and 'read_template()') to the manifest file
        named by 'self.manifest'.
        """
        if os.path.isfile(path):
            with open(path) as fp:
                first_line = fp.readline()

            if first_line != '# file GENERATED by packaging, do NOT edit\n':
                logger.info("not writing to manually maintained "
                            "manifest file %r", path)
                return

        self.sort()
        self.remove_duplicates()
        content = self.files[:]
        content.insert(0, '# file GENERATED by packaging, do NOT edit')
        logger.info("writing manifest file %r", path)
        write_file(path, content)

    def read(self, path):
        """Read the manifest file (named by 'self.manifest') and use it to
        fill in 'self.filelist', the list of files to include in the source
        distribution.
        """
        logger.info("reading manifest file %r", path)
        with open(path) as manifest:
            for line in manifest.readlines():
                self.append(line)

    def exclude_pattern(self, pattern, anchor=True, prefix=None,
                        is_regex=False):
        """Remove strings (presumably filenames) from 'files' that match
        'pattern'.

        Other parameters are the same as for 'include_pattern()', above.
        The list 'self.files' is modified in place. Return True if files are
        found.
        """
        files_found = False
        pattern_re = _translate_pattern(pattern, anchor, prefix, is_regex)
        for i in range(len(self.files) - 1, -1, -1):
            if pattern_re.search(self.files[i]):
                del self.files[i]
                files_found = True

        return files_found

    #
    # Private API
    #

    def _parse_template_line(self, line):
        words = line.split()
        if len(words) == 1 and words[0] not in (
              'include', 'exclude', 'global-include', 'global-exclude',
              'recursive-include', 'recursive-exclude', 'graft', 'prune'):
            # no action given, let's use the default 'include'
            words.insert(0, 'include')

        action = words[0]
        patterns = dir = dir_pattern = None

        if action in ('include', 'exclude',
                      'global-include', 'global-exclude'):
            if len(words) < 2:
                raise PackagingTemplateError(
                      "%r expects <pattern1> <pattern2> ..." % action)

            patterns = [convert_path(word) for word in words[1:]]

        elif action in ('recursive-include', 'recursive-exclude'):
            if len(words) < 3:
                raise PackagingTemplateError(
                      "%r expects <dir> <pattern1> <pattern2> ..." % action)

            dir = convert_path(words[1])
            patterns = [convert_path(word) for word in words[2:]]

        elif action in ('graft', 'prune'):
            if len(words) != 2:
                raise PackagingTemplateError(
                     "%r expects a single <dir_pattern>" % action)

            dir_pattern = convert_path(words[1])

        else:
            raise PackagingTemplateError("unknown action %r" % action)

        return action, patterns, dir, dir_pattern

    def _process_template_line(self, line):
        # Parse the line: split it up, make sure the right number of words
        # is there, and return the relevant words.  'action' is always
        # defined: it's the first word of the line.  Which of the other
        # three are defined depends on the action; it'll be either
        # patterns, (dir and patterns), or (dir_pattern).
        action, patterns, dir, dir_pattern = self._parse_template_line(line)

        # OK, now we know that the action is valid and we have the
        # right number of words on the line for that action -- so we
        # can proceed with minimal error-checking.
        if action == 'include':
            for pattern in patterns:
                if not self._include_pattern(pattern, anchor=True):
                    logger.warning("no files found matching %r", pattern)

        elif action == 'exclude':
            for pattern in patterns:
                if not self.exclude_pattern(pattern, anchor=True):
                    logger.warning("no previously-included files "
                                   "found matching %r", pattern)

        elif action == 'global-include':
            for pattern in patterns:
                if not self._include_pattern(pattern, anchor=False):
                    logger.warning("no files found matching %r "
                                   "anywhere in distribution", pattern)

        elif action == 'global-exclude':
            for pattern in patterns:
                if not self.exclude_pattern(pattern, anchor=False):
                    logger.warning("no previously-included files "
                                   "matching %r found anywhere in "
                                   "distribution", pattern)

        elif action == 'recursive-include':
            for pattern in patterns:
                if not self._include_pattern(pattern, prefix=dir):
                    logger.warning("no files found matching %r "
                                   "under directory %r", pattern, dir)

        elif action == 'recursive-exclude':
            for pattern in patterns:
                if not self.exclude_pattern(pattern, prefix=dir):
                    logger.warning("no previously-included files "
                                   "matching %r found under directory %r",
                                   pattern, dir)

        elif action == 'graft':
            if not self._include_pattern(None, prefix=dir_pattern):
                logger.warning("no directories found matching %r",
                               dir_pattern)

        elif action == 'prune':
            if not self.exclude_pattern(None, prefix=dir_pattern):
                logger.warning("no previously-included directories found "
                               "matching %r", dir_pattern)
        else:
            raise PackagingInternalError(
                "this cannot happen: invalid action %r" % action)

    def _include_pattern(self, pattern, anchor=True, prefix=None,
                         is_regex=False):
        """Select strings (presumably filenames) from 'self.files' that
        match 'pattern', a Unix-style wildcard (glob) pattern.

        Patterns are not quite the same as implemented by the 'fnmatch'
        module: '*' and '?'  match non-special characters, where "special"
        is platform-dependent: slash on Unix; colon, slash, and backslash on
        DOS/Windows; and colon on Mac OS.

        If 'anchor' is true (the default), then the pattern match is more
        stringent: "*.py" will match "foo.py" but not "foo/bar.py".  If
        'anchor' is false, both of these will match.

        If 'prefix' is supplied, then only filenames starting with 'prefix'
        (itself a pattern) and ending with 'pattern', with anything in between
        them, will match.  'anchor' is ignored in this case.

        If 'is_regex' is true, 'anchor' and 'prefix' are ignored, and
        'pattern' is assumed to be either a string containing a regex or a
        regex object -- no translation is done, the regex is just compiled
        and used as-is.

        Selected strings will be added to self.files.

        Return True if files are found.
        """
        files_found = False
        pattern_re = _translate_pattern(pattern, anchor, prefix, is_regex)

        # delayed loading of allfiles list
        if self.allfiles is None:
            self.findall()

        for name in self.allfiles:
            if pattern_re.search(name):
                self.files.append(name)
                files_found = True

        return files_found


#
# Utility functions
#
def _findall(dir=os.curdir):
    """Find all files under 'dir' and return the list of full filenames
    (relative to 'dir').
    """
    from stat import S_ISREG, S_ISDIR, S_ISLNK

    list = []
    stack = [dir]
    pop = stack.pop
    push = stack.append

    while stack:
        dir = pop()
        names = os.listdir(dir)

        for name in names:
            if dir != os.curdir:        # avoid the dreaded "./" syndrome
                fullname = os.path.join(dir, name)
            else:
                fullname = name

            # Avoid excess stat calls -- just one will do, thank you!
            stat = os.stat(fullname)
            mode = stat.st_mode
            if S_ISREG(mode):
                list.append(fullname)
            elif S_ISDIR(mode) and not S_ISLNK(mode):
                push(fullname)

    return list


def _glob_to_re(pattern):
    """Translate a shell-like glob pattern to a regular expression.

    Return a string containing the regex.  Differs from
    'fnmatch.translate()' in that '*' does not match "special characters"
    (which are platform-specific).
    """
    pattern_re = fnmatch.translate(pattern)

    # '?' and '*' in the glob pattern become '.' and '.*' in the RE, which
    # IMHO is wrong -- '?' and '*' aren't supposed to match slash in Unix,
    # and by extension they shouldn't match such "special characters" under
    # any OS.  So change all non-escaped dots in the RE to match any
    # character except the special characters.
    # XXX currently the "special characters" are just slash -- i.e. this is
    # Unix-only.
    pattern_re = re.sub(r'((?<!\\)(\\\\)*)\.', r'\1[^/]', pattern_re)

    return pattern_re


def _translate_pattern(pattern, anchor=True, prefix=None, is_regex=False):
    """Translate a shell-like wildcard pattern to a compiled regular
    expression.

    Return the compiled regex.  If 'is_regex' true,
    then 'pattern' is directly compiled to a regex (if it's a string)
    or just returned as-is (assumes it's a regex object).
    """
    if is_regex:
        if isinstance(pattern, str):
            return re.compile(pattern)
        else:
            return pattern

    if pattern:
        pattern_re = _glob_to_re(pattern)
    else:
        pattern_re = ''

    if prefix is not None:
        # ditch end of pattern character
        empty_pattern = _glob_to_re('')
        prefix_re = _glob_to_re(prefix)[:-len(empty_pattern)]
        # paths should always use / in manifest templates
        pattern_re = "^%s/.*%s" % (prefix_re, pattern_re)
    else:                               # no prefix -- respect anchor flag
        if anchor:
            pattern_re = "^" + pattern_re

    return re.compile(pattern_re)
