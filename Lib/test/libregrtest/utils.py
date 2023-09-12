import atexit
import contextlib
import faulthandler
import locale
import math
import os.path
import platform
import random
import sys
import sysconfig
import tempfile
import textwrap

from test import support
from test.support import os_helper
from test.support import threading_helper


MS_WINDOWS = (sys.platform == 'win32')

# bpo-38203: Maximum delay in seconds to exit Python (call Py_Finalize()).
# Used to protect against threading._shutdown() hang.
# Must be smaller than buildbot "1200 seconds without output" limit.
EXIT_TIMEOUT = 120.0


# Types for types hints
StrPath = str
TestName = str
StrJSON = str
TestTuple = tuple[TestName, ...]
TestList = list[TestName]
# --match and --ignore options: list of patterns
# ('*' joker character can be used)
FilterTuple = tuple[TestName, ...]
FilterDict = dict[TestName, FilterTuple]


def format_duration(seconds):
    ms = math.ceil(seconds * 1e3)
    seconds, ms = divmod(ms, 1000)
    minutes, seconds = divmod(seconds, 60)
    hours, minutes = divmod(minutes, 60)

    parts = []
    if hours:
        parts.append('%s hour' % hours)
    if minutes:
        parts.append('%s min' % minutes)
    if seconds:
        if parts:
            # 2 min 1 sec
            parts.append('%s sec' % seconds)
        else:
            # 1.0 sec
            parts.append('%.1f sec' % (seconds + ms / 1000))
    if not parts:
        return '%s ms' % ms

    parts = parts[:2]
    return ' '.join(parts)


def strip_py_suffix(names: list[str]):
    if not names:
        return
    for idx, name in enumerate(names):
        basename, ext = os.path.splitext(name)
        if ext == '.py':
            names[idx] = basename


def plural(n, singular, plural=None):
    if n == 1:
        return singular
    elif plural is not None:
        return plural
    else:
        return singular + 's'


def count(n, word):
    if n == 1:
        return f"{n} {word}"
    else:
        return f"{n} {word}s"


def printlist(x, width=70, indent=4, file=None):
    """Print the elements of iterable x to stdout.

    Optional arg width (default 70) is the maximum line length.
    Optional arg indent (default 4) is the number of blanks with which to
    begin each line.
    """

    blanks = ' ' * indent
    # Print the sorted list: 'x' may be a '--random' list or a set()
    print(textwrap.fill(' '.join(str(elt) for elt in sorted(x)), width,
                        initial_indent=blanks, subsequent_indent=blanks),
          file=file)


def print_warning(msg):
    support.print_warning(msg)


orig_unraisablehook = None


def regrtest_unraisable_hook(unraisable):
    global orig_unraisablehook
    support.environment_altered = True
    support.print_warning("Unraisable exception")
    old_stderr = sys.stderr
    try:
        support.flush_std_streams()
        sys.stderr = support.print_warning.orig_stderr
        orig_unraisablehook(unraisable)
        sys.stderr.flush()
    finally:
        sys.stderr = old_stderr


def setup_unraisable_hook():
    global orig_unraisablehook
    orig_unraisablehook = sys.unraisablehook
    sys.unraisablehook = regrtest_unraisable_hook


orig_threading_excepthook = None


def regrtest_threading_excepthook(args):
    global orig_threading_excepthook
    support.environment_altered = True
    support.print_warning(f"Uncaught thread exception: {args.exc_type.__name__}")
    old_stderr = sys.stderr
    try:
        support.flush_std_streams()
        sys.stderr = support.print_warning.orig_stderr
        orig_threading_excepthook(args)
        sys.stderr.flush()
    finally:
        sys.stderr = old_stderr


def setup_threading_excepthook():
    global orig_threading_excepthook
    import threading
    orig_threading_excepthook = threading.excepthook
    threading.excepthook = regrtest_threading_excepthook


def clear_caches():
    # Clear the warnings registry, so they can be displayed again
    for mod in sys.modules.values():
        if hasattr(mod, '__warningregistry__'):
            del mod.__warningregistry__

    # Flush standard output, so that buffered data is sent to the OS and
    # associated Python objects are reclaimed.
    for stream in (sys.stdout, sys.stderr, sys.__stdout__, sys.__stderr__):
        if stream is not None:
            stream.flush()

    try:
        re = sys.modules['re']
    except KeyError:
        pass
    else:
        re.purge()

    try:
        _strptime = sys.modules['_strptime']
    except KeyError:
        pass
    else:
        _strptime._regex_cache.clear()

    try:
        urllib_parse = sys.modules['urllib.parse']
    except KeyError:
        pass
    else:
        urllib_parse.clear_cache()

    try:
        urllib_request = sys.modules['urllib.request']
    except KeyError:
        pass
    else:
        urllib_request.urlcleanup()

    try:
        linecache = sys.modules['linecache']
    except KeyError:
        pass
    else:
        linecache.clearcache()

    try:
        mimetypes = sys.modules['mimetypes']
    except KeyError:
        pass
    else:
        mimetypes._default_mime_types()

    try:
        filecmp = sys.modules['filecmp']
    except KeyError:
        pass
    else:
        filecmp._cache.clear()

    try:
        struct = sys.modules['struct']
    except KeyError:
        pass
    else:
        struct._clearcache()

    try:
        doctest = sys.modules['doctest']
    except KeyError:
        pass
    else:
        doctest.master = None

    try:
        ctypes = sys.modules['ctypes']
    except KeyError:
        pass
    else:
        ctypes._reset_cache()

    try:
        typing = sys.modules['typing']
    except KeyError:
        pass
    else:
        for f in typing._cleanups:
            f()

    try:
        fractions = sys.modules['fractions']
    except KeyError:
        pass
    else:
        fractions._hash_algorithm.cache_clear()

    try:
        inspect = sys.modules['inspect']
    except KeyError:
        pass
    else:
        inspect._shadowed_dict_from_mro_tuple.cache_clear()


def get_build_info():
    # Get most important configure and build options as a list of strings.
    # Example: ['debug', 'ASAN+MSAN'] or ['release', 'LTO+PGO'].

    config_args = sysconfig.get_config_var('CONFIG_ARGS') or ''
    cflags = sysconfig.get_config_var('PY_CFLAGS') or ''
    cflags_nodist = sysconfig.get_config_var('PY_CFLAGS_NODIST') or ''
    ldflags_nodist = sysconfig.get_config_var('PY_LDFLAGS_NODIST') or ''

    build = []

    # --disable-gil
    if sysconfig.get_config_var('Py_NOGIL'):
        build.append("nogil")

    if hasattr(sys, 'gettotalrefcount'):
        # --with-pydebug
        build.append('debug')

        if '-DNDEBUG' in (cflags + cflags_nodist):
            build.append('without_assert')
    else:
        build.append('release')

        if '--with-assertions' in config_args:
            build.append('with_assert')
        elif '-DNDEBUG' not in (cflags + cflags_nodist):
            build.append('with_assert')

    # --enable-framework=name
    framework = sysconfig.get_config_var('PYTHONFRAMEWORK')
    if framework:
        build.append(f'framework={framework}')

    # --enable-shared
    shared = int(sysconfig.get_config_var('PY_ENABLE_SHARED') or '0')
    if shared:
        build.append('shared')

    # --with-lto
    optimizations = []
    if '-flto=thin' in ldflags_nodist:
        optimizations.append('ThinLTO')
    elif '-flto' in ldflags_nodist:
        optimizations.append('LTO')

    # --enable-optimizations
    pgo_options = (
        # GCC
        '-fprofile-use',
        # clang: -fprofile-instr-use=code.profclangd
        '-fprofile-instr-use',
        # ICC
        "-prof-use",
    )
    if any(option in cflags_nodist for option in pgo_options):
        optimizations.append('PGO')
    if optimizations:
        build.append('+'.join(optimizations))

    # --with-address-sanitizer
    sanitizers = []
    if support.check_sanitizer(address=True):
        sanitizers.append("ASAN")
    # --with-memory-sanitizer
    if support.check_sanitizer(memory=True):
        sanitizers.append("MSAN")
    # --with-undefined-behavior-sanitizer
    if support.check_sanitizer(ub=True):
        sanitizers.append("UBSAN")
    if sanitizers:
        build.append('+'.join(sanitizers))

    # --with-trace-refs
    if hasattr(sys, 'getobjects'):
        build.append("TraceRefs")
    # --enable-pystats
    if hasattr(sys, '_stats_on'):
        build.append("pystats")
    # --with-valgrind
    if sysconfig.get_config_var('WITH_VALGRIND'):
        build.append("valgrind")
    # --with-dtrace
    if sysconfig.get_config_var('WITH_DTRACE'):
        build.append("dtrace")

    return build


def get_temp_dir(tmp_dir):
    if tmp_dir:
        tmp_dir = os.path.expanduser(tmp_dir)
    else:
        # When tests are run from the Python build directory, it is best practice
        # to keep the test files in a subfolder.  This eases the cleanup of leftover
        # files using the "make distclean" command.
        if sysconfig.is_python_build():
            tmp_dir = sysconfig.get_config_var('abs_builddir')
            if tmp_dir is None:
                # bpo-30284: On Windows, only srcdir is available. Using
                # abs_builddir mostly matters on UNIX when building Python
                # out of the source tree, especially when the source tree
                # is read only.
                tmp_dir = sysconfig.get_config_var('srcdir')
            tmp_dir = os.path.join(tmp_dir, 'build')
        else:
            tmp_dir = tempfile.gettempdir()

    return os.path.abspath(tmp_dir)


def fix_umask():
    if support.is_emscripten:
        # Emscripten has default umask 0o777, which breaks some tests.
        # see https://github.com/emscripten-core/emscripten/issues/17269
        old_mask = os.umask(0)
        if old_mask == 0o777:
            os.umask(0o027)
        else:
            os.umask(old_mask)


def get_work_dir(*, parent_dir: StrPath = '', worker: bool = False):
    # Define a writable temp dir that will be used as cwd while running
    # the tests. The name of the dir includes the pid to allow parallel
    # testing (see the -j option).
    # Emscripten and WASI have stubbed getpid(), Emscripten has only
    # milisecond clock resolution. Use randint() instead.
    if sys.platform in {"emscripten", "wasi"}:
        nounce = random.randint(0, 1_000_000)
    else:
        nounce = os.getpid()

    if worker:
        work_dir = 'test_python_worker_{}'.format(nounce)
    else:
        work_dir = 'test_python_{}'.format(nounce)
    work_dir += os_helper.FS_NONASCII
    if parent_dir:
        work_dir = os.path.join(parent_dir, work_dir)
    return work_dir


@contextlib.contextmanager
def exit_timeout():
    try:
        yield
    except SystemExit as exc:
        # bpo-38203: Python can hang at exit in Py_Finalize(), especially
        # on threading._shutdown() call: put a timeout
        if threading_helper.can_start_thread:
            faulthandler.dump_traceback_later(EXIT_TIMEOUT, exit=True)
        sys.exit(exc.code)


def remove_testfn(test_name: TestName, verbose: int) -> None:
    # Try to clean up os_helper.TESTFN if left behind.
    #
    # While tests shouldn't leave any files or directories behind, when a test
    # fails that can be tedious for it to arrange.  The consequences can be
    # especially nasty on Windows, since if a test leaves a file open, it
    # cannot be deleted by name (while there's nothing we can do about that
    # here either, we can display the name of the offending test, which is a
    # real help).
    name = os_helper.TESTFN
    if not os.path.exists(name):
        return

    if os.path.isdir(name):
        import shutil
        kind, nuker = "directory", shutil.rmtree
    elif os.path.isfile(name):
        kind, nuker = "file", os.unlink
    else:
        raise RuntimeError(f"os.path says {name!r} exists but is neither "
                           f"directory nor file")

    if verbose:
        print_warning(f"{test_name} left behind {kind} {name!r}")
        support.environment_altered = True

    try:
        import stat
        # fix possible permissions problems that might prevent cleanup
        os.chmod(name, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
        nuker(name)
    except Exception as exc:
        print_warning(f"{test_name} left behind {kind} {name!r} "
                      f"and it couldn't be removed: {exc}")


def abs_module_name(test_name: TestName, test_dir: StrPath | None) -> TestName:
    if test_name.startswith('test.') or test_dir:
        return test_name
    else:
        # Import it from the test package
        return 'test.' + test_name


# gh-90681: When rerunning tests, we might need to rerun the whole
# class or module suite if some its life-cycle hooks fail.
# Test level hooks are not affected.
_TEST_LIFECYCLE_HOOKS = frozenset((
    'setUpClass', 'tearDownClass',
    'setUpModule', 'tearDownModule',
))

def normalize_test_name(test_full_name, *, is_error=False):
    short_name = test_full_name.split(" ")[0]
    if is_error and short_name in _TEST_LIFECYCLE_HOOKS:
        if test_full_name.startswith(('setUpModule (', 'tearDownModule (')):
            # if setUpModule() or tearDownModule() failed, don't filter
            # tests with the test file name, don't use use filters.
            return None

        # This means that we have a failure in a life-cycle hook,
        # we need to rerun the whole module or class suite.
        # Basically the error looks like this:
        #    ERROR: setUpClass (test.test_reg_ex.RegTest)
        # or
        #    ERROR: setUpModule (test.test_reg_ex)
        # So, we need to parse the class / module name.
        lpar = test_full_name.index('(')
        rpar = test_full_name.index(')')
        return test_full_name[lpar + 1: rpar].split('.')[-1]
    return short_name


def replace_stdout():
    """Set stdout encoder error handler to backslashreplace (as stderr error
    handler) to avoid UnicodeEncodeError when printing a traceback"""
    stdout = sys.stdout
    try:
        fd = stdout.fileno()
    except ValueError:
        # On IDLE, sys.stdout has no file descriptor and is not a TextIOWrapper
        # object. Leaving sys.stdout unchanged.
        #
        # Catch ValueError to catch io.UnsupportedOperation on TextIOBase
        # and ValueError on a closed stream.
        return

    sys.stdout = open(fd, 'w',
        encoding=stdout.encoding,
        errors="backslashreplace",
        closefd=False,
        newline='\n')

    def restore_stdout():
        sys.stdout.close()
        sys.stdout = stdout
    atexit.register(restore_stdout)


def adjust_rlimit_nofile():
    """
    On macOS the default fd limit (RLIMIT_NOFILE) is sometimes too low (256)
    for our test suite to succeed. Raise it to something more reasonable. 1024
    is a common Linux default.
    """
    try:
        import resource
    except ImportError:
        return

    fd_limit, max_fds = resource.getrlimit(resource.RLIMIT_NOFILE)

    desired_fds = 1024

    if fd_limit < desired_fds and fd_limit < max_fds:
        new_fd_limit = min(desired_fds, max_fds)
        try:
            resource.setrlimit(resource.RLIMIT_NOFILE,
                               (new_fd_limit, max_fds))
            print(f"Raised RLIMIT_NOFILE: {fd_limit} -> {new_fd_limit}")
        except (ValueError, OSError) as err:
            print_warning(f"Unable to raise RLIMIT_NOFILE from {fd_limit} to "
                          f"{new_fd_limit}: {err}.")


def display_header():
    # Print basic platform information
    print("==", platform.python_implementation(), *sys.version.split())
    print("==", platform.platform(aliased=True),
                  "%s-endian" % sys.byteorder)
    print("== Python build:", ' '.join(get_build_info()))
    print("== cwd:", os.getcwd())
    cpu_count = os.cpu_count()
    if cpu_count:
        print("== CPU count:", cpu_count)
    print("== encodings: locale=%s, FS=%s"
          % (locale.getencoding(), sys.getfilesystemencoding()))

    # This makes it easier to remember what to set in your local
    # environment when trying to reproduce a sanitizer failure.
    asan = support.check_sanitizer(address=True)
    msan = support.check_sanitizer(memory=True)
    ubsan = support.check_sanitizer(ub=True)
    sanitizers = []
    if asan:
        sanitizers.append("address")
    if msan:
        sanitizers.append("memory")
    if ubsan:
        sanitizers.append("undefined behavior")
    if not sanitizers:
        return

    print(f"== sanitizers: {', '.join(sanitizers)}")
    for sanitizer, env_var in (
        (asan, "ASAN_OPTIONS"),
        (msan, "MSAN_OPTIONS"),
        (ubsan, "UBSAN_OPTIONS"),
    ):
        options= os.environ.get(env_var)
        if sanitizer and options is not None:
            print(f"== {env_var}={options!r}")


def cleanup_temp_dir(tmp_dir: StrPath):
    import glob

    path = os.path.join(glob.escape(tmp_dir), 'test_python_*')
    print("Cleanup %s directory" % tmp_dir)
    for name in glob.glob(path):
        if os.path.isdir(name):
            print("Remove directory: %s" % name)
            os_helper.rmtree(name)
        else:
            print("Remove file: %s" % name)
            os_helper.unlink(name)
