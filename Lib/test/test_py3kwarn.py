import unittest
import sys
from test.test_support import (catch_warning, CleanImport,
                               TestSkipped, run_unittest)
import warnings

from contextlib import nested

if not sys.py3kwarning:
    raise TestSkipped('%s must be run with the -3 flag' % __name__)


class TestPy3KWarnings(unittest.TestCase):

    def test_backquote(self):
        expected = 'backquote not supported in 3.x; use repr()'
        with catch_warning() as w:
            exec "`2`" in {}
        self.assertWarning(None, w, expected)

    def test_bool_assign(self):
        # So we don't screw up our globals
        def safe_exec(expr):
            def f(**kwargs): pass
            exec expr in {'f' : f}

        expected = "assignment to True or False is forbidden in 3.x"
        with catch_warning() as w:
            safe_exec("True = False")
            self.assertWarning(None, w, expected)
            safe_exec("False = True")
            self.assertWarning(None, w, expected)
            try:
                safe_exec("obj.False = True")
            except NameError: pass
            self.assertWarning(None, w, expected)
            try:
                safe_exec("obj.True = False")
            except NameError: pass
            self.assertWarning(None, w, expected)
            safe_exec("def False(): pass")
            self.assertWarning(None, w, expected)
            safe_exec("def True(): pass")
            self.assertWarning(None, w, expected)
            safe_exec("class False: pass")
            self.assertWarning(None, w, expected)
            safe_exec("class True: pass")
            self.assertWarning(None, w, expected)
            safe_exec("def f(True=43): pass")
            self.assertWarning(None, w, expected)
            safe_exec("def f(False=None): pass")
            self.assertWarning(None, w, expected)
            safe_exec("f(False=True)")
            self.assertWarning(None, w, expected)
            safe_exec("f(True=1)")
            self.assertWarning(None, w, expected)


    def test_type_inequality_comparisons(self):
        expected = 'type inequality comparisons not supported in 3.x'
        with catch_warning() as w:
            self.assertWarning(int < str, w, expected)
            self.assertWarning(type < object, w, expected)

    def test_object_inequality_comparisons(self):
        expected = 'comparing unequal types not supported in 3.x'
        with catch_warning() as w:
            self.assertWarning(str < [], w, expected)
            self.assertWarning(object() < (1, 2), w, expected)

    def test_dict_inequality_comparisons(self):
        expected = 'dict inequality comparisons not supported in 3.x'
        with catch_warning() as w:
            self.assertWarning({} < {2:3}, w, expected)
            self.assertWarning({} <= {}, w, expected)
            self.assertWarning({} > {2:3}, w, expected)
            self.assertWarning({2:3} >= {}, w, expected)

    def test_cell_inequality_comparisons(self):
        expected = 'cell comparisons not supported in 3.x'
        def f(x):
            def g():
                return x
            return g
        cell0, = f(0).func_closure
        cell1, = f(1).func_closure
        with catch_warning() as w:
            self.assertWarning(cell0 == cell1, w, expected)
            self.assertWarning(cell0 < cell1, w, expected)

    def test_code_inequality_comparisons(self):
        expected = 'code inequality comparisons not supported in 3.x'
        def f(x):
            pass
        def g(x):
            pass
        with catch_warning() as w:
            self.assertWarning(f.func_code < g.func_code, w, expected)
            self.assertWarning(f.func_code <= g.func_code, w, expected)
            self.assertWarning(f.func_code >= g.func_code, w, expected)
            self.assertWarning(f.func_code > g.func_code, w, expected)

    def test_builtin_function_or_method_comparisons(self):
        expected = ('builtin_function_or_method '
                    'inequality comparisons not supported in 3.x')
        func = eval
        meth = {}.get
        with catch_warning() as w:
            self.assertWarning(func < meth, w, expected)
            self.assertWarning(func > meth, w, expected)
            self.assertWarning(meth <= func, w, expected)
            self.assertWarning(meth >= func, w, expected)

    def assertWarning(self, _, warning, expected_message):
        self.assertEqual(str(warning.message), expected_message)

    def test_sort_cmp_arg(self):
        expected = "the cmp argument is not supported in 3.x"
        lst = range(5)
        cmp = lambda x,y: -1

        with catch_warning() as w:
            self.assertWarning(lst.sort(cmp=cmp), w, expected)
            self.assertWarning(sorted(lst, cmp=cmp), w, expected)
            self.assertWarning(lst.sort(cmp), w, expected)
            self.assertWarning(sorted(lst, cmp), w, expected)

    def test_sys_exc_clear(self):
        expected = 'sys.exc_clear() not supported in 3.x; use except clauses'
        with catch_warning() as w:
            self.assertWarning(sys.exc_clear(), w, expected)

    def test_methods_members(self):
        expected = '__members__ and __methods__ not supported in 3.x'
        class C:
            __methods__ = ['a']
            __members__ = ['b']
        c = C()
        with catch_warning() as w:
            self.assertWarning(dir(c), w, expected)

    def test_softspace(self):
        expected = 'file.softspace not supported in 3.x'
        with file(__file__) as f:
            with catch_warning() as w:
                self.assertWarning(f.softspace, w, expected)
            def set():
                f.softspace = 0
            with catch_warning() as w:
                self.assertWarning(set(), w, expected)

    def test_tuple_parameter_unpacking(self):
        expected = "tuple parameter unpacking has been removed in 3.x"
        with catch_warning() as w:
            exec "def f((a, b)): pass"
            self.assertWarning(None, w, expected)

    def test_buffer(self):
        expected = 'buffer() not supported in 3.x; use memoryview()'
        with catch_warning() as w:
            self.assertWarning(buffer('a'), w, expected)

    def test_file_xreadlines(self):
        expected = ("f.xreadlines() not supported in 3.x, "
                    "try 'for line in f' instead")
        with file(__file__) as f:
            with catch_warning() as w:
                self.assertWarning(f.xreadlines(), w, expected)


class TestStdlibRemovals(unittest.TestCase):

    # test.testall not tested as it executes all unit tests as an
    # import side-effect.
    all_platforms = ('audiodev', 'imputil', 'mutex', 'user', 'new', 'rexec',
                        'Bastion', 'compiler', 'dircache', 'mimetools', 'fpformat',
                        'ihooks', 'mhlib', 'statvfs', 'htmllib', 'sgmllib', 'rfc822')
    inclusive_platforms = {'irix' : ('pure', 'AL', 'al', 'CD', 'cd', 'cddb',
                                     'cdplayer', 'CL', 'cl', 'DEVICE', 'GL',
                                     'gl', 'ERRNO', 'FILE', 'FL', 'flp', 'fl',
                                     'fm', 'GET', 'GLWS', 'imgfile', 'IN',
                                     'IOCTL', 'jpeg', 'panel', 'panelparser',
                                     'readcd', 'SV', 'torgb', 'WAIT'),
                          'darwin' : ('autoGIL', 'Carbon', 'OSATerminology',
                                      'icglue', 'Nav', 'MacOS', 'aepack',
                                      'aetools', 'aetypes', 'applesingle',
                                      'appletrawmain', 'appletrunner',
                                      'argvemulator', 'bgenlocations',
                                      'EasyDialogs', 'macerrors', 'macostools',
                                      'findertools', 'FrameWork', 'ic',
                                      'gensuitemodule', 'icopen', 'macresource',
                                      'MiniAEFrame', 'pimp', 'PixMapWrapper',
                                      'terminalcommand', 'videoreader',
                                      '_builtinSuites', 'CodeWarrior',
                                      'Explorer', 'Finder', 'Netscape',
                                      'StdSuites', 'SystemEvents', 'Terminal',
                                      'cfmfile', 'bundlebuilder', 'buildtools',
                                      'ColorPicker', 'Audio_mac'),
                           'sunos5' : ('sunaudiodev', 'SUNAUDIODEV'),
                          }
    optional_modules = ('bsddb185', 'Canvas', 'dl', 'linuxaudiodev', 'imageop',
                        'sv', 'cPickle')

    def check_removal(self, module_name, optional=False):
        """Make sure the specified module, when imported, raises a
        DeprecationWarning and specifies itself in the message."""
        with nested(CleanImport(module_name), catch_warning(record=False)):
            warnings.filterwarnings("error", ".+ removed",
                                    DeprecationWarning, __name__)
            try:
                __import__(module_name, level=0)
            except DeprecationWarning as exc:
                self.assert_(module_name in exc.args[0],
                             "%s warning didn't contain module name"
                             % module_name)
            except ImportError:
                if not optional:
                    self.fail("Non-optional module {0} raised an "
                              "ImportError.".format(module_name))
            else:
                self.fail("DeprecationWarning not raised for {0}"
                            .format(module_name))

    def test_platform_independent_removals(self):
        # Make sure that the modules that are available on all platforms raise
        # the proper DeprecationWarning.
        for module_name in self.all_platforms:
            self.check_removal(module_name)

    def test_platform_specific_removals(self):
        # Test the removal of platform-specific modules.
        for module_name in self.inclusive_platforms.get(sys.platform, []):
            self.check_removal(module_name, optional=True)

    def test_optional_module_removals(self):
        # Test the removal of modules that may or may not be built.
        for module_name in self.optional_modules:
            self.check_removal(module_name, optional=True)

    def test_os_path_walk(self):
        msg = "In 3.x, os.path.walk is removed in favor of os.walk."
        def dumbo(where, names, args): pass
        for path_mod in ("ntpath", "macpath", "os2emxpath", "posixpath"):
            mod = __import__(path_mod)
            with catch_warning() as w:
                mod.walk("crashers", dumbo, None)
            self.assertEquals(str(w.message), msg)

    def test_commands_members(self):
        import commands
        members = {"mk2arg" : 2, "mkarg" : 1, "getstatus" : 1}
        for name, arg_count in members.items():
            with catch_warning(record=False):
                warnings.filterwarnings("error")
                func = getattr(commands, name)
                self.assertRaises(DeprecationWarning, func, *([None]*arg_count))

    def test_mutablestring_removal(self):
        # UserString.MutableString has been removed in 3.0.
        import UserString
        with catch_warning(record=False):
            warnings.filterwarnings("error", ".*MutableString",
                                    DeprecationWarning)
            self.assertRaises(DeprecationWarning, UserString.MutableString)


def test_main():
    with catch_warning():
        warnings.simplefilter("always")
        run_unittest(TestPy3KWarnings,
                     TestStdlibRemovals)

if __name__ == '__main__':
    test_main()
