"""distutils.ccompiler

Contains CCompiler, an abstract base class that defines the interface
for the Distutils compiler abstraction model."""

# created 1999/07/05, Greg Ward

__revision__ = "$Id$"

import sys, os
from types import *
from copy import copy
from distutils.errors import *
from distutils.spawn import spawn
from distutils.util import move_file, mkpath, newer_pairwise, newer_group


class CCompiler:
    """Abstract base class to define the interface that must be implemented
       by real compiler abstraction classes.  Might have some use as a
       place for shared code, but it's not yet clear what code can be
       shared between compiler abstraction models for different platforms.

       The basic idea behind a compiler abstraction class is that each
       instance can be used for all the compile/link steps in building
       a single project.  Thus, attributes common to all of those compile
       and link steps -- include directories, macros to define, libraries
       to link against, etc. -- are attributes of the compiler instance.
       To allow for variability in how individual files are treated,
       most (all?) of those attributes may be varied on a per-compilation
       or per-link basis."""

    # 'compiler_type' is a class attribute that identifies this class.  It
    # keeps code that wants to know what kind of compiler it's dealing with
    # from having to import all possible compiler classes just to do an
    # 'isinstance'.  In concrete CCompiler subclasses, 'compiler_type'
    # should really, really be one of the keys of the 'compiler_class'
    # dictionary (see below -- used by the 'new_compiler()' factory
    # function) -- authors of new compiler interface classes are
    # responsible for updating 'compiler_class'!
    compiler_type = None

    # XXX things not handled by this compiler abstraction model:
    #   * client can't provide additional options for a compiler,
    #     e.g. warning, optimization, debugging flags.  Perhaps this
    #     should be the domain of concrete compiler abstraction classes
    #     (UnixCCompiler, MSVCCompiler, etc.) -- or perhaps the base
    #     class should have methods for the common ones.
    #   * can't put output files (object files, libraries, whatever)
    #     into a separate directory from their inputs.  Should this be
    #     handled by an 'output_dir' attribute of the whole object, or a
    #     parameter to the compile/link_* methods, or both?
    #   * can't completely override the include or library searchg
    #     path, ie. no "cc -I -Idir1 -Idir2" or "cc -L -Ldir1 -Ldir2".
    #     I'm not sure how widely supported this is even by Unix
    #     compilers, much less on other platforms.  And I'm even less
    #     sure how useful it is; maybe for cross-compiling, but
    #     support for that is a ways off.  (And anyways, cross
    #     compilers probably have a dedicated binary with the
    #     right paths compiled in.  I hope.)
    #   * can't do really freaky things with the library list/library
    #     dirs, e.g. "-Ldir1 -lfoo -Ldir2 -lfoo" to link against
    #     different versions of libfoo.a in different locations.  I
    #     think this is useless without the ability to null out the
    #     library search path anyways.
    

    # Subclasses that rely on the standard filename generation methods
    # implemented below should override these; see the comment near
    # those methods ('object_filenames()' et. al.) for details:
    src_extensions = None               # list of strings
    obj_extension = None                # string
    static_lib_extension = None
    shared_lib_extension = None         # string
    static_lib_format = None            # format string
    shared_lib_format = None            # prob. same as static_lib_format
    exe_extension = None                # string


    def __init__ (self,
                  verbose=0,
                  dry_run=0,
                  force=0):

        self.verbose = verbose
        self.dry_run = dry_run
        self.force = force

        # 'output_dir': a common output directory for object, library,
        # shared object, and shared library files
        self.output_dir = None

        # 'macros': a list of macro definitions (or undefinitions).  A
        # macro definition is a 2-tuple (name, value), where the value is
        # either a string or None (no explicit value).  A macro
        # undefinition is a 1-tuple (name,).
        self.macros = []

        # 'include_dirs': a list of directories to search for include files
        self.include_dirs = []

        # 'libraries': a list of libraries to include in any link
        # (library names, not filenames: eg. "foo" not "libfoo.a")
        self.libraries = []

        # 'library_dirs': a list of directories to search for libraries
        self.library_dirs = []

        # 'runtime_library_dirs': a list of directories to search for
        # shared libraries/objects at runtime
        self.runtime_library_dirs = []

        # 'objects': a list of object files (or similar, such as explicitly
        # named library files) to include on any link
        self.objects = []

    # __init__ ()


    def _find_macro (self, name):
        i = 0
        for defn in self.macros:
            if defn[0] == name:
                return i
            i = i + 1

        return None


    def _check_macro_definitions (self, definitions):
        """Ensures that every element of 'definitions' is a valid macro
           definition, ie. either (name,value) 2-tuple or a (name,)
           tuple.  Do nothing if all definitions are OK, raise 
           TypeError otherwise."""

        for defn in definitions:
            if not (type (defn) is TupleType and
                    (len (defn) == 1 or
                     (len (defn) == 2 and
                      (type (defn[1]) is StringType or defn[1] is None))) and
                    type (defn[0]) is StringType):
                raise TypeError, \
                      ("invalid macro definition '%s': " % defn) + \
                      "must be tuple (string,), (string, string), or " + \
                      "(string, None)"


    # -- Bookkeeping methods -------------------------------------------

    def define_macro (self, name, value=None):
        """Define a preprocessor macro for all compilations driven by
           this compiler object.  The optional parameter 'value' should be
           a string; if it is not supplied, then the macro will be defined
           without an explicit value and the exact outcome depends on the
           compiler used (XXX true? does ANSI say anything about this?)"""

        # Delete from the list of macro definitions/undefinitions if
        # already there (so that this one will take precedence).
        i = self._find_macro (name)
        if i is not None:
            del self.macros[i]

        defn = (name, value)
        self.macros.append (defn)


    def undefine_macro (self, name):
        """Undefine a preprocessor macro for all compilations driven by
           this compiler object.  If the same macro is defined by
           'define_macro()' and undefined by 'undefine_macro()' the last
           call takes precedence (including multiple redefinitions or
           undefinitions).  If the macro is redefined/undefined on a
           per-compilation basis (ie. in the call to 'compile()'), then
           that takes precedence."""

        # Delete from the list of macro definitions/undefinitions if
        # already there (so that this one will take precedence).
        i = self._find_macro (name)
        if i is not None:
            del self.macros[i]

        undefn = (name,)
        self.macros.append (undefn)


    def add_include_dir (self, dir):
        """Add 'dir' to the list of directories that will be searched
           for header files.  The compiler is instructed to search
           directories in the order in which they are supplied by
           successive calls to 'add_include_dir()'."""
        self.include_dirs.append (dir)

    def set_include_dirs (self, dirs):
        """Set the list of directories that will be searched to 'dirs'
           (a list of strings).  Overrides any preceding calls to
           'add_include_dir()'; subsequence calls to 'add_include_dir()'
           add to the list passed to 'set_include_dirs()'.  This does
           not affect any list of standard include directories that
           the compiler may search by default."""
        self.include_dirs = copy (dirs)


    def add_library (self, libname):
        """Add 'libname' to the list of libraries that will be included
           in all links driven by this compiler object.  Note that
           'libname' should *not* be the name of a file containing a
           library, but the name of the library itself: the actual filename
           will be inferred by the linker, the compiler, or the compiler
           abstraction class (depending on the platform).

           The linker will be instructed to link against libraries in the
           order they were supplied to 'add_library()' and/or
           'set_libraries()'.  It is perfectly valid to duplicate library
           names; the linker will be instructed to link against libraries
           as many times as they are mentioned."""
        self.libraries.append (libname)

    def set_libraries (self, libnames):
        """Set the list of libraries to be included in all links driven
           by this compiler object to 'libnames' (a list of strings).
           This does not affect any standard system libraries that the
           linker may include by default."""

        self.libraries = copy (libnames)


    def add_library_dir (self, dir):
        """Add 'dir' to the list of directories that will be searched for
           libraries specified to 'add_library()' and 'set_libraries()'.
           The linker will be instructed to search for libraries in the
           order they are supplied to 'add_library_dir()' and/or
           'set_library_dirs()'."""
        self.library_dirs.append (dir)

    def set_library_dirs (self, dirs):
        """Set the list of library search directories to 'dirs' (a list
           of strings).  This does not affect any standard library
           search path that the linker may search by default."""
        self.library_dirs = copy (dirs)


    def add_runtime_library_dir (self, dir):
        """Add 'dir' to the list of directories that will be searched for
           shared libraries at runtime."""
        self.runtime_library_dirs.append (dir)

    def set_runtime_library_dirs (self, dirs):
        """Set the list of directories to search for shared libraries
           at runtime to 'dirs' (a list of strings).  This does not affect
           any standard search path that the runtime linker may search by
           default."""
        self.runtime_library_dirs = copy (dirs)


    def add_link_object (self, object):
        """Add 'object' to the list of object files (or analogues, such
           as explictly named library files or the output of "resource
           compilers") to be included in every link driven by this
           compiler object."""
        self.objects.append (object)

    def set_link_objects (self, objects):
        """Set the list of object files (or analogues) to be included
           in every link to 'objects'.  This does not affect any
           standard object files that the linker may include by default
           (such as system libraries)."""
        self.objects = copy (objects)


    # -- Priviate utility methods --------------------------------------
    # (here for the convenience of subclasses)

    def _fix_compile_args (self, output_dir, macros, include_dirs):
        """Typecheck and fix-up some of the arguments to the 'compile()' method,
           and return fixed-up values.  Specifically: if 'output_dir' is
           None, replaces it with 'self.output_dir'; ensures that 'macros'
           is a list, and augments it with 'self.macros'; ensures that
           'include_dirs' is a list, and augments it with
           'self.include_dirs'.  Guarantees that the returned values are of
           the correct type, i.e. for 'output_dir' either string or None,
           and for 'macros' and 'include_dirs' either list or None."""

        if output_dir is None:
            output_dir = self.output_dir
        elif type (output_dir) is not StringType:
            raise TypeError, "'output_dir' must be a string or None"

        if macros is None:
            macros = self.macros
        elif type (macros) is ListType:
            macros = macros + (self.macros or [])
        else:
            raise TypeError, \
                  "'macros' (if supplied) must be a list of tuples"

        if include_dirs is None:
            include_dirs = self.include_dirs
        elif type (include_dirs) in (ListType, TupleType):
            include_dirs = list (include_dirs) + (self.include_dirs or [])
        else:
            raise TypeError, \
                  "'include_dirs' (if supplied) must be a list of strings"
                    
        return (output_dir, macros, include_dirs)

    # _fix_compile_args ()


    def _prep_compile (self, sources, output_dir):
        """Determine the list of object files corresponding to 'sources', and
           figure out which ones really need to be recompiled.  Return a list
           of all object files and a dictionary telling which source files can
           be skipped."""

        # Get the list of expected output (object) files 
        objects = self.object_filenames (sources,
                                         output_dir=output_dir)

        if self.force:
            skip_source = {}            # rebuild everything
            for source in sources:
                skip_source[source] = 0
        else:
            # Figure out which source files we have to recompile according
            # to a simplistic check -- we just compare the source and
            # object file, no deep dependency checking involving header
            # files.
            skip_source = {}            # rebuild everything
            for source in sources:      # no wait, rebuild nothing
                skip_source[source] = 1

            (n_sources, n_objects) = newer_pairwise (sources, objects)
            for source in n_sources:    # no really, only rebuild what's out-of-date
                skip_source[source] = 0

        return (objects, skip_source)

    # _prep_compile ()


    def _fix_object_args (self, objects, output_dir):
        """Typecheck and fix up some arguments supplied to various
           methods.  Specifically: ensure that 'objects' is a list; if
           output_dir is None, replace with self.output_dir.  Return fixed
           versions of 'objects' and 'output_dir'."""

        if type (objects) not in (ListType, TupleType):
            raise TypeError, \
                  "'objects' must be a list or tuple of strings"
        objects = list (objects)
            
        if output_dir is None:
            output_dir = self.output_dir
        elif type (output_dir) is not StringType:
            raise TypeError, "'output_dir' must be a string or None"

        return (objects, output_dir)


    def _fix_lib_args (self, libraries, library_dirs, runtime_library_dirs):
        """Typecheck and fix up some of the arguments supplied to the
           'link_*' methods.  Specifically: ensure that all arguments are
           lists, and augment them with their permanent versions
           (eg. 'self.libraries' augments 'libraries').  Return a tuple
           with fixed versions of all arguments."""

        if libraries is None:
            libraries = self.libraries
        elif type (libraries) in (ListType, TupleType):
            libraries = list (libraries) + (self.libraries or [])
        else:
            raise TypeError, \
                  "'libraries' (if supplied) must be a list of strings"

        if library_dirs is None:
            library_dirs = self.library_dirs
        elif type (library_dirs) in (ListType, TupleType):
            library_dirs = list (library_dirs) + (self.library_dirs or [])
        else:
            raise TypeError, \
                  "'library_dirs' (if supplied) must be a list of strings"

        if runtime_library_dirs is None:
            runtime_library_dirs = self.runtime_library_dirs
        elif type (runtime_library_dirs) in (ListType, TupleType):
            runtime_library_dirs = (list (runtime_library_dirs) +
                                    (self.runtime_library_dirs or []))
        else:
            raise TypeError, \
                  "'runtime_library_dirs' (if supplied) " + \
                  "must be a list of strings"

        return (libraries, library_dirs, runtime_library_dirs)

    # _fix_lib_args ()


    def _need_link (self, objects, output_file):
        """Return true if we need to relink the files listed in 'objects' to
           recreate 'output_file'."""

        if self.force:
            return 1
        else:
            if self.dry_run:
                newer = newer_group (objects, output_file, missing='newer')
            else:
                newer = newer_group (objects, output_file)
            return newer

    # _need_link ()


    # -- Worker methods ------------------------------------------------
    # (must be implemented by subclasses)

    def compile (self,
                 sources,
                 output_dir=None,
                 macros=None,
                 include_dirs=None,
                 debug=0,
                 extra_preargs=None,
                 extra_postargs=None):
        """Compile one or more C/C++ source files.  'sources' must be
           a list of strings, each one the name of a C/C++ source
           file.  Return a list of object filenames, one per source
           filename in 'sources'.  Depending on the implementation,
           not all source files will necessarily be compiled, but
           all corresponding object filenames will be returned.

           If 'output_dir' is given, object files will be put under it,
           while retaining their original path component.  That is,
           "foo/bar.c" normally compiles to "foo/bar.o" (for a Unix
           implementation); if 'output_dir' is "build", then it would
           compile to "build/foo/bar.o".

           'macros', if given, must be a list of macro definitions.  A
           macro definition is either a (name, value) 2-tuple or a (name,)
           1-tuple.  The former defines a macro; if the value is None, the
           macro is defined without an explicit value.  The 1-tuple case
           undefines a macro.  Later definitions/redefinitions/
           undefinitions take precedence.

           'include_dirs', if given, must be a list of strings, the
           directories to add to the default include file search path for
           this compilation only.

           'debug' is a boolean; if true, the compiler will be instructed
           to output debug symbols in (or alongside) the object file(s).

           'extra_preargs' and 'extra_postargs' are implementation-
           dependent.  On platforms that have the notion of a command-line
           (e.g. Unix, DOS/Windows), they are most likely lists of strings:
           extra command-line arguments to prepand/append to the compiler
           command line.  On other platforms, consult the implementation
           class documentation.  In any event, they are intended as an
           escape hatch for those occasions when the abstract compiler
           framework doesn't cut the mustard."""
           
        pass


    def create_static_lib (self,
                           objects,
                           output_libname,
                           output_dir=None,
                           debug=0):
        """Link a bunch of stuff together to create a static library
           file.  The "bunch of stuff" consists of the list of object
           files supplied as 'objects', the extra object files supplied
           to 'add_link_object()' and/or 'set_link_objects()', the
           libraries supplied to 'add_library()' and/or
           'set_libraries()', and the libraries supplied as 'libraries'
           (if any).

           'output_libname' should be a library name, not a filename; the
           filename will be inferred from the library name.  'output_dir'
           is the directory where the library file will be put.

           'debug' is a boolean; if true, debugging information will be
           included in the library (note that on most platforms, it is the
           compile step where this matters: the 'debug' flag is included
           here just for consistency)."""

        pass
    

    def link_shared_lib (self,
                         objects,
                         output_libname,
                         output_dir=None,
                         libraries=None,
                         library_dirs=None,
                         runtime_library_dirs=None,
                         export_symbols=None,
                         debug=0,
                         extra_preargs=None,
                         extra_postargs=None):
        """Link a bunch of stuff together to create a shared library
           file.  Similar semantics to 'create_static_lib()', with the
           addition of other libraries to link against and directories to
           search for them.  Also, of course, the type and name of
           the generated file will almost certainly be different, as will
           the program used to create it.

           'libraries' is a list of libraries to link against.  These are
           library names, not filenames, since they're translated into
           filenames in a platform-specific way (eg. "foo" becomes
           "libfoo.a" on Unix and "foo.lib" on DOS/Windows).  However, they
           can include a directory component, which means the linker will
           look in that specific directory rather than searching all the
           normal locations.

           'library_dirs', if supplied, should be a list of directories to
           search for libraries that were specified as bare library names
           (ie. no directory component).  These are on top of the system
           default and those supplied to 'add_library_dir()' and/or
           'set_library_dirs()'.  'runtime_library_dirs' is a list of
           directories that will be embedded into the shared library and
           used to search for other shared libraries that *it* depends on
           at run-time.  (This may only be relevant on Unix.)

           'export_symbols' is a list of symbols that the shared library
           will export.  (This appears to be relevant only on Windows.)

           'debug' is as for 'compile()' and 'create_static_lib()', with the
           slight distinction that it actually matters on most platforms
           (as opposed to 'create_static_lib()', which includes a 'debug'
           flag mostly for form's sake).

           'extra_preargs' and 'extra_postargs' are as for 'compile()'
           (except of course that they supply command-line arguments
           for the particular linker being used)."""           

        pass
    

    def link_shared_object (self,
                            objects,
                            output_filename,
                            output_dir=None,
                            libraries=None,
                            library_dirs=None,
                            runtime_library_dirs=None,
                            export_symbols=None,
                            debug=0,
                            extra_preargs=None,
                            extra_postargs=None):
        """Link a bunch of stuff together to create a shared object
           file.  Much like 'link_shared_lib()', except the output filename
           is explicitly supplied as 'output_filename'.  If 'output_dir' is
           supplied, 'output_filename' is relative to it
           (i.e. 'output_filename' can provide directory components if
           needed)."""
        pass


    def link_executable (self,
                         objects,
                         output_progname,
                         output_dir=None,
                         libraries=None,
                         library_dirs=None,
                         runtime_library_dirs=None,
                         debug=0,
                         extra_preargs=None,
                         extra_postargs=None):
        """Link a bunch of stuff together to create a binary executable
           file.  The "bunch of stuff" is as for 'link_shared_lib()'.
           'output_progname' should be the base name of the executable
           program--e.g. on Unix the same as the output filename, but
           on DOS/Windows ".exe" will be appended."""
        pass



    # -- Miscellaneous methods -----------------------------------------
    # These are all used by the 'gen_lib_options() function; there is
    # no appropriate default implementation so subclasses should
    # implement all of these.

    def library_dir_option (self, dir):
        """Return the compiler option to add 'dir' to the list of directories
           searched for libraries."""
        raise NotImplementedError

    def runtime_library_dir_option (self, dir):
        """Return the compiler option to add 'dir' to the list of directories
           searched for runtime libraries."""
        raise NotImplementedError

    def library_option (self, lib):
        """Return the compiler option to add 'dir' to the list of libraries
           linked into the shared library or executable."""
        raise NotImplementedError

    def find_library_file (self, dirs, lib):
        """Search the specified list of directories for a static or shared
           library file 'lib' and return the full path to that file. Return
           None if it wasn't found in any of the specified directories."""
        raise NotImplementedError


    # -- Filename generation methods -----------------------------------

    # The default implementation of the filename generating methods are
    # prejudiced towards the Unix/DOS/Windows view of the world:
    #   * object files are named by replacing the source file extension
    #     (eg. .c/.cpp -> .o/.obj)
    #   * library files (shared or static) are named by plugging the
    #     library name and extension into a format string, eg.
    #     "lib%s.%s" % (lib_name, ".a") for Unix static libraries
    #   * executables are named by appending an extension (possibly
    #     empty) to the program name: eg. progname + ".exe" for
    #     Windows
    #
    # To reduce redundant code, these methods expect to find
    # several attributes in the current object (presumably defined
    # as class attributes):
    #   * src_extensions -
    #     list of C/C++ source file extensions, eg. ['.c', '.cpp']
    #   * obj_extension -
    #     object file extension, eg. '.o' or '.obj'
    #   * static_lib_extension -
    #     extension for static library files, eg. '.a' or '.lib'
    #   * shared_lib_extension -
    #     extension for shared library/object files, eg. '.so', '.dll'
    #   * static_lib_format -
    #     format string for generating static library filenames,
    #     eg. 'lib%s.%s' or '%s.%s'
    #   * shared_lib_format
    #     format string for generating shared library filenames
    #     (probably same as static_lib_format, since the extension
    #     is one of the intended parameters to the format string)
    #   * exe_extension -
    #     extension for executable files, eg. '' or '.exe'

    def object_filenames (self,
                          source_filenames,
                          strip_dir=0,
                          output_dir=''):
        if output_dir is None: output_dir = ''
        obj_names = []
        for src_name in source_filenames:
            (base, ext) = os.path.splitext (src_name)
            if ext not in self.src_extensions:
                continue
            if strip_dir:
                base = os.path.basename (base)
            obj_names.append (os.path.join (output_dir,
                                            base + self.obj_extension))
        return obj_names

    # object_filenames ()


    def shared_object_filename (self,
                                basename,
                                strip_dir=0,
                                output_dir=''):
        if output_dir is None: output_dir = ''
        if strip_dir:
            basename = os.path.basename (basename)
        return os.path.join (output_dir, basename + self.shared_lib_extension)


    def library_filename (self,
                          libname,
                          lib_type='static',     # or 'shared'
                          strip_dir=0,
                          output_dir=''):

        if output_dir is None: output_dir = ''
        if lib_type not in ("static","shared"):
            raise ValueError, "'lib_type' must be \"static\" or \"shared\""
        fmt = getattr (self, lib_type + "_lib_format")
        ext = getattr (self, lib_type + "_lib_extension")

        (dir, base) = os.path.split (libname)
        filename = fmt % (base, ext)
        if strip_dir:
            dir = ''

        return os.path.join (output_dir, dir, filename)


    # -- Utility methods -----------------------------------------------

    def announce (self, msg, level=1):
        if self.verbose >= level:
            print msg

    def warn (self, msg):
        sys.stderr.write ("warning: %s\n" % msg)

    def spawn (self, cmd):
        spawn (cmd, verbose=self.verbose, dry_run=self.dry_run)

    def move_file (self, src, dst):
        return move_file (src, dst, verbose=self.verbose, dry_run=self.dry_run)

    def mkpath (self, name, mode=0777):
        mkpath (name, mode, self.verbose, self.dry_run)


# class CCompiler


# Map a platform ('posix', 'nt') to the default compiler type for
# that platform.
default_compiler = { 'posix': 'unix',
                     'nt': 'msvc',
                   }

# Map compiler types to (module_name, class_name) pairs -- ie. where to
# find the code that implements an interface to this compiler.  (The module
# is assumed to be in the 'distutils' package.)
compiler_class = { 'unix': ('unixccompiler', 'UnixCCompiler'),
                   'msvc': ('msvccompiler', 'MSVCCompiler'),
                 }


def new_compiler (plat=None,
                  compiler=None,
                  verbose=0,
                  dry_run=0,
                  force=0):

    """Generate an instance of some CCompiler subclass for the supplied
       platform/compiler combination.  'plat' defaults to 'os.name'
       (eg. 'posix', 'nt'), and 'compiler' defaults to the default
       compiler for that platform.  Currently only 'posix' and 'nt'
       are supported, and the default compilers are "traditional Unix
       interface" (UnixCCompiler class) and Visual C++ (MSVCCompiler
       class).  Note that it's perfectly possible to ask for a Unix
       compiler object under Windows, and a Microsoft compiler object
       under Unix -- if you supply a value for 'compiler', 'plat'
       is ignored."""

    if plat is None:
        plat = os.name

    try:
        if compiler is None:
            compiler = default_compiler[plat]
        
        (module_name, class_name) = compiler_class[compiler]
    except KeyError:
        msg = "don't know how to compile C/C++ code on platform '%s'" % plat
        if compiler is not None:
            msg = msg + " with '%s' compiler" % compiler
        raise DistutilsPlatformError, msg
              
    try:
        module_name = "distutils." + module_name
        __import__ (module_name)
        module = sys.modules[module_name]
        klass = vars(module)[class_name]
    except ImportError:
        raise DistutilsModuleError, \
              "can't compile C/C++ code: unable to load module '%s'" % \
              module_name
    except KeyError:
        raise DistutilsModuleError, \
              ("can't compile C/C++ code: unable to find class '%s' " +
               "in module '%s'") % (class_name, module_name)

    return klass (verbose, dry_run, force)


def gen_preprocess_options (macros, include_dirs):
    """Generate C pre-processor options (-D, -U, -I) as used by at
       least two types of compilers: the typical Unix compiler and Visual
       C++.  'macros' is the usual thing, a list of 1- or 2-tuples, where
       (name,) means undefine (-U) macro 'name', and (name,value) means
       define (-D) macro 'name' to 'value'.  'include_dirs' is just a list of
       directory names to be added to the header file search path (-I).
       Returns a list of command-line options suitable for either
       Unix compilers or Visual C++."""
    
    # XXX it would be nice (mainly aesthetic, and so we don't generate
    # stupid-looking command lines) to go over 'macros' and eliminate
    # redundant definitions/undefinitions (ie. ensure that only the
    # latest mention of a particular macro winds up on the command
    # line).  I don't think it's essential, though, since most (all?)
    # Unix C compilers only pay attention to the latest -D or -U
    # mention of a macro on their command line.  Similar situation for
    # 'include_dirs'.  I'm punting on both for now.  Anyways, weeding out
    # redundancies like this should probably be the province of
    # CCompiler, since the data structures used are inherited from it
    # and therefore common to all CCompiler classes.

    pp_opts = []
    for macro in macros:

        if not (type (macro) is TupleType and
                1 <= len (macro) <= 2):
            raise TypeError, \
                  ("bad macro definition '%s': " +
                   "each element of 'macros' list must be a 1- or 2-tuple") % \
                  macro

        if len (macro) == 1:        # undefine this macro
            pp_opts.append ("-U%s" % macro[0])
        elif len (macro) == 2:
            if macro[1] is None:    # define with no explicit value
                pp_opts.append ("-D%s" % macro[0])
            else:
                # XXX *don't* need to be clever about quoting the
                # macro value here, because we're going to avoid the
                # shell at all costs when we spawn the command!
                pp_opts.append ("-D%s=%s" % macro)

    for dir in include_dirs:
        pp_opts.append ("-I%s" % dir)

    return pp_opts

# gen_preprocess_options ()


def gen_lib_options (compiler, library_dirs, runtime_library_dirs, libraries):
    """Generate linker options for searching library directories and
       linking with specific libraries.  'libraries' and 'library_dirs'
       are, respectively, lists of library names (not filenames!) and
       search directories.  Returns a list of command-line options suitable
       for use with some compiler (depending on the two format strings
       passed in)."""

    lib_opts = []

    for dir in library_dirs:
        lib_opts.append (compiler.library_dir_option (dir))

    for dir in runtime_library_dirs:
        lib_opts.append (compiler.runtime_library_dir_option (dir))

    # XXX it's important that we *not* remove redundant library mentions!
    # sometimes you really do have to say "-lfoo -lbar -lfoo" in order to
    # resolve all symbols.  I just hope we never have to say "-lfoo obj.o
    # -lbar" to get things to work -- that's certainly a possibility, but a
    # pretty nasty way to arrange your C code.

    for lib in libraries:
        (lib_dir, lib_name) = os.path.split (lib)
        if lib_dir:
            lib_file = compiler.find_library_file ([lib_dir], lib_name)
            if lib_file:
                lib_opts.append (lib_file)
            else:
                compiler.warn ("no library file corresponding to "
                               "'%s' found (skipping)" % lib)
        else:
            lib_opts.append (compiler.library_option (lib))

    return lib_opts

# gen_lib_options ()
