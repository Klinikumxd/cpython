/* Python interpreter main program */

#include "Python.h"
#include "osdefs.h"
#include "internal/pystate.h"

#include <locale.h>

#if defined(MS_WINDOWS) || defined(__CYGWIN__)
#include <windows.h>
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#endif

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if defined(MS_WINDOWS)
#define PYTHONHOMEHELP "<prefix>\\lib"
#else
#define PYTHONHOMEHELP "<prefix>/pythonX.X"
#endif

#include "pygetopt.h"

#define COPYRIGHT \
    "Type \"help\", \"copyright\", \"credits\" or \"license\" " \
    "for more information."

#ifdef __cplusplus
extern "C" {
#endif

/* For Py_GetArgcArgv(); set by main() */
static wchar_t **orig_argv;
static int orig_argc;

/* command line options */
#define BASE_OPTS L"bBc:dEhiIJm:OqRsStuvVW:xX:?"

#define PROGRAM_OPTS BASE_OPTS

/* Short usage message (with %s for argv0) */
static const char usage_line[] =
"usage: %ls [option] ... [-c cmd | -m mod | file | -] [arg] ...\n";

/* Long usage message, split into parts < 512 bytes */
static const char usage_1[] = "\
Options and arguments (and corresponding environment variables):\n\
-b     : issue warnings about str(bytes_instance), str(bytearray_instance)\n\
         and comparing bytes/bytearray with str. (-bb: issue errors)\n\
-B     : don't write .pyc files on import; also PYTHONDONTWRITEBYTECODE=x\n\
-c cmd : program passed in as string (terminates option list)\n\
-d     : debug output from parser; also PYTHONDEBUG=x\n\
-E     : ignore PYTHON* environment variables (such as PYTHONPATH)\n\
-h     : print this help message and exit (also --help)\n\
";
static const char usage_2[] = "\
-i     : inspect interactively after running script; forces a prompt even\n\
         if stdin does not appear to be a terminal; also PYTHONINSPECT=x\n\
-I     : isolate Python from the user's environment (implies -E and -s)\n\
-m mod : run library module as a script (terminates option list)\n\
-O     : optimize generated bytecode slightly; also PYTHONOPTIMIZE=x\n\
-OO    : remove doc-strings in addition to the -O optimizations\n\
-q     : don't print version and copyright messages on interactive startup\n\
-s     : don't add user site directory to sys.path; also PYTHONNOUSERSITE\n\
-S     : don't imply 'import site' on initialization\n\
";
static const char usage_3[] = "\
-u     : force the stdout and stderr streams to be unbuffered;\n\
         this option has no effect on stdin; also PYTHONUNBUFFERED=x\n\
-v     : verbose (trace import statements); also PYTHONVERBOSE=x\n\
         can be supplied multiple times to increase verbosity\n\
-V     : print the Python version number and exit (also --version)\n\
         when given twice, print more information about the build\n\
-W arg : warning control; arg is action:message:category:module:lineno\n\
         also PYTHONWARNINGS=arg\n\
-x     : skip first line of source, allowing use of non-Unix forms of #!cmd\n\
-X opt : set implementation-specific option\n\
";
static const char usage_4[] = "\
file   : program read from script file\n\
-      : program read from stdin (default; interactive mode if a tty)\n\
arg ...: arguments passed to program in sys.argv[1:]\n\n\
Other environment variables:\n\
PYTHONSTARTUP: file executed on interactive startup (no default)\n\
PYTHONPATH   : '%lc'-separated list of directories prefixed to the\n\
               default module search path.  The result is sys.path.\n\
";
static const char usage_5[] =
"PYTHONHOME   : alternate <prefix> directory (or <prefix>%lc<exec_prefix>).\n"
"               The default module search path uses %s.\n"
"PYTHONCASEOK : ignore case in 'import' statements (Windows).\n"
"PYTHONIOENCODING: Encoding[:errors] used for stdin/stdout/stderr.\n"
"PYTHONFAULTHANDLER: dump the Python traceback on fatal errors.\n";
static const char usage_6[] =
"PYTHONHASHSEED: if this variable is set to 'random', a random value is used\n"
"   to seed the hashes of str, bytes and datetime objects.  It can also be\n"
"   set to an integer in the range [0,4294967295] to get hash values with a\n"
"   predictable seed.\n"
"PYTHONMALLOC: set the Python memory allocators and/or install debug hooks\n"
"   on Python memory allocators. Use PYTHONMALLOC=debug to install debug\n"
"   hooks.\n"
"PYTHONCOERCECLOCALE: if this variable is set to 0, it disables the locale\n"
"   coercion behavior. Use PYTHONCOERCECLOCALE=warn to request display of\n"
"   locale coercion and locale compatibility warnings on stderr.\n";

static void
usage(int error, const wchar_t* program)
{
    FILE *f = error ? stderr : stdout;

    fprintf(f, usage_line, program);
    if (error)
        fprintf(f, "Try `python -h' for more information.\n");
    else {
        fputs(usage_1, f);
        fputs(usage_2, f);
        fputs(usage_3, f);
        fprintf(f, usage_4, (wint_t)DELIM);
        fprintf(f, usage_5, (wint_t)DELIM, PYTHONHOMEHELP);
        fputs(usage_6, f);
    }
}

static void
pymain_run_statup(PyCompilerFlags *cf)
{
    char *startup = Py_GETENV("PYTHONSTARTUP");
    if (startup == NULL || startup[0] == '\0') {
        return;
    }

    FILE *fp = _Py_fopen(startup, "r");
    if (fp == NULL) {
        int save_errno = errno;
        PySys_WriteStderr("Could not open PYTHONSTARTUP\n");
        errno = save_errno;

        PyErr_SetFromErrnoWithFilename(PyExc_OSError,
                        startup);
        PyErr_Print();
        PyErr_Clear();
        return;
    }

    (void) PyRun_SimpleFileExFlags(fp, startup, 0, cf);
    PyErr_Clear();
    fclose(fp);
}

static void
pymain_run_interactive_hook(void)
{
    PyObject *sys, *hook, *result;
    sys = PyImport_ImportModule("sys");
    if (sys == NULL) {
        goto error;
    }

    hook = PyObject_GetAttrString(sys, "__interactivehook__");
    Py_DECREF(sys);
    if (hook == NULL) {
        PyErr_Clear();
        return;
    }

    result = _PyObject_CallNoArg(hook);
    Py_DECREF(hook);
    if (result == NULL) {
        goto error;
    }
    Py_DECREF(result);

    return;

error:
    PySys_WriteStderr("Failed calling sys.__interactivehook__\n");
    PyErr_Print();
    PyErr_Clear();
}


static int
pymain_run_module(wchar_t *modname, int set_argv0)
{
    PyObject *module, *runpy, *runmodule, *runargs, *result;
    runpy = PyImport_ImportModule("runpy");
    if (runpy == NULL) {
        fprintf(stderr, "Could not import runpy module\n");
        PyErr_Print();
        return -1;
    }
    runmodule = PyObject_GetAttrString(runpy, "_run_module_as_main");
    if (runmodule == NULL) {
        fprintf(stderr, "Could not access runpy._run_module_as_main\n");
        PyErr_Print();
        Py_DECREF(runpy);
        return -1;
    }
    module = PyUnicode_FromWideChar(modname, wcslen(modname));
    if (module == NULL) {
        fprintf(stderr, "Could not convert module name to unicode\n");
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        return -1;
    }
    runargs = Py_BuildValue("(Oi)", module, set_argv0);
    if (runargs == NULL) {
        fprintf(stderr,
            "Could not create arguments for runpy._run_module_as_main\n");
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        Py_DECREF(module);
        return -1;
    }
    result = PyObject_Call(runmodule, runargs, NULL);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_DECREF(runpy);
    Py_DECREF(runmodule);
    Py_DECREF(module);
    Py_DECREF(runargs);
    if (result == NULL) {
        return -1;
    }
    Py_DECREF(result);
    return 0;
}

static PyObject *
pymain_get_importer(wchar_t *filename)
{
    PyObject *sys_path0 = NULL, *importer;

    sys_path0 = PyUnicode_FromWideChar(filename, wcslen(filename));
    if (sys_path0 == NULL) {
        goto error;
    }

    importer = PyImport_GetImporter(sys_path0);
    if (importer == NULL) {
        goto error;
    }

    if (importer == Py_None) {
        Py_DECREF(sys_path0);
        Py_DECREF(importer);
        return NULL;
    }

    Py_DECREF(importer);
    return sys_path0;

error:
    Py_XDECREF(sys_path0);
    PySys_WriteStderr("Failed checking if argv[0] is an import path entry\n");
    PyErr_Print();
    PyErr_Clear();
    return NULL;
}


static int
pymain_run_command(wchar_t *command, PyCompilerFlags *cf)
{
    PyObject *unicode, *bytes;
    int ret;

    unicode = PyUnicode_FromWideChar(command, -1);
    if (unicode == NULL) {
        goto error;
    }

    bytes = PyUnicode_AsUTF8String(unicode);
    Py_DECREF(unicode);
    if (bytes == NULL) {
        goto error;
    }

    ret = PyRun_SimpleStringFlags(PyBytes_AsString(bytes), cf);
    Py_DECREF(bytes);
    return (ret != 0);

error:
    PySys_WriteStderr("Unable to decode the command from the command line:\n");
    PyErr_Print();
    return 1;
}


static int
pymain_run_file(FILE *fp, const wchar_t *filename, PyCompilerFlags *p_cf)
{
    PyObject *unicode, *bytes = NULL;
    const char *filename_str;
    int run;

    /* call pending calls like signal handlers (SIGINT) */
    if (Py_MakePendingCalls() == -1) {
        PyErr_Print();
        return 1;
    }

    if (filename) {
        unicode = PyUnicode_FromWideChar(filename, wcslen(filename));
        if (unicode != NULL) {
            bytes = PyUnicode_EncodeFSDefault(unicode);
            Py_DECREF(unicode);
        }
        if (bytes != NULL) {
            filename_str = PyBytes_AsString(bytes);
        }
        else {
            PyErr_Clear();
            filename_str = "<encoding error>";
        }
    }
    else {
        filename_str = "<stdin>";
    }

    run = PyRun_AnyFileExFlags(fp, filename_str, filename != NULL, p_cf);
    Py_XDECREF(bytes);
    return run != 0;
}


/* Main program */

/*TODO: Add arg processing to PEP 432 as a new configuration setup API
 */
typedef struct {
    size_t len;
    wchar_t **options;
} _Py_OptList;

typedef struct {
    wchar_t *filename;           /* Trailing arg without -c or -m */
    wchar_t *command;            /* -c argument */
    wchar_t *module;             /* -m argument */
    _Py_OptList warning_options; /* -W options */
    PyObject *extra_options;     /* -X options */
    int print_help;              /* -h, -? options */
    int print_version;           /* -V option */
    int bytes_warning;           /* Py_BytesWarningFlag */
    int debug;                   /* Py_DebugFlag */
    int inspect;                 /* Py_InspectFlag */
    int interactive;             /* Py_InteractiveFlag */
    int isolated;                /* Py_IsolatedFlag */
    int optimization_level;      /* Py_OptimizeFlag */
    int dont_write_bytecode;     /* Py_DontWriteBytecodeFlag */
    int no_user_site_directory;  /* Py_NoUserSiteDirectory */
    int no_site_import;          /* Py_NoSiteFlag */
    int use_unbuffered_io;       /* Py_UnbufferedStdioFlag */
    int verbosity;               /* Py_VerboseFlag */
    int quiet_flag;              /* Py_QuietFlag */
    int skip_first_line;         /* -x option */
    _Py_OptList xoptions;        /* -X options */
} _Py_CommandLineDetails;

/* Structure used by Py_Main() to pass data to subfunctions */
typedef struct {
    /* Exit status ("exit code") */
    int status;
    PyCompilerFlags cf;
    /* non-zero is stdin is a TTY or if -i option is used */
    int stdin_is_interactive;
    _PyCoreConfig core_config;
    _Py_CommandLineDetails cmdline;
    PyObject *main_importer_path;
    /* non-zero if filename, command (-c) or module (-m) is set
       on the command line */
    int run_code;
    wchar_t *program_name;
    /* Error message if a function failed */
    _PyInitError err;
    /* PYTHONWARNINGS env var */
    _Py_OptList env_warning_options;
    int argc;
    wchar_t **argv;
} _PyMain;

/* .cmdline is initialized to zeros */
#define _PyMain_INIT \
    {.status = 0, \
     .cf = {.cf_flags = 0}, \
     .core_config = _PyCoreConfig_INIT, \
     .main_importer_path = NULL, \
     .run_code = -1, \
     .program_name = NULL, \
     .err = _Py_INIT_OK(), \
     .env_warning_options = {0, NULL}}


#define INIT_NO_MEMORY() _Py_INIT_ERR("memory allocation failed")


static void
pymain_optlist_clear(_Py_OptList *list)
{
    for (size_t i=0; i < list->len; i++) {
        PyMem_RawFree(list->options[i]);
    }
    PyMem_RawFree(list->options);
    list->len = 0;
    list->options = NULL;
}

static void
pymain_free_impl(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;
    pymain_optlist_clear(&cmdline->warning_options);
    pymain_optlist_clear(&cmdline->xoptions);
    PyMem_RawFree(cmdline->command);

    pymain_optlist_clear(&pymain->env_warning_options);
    Py_CLEAR(pymain->main_importer_path);
    PyMem_RawFree(pymain->program_name);

#ifdef __INSURE__
    /* Insure++ is a memory analysis tool that aids in discovering
     * memory leaks and other memory problems.  On Python exit, the
     * interned string dictionaries are flagged as being in use at exit
     * (which it is).  Under normal circumstances, this is fine because
     * the memory will be automatically reclaimed by the system.  Under
     * memory debugging, it's a huge source of useless noise, so we
     * trade off slower shutdown for less distraction in the memory
     * reports.  -baw
     */
    _Py_ReleaseInternedUnicodeStrings();
#endif /* __INSURE__ */
}

static void
pymain_free(_PyMain *pymain)
{
    /* Call pymain_free() with the memory allocator used by pymain_init() */
    PyMemAllocatorEx old_alloc, raw_alloc;
    PyMem_GetAllocator(PYMEM_DOMAIN_RAW, &old_alloc);
    _PyMem_GetDefaultRawAllocator(&raw_alloc);
    PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &raw_alloc);

    pymain_free_impl(pymain);

    PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &old_alloc);
}


static int
pymain_run_main_from_importer(_PyMain *pymain)
{
    PyObject *sys_path0 = pymain->main_importer_path;
    PyObject *sys_path;
    int sts;

    /* Assume sys_path0 has already been checked by pymain_get_importer(),
     * so put it in sys.path[0] and import __main__ */
    sys_path = PySys_GetObject("path");
    if (sys_path == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "unable to get sys.path");
        goto error;
    }

    sts = PyList_Insert(sys_path, 0, sys_path0);
    if (sts) {
        sys_path0 = NULL;
        goto error;
    }

    sts = pymain_run_module(L"__main__", 0);
    return sts != 0;

error:
    Py_CLEAR(pymain->main_importer_path);
    PyErr_Print();
    return 1;
}


static wchar_t*
pymain_strdup(wchar_t *str)
{
    size_t len = wcslen(str) + 1;  /* +1 for NUL character */
    wchar_t *str2 = PyMem_RawMalloc(sizeof(wchar_t) * len);
    if (str2 == NULL) {
        return NULL;
    }
    memcpy(str2, str, len * sizeof(wchar_t));
    return str2;
}


static int
pymain_optlist_append(_Py_OptList *list, wchar_t *str)
{
    wchar_t *str2 = pymain_strdup(str);
    if (str2 == NULL) {
        return -1;
    }

    size_t size = (list->len + 1) * sizeof(list[0]);
    wchar_t **options2 = (wchar_t **)PyMem_RawRealloc(list->options, size);
    if (options2 == NULL) {
        PyMem_RawFree(str2);
        return -1;
    }
    options2[list->len] = str2;
    list->options = options2;
    list->len++;
    return 0;
}


/* Parse the command line arguments
   Return 0 on success.
   Return 1 if parsing failed.
   Set pymain->err and return -1 on other errors. */
static int
pymain_parse_cmdline(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;

    _PyOS_ResetGetOpt();
    do {
        int c = _PyOS_GetOpt(pymain->argc, pymain->argv, PROGRAM_OPTS);
        if (c == EOF) {
            break;
        }

        if (c == 'c') {
            /* -c is the last option; following arguments
               that look like options are left for the
               command to interpret. */
            size_t len = wcslen(_PyOS_optarg) + 1 + 1;
            wchar_t *command = PyMem_RawMalloc(sizeof(wchar_t) * len);
            if (command == NULL) {
                goto out_of_memory;
            }
            memcpy(command, _PyOS_optarg, len * sizeof(wchar_t));
            command[len - 2] = '\n';
            command[len - 1] = 0;
            cmdline->command = command;
            break;
        }

        if (c == 'm') {
            /* -m is the last option; following arguments
               that look like options are left for the
               module to interpret. */
            cmdline->module = _PyOS_optarg;
            break;
        }

        switch (c) {
        case 'b':
            cmdline->bytes_warning++;
            break;

        case 'd':
            cmdline->debug++;
            break;

        case 'i':
            cmdline->inspect++;
            cmdline->interactive++;
            break;

        case 'I':
            pymain->core_config.ignore_environment++;
            cmdline->isolated++;
            cmdline->no_user_site_directory++;
            break;

        /* case 'J': reserved for Jython */

        case 'O':
            cmdline->optimization_level++;
            break;

        case 'B':
            cmdline->dont_write_bytecode++;
            break;

        case 's':
            cmdline->no_user_site_directory++;
            break;

        case 'S':
            cmdline->no_site_import++;
            break;

        case 'E':
            pymain->core_config.ignore_environment++;
            break;

        case 't':
            /* ignored for backwards compatibility */
            break;

        case 'u':
            cmdline->use_unbuffered_io = 1;
            break;

        case 'v':
            cmdline->verbosity++;
            break;

        case 'x':
            cmdline->skip_first_line = 1;
            break;

        case 'h':
        case '?':
            cmdline->print_help++;
            break;

        case 'V':
            cmdline->print_version++;
            break;

        case 'W':
            if (pymain_optlist_append(&cmdline->warning_options,
                                      _PyOS_optarg) < 0) {
                goto out_of_memory;
            }
            break;

        case 'X':
            if (pymain_optlist_append(&cmdline->xoptions,
                                      _PyOS_optarg) < 0) {
                goto out_of_memory;
            }
            break;

        case 'q':
            cmdline->quiet_flag++;
            break;

        case 'R':
            /* Ignored */
            break;

        /* This space reserved for other options */

        default:
            /* unknown argument: parsing failed */
            return 1;
        }
    } while (1);

    if (cmdline->command == NULL && cmdline->module == NULL
        && _PyOS_optind < pymain->argc
        && wcscmp(pymain->argv[_PyOS_optind], L"-") != 0)
    {
        cmdline->filename = pymain->argv[_PyOS_optind];
    }

    return 0;

out_of_memory:
    pymain->err = INIT_NO_MEMORY();
    return -1;
}


static void
maybe_set_flag(int *flag, int value)
{
    /* Helper to set flag variables from command line options
    *   - uses the higher of the two values if they're both set
    *   - otherwise leaves the flag unset
    */
    if (*flag < value) {
        *flag = value;
    }
}


static int
pymain_add_xoptions(_PyMain *pymain)
{
    _Py_OptList *options = &pymain->cmdline.xoptions;
    for (size_t i=0; i < options->len; i++) {
        wchar_t *option = options->options[i];
        if (_PySys_AddXOptionWithError(option) < 0) {
            pymain->err = INIT_NO_MEMORY();
            return -1;
        }
    }
    return 0;
}


static int
pymain_add_warnings_optlist(_Py_OptList *warnings)
{
    for (size_t i = 0; i < warnings->len; i++) {
        PyObject *option = PyUnicode_FromWideChar(warnings->options[i], -1);
        if (option == NULL) {
            return -1;
        }
        if (_PySys_AddWarnOptionWithError(option)) {
            Py_DECREF(option);
            return -1;
        }
        Py_DECREF(option);
    }
    return 0;
}

static int
pymain_add_warnings_options(_PyMain *pymain)
{
    PySys_ResetWarnOptions();

    if (pymain_add_warnings_optlist(&pymain->env_warning_options) < 0) {
        pymain->err = INIT_NO_MEMORY();
        return -1;
    }
    if (pymain_add_warnings_optlist(&pymain->cmdline.warning_options) < 0) {
        pymain->err = INIT_NO_MEMORY();
        return -1;
    }
    return 0;
}


/* Get warning options from PYTHONWARNINGS environment variable.
   Return 0 on success.
   Set pymain->err and return -1 on error. */
static int
pymain_warnings_envvar(_PyMain *pymain)
{
    if (Py_IgnoreEnvironmentFlag) {
        return 0;
    }

#ifdef MS_WINDOWS
    wchar_t *wp;

    if ((wp = _wgetenv(L"PYTHONWARNINGS")) && *wp != L'\0') {
        wchar_t *buf, *warning, *context = NULL;

        buf = (wchar_t *)PyMem_RawMalloc((wcslen(wp) + 1) * sizeof(wchar_t));
        if (buf == NULL) {
            goto out_of_memory;
        }
        wcscpy(buf, wp);
        for (warning = wcstok_s(buf, L",", &context);
             warning != NULL;
             warning = wcstok_s(NULL, L",", &context)) {

            if (pymain_optlist_append(&pymain->env_warning_options,
                                      warning) < 0) {
                PyMem_RawFree(buf);
                goto out_of_memory;
            }
        }
        PyMem_RawFree(buf);
    }
#else
    char *p;

    if ((p = Py_GETENV("PYTHONWARNINGS")) && *p != '\0') {
        char *buf, *oldloc;

        /* settle for strtok here as there's no one standard
           C89 wcstok */
        buf = (char *)PyMem_RawMalloc(strlen(p) + 1);
        if (buf == NULL) {
            goto out_of_memory;
        }
        strcpy(buf, p);
        oldloc = _PyMem_RawStrdup(setlocale(LC_ALL, NULL));
        setlocale(LC_ALL, "");
        for (p = strtok(buf, ","); p != NULL; p = strtok(NULL, ",")) {
            size_t len;
            wchar_t *warning = Py_DecodeLocale(p, &len);
            if (warning == NULL) {
                if (len == (size_t)-2) {
                    pymain->err = _Py_INIT_ERR("failed to decode "
                                               "PYTHONWARNINGS");
                    return -1;
                }
                else {
                    goto out_of_memory;
                }
            }
            if (pymain_optlist_append(&pymain->env_warning_options,
                                      warning) < 0) {
                PyMem_RawFree(warning);
                goto out_of_memory;
            }
            PyMem_RawFree(warning);
        }
        setlocale(LC_ALL, oldloc);
        PyMem_RawFree(oldloc);
        PyMem_RawFree(buf);
    }
#endif
    return 0;

out_of_memory:
    pymain->err = INIT_NO_MEMORY();
    return -1;
}


static void
pymain_init_stdio(_PyMain *pymain)
{
    pymain->stdin_is_interactive = (isatty(fileno(stdin))
                                    || Py_InteractiveFlag);

#if defined(MS_WINDOWS) || defined(__CYGWIN__)
    /* don't translate newlines (\r\n <=> \n) */
    _setmode(fileno(stdin), O_BINARY);
    _setmode(fileno(stdout), O_BINARY);
    _setmode(fileno(stderr), O_BINARY);
#endif

    if (Py_UnbufferedStdioFlag) {
#ifdef HAVE_SETVBUF
        setvbuf(stdin,  (char *)NULL, _IONBF, BUFSIZ);
        setvbuf(stdout, (char *)NULL, _IONBF, BUFSIZ);
        setvbuf(stderr, (char *)NULL, _IONBF, BUFSIZ);
#else /* !HAVE_SETVBUF */
        setbuf(stdin,  (char *)NULL);
        setbuf(stdout, (char *)NULL);
        setbuf(stderr, (char *)NULL);
#endif /* !HAVE_SETVBUF */
    }
    else if (Py_InteractiveFlag) {
#ifdef MS_WINDOWS
        /* Doesn't have to have line-buffered -- use unbuffered */
        /* Any set[v]buf(stdin, ...) screws up Tkinter :-( */
        setvbuf(stdout, (char *)NULL, _IONBF, BUFSIZ);
#else /* !MS_WINDOWS */
#ifdef HAVE_SETVBUF
        setvbuf(stdin,  (char *)NULL, _IOLBF, BUFSIZ);
        setvbuf(stdout, (char *)NULL, _IOLBF, BUFSIZ);
#endif /* HAVE_SETVBUF */
#endif /* !MS_WINDOWS */
        /* Leave stderr alone - it should be unbuffered anyway. */
    }
}


/* Get the program name: use PYTHONEXECUTABLE and __PYVENV_LAUNCHER__
   environment variables on macOS if available, use argv[0] by default.

   Return 0 on success.
   Set pymain->err and return -1 on error. */
static int
pymain_get_program_name(_PyMain *pymain)
{
    assert(pymain->program_name == NULL);
#ifdef __APPLE__
    char *p;
    /* On MacOS X, when the Python interpreter is embedded in an
       application bundle, it gets executed by a bootstrapping script
       that does os.execve() with an argv[0] that's different from the
       actual Python executable. This is needed to keep the Finder happy,
       or rather, to work around Apple's overly strict requirements of
       the process name. However, we still need a usable sys.executable,
       so the actual executable path is passed in an environment variable.
       See Lib/plat-mac/bundlebuiler.py for details about the bootstrap
       script. */
    if ((p = Py_GETENV("PYTHONEXECUTABLE")) && *p != '\0') {
        wchar_t* buffer;
        size_t len = strlen(p) + 1;

        buffer = PyMem_RawMalloc(len * sizeof(wchar_t));
        if (buffer == NULL) {
            goto out_of_memory;
        }

        mbstowcs(buffer, p, len);
        pymain->program_name = buffer;
    }
#ifdef WITH_NEXT_FRAMEWORK
    else {
        char* pyvenv_launcher = getenv("__PYVENV_LAUNCHER__");
        if (pyvenv_launcher && *pyvenv_launcher) {
            /* Used by Mac/Tools/pythonw.c to forward
             * the argv0 of the stub executable
             */
            size_t len;
            wchar_t* wbuf = Py_DecodeLocale(pyvenv_launcher, &len);
            if (wbuf == NULL) {
                if (len == (size_t)-2) {
                    pymain->err = _Py_INIT_ERR("failed to decode "
                                               "__PYVENV_LAUNCHER__");
                    return -1;
                }
                else {
                    goto out_of_memory;
                }
            }
            pymain->program_name = wbuf;
        }
    }
#endif   /* WITH_NEXT_FRAMEWORK */
#endif   /* __APPLE__ */

    if (pymain->program_name == NULL) {
        /* Use argv[0] by default */
        pymain->program_name = pymain_strdup(pymain->argv[0]);
        if (pymain->program_name == NULL) {
            goto out_of_memory;
        }
    }
    return 0;

out_of_memory:
    pymain->err = INIT_NO_MEMORY();
    return -1;
}


/* Initialize the main interpreter.
 *
 * Replaces previous call to Py_Initialize()
 *
 * TODO: Move environment queries (etc) into Py_ReadConfig
 *
 * Return 0 on success.
 * Set pymain->err and return -1 on error.
 */
static int
pymain_init_main_interpreter(_PyMain *pymain)
{
    _PyMainInterpreterConfig config = _PyMainInterpreterConfig_INIT;
    _PyInitError err;

    /* TODO: Moar config options! */
    config.install_signal_handlers = 1;

    /* TODO: Print any exceptions raised by these operations */
    err = _Py_ReadMainInterpreterConfig(&config);
    if (_Py_INIT_FAILED(err)) {
        pymain->err = err;
        return -1;
    }

    err = _Py_InitializeMainInterpreter(&config);
    if (_Py_INIT_FAILED(err)) {
        pymain->err = err;
        return -1;
    }
    return 0;
}


static void
pymain_header(_PyMain *pymain)
{
    /* TODO: Move this to _PyRun_PrepareMain */
    if (Py_QuietFlag) {
        return;
    }

    if (!Py_VerboseFlag && (pymain->run_code || !pymain->stdin_is_interactive)) {
        return;
    }

    fprintf(stderr, "Python %s on %s\n", Py_GetVersion(), Py_GetPlatform());
    if (!Py_NoSiteFlag) {
        fprintf(stderr, "%s\n", COPYRIGHT);
    }
}


static void
pymain_init_argv(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;

    /* TODO: Move this to _Py_InitializeMainInterpreter */
    if (cmdline->command != NULL) {
        /* Backup _PyOS_optind and force sys.argv[0] = '-c' */
        _PyOS_optind--;
        pymain->argv[_PyOS_optind] = L"-c";
    }

    if (cmdline->module != NULL) {
        /* Backup _PyOS_optind and force sys.argv[0] = '-m'*/
        _PyOS_optind--;
        pymain->argv[_PyOS_optind] = L"-m";
    }

    if (cmdline->filename != NULL) {
        pymain->main_importer_path = pymain_get_importer(cmdline->filename);
    }

    int update_path;
    if (pymain->main_importer_path != NULL) {
        /* Let pymain_run_main_from_importer() adjust sys.path[0] later */
        update_path = 0;
    } else {
        /* Use config settings to decide whether or not to update sys.path[0] */
        update_path = (Py_IsolatedFlag == 0);
    }
    PySys_SetArgvEx(pymain->argc - _PyOS_optind,
                    pymain->argv + _PyOS_optind,
                    update_path);
}


/* Set Py_XXX global configuration variables */
static void
pymain_set_global_config(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;
    maybe_set_flag(&Py_BytesWarningFlag, cmdline->bytes_warning);
    maybe_set_flag(&Py_DebugFlag, cmdline->debug);
    maybe_set_flag(&Py_InspectFlag, cmdline->inspect);
    maybe_set_flag(&Py_InteractiveFlag, cmdline->interactive);
    maybe_set_flag(&Py_IsolatedFlag, cmdline->isolated);
    maybe_set_flag(&Py_OptimizeFlag, cmdline->optimization_level);
    maybe_set_flag(&Py_DontWriteBytecodeFlag, cmdline->dont_write_bytecode);
    maybe_set_flag(&Py_NoUserSiteDirectory, cmdline->no_user_site_directory);
    maybe_set_flag(&Py_NoSiteFlag, cmdline->no_site_import);
    maybe_set_flag(&Py_UnbufferedStdioFlag, cmdline->use_unbuffered_io);
    maybe_set_flag(&Py_VerboseFlag, cmdline->verbosity);
    maybe_set_flag(&Py_QuietFlag, cmdline->quiet_flag);

    maybe_set_flag(&Py_IgnoreEnvironmentFlag, pymain->core_config.ignore_environment);
}


/* Propagate options parsed from the command line and environment variables
   to the Python runtime.

   Return 0 on success, or set pymain->err and return -1 on error. */
static int
pymain_configure_pyruntime(_PyMain *pymain)
{
    Py_SetProgramName(pymain->program_name);
    /* Don't free program_name here: the argument to Py_SetProgramName
       must remain valid until Py_FinalizeEx is called. The string is freed
       by pymain_free(). */

    if (pymain_add_xoptions(pymain)) {
        return -1;
    }
    if (pymain_add_warnings_options(pymain)) {
        return -1;
    }
    return 0;
}


static void
pymain_import_readline(_PyMain *pymain)
{
    if (Py_IsolatedFlag) {
        return;
    }
    if (!Py_InspectFlag && pymain->run_code) {
        return;
    }
    if (!isatty(fileno(stdin))) {
        return;
    }

    PyObject *mod = PyImport_ImportModule("readline");
    if (mod == NULL) {
        PyErr_Clear();
    }
    else {
        Py_DECREF(mod);
    }
}


static FILE*
pymain_open_filename(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;
    FILE* fp;

    fp = _Py_wfopen(cmdline->filename, L"r");
    if (fp == NULL) {
        char *cfilename_buffer;
        const char *cfilename;
        int err = errno;
        cfilename_buffer = Py_EncodeLocale(cmdline->filename, NULL);
        if (cfilename_buffer != NULL)
            cfilename = cfilename_buffer;
        else
            cfilename = "<unprintable file name>";
        fprintf(stderr, "%ls: can't open file '%s': [Errno %d] %s\n",
                pymain->argv[0], cfilename, err, strerror(err));
        PyMem_Free(cfilename_buffer);
        pymain->status = 2;
        return NULL;
    }

    if (cmdline->skip_first_line) {
        int ch;
        /* Push back first newline so line numbers
           remain the same */
        while ((ch = getc(fp)) != EOF) {
            if (ch == '\n') {
                (void)ungetc(ch, fp);
                break;
            }
        }
    }

    struct _Py_stat_struct sb;
    if (_Py_fstat_noraise(fileno(fp), &sb) == 0 &&
            S_ISDIR(sb.st_mode)) {
        fprintf(stderr,
                "%ls: '%ls' is a directory, cannot continue\n",
                pymain->argv[0], cmdline->filename);
        fclose(fp);
        pymain->status = 1;
        return NULL;
    }

    return fp;
}


static void
pymain_run(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;

    if (cmdline->filename == NULL && pymain->stdin_is_interactive) {
        Py_InspectFlag = 0; /* do exit on SystemExit */
        pymain_run_statup(&pymain->cf);
        pymain_run_interactive_hook();
    }
    /* XXX */

    if (pymain->main_importer_path != NULL) {
        pymain->status = pymain_run_main_from_importer(pymain);
        return;
    }

    FILE *fp;
    if (cmdline->filename != NULL) {
        fp = pymain_open_filename(pymain);
        if (fp == NULL) {
            return;
        }
    }
    else {
        fp = stdin;
    }

    pymain->status = pymain_run_file(fp, cmdline->filename, &pymain->cf);
}


static void
pymain_repl(_PyMain *pymain)
{
    char *p;

    /* Check this environment variable at the end, to give programs the
     * opportunity to set it from Python.
     */
    if (!Py_InspectFlag &&
        (p = Py_GETENV("PYTHONINSPECT")) && *p != '\0')
    {
        Py_InspectFlag = 1;
    }

    if (Py_InspectFlag && pymain->stdin_is_interactive && pymain->run_code) {
        Py_InspectFlag = 0;
        pymain_run_interactive_hook();
        /* XXX */
        int res = PyRun_AnyFileFlags(stdin, "<stdin>", &pymain->cf);
        pymain->status = (res != 0);
    }
}


/* Parse the command line.
   Handle --version and --help options directly.

   Return 1 if Python must exit.
   Return 0 on success.
   Set pymain->err and return -1 on failure. */
static int
pymain_init_cmdline(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;

    int res = pymain_parse_cmdline(pymain);
    if (res < 0) {
        return -1;
    }
    if (res) {
        usage(1, pymain->argv[0]);
        pymain->status = 2;
        return 1;
    }

    if (cmdline->print_help) {
        usage(0, pymain->argv[0]);
        pymain->status = 0;
        return 1;
    }

    if (cmdline->print_version) {
        printf("Python %s\n",
               (cmdline->print_version >= 2) ? Py_GetVersion() : PY_VERSION);
        return 1;
    }

    pymain->run_code = (cmdline->command != NULL || cmdline->filename != NULL
                        || cmdline->module != NULL);

    return 0;
}


/* Initialize Py_Main().
   This code must not use Python runtime apart PyMem_Raw memory allocator.

   Return 0 on success.
   Return 1 if Python is done and must exit.
   Set pymain->err and return -1 on error. */
static int
pymain_init_impl(_PyMain *pymain)
{
    _PyCoreConfig *core_config = &pymain->core_config;
    core_config->_disable_importlib = 0;

    orig_argc = pymain->argc;           /* For Py_GetArgcArgv() */
    orig_argv = pymain->argv;

    /* Parse the command line */
    int res = pymain_init_cmdline(pymain);
    if (res < 0) {
        return -1;
    }
    if (res > 0) {
        return 1;
    }

    pymain_set_global_config(pymain);
    pymain_init_stdio(pymain);

    /* Get environment variables */
    if (pymain_warnings_envvar(pymain) < 0) {
        return -1;
    }
    if (pymain_get_program_name(pymain) < 0) {
        return -1;
    }
    core_config->allocator = Py_GETENV("PYTHONMALLOC");

    return 0;
}


static int
pymain_init(_PyMain *pymain)
{
    pymain->err = _PyRuntime_Initialize();
    if (_Py_INIT_FAILED(pymain->err)) {
        return -1;
    }

    /* Make sure that all memory allocated in pymain_init() is allocated
       by malloc() */
    PyMemAllocatorEx old_alloc, raw_alloc;
    PyMem_GetAllocator(PYMEM_DOMAIN_RAW, &old_alloc);
    _PyMem_GetDefaultRawAllocator(&raw_alloc);
    PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &raw_alloc);

    int res = pymain_init_impl(pymain);

    /* Restore the old memory allocator */
    PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &old_alloc);

    return res;
}

static int
pymain_core(_PyMain *pymain)
{
    _Py_CommandLineDetails *cmdline = &pymain->cmdline;

    pymain->err = _Py_InitializeCore(&pymain->core_config);
    if (_Py_INIT_FAILED(pymain->err)) {
        return -1;
    }

    if (pymain_configure_pyruntime(pymain)) {
        return -1;
    }

    if (pymain_init_main_interpreter(pymain)) {
        return -1;
    }

    pymain_header(pymain);
    pymain_import_readline(pymain);

    pymain_init_argv(pymain);

    if (cmdline->command) {
        pymain->status = pymain_run_command(cmdline->command, &pymain->cf);
    }
    else if (cmdline->module) {
        pymain->status = (pymain_run_module(cmdline->module, 1) != 0);
    }
    else {
        pymain_run(pymain);
    }
    pymain_repl(pymain);

    if (Py_FinalizeEx() < 0) {
        /* Value unlikely to be confused with a non-error exit status or
        other special meaning */
        pymain->status = 120;
    }

    return 0;
}


int
Py_Main(int argc, wchar_t **argv)
{
    _PyMain pymain = _PyMain_INIT;
    memset(&pymain.cmdline, 0, sizeof(pymain.cmdline));
    pymain.argc = argc;
    pymain.argv = argv;

    int res = pymain_init(&pymain);
    if (res < 0) {
        _Py_FatalInitError(pymain.err);
    }
    if (res == 0) {
        res = pymain_core(&pymain);
        if (res < 0) {
            _Py_FatalInitError(pymain.err);
        }
    }

    pymain_free(&pymain);

    return pymain.status;
}

/* this is gonna seem *real weird*, but if you put some other code between
   Py_Main() and Py_GetArgcArgv() you will need to adjust the test in the
   while statement in Misc/gdbinit:ppystack */

/* Make the *original* argc/argv available to other modules.
   This is rare, but it is needed by the secureware extension. */

void
Py_GetArgcArgv(int *argc, wchar_t ***argv)
{
    *argc = orig_argc;
    *argv = orig_argv;
}

#ifdef __cplusplus
}
#endif
