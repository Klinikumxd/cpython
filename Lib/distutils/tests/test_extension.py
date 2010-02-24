"""Tests for distutils.extension."""
import os
import sys
import unittest
import warnings

from test.support import check_warnings
from distutils.extension import read_setup_file, Extension
from distutils.tests.support import capture_warnings

class ExtensionTestCase(unittest.TestCase):

    @capture_warnings
    def test_read_setup_file(self):
        # trying to read a Setup file
        # (sample extracted from the PyGame project)
        setup = os.path.join(os.path.dirname(__file__), 'Setup.sample')

        exts = read_setup_file(setup)
        names = [ext.name for ext in exts]
        names.sort()

        # here are the extensions read_setup_file should have created
        # out of the file
        wanted = ['_arraysurfarray', '_camera', '_numericsndarray',
                  '_numericsurfarray', 'base', 'bufferproxy', 'cdrom',
                  'color', 'constants', 'display', 'draw', 'event',
                  'fastevent', 'font', 'gfxdraw', 'image', 'imageext',
                  'joystick', 'key', 'mask', 'mixer', 'mixer_music',
                  'mouse', 'movie', 'overlay', 'pixelarray', 'pypm',
                  'rect', 'rwobject', 'scrap', 'surface', 'surflock',
                  'time', 'transform']

        self.assertEquals(names, wanted)

    @unittest.skipIf(sys.flags.optimize >= 2,
                     "Assertions are omitted with -O2 and above")
    def test_extension_init_assertions(self):
        # The first argument, which is the name, must be a string.
        self.assertRaises(AssertionError, Extension, 1, [])

        # the second argument, which is the list of files, must
        # be a list of strings
        self.assertRaises(AssertionError, Extension, 'name', 'file')
        self.assertRaises(AssertionError, Extension, 'name', ['file', 1])

    def test_extension_init(self):
        ext = Extension('name', [])
        self.assertEquals(ext.name, 'name')


        ext = Extension('name', ['file1', 'file2'])
        self.assertEquals(ext.sources, ['file1', 'file2'])

        # others arguments have defaults
        for attr in ('include_dirs', 'define_macros', 'undef_macros',
                     'library_dirs', 'libraries', 'runtime_library_dirs',
                     'extra_objects', 'extra_compile_args', 'extra_link_args',
                     'export_symbols', 'swig_opts', 'depends'):
            self.assertEquals(getattr(ext, attr), [])

        self.assertEquals(ext.language, None)
        self.assertEquals(ext.optional, None)

        # if there are unknown keyword options, warn about them
        with check_warnings() as w:
            warnings.simplefilter('always')
            ext = Extension('name', ['file1', 'file2'], chic=True)

        self.assertEquals(len(w.warnings), 1)
        self.assertEquals(str(w.warnings[0].message),
                          "Unknown Extension options: 'chic'")

def test_suite():
    return unittest.makeSuite(ExtensionTestCase)

if __name__ == "__main__":
    unittest.main(defaultTest="test_suite")
