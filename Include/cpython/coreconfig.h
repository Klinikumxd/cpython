#ifndef Py_PYCORECONFIG_H
#define Py_PYCORECONFIG_H
#ifndef Py_LIMITED_API
#ifdef __cplusplus
extern "C" {
#endif

/* --- _PyInitError ----------------------------------------------- */

typedef struct {
    enum {
        _Py_INIT_ERR_TYPE_OK=0,
        _Py_INIT_ERR_TYPE_ERROR=1,
        _Py_INIT_ERR_TYPE_EXIT=2
    } _type;
    const char *_func;
    const char *err_msg;
    int exitcode;
} _PyInitError;

PyAPI_FUNC(_PyInitError) _PyInitError_Ok(void);
PyAPI_FUNC(_PyInitError) _PyInitError_Error(const char *err_msg);
PyAPI_FUNC(_PyInitError) _PyInitError_NoMemory(void);
PyAPI_FUNC(_PyInitError) _PyInitError_Exit(int exitcode);
PyAPI_FUNC(int) _PyInitError_IsError(_PyInitError err);
PyAPI_FUNC(int) _PyInitError_IsExit(_PyInitError err);
PyAPI_FUNC(int) _PyInitError_Failed(_PyInitError err);

/* --- _PyWstrList ------------------------------------------------ */

typedef struct {
    /* If length is greater than zero, items must be non-NULL
       and all items strings must be non-NULL */
    Py_ssize_t length;
    wchar_t **items;
} _PyWstrList;


/* --- _PyPreConfig ----------------------------------------------- */

#define _Py_CONFIG_VERSION 1

typedef struct {
    int _config_version;  /* Internal configuration version,
                             used for ABI compatibility */

    /* If greater than 0, enable isolated mode: sys.path contains
       neither the script's directory nor the user's site-packages directory.

       Set to 1 by the -I command line option. If set to -1 (default), inherit
       Py_IsolatedFlag value. */
    int isolated;

    /* If greater than 0: use environment variables.
       Set to 0 by -E command line option. If set to -1 (default), it is
       set to !Py_IgnoreEnvironmentFlag. */
    int use_environment;

    /* Set the LC_CTYPE locale to the user preferred locale? If equals to 0,
       set coerce_c_locale and coerce_c_locale_warn to 0. */
    int configure_locale;

    /* Coerce the LC_CTYPE locale if it's equal to "C"? (PEP 538)

       Set to 0 by PYTHONCOERCECLOCALE=0. Set to 1 by PYTHONCOERCECLOCALE=1.
       Set to 2 if the user preferred LC_CTYPE locale is "C".

       If it is equal to 1, LC_CTYPE locale is read to decide it it should be
       coerced or not (ex: PYTHONCOERCECLOCALE=1). Internally, it is set to 2
       if the LC_CTYPE locale must be coerced.

       Disable by default (set to 0). Set it to -1 to let Python decides if it
       should be enabled or not. */
    int coerce_c_locale;

    /* Emit a warning if the LC_CTYPE locale is coerced?

       Set to 1 by PYTHONCOERCECLOCALE=warn.

       Disable by default (set to 0). Set it to -1 to let Python decides if it
       should be enabled or not. */
    int coerce_c_locale_warn;

#ifdef MS_WINDOWS
    /* If greater than 1, use the "mbcs" encoding instead of the UTF-8
       encoding for the filesystem encoding.

       Set to 1 if the PYTHONLEGACYWINDOWSFSENCODING environment variable is
       set to a non-empty string. If set to -1 (default), inherit
       Py_LegacyWindowsFSEncodingFlag value.

       See PEP 529 for more details. */
    int legacy_windows_fs_encoding;
#endif

    /* Enable UTF-8 mode? (PEP 540)

       Disabled by default (equals to 0).

       Set to 1 by "-X utf8" and "-X utf8=1" command line options.
       Set to 1 by PYTHONUTF8=1 environment variable.

       Set to 0 by "-X utf8=0" and PYTHONUTF8=0.

       If equals to -1, it is set to 1 if the LC_CTYPE locale is "C" or
       "POSIX", otherwise it is set to 0.

       If equals to -2, inherit Py_UTF8Mode value value (which is equal to 0
       by default). */
    int utf8_mode;

    int dev_mode;           /* Development mode. PYTHONDEVMODE, -X dev */

    /* Memory allocator: PYTHONMALLOC env var */
    PyMemAllocatorName allocator;
} _PyPreConfig;

#ifdef MS_WINDOWS
#  define _PyPreConfig_WINDOWS_INIT \
        .legacy_windows_fs_encoding = -1,
#else
#  define _PyPreConfig_WINDOWS_INIT
#endif

#define _PyPreConfig_INIT \
    (_PyPreConfig){ \
        _PyPreConfig_WINDOWS_INIT \
        ._config_version = _Py_CONFIG_VERSION, \
        .isolated = -1, \
        .use_environment = -1, \
        .configure_locale = 1, \
        .utf8_mode = -2, \
        .dev_mode = -1, \
        .allocator = PYMEM_ALLOCATOR_NOT_SET}

PyAPI_FUNC(void) _PyPreConfig_Init(_PyPreConfig *config);
PyAPI_FUNC(void) _PyPreConfig_InitPythonConfig(_PyPreConfig *config);
PyAPI_FUNC(void) _PyPreConfig_InitIsolateConfig(_PyPreConfig *config);


/* --- _PyCoreConfig ---------------------------------------------- */

typedef struct {
    int _config_version;  /* Internal configuration version,
                             used for ABI compatibility */

    int isolated;         /* Isolated mode? see _PyPreConfig.isolated */
    int use_environment;  /* Use environment variables? see _PyPreConfig.use_environment */
    int dev_mode;         /* Development mode? See _PyPreConfig.dev_mode */

    /* Install signal handlers? Yes by default. */
    int install_signal_handlers;

    int use_hash_seed;      /* PYTHONHASHSEED=x */
    unsigned long hash_seed;

    /* Enable faulthandler?
       Set to 1 by -X faulthandler and PYTHONFAULTHANDLER. -1 means unset. */
    int faulthandler;

    /* Enable tracemalloc?
       Set by -X tracemalloc=N and PYTHONTRACEMALLOC. -1 means unset */
    int tracemalloc;

    int import_time;        /* PYTHONPROFILEIMPORTTIME, -X importtime */
    int show_ref_count;     /* -X showrefcount */
    int show_alloc_count;   /* -X showalloccount */
    int dump_refs;          /* PYTHONDUMPREFS */
    int malloc_stats;       /* PYTHONMALLOCSTATS */

    /* Python filesystem encoding and error handler:
       sys.getfilesystemencoding() and sys.getfilesystemencodeerrors().

       Default encoding and error handler:

       * if Py_SetStandardStreamEncoding() has been called: they have the
         highest priority;
       * PYTHONIOENCODING environment variable;
       * The UTF-8 Mode uses UTF-8/surrogateescape;
       * If Python forces the usage of the ASCII encoding (ex: C locale
         or POSIX locale on FreeBSD or HP-UX), use ASCII/surrogateescape;
       * locale encoding: ANSI code page on Windows, UTF-8 on Android and
         VxWorks, LC_CTYPE locale encoding on other platforms;
       * On Windows, "surrogateescape" error handler;
       * "surrogateescape" error handler if the LC_CTYPE locale is "C" or "POSIX";
       * "surrogateescape" error handler if the LC_CTYPE locale has been coerced
         (PEP 538);
       * "strict" error handler.

       Supported error handlers: "strict", "surrogateescape" and
       "surrogatepass". The surrogatepass error handler is only supported
       if Py_DecodeLocale() and Py_EncodeLocale() use directly the UTF-8 codec;
       it's only used on Windows.

       initfsencoding() updates the encoding to the Python codec name.
       For example, "ANSI_X3.4-1968" is replaced with "ascii".

       On Windows, sys._enablelegacywindowsfsencoding() sets the
       encoding/errors to mbcs/replace at runtime.


       See Py_FileSystemDefaultEncoding and Py_FileSystemDefaultEncodeErrors.
       */
    wchar_t *filesystem_encoding;
    wchar_t *filesystem_errors;

    wchar_t *pycache_prefix;  /* PYTHONPYCACHEPREFIX, -X pycache_prefix=PATH */
    int parse_argv;           /* Parse argv command line arguments? */

    /* Command line arguments (sys.argv).

       Set parse_argv to 1 to parse argv as Python command line arguments
       and then strip Python arguments from argv.

       If argv is empty, an empty string is added to ensure that sys.argv
       always exists and is never empty. */
    _PyWstrList argv;

    /* Program name:

       - If Py_SetProgramName() was called, use its value.
       - On macOS, use PYTHONEXECUTABLE environment variable if set.
       - If WITH_NEXT_FRAMEWORK macro is defined, use __PYVENV_LAUNCHER__
         environment variable is set.
       - Use argv[0] if available and non-empty.
       - Use "python" on Windows, or "python3 on other platforms. */
    wchar_t *program_name;

    _PyWstrList xoptions;     /* Command line -X options */
    _PyWstrList warnoptions;  /* Warnings options */

    /* If equal to zero, disable the import of the module site and the
       site-dependent manipulations of sys.path that it entails. Also disable
       these manipulations if site is explicitly imported later (call
       site.main() if you want them to be triggered).

       Set to 0 by the -S command line option. If set to -1 (default), it is
       set to !Py_NoSiteFlag. */
    int site_import;

    /* Bytes warnings:

       * If equal to 1, issue a warning when comparing bytes or bytearray with
         str or bytes with int.
       * If equal or greater to 2, issue an error.

       Incremented by the -b command line option. If set to -1 (default), inherit
       Py_BytesWarningFlag value. */
    int bytes_warning;

    /* If greater than 0, enable inspect: when a script is passed as first
       argument or the -c option is used, enter interactive mode after
       executing the script or the command, even when sys.stdin does not appear
       to be a terminal.

       Incremented by the -i command line option. Set to 1 if the PYTHONINSPECT
       environment variable is non-empty. If set to -1 (default), inherit
       Py_InspectFlag value. */
    int inspect;

    /* If greater than 0: enable the interactive mode (REPL).

       Incremented by the -i command line option. If set to -1 (default),
       inherit Py_InteractiveFlag value. */
    int interactive;

    /* Optimization level.

       Incremented by the -O command line option. Set by the PYTHONOPTIMIZE
       environment variable. If set to -1 (default), inherit Py_OptimizeFlag
       value. */
    int optimization_level;

    /* If greater than 0, enable the debug mode: turn on parser debugging
       output (for expert only, depending on compilation options).

       Incremented by the -d command line option. Set by the PYTHONDEBUG
       environment variable. If set to -1 (default), inherit Py_DebugFlag
       value. */
    int parser_debug;

    /* If equal to 0, Python won't try to write ``.pyc`` files on the
       import of source modules.

       Set to 0 by the -B command line option and the PYTHONDONTWRITEBYTECODE
       environment variable. If set to -1 (default), it is set to
       !Py_DontWriteBytecodeFlag. */
    int write_bytecode;

    /* If greater than 0, enable the verbose mode: print a message each time a
       module is initialized, showing the place (filename or built-in module)
       from which it is loaded.

       If greater or equal to 2, print a message for each file that is checked
       for when searching for a module. Also provides information on module
       cleanup at exit.

       Incremented by the -v option. Set by the PYTHONVERBOSE environment
       variable. If set to -1 (default), inherit Py_VerboseFlag value. */
    int verbose;

    /* If greater than 0, enable the quiet mode: Don't display the copyright
       and version messages even in interactive mode.

       Incremented by the -q option. If set to -1 (default), inherit
       Py_QuietFlag value. */
    int quiet;

   /* If greater than 0, don't add the user site-packages directory to
      sys.path.

      Set to 0 by the -s and -I command line options , and the PYTHONNOUSERSITE
      environment variable. If set to -1 (default), it is set to
      !Py_NoUserSiteDirectory. */
    int user_site_directory;

    /* If non-zero, configure C standard steams (stdio, stdout,
       stderr):

       - Set O_BINARY mode on Windows.
       - If buffered_stdio is equal to zero, make streams unbuffered.
         Otherwise, enable streams buffering if interactive is non-zero. */
    int configure_c_stdio;

    /* If equal to 0, enable unbuffered mode: force the stdout and stderr
       streams to be unbuffered.

       Set to 0 by the -u option. Set by the PYTHONUNBUFFERED environment
       variable.
       If set to -1 (default), it is set to !Py_UnbufferedStdioFlag. */
    int buffered_stdio;

    /* Encoding of sys.stdin, sys.stdout and sys.stderr.
       Value set from PYTHONIOENCODING environment variable and
       Py_SetStandardStreamEncoding() function.
       See also 'stdio_errors' attribute. */
    wchar_t *stdio_encoding;

    /* Error handler of sys.stdin and sys.stdout.
       Value set from PYTHONIOENCODING environment variable and
       Py_SetStandardStreamEncoding() function.
       See also 'stdio_encoding' attribute. */
    wchar_t *stdio_errors;

#ifdef MS_WINDOWS
    /* If greater than zero, use io.FileIO instead of WindowsConsoleIO for sys
       standard streams.

       Set to 1 if the PYTHONLEGACYWINDOWSSTDIO environment variable is set to
       a non-empty string. If set to -1 (default), inherit
       Py_LegacyWindowsStdioFlag value.

       See PEP 528 for more details. */
    int legacy_windows_stdio;
#endif

    /* --- Path configuration inputs ------------ */

    wchar_t *module_search_path_env; /* PYTHONPATH environment variable */
    wchar_t *home;          /* PYTHONHOME environment variable,
                               see also Py_SetPythonHome(). */

    /* --- Path configuration outputs ----------- */

    int use_module_search_paths;  /* If non-zero, use module_search_paths */
    _PyWstrList module_search_paths;  /* sys.path paths. Computed if
                                       use_module_search_paths is equal
                                       to zero. */

    wchar_t *executable;    /* sys.executable */
    wchar_t *prefix;        /* sys.prefix */
    wchar_t *base_prefix;   /* sys.base_prefix */
    wchar_t *exec_prefix;   /* sys.exec_prefix */
    wchar_t *base_exec_prefix;  /* sys.base_exec_prefix */
#ifdef MS_WINDOWS
    wchar_t *dll_path;      /* Windows DLL path */
#endif

    /* --- Parameter only used by Py_Main() ---------- */

    /* Skip the first line of the source ('run_filename' parameter), allowing use of non-Unix forms of
       "#!cmd".  This is intended for a DOS specific hack only.

       Set by the -x command line option. */
    int skip_source_first_line;

    wchar_t *run_command;   /* -c command line argument */
    wchar_t *run_module;    /* -m command line argument */
    wchar_t *run_filename;  /* Trailing command line argument without -c or -m */

    /* --- Private fields ---------------------------- */

    /* Install importlib? If set to 0, importlib is not initialized at all.
       Needed by freeze_importlib. */
    int _install_importlib;

    /* Value of the --check-hash-based-pycs command line option:

       - "default" means the 'check_source' flag in hash-based pycs
         determines invalidation
       - "always" causes the interpreter to hash the source file for
         invalidation regardless of value of 'check_source' bit
       - "never" causes the interpreter to always assume hash-based pycs are
         valid

       The default value is "default".

       See PEP 552 "Deterministic pycs" for more details. */
    wchar_t *check_hash_pycs_mode;

    /* If greater than 0, suppress _PyPathConfig_Calculate() warnings on Unix.
       The parameter has no effect on Windows.

       If set to -1 (default), inherit !Py_FrozenFlag value. */
    int pathconfig_warnings;

    /* If equal to 0, stop Python initialization before the "main" phase */
    int _init_main;

} _PyCoreConfig;

#ifdef MS_WINDOWS
#  define _PyCoreConfig_WINDOWS_INIT \
        .legacy_windows_stdio = -1,
#else
#  define _PyCoreConfig_WINDOWS_INIT
#endif

#define _PyCoreConfig_INIT \
    (_PyCoreConfig){ \
        _PyCoreConfig_WINDOWS_INIT \
        ._config_version = _Py_CONFIG_VERSION, \
        .isolated = -1, \
        .use_environment = -1, \
        .dev_mode = -1, \
        .install_signal_handlers = 1, \
        .use_hash_seed = -1, \
        .faulthandler = -1, \
        .tracemalloc = -1, \
        .use_module_search_paths = 0, \
        .parse_argv = 0, \
        .site_import = -1, \
        .bytes_warning = -1, \
        .inspect = -1, \
        .interactive = -1, \
        .optimization_level = -1, \
        .parser_debug= -1, \
        .write_bytecode = -1, \
        .verbose = -1, \
        .quiet = -1, \
        .user_site_directory = -1, \
        .configure_c_stdio = 0, \
        .buffered_stdio = -1, \
        ._install_importlib = 1, \
        .check_hash_pycs_mode = NULL, \
        .pathconfig_warnings = -1, \
        ._init_main = 1}
/* Note: _PyCoreConfig_INIT sets other fields to 0/NULL */

PyAPI_FUNC(void) _PyCoreConfig_Init(_PyCoreConfig *config);
PyAPI_FUNC(_PyInitError) _PyCoreConfig_InitPythonConfig(_PyCoreConfig *config);
PyAPI_FUNC(_PyInitError) _PyCoreConfig_InitIsolateConfig(_PyCoreConfig *config);

#ifdef __cplusplus
}
#endif
#endif /* !Py_LIMITED_API */
#endif /* !Py_PYCORECONFIG_H */
