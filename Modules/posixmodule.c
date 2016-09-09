
/* POSIX module implementation */

/* This file is also used for Windows NT/MS-Win.  In that case the
   module actually calls itself 'nt', not 'posix', and a few
   functions are either unimplemented or implemented differently.  The source
   assumes that for Windows NT, the macro 'MS_WINDOWS' is defined independent
   of the compiler used.  Different compilers define their own feature
   test macro, e.g. '_MSC_VER'. */



#ifdef __APPLE__
   /*
    * Step 1 of support for weak-linking a number of symbols existing on
    * OSX 10.4 and later, see the comment in the #ifdef __APPLE__ block
    * at the end of this file for more information.
    */
#  pragma weak lchown
#  pragma weak statvfs
#  pragma weak fstatvfs

#endif /* __APPLE__ */

#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "structmember.h"
#ifndef MS_WINDOWS
#include "posixmodule.h"
#else
#include "winreparse.h"
#endif

/* On android API level 21, 'AT_EACCESS' is not declared although
 * HAVE_FACCESSAT is defined. */
#ifdef __ANDROID__
#undef HAVE_FACCESSAT
#endif

#include <stdio.h>  /* needed for ctermid() */

#ifdef __cplusplus
extern "C" {
#endif

PyDoc_STRVAR(posix__doc__,
"This module provides access to operating system functionality that is\n\
standardized by the C Standard and the POSIX standard (a thinly\n\
disguised Unix interface).  Refer to the library manual and\n\
corresponding Unix manual entries for more information on calls.");


#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>           /* For WNOHANG */
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_GRP_H
#include <grp.h>
#endif

#ifdef HAVE_SYSEXITS_H
#include <sysexits.h>
#endif /* HAVE_SYSEXITS_H */

#ifdef HAVE_SYS_LOADAVG_H
#include <sys/loadavg.h>
#endif

#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#ifdef HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#endif

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#if !defined(CPU_ALLOC) && defined(HAVE_SCHED_SETAFFINITY)
#undef HAVE_SCHED_SETAFFINITY
#endif

#if defined(HAVE_SYS_XATTR_H) && defined(__GLIBC__) && !defined(__FreeBSD_kernel__) && !defined(__GNU__)
#define USE_XATTRS
#endif

#ifdef USE_XATTRS
#include <sys/xattr.h>
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__APPLE__)
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#endif

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef __hpux
#include <sys/mpctl.h>
#endif

#if defined(__DragonFly__) || \
    defined(__OpenBSD__)   || \
    defined(__FreeBSD__)   || \
    defined(__NetBSD__)    || \
    defined(__APPLE__)
#include <sys/sysctl.h>
#endif

#ifdef HAVE_LINUX_RANDOM_H
#  include <linux/random.h>
#endif
#ifdef HAVE_GETRANDOM_SYSCALL
#  include <sys/syscall.h>
#endif

#if defined(MS_WINDOWS)
#  define TERMSIZE_USE_CONIO
#elif defined(HAVE_SYS_IOCTL_H)
#  include <sys/ioctl.h>
#  if defined(HAVE_TERMIOS_H)
#    include <termios.h>
#  endif
#  if defined(TIOCGWINSZ)
#    define TERMSIZE_USE_IOCTL
#  endif
#endif /* MS_WINDOWS */

/* Various compilers have only certain posix functions */
/* XXX Gosh I wish these were all moved into pyconfig.h */
#if defined(__WATCOMC__) && !defined(__QNX__)           /* Watcom compiler */
#define HAVE_OPENDIR    1
#define HAVE_SYSTEM     1
#include <process.h>
#else
#ifdef _MSC_VER         /* Microsoft compiler */
#define HAVE_GETPPID    1
#define HAVE_GETLOGIN   1
#define HAVE_SPAWNV     1
#define HAVE_EXECV      1
#define HAVE_WSPAWNV    1
#define HAVE_WEXECV     1
#define HAVE_PIPE       1
#define HAVE_SYSTEM     1
#define HAVE_CWAIT      1
#define HAVE_FSYNC      1
#define fsync _commit
#else
/* Unix functions that the configure script doesn't check for */
#define HAVE_EXECV      1
#define HAVE_FORK       1
#if defined(__USLC__) && defined(__SCO_VERSION__)       /* SCO UDK Compiler */
#define HAVE_FORK1      1
#endif
#define HAVE_GETEGID    1
#define HAVE_GETEUID    1
#define HAVE_GETGID     1
#define HAVE_GETPPID    1
#define HAVE_GETUID     1
#define HAVE_KILL       1
#define HAVE_OPENDIR    1
#define HAVE_PIPE       1
#define HAVE_SYSTEM     1
#define HAVE_WAIT       1
#define HAVE_TTYNAME    1
#endif  /* _MSC_VER */
#endif  /* ! __WATCOMC__ || __QNX__ */


/*[clinic input]
# one of the few times we lie about this name!
module os
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=94a0f0f978acae17]*/

#ifndef _MSC_VER

#if defined(__sgi)&&_COMPILER_VERSION>=700
/* declare ctermid_r if compiling with MIPSPro 7.x in ANSI C mode
   (default) */
extern char        *ctermid_r(char *);
#endif

#ifndef HAVE_UNISTD_H
#if defined(PYCC_VACPP)
extern int mkdir(char *);
#else
#if ( defined(__WATCOMC__) || defined(_MSC_VER) ) && !defined(__QNX__)
extern int mkdir(const char *);
#else
extern int mkdir(const char *, mode_t);
#endif
#endif
#if defined(__IBMC__) || defined(__IBMCPP__)
extern int chdir(char *);
extern int rmdir(char *);
#else
extern int chdir(const char *);
extern int rmdir(const char *);
#endif
extern int chmod(const char *, mode_t);
/*#ifdef HAVE_FCHMOD
extern int fchmod(int, mode_t);
#endif*/
/*#ifdef HAVE_LCHMOD
extern int lchmod(const char *, mode_t);
#endif*/
extern int chown(const char *, uid_t, gid_t);
extern char *getcwd(char *, int);
extern char *strerror(int);
extern int link(const char *, const char *);
extern int rename(const char *, const char *);
extern int stat(const char *, struct stat *);
extern int unlink(const char *);
#ifdef HAVE_SYMLINK
extern int symlink(const char *, const char *);
#endif /* HAVE_SYMLINK */
#ifdef HAVE_LSTAT
extern int lstat(const char *, struct stat *);
#endif /* HAVE_LSTAT */
#endif /* !HAVE_UNISTD_H */

#endif /* !_MSC_VER */

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif /* HAVE_UTIME_H */

#ifdef HAVE_SYS_UTIME_H
#include <sys/utime.h>
#define HAVE_UTIME_H /* pretend we do for the rest of this file */
#endif /* HAVE_SYS_UTIME_H */

#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif /* HAVE_SYS_TIMES_H */

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */

#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif /* HAVE_SYS_UTSNAME_H */

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#if defined(__WATCOMC__) && !defined(__QNX__)
#include <direct.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#endif
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#ifdef _MSC_VER
#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_PROCESS_H
#include <process.h>
#endif
#ifndef VOLUME_NAME_DOS
#define VOLUME_NAME_DOS 0x0
#endif
#ifndef VOLUME_NAME_NT
#define VOLUME_NAME_NT  0x2
#endif
#ifndef IO_REPARSE_TAG_SYMLINK
#define IO_REPARSE_TAG_SYMLINK (0xA000000CL)
#endif
#ifndef IO_REPARSE_TAG_MOUNT_POINT
#define IO_REPARSE_TAG_MOUNT_POINT (0xA0000003L)
#endif
#include "osdefs.h"
#include <malloc.h>
#include <windows.h>
#include <shellapi.h>   /* for ShellExecute() */
#include <lmcons.h>     /* for UNLEN */
#ifdef SE_CREATE_SYMBOLIC_LINK_NAME /* Available starting with Vista */
#define HAVE_SYMLINK
static int win32_can_symlink = 0;
#endif
#endif /* _MSC_VER */

#ifndef MAXPATHLEN
#if defined(PATH_MAX) && PATH_MAX > 1024
#define MAXPATHLEN PATH_MAX
#else
#define MAXPATHLEN 1024
#endif
#endif /* MAXPATHLEN */

#ifdef UNION_WAIT
/* Emulate some macros on systems that have a union instead of macros */

#ifndef WIFEXITED
#define WIFEXITED(u_wait) (!(u_wait).w_termsig && !(u_wait).w_coredump)
#endif

#ifndef WEXITSTATUS
#define WEXITSTATUS(u_wait) (WIFEXITED(u_wait)?((u_wait).w_retcode):-1)
#endif

#ifndef WTERMSIG
#define WTERMSIG(u_wait) ((u_wait).w_termsig)
#endif

#define WAIT_TYPE union wait
#define WAIT_STATUS_INT(s) (s.w_status)

#else /* !UNION_WAIT */
#define WAIT_TYPE int
#define WAIT_STATUS_INT(s) (s)
#endif /* UNION_WAIT */

/* Don't use the "_r" form if we don't need it (also, won't have a
   prototype for it, at least on Solaris -- maybe others as well?). */
#if defined(HAVE_CTERMID_R) && defined(WITH_THREAD)
#define USE_CTERMID_R
#endif

/* choose the appropriate stat and fstat functions and return structs */
#undef STAT
#undef FSTAT
#undef STRUCT_STAT
#ifdef MS_WINDOWS
#       define STAT win32_stat
#       define LSTAT win32_lstat
#       define FSTAT _Py_fstat_noraise
#       define STRUCT_STAT struct _Py_stat_struct
#else
#       define STAT stat
#       define LSTAT lstat
#       define FSTAT fstat
#       define STRUCT_STAT struct stat
#endif

#if defined(MAJOR_IN_MKDEV)
#include <sys/mkdev.h>
#else
#if defined(MAJOR_IN_SYSMACROS)
#include <sys/sysmacros.h>
#endif
#if defined(HAVE_MKNOD) && defined(HAVE_SYS_MKDEV_H)
#include <sys/mkdev.h>
#endif
#endif

#define DWORD_MAX 4294967295U

#ifdef MS_WINDOWS
#define INITFUNC PyInit_nt
#define MODNAME "nt"
#else
#define INITFUNC PyInit_posix
#define MODNAME "posix"
#endif

#ifdef MS_WINDOWS
/* defined in fileutils.c */
PyAPI_FUNC(void) _Py_time_t_to_FILE_TIME(time_t, int, FILETIME *);
PyAPI_FUNC(void) _Py_attribute_data_to_stat(BY_HANDLE_FILE_INFORMATION *,
                                            ULONG, struct _Py_stat_struct *);
#endif

#ifdef MS_WINDOWS
static int
win32_warn_bytes_api()
{
    return PyErr_WarnEx(PyExc_DeprecationWarning,
        "The Windows bytes API has been deprecated, "
        "use Unicode filenames instead",
        1);
}
#endif


#ifndef MS_WINDOWS
PyObject *
_PyLong_FromUid(uid_t uid)
{
    if (uid == (uid_t)-1)
        return PyLong_FromLong(-1);
    return PyLong_FromUnsignedLong(uid);
}

PyObject *
_PyLong_FromGid(gid_t gid)
{
    if (gid == (gid_t)-1)
        return PyLong_FromLong(-1);
    return PyLong_FromUnsignedLong(gid);
}

int
_Py_Uid_Converter(PyObject *obj, void *p)
{
    uid_t uid;
    PyObject *index;
    int overflow;
    long result;
    unsigned long uresult;

    index = PyNumber_Index(obj);
    if (index == NULL) {
        PyErr_Format(PyExc_TypeError,
                     "uid should be integer, not %.200s",
                     Py_TYPE(obj)->tp_name);
        return 0;
    }

    /*
     * Handling uid_t is complicated for two reasons:
     *  * Although uid_t is (always?) unsigned, it still
     *    accepts -1.
     *  * We don't know its size in advance--it may be
     *    bigger than an int, or it may be smaller than
     *    a long.
     *
     * So a bit of defensive programming is in order.
     * Start with interpreting the value passed
     * in as a signed long and see if it works.
     */

    result = PyLong_AsLongAndOverflow(index, &overflow);

    if (!overflow) {
        uid = (uid_t)result;

        if (result == -1) {
            if (PyErr_Occurred())
                goto fail;
            /* It's a legitimate -1, we're done. */
            goto success;
        }

        /* Any other negative number is disallowed. */
        if (result < 0)
            goto underflow;

        /* Ensure the value wasn't truncated. */
        if (sizeof(uid_t) < sizeof(long) &&
            (long)uid != result)
            goto underflow;
        goto success;
    }

    if (overflow < 0)
        goto underflow;

    /*
     * Okay, the value overflowed a signed long.  If it
     * fits in an *unsigned* long, it may still be okay,
     * as uid_t may be unsigned long on this platform.
     */
    uresult = PyLong_AsUnsignedLong(index);
    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_OverflowError))
            goto overflow;
        goto fail;
    }

    uid = (uid_t)uresult;

    /*
     * If uid == (uid_t)-1, the user actually passed in ULONG_MAX,
     * but this value would get interpreted as (uid_t)-1  by chown
     * and its siblings.   That's not what the user meant!  So we
     * throw an overflow exception instead.   (We already
     * handled a real -1 with PyLong_AsLongAndOverflow() above.)
     */
    if (uid == (uid_t)-1)
        goto overflow;

    /* Ensure the value wasn't truncated. */
    if (sizeof(uid_t) < sizeof(long) &&
        (unsigned long)uid != uresult)
        goto overflow;
    /* fallthrough */

success:
    Py_DECREF(index);
    *(uid_t *)p = uid;
    return 1;

underflow:
    PyErr_SetString(PyExc_OverflowError,
                    "uid is less than minimum");
    goto fail;

overflow:
    PyErr_SetString(PyExc_OverflowError,
                    "uid is greater than maximum");
    /* fallthrough */

fail:
    Py_DECREF(index);
    return 0;
}

int
_Py_Gid_Converter(PyObject *obj, void *p)
{
    gid_t gid;
    PyObject *index;
    int overflow;
    long result;
    unsigned long uresult;

    index = PyNumber_Index(obj);
    if (index == NULL) {
        PyErr_Format(PyExc_TypeError,
                     "gid should be integer, not %.200s",
                     Py_TYPE(obj)->tp_name);
        return 0;
    }

    /*
     * Handling gid_t is complicated for two reasons:
     *  * Although gid_t is (always?) unsigned, it still
     *    accepts -1.
     *  * We don't know its size in advance--it may be
     *    bigger than an int, or it may be smaller than
     *    a long.
     *
     * So a bit of defensive programming is in order.
     * Start with interpreting the value passed
     * in as a signed long and see if it works.
     */

    result = PyLong_AsLongAndOverflow(index, &overflow);

    if (!overflow) {
        gid = (gid_t)result;

        if (result == -1) {
            if (PyErr_Occurred())
                goto fail;
            /* It's a legitimate -1, we're done. */
            goto success;
        }

        /* Any other negative number is disallowed. */
        if (result < 0) {
            goto underflow;
        }

        /* Ensure the value wasn't truncated. */
        if (sizeof(gid_t) < sizeof(long) &&
            (long)gid != result)
            goto underflow;
        goto success;
    }

    if (overflow < 0)
        goto underflow;

    /*
     * Okay, the value overflowed a signed long.  If it
     * fits in an *unsigned* long, it may still be okay,
     * as gid_t may be unsigned long on this platform.
     */
    uresult = PyLong_AsUnsignedLong(index);
    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_OverflowError))
            goto overflow;
        goto fail;
    }

    gid = (gid_t)uresult;

    /*
     * If gid == (gid_t)-1, the user actually passed in ULONG_MAX,
     * but this value would get interpreted as (gid_t)-1  by chown
     * and its siblings.   That's not what the user meant!  So we
     * throw an overflow exception instead.   (We already
     * handled a real -1 with PyLong_AsLongAndOverflow() above.)
     */
    if (gid == (gid_t)-1)
        goto overflow;

    /* Ensure the value wasn't truncated. */
    if (sizeof(gid_t) < sizeof(long) &&
        (unsigned long)gid != uresult)
        goto overflow;
    /* fallthrough */

success:
    Py_DECREF(index);
    *(gid_t *)p = gid;
    return 1;

underflow:
    PyErr_SetString(PyExc_OverflowError,
                    "gid is less than minimum");
    goto fail;

overflow:
    PyErr_SetString(PyExc_OverflowError,
                    "gid is greater than maximum");
    /* fallthrough */

fail:
    Py_DECREF(index);
    return 0;
}
#endif /* MS_WINDOWS */


#define _PyLong_FromDev PyLong_FromLongLong


#if defined(HAVE_MKNOD) && defined(HAVE_MAKEDEV)
static int
_Py_Dev_Converter(PyObject *obj, void *p)
{
    *((dev_t *)p) = PyLong_AsUnsignedLongLong(obj);
    if (PyErr_Occurred())
        return 0;
    return 1;
}
#endif /* HAVE_MKNOD && HAVE_MAKEDEV */


#ifdef AT_FDCWD
/*
 * Why the (int) cast?  Solaris 10 defines AT_FDCWD as 0xffd19553 (-3041965);
 * without the int cast, the value gets interpreted as uint (4291925331),
 * which doesn't play nicely with all the initializer lines in this file that
 * look like this:
 *      int dir_fd = DEFAULT_DIR_FD;
 */
#define DEFAULT_DIR_FD (int)AT_FDCWD
#else
#define DEFAULT_DIR_FD (-100)
#endif

static int
_fd_converter(PyObject *o, int *p)
{
    int overflow;
    long long_value;

    PyObject *index = PyNumber_Index(o);
    if (index == NULL) {
        return 0;
    }

    assert(PyLong_Check(index));
    long_value = PyLong_AsLongAndOverflow(index, &overflow);
    Py_DECREF(index);
    assert(!PyErr_Occurred());
    if (overflow > 0 || long_value > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "fd is greater than maximum");
        return 0;
    }
    if (overflow < 0 || long_value < INT_MIN) {
        PyErr_SetString(PyExc_OverflowError,
                        "fd is less than minimum");
        return 0;
    }

    *p = (int)long_value;
    return 1;
}

static int
dir_fd_converter(PyObject *o, void *p)
{
    if (o == Py_None) {
        *(int *)p = DEFAULT_DIR_FD;
        return 1;
    }
    else if (PyIndex_Check(o)) {
        return _fd_converter(o, (int *)p);
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "argument should be integer or None, not %.200s",
                     Py_TYPE(o)->tp_name);
        return 0;
    }
}


/*
 * A PyArg_ParseTuple "converter" function
 * that handles filesystem paths in the manner
 * preferred by the os module.
 *
 * path_converter accepts (Unicode) strings and their
 * subclasses, and bytes and their subclasses.  What
 * it does with the argument depends on the platform:
 *
 *   * On Windows, if we get a (Unicode) string we
 *     extract the wchar_t * and return it; if we get
 *     bytes we decode to wchar_t * and return that.
 *
 *   * On all other platforms, strings are encoded
 *     to bytes using PyUnicode_FSConverter, then we
 *     extract the char * from the bytes object and
 *     return that.
 *
 * path_converter also optionally accepts signed
 * integers (representing open file descriptors) instead
 * of path strings.
 *
 * Input fields:
 *   path.nullable
 *     If nonzero, the path is permitted to be None.
 *   path.allow_fd
 *     If nonzero, the path is permitted to be a file handle
 *     (a signed int) instead of a string.
 *   path.function_name
 *     If non-NULL, path_converter will use that as the name
 *     of the function in error messages.
 *     (If path.function_name is NULL it omits the function name.)
 *   path.argument_name
 *     If non-NULL, path_converter will use that as the name
 *     of the parameter in error messages.
 *     (If path.argument_name is NULL it uses "path".)
 *
 * Output fields:
 *   path.wide
 *     Points to the path if it was expressed as Unicode
 *     and was not encoded.  (Only used on Windows.)
 *   path.narrow
 *     Points to the path if it was expressed as bytes,
 *     or it was Unicode and was encoded to bytes. (On Windows,
 *     is an non-zero integer if the path was expressed as bytes.
 *     The type is deliberately incompatible to prevent misuse.)
 *   path.fd
 *     Contains a file descriptor if path.accept_fd was true
 *     and the caller provided a signed integer instead of any
 *     sort of string.
 *
 *     WARNING: if your "path" parameter is optional, and is
 *     unspecified, path_converter will never get called.
 *     So if you set allow_fd, you *MUST* initialize path.fd = -1
 *     yourself!
 *   path.length
 *     The length of the path in characters, if specified as
 *     a string.
 *   path.object
 *     The original object passed in.
 *   path.cleanup
 *     For internal use only.  May point to a temporary object.
 *     (Pay no attention to the man behind the curtain.)
 *
 *   At most one of path.wide or path.narrow will be non-NULL.
 *   If path was None and path.nullable was set,
 *     or if path was an integer and path.allow_fd was set,
 *     both path.wide and path.narrow will be NULL
 *     and path.length will be 0.
 *
 *   path_converter takes care to not write to the path_t
 *   unless it's successful.  However it must reset the
 *   "cleanup" field each time it's called.
 *
 * Use as follows:
 *      path_t path;
 *      memset(&path, 0, sizeof(path));
 *      PyArg_ParseTuple(args, "O&", path_converter, &path);
 *      // ... use values from path ...
 *      path_cleanup(&path);
 *
 * (Note that if PyArg_Parse fails you don't need to call
 * path_cleanup().  However it is safe to do so.)
 */
typedef struct {
    const char *function_name;
    const char *argument_name;
    int nullable;
    int allow_fd;
    const wchar_t *wide;
#ifdef MS_WINDOWS
    BOOL narrow;
#else
    const char *narrow;
#endif
    int fd;
    Py_ssize_t length;
    PyObject *object;
    PyObject *cleanup;
} path_t;

#ifdef MS_WINDOWS
#define PATH_T_INITIALIZE(function_name, argument_name, nullable, allow_fd) \
    {function_name, argument_name, nullable, allow_fd, NULL, FALSE, -1, 0, NULL, NULL}
#else
#define PATH_T_INITIALIZE(function_name, argument_name, nullable, allow_fd) \
    {function_name, argument_name, nullable, allow_fd, NULL, NULL, -1, 0, NULL, NULL}
#endif

static void
path_cleanup(path_t *path) {
    if (path->cleanup) {
        Py_CLEAR(path->cleanup);
    }
}

static int
path_converter(PyObject *o, void *p)
{
    path_t *path = (path_t *)p;
    PyObject *bytes, *to_cleanup = NULL;
    Py_ssize_t length;
    int is_index, is_buffer, is_bytes, is_unicode;
    /* Default to failure, forcing explicit signaling of succcess. */
    int ret = 0;
    const char *narrow;
#ifdef MS_WINDOWS
    PyObject *wo;
    const wchar_t *wide;
#endif

#define FORMAT_EXCEPTION(exc, fmt) \
    PyErr_Format(exc, "%s%s" fmt, \
        path->function_name ? path->function_name : "", \
        path->function_name ? ": "                : "", \
        path->argument_name ? path->argument_name : "path")

    /* Py_CLEANUP_SUPPORTED support */
    if (o == NULL) {
        path_cleanup(path);
        return 1;
    }

    /* Ensure it's always safe to call path_cleanup(). */
    path->cleanup = NULL;

    if ((o == Py_None) && path->nullable) {
        path->wide = NULL;
#ifdef MS_WINDOWS
        path->narrow = FALSE;
#else
        path->narrow = NULL;
#endif
        path->length = 0;
        path->object = o;
        path->fd = -1;
        return 1;
    }

    /* Only call this here so that we don't treat the return value of
       os.fspath() as an fd or buffer. */
    is_index = path->allow_fd && PyIndex_Check(o);
    is_buffer = PyObject_CheckBuffer(o);
    is_bytes = PyBytes_Check(o);
    is_unicode = PyUnicode_Check(o);

    if (!is_index && !is_buffer && !is_unicode && !is_bytes) {
        /* Inline PyOS_FSPath() for better error messages. */
        _Py_IDENTIFIER(__fspath__);
        PyObject *func = NULL;

        func = _PyObject_LookupSpecial(o, &PyId___fspath__);
        if (NULL == func) {
            goto error_exit;
        }

        o = to_cleanup = PyObject_CallFunctionObjArgs(func, NULL);
        Py_DECREF(func);
        if (NULL == o) {
            goto error_exit;
        }
        else if (PyUnicode_Check(o)) {
            is_unicode = 1;
        }
        else if (PyBytes_Check(o)) {
            is_bytes = 1;
        }
        else {
            goto error_exit;
        }
    }

    if (is_unicode) {
#ifdef MS_WINDOWS
        wide = PyUnicode_AsWideCharString(o, &length);
        if (!wide) {
            goto exit;
        }
        if (length > 32767) {
            FORMAT_EXCEPTION(PyExc_ValueError, "%s too long for Windows");
            goto exit;
        }
        if (wcslen(wide) != length) {
            FORMAT_EXCEPTION(PyExc_ValueError, "embedded null character in %s");
            goto exit;
        }

        path->wide = wide;
        path->length = length;
        path->object = o;
        path->fd = -1;
        ret = 1;
        goto exit;
#else
        if (!PyUnicode_FSConverter(o, &bytes)) {
            goto exit;
        }
#endif
    }
    else if (is_bytes) {
        bytes = o;
        Py_INCREF(bytes);
    }
    else if (is_buffer) {
        if (PyErr_WarnFormat(PyExc_DeprecationWarning, 1,
            "%s%s%s should be %s, not %.200s",
            path->function_name ? path->function_name : "",
            path->function_name ? ": "                : "",
            path->argument_name ? path->argument_name : "path",
            path->allow_fd && path->nullable ? "string, bytes, os.PathLike, "
                                               "integer or None" :
            path->allow_fd ? "string, bytes, os.PathLike or integer" :
            path->nullable ? "string, bytes, os.PathLike or None" :
                             "string, bytes or os.PathLike",
            Py_TYPE(o)->tp_name)) {
            goto exit;
        }
        bytes = PyBytes_FromObject(o);
        if (!bytes) {
            goto exit;
        }
    }
    else if (is_index) {
        if (!_fd_converter(o, &path->fd)) {
            goto exit;
        }
        path->wide = NULL;
#ifdef MS_WINDOWS
        path->narrow = FALSE;
#else
        path->narrow = NULL;
#endif
        path->length = 0;
        path->object = o;
        ret = 1;
        goto exit;
    }
    else {
 error_exit:
        PyErr_Format(PyExc_TypeError, "%s%s%s should be %s, not %.200s",
            path->function_name ? path->function_name : "",
            path->function_name ? ": "                : "",
            path->argument_name ? path->argument_name : "path",
            path->allow_fd && path->nullable ? "string, bytes, os.PathLike, "
                                               "integer or None" :
            path->allow_fd ? "string, bytes, os.PathLike or integer" :
            path->nullable ? "string, bytes, os.PathLike or None" :
                             "string, bytes or os.PathLike",
            Py_TYPE(o)->tp_name);
        goto exit;
    }

    length = PyBytes_GET_SIZE(bytes);
    narrow = PyBytes_AS_STRING(bytes);
    if ((size_t)length != strlen(narrow)) {
        FORMAT_EXCEPTION(PyExc_ValueError, "embedded null character in %s");
        Py_DECREF(bytes);
        goto exit;
    }

#ifdef MS_WINDOWS
    wo = PyUnicode_DecodeFSDefaultAndSize(
        narrow,
        length
    );
    if (!wo) {
        goto exit;
    }

    wide = PyUnicode_AsWideCharString(wo, &length);
    Py_DECREF(wo);

    if (!wide) {
        goto exit;
    }
    if (length > 32767) {
        FORMAT_EXCEPTION(PyExc_ValueError, "%s too long for Windows");
        goto exit;
    }
    if (wcslen(wide) != length) {
        FORMAT_EXCEPTION(PyExc_ValueError, "embedded null character in %s");
        goto exit;
    }
    path->wide = wide;
    path->narrow = TRUE;
#else
    path->wide = NULL;
    path->narrow = narrow;
#endif
    path->length = length;
    path->object = o;
    path->fd = -1;
    if (bytes == o) {
        Py_DECREF(bytes);
        ret = 1;
    }
    else {
        path->cleanup = bytes;
        ret = Py_CLEANUP_SUPPORTED;
    }
 exit:
    Py_XDECREF(to_cleanup);
    return ret;
}

static void
argument_unavailable_error(const char *function_name, const char *argument_name)
{
    PyErr_Format(PyExc_NotImplementedError,
        "%s%s%s unavailable on this platform",
        (function_name != NULL) ? function_name : "",
        (function_name != NULL) ? ": ": "",
        argument_name);
}

static int
dir_fd_unavailable(PyObject *o, void *p)
{
    int dir_fd;
    if (!dir_fd_converter(o, &dir_fd))
        return 0;
    if (dir_fd != DEFAULT_DIR_FD) {
        argument_unavailable_error(NULL, "dir_fd");
        return 0;
    }
    *(int *)p = dir_fd;
    return 1;
}

static int
fd_specified(const char *function_name, int fd)
{
    if (fd == -1)
        return 0;

    argument_unavailable_error(function_name, "fd");
    return 1;
}

static int
follow_symlinks_specified(const char *function_name, int follow_symlinks)
{
    if (follow_symlinks)
        return 0;

    argument_unavailable_error(function_name, "follow_symlinks");
    return 1;
}

static int
path_and_dir_fd_invalid(const char *function_name, path_t *path, int dir_fd)
{
    if (!path->wide && (dir_fd != DEFAULT_DIR_FD)
#ifndef MS_WINDOWS
        && !path->narrow
#endif
    ) {
        PyErr_Format(PyExc_ValueError,
                     "%s: can't specify dir_fd without matching path",
                     function_name);
        return 1;
    }
    return 0;
}

static int
dir_fd_and_fd_invalid(const char *function_name, int dir_fd, int fd)
{
    if ((dir_fd != DEFAULT_DIR_FD) && (fd != -1)) {
        PyErr_Format(PyExc_ValueError,
                     "%s: can't specify both dir_fd and fd",
                     function_name);
        return 1;
    }
    return 0;
}

static int
fd_and_follow_symlinks_invalid(const char *function_name, int fd,
                               int follow_symlinks)
{
    if ((fd > 0) && (!follow_symlinks)) {
        PyErr_Format(PyExc_ValueError,
                     "%s: cannot use fd and follow_symlinks together",
                     function_name);
        return 1;
    }
    return 0;
}

static int
dir_fd_and_follow_symlinks_invalid(const char *function_name, int dir_fd,
                                   int follow_symlinks)
{
    if ((dir_fd != DEFAULT_DIR_FD) && (!follow_symlinks)) {
        PyErr_Format(PyExc_ValueError,
                     "%s: cannot use dir_fd and follow_symlinks together",
                     function_name);
        return 1;
    }
    return 0;
}

#ifdef MS_WINDOWS
    typedef long long Py_off_t;
#else
    typedef off_t Py_off_t;
#endif

static int
Py_off_t_converter(PyObject *arg, void *addr)
{
#ifdef HAVE_LARGEFILE_SUPPORT
    *((Py_off_t *)addr) = PyLong_AsLongLong(arg);
#else
    *((Py_off_t *)addr) = PyLong_AsLong(arg);
#endif
    if (PyErr_Occurred())
        return 0;
    return 1;
}

static PyObject *
PyLong_FromPy_off_t(Py_off_t offset)
{
#ifdef HAVE_LARGEFILE_SUPPORT
    return PyLong_FromLongLong(offset);
#else
    return PyLong_FromLong(offset);
#endif
}

#ifdef MS_WINDOWS

static int
win32_get_reparse_tag(HANDLE reparse_point_handle, ULONG *reparse_tag)
{
    char target_buffer[_Py_MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    _Py_REPARSE_DATA_BUFFER *rdb = (_Py_REPARSE_DATA_BUFFER *)target_buffer;
    DWORD n_bytes_returned;

    if (0 == DeviceIoControl(
        reparse_point_handle,
        FSCTL_GET_REPARSE_POINT,
        NULL, 0, /* in buffer */
        target_buffer, sizeof(target_buffer),
        &n_bytes_returned,
        NULL)) /* we're not using OVERLAPPED_IO */
        return FALSE;

    if (reparse_tag)
        *reparse_tag = rdb->ReparseTag;

    return TRUE;
}

#endif /* MS_WINDOWS */

/* Return a dictionary corresponding to the POSIX environment table */
#if defined(WITH_NEXT_FRAMEWORK) || (defined(__APPLE__) && defined(Py_ENABLE_SHARED))
/* On Darwin/MacOSX a shared library or framework has no access to
** environ directly, we must obtain it with _NSGetEnviron(). See also
** man environ(7).
*/
#include <crt_externs.h>
static char **environ;
#elif !defined(_MSC_VER) && ( !defined(__WATCOMC__) || defined(__QNX__) )
extern char **environ;
#endif /* !_MSC_VER */

static PyObject *
convertenviron(void)
{
    PyObject *d;
#ifdef MS_WINDOWS
    wchar_t **e;
#else
    char **e;
#endif

    d = PyDict_New();
    if (d == NULL)
        return NULL;
#if defined(WITH_NEXT_FRAMEWORK) || (defined(__APPLE__) && defined(Py_ENABLE_SHARED))
    if (environ == NULL)
        environ = *_NSGetEnviron();
#endif
#ifdef MS_WINDOWS
    /* _wenviron must be initialized in this way if the program is started
       through main() instead of wmain(). */
    _wgetenv(L"");
    if (_wenviron == NULL)
        return d;
    /* This part ignores errors */
    for (e = _wenviron; *e != NULL; e++) {
        PyObject *k;
        PyObject *v;
        const wchar_t *p = wcschr(*e, L'=');
        if (p == NULL)
            continue;
        k = PyUnicode_FromWideChar(*e, (Py_ssize_t)(p-*e));
        if (k == NULL) {
            PyErr_Clear();
            continue;
        }
        v = PyUnicode_FromWideChar(p+1, wcslen(p+1));
        if (v == NULL) {
            PyErr_Clear();
            Py_DECREF(k);
            continue;
        }
        if (PyDict_GetItem(d, k) == NULL) {
            if (PyDict_SetItem(d, k, v) != 0)
                PyErr_Clear();
        }
        Py_DECREF(k);
        Py_DECREF(v);
    }
#else
    if (environ == NULL)
        return d;
    /* This part ignores errors */
    for (e = environ; *e != NULL; e++) {
        PyObject *k;
        PyObject *v;
        const char *p = strchr(*e, '=');
        if (p == NULL)
            continue;
        k = PyBytes_FromStringAndSize(*e, (int)(p-*e));
        if (k == NULL) {
            PyErr_Clear();
            continue;
        }
        v = PyBytes_FromStringAndSize(p+1, strlen(p+1));
        if (v == NULL) {
            PyErr_Clear();
            Py_DECREF(k);
            continue;
        }
        if (PyDict_GetItem(d, k) == NULL) {
            if (PyDict_SetItem(d, k, v) != 0)
                PyErr_Clear();
        }
        Py_DECREF(k);
        Py_DECREF(v);
    }
#endif
    return d;
}

/* Set a POSIX-specific error from errno, and return NULL */

static PyObject *
posix_error(void)
{
    return PyErr_SetFromErrno(PyExc_OSError);
}

#ifdef MS_WINDOWS
static PyObject *
win32_error(const char* function, const char* filename)
{
    /* XXX We should pass the function name along in the future.
       (winreg.c also wants to pass the function name.)
       This would however require an additional param to the
       Windows error object, which is non-trivial.
    */
    errno = GetLastError();
    if (filename)
        return PyErr_SetFromWindowsErrWithFilename(errno, filename);
    else
        return PyErr_SetFromWindowsErr(errno);
}

static PyObject *
win32_error_object(const char* function, PyObject* filename)
{
    /* XXX - see win32_error for comments on 'function' */
    errno = GetLastError();
    if (filename)
        return PyErr_SetExcFromWindowsErrWithFilenameObject(
                    PyExc_OSError,
                    errno,
                    filename);
    else
        return PyErr_SetFromWindowsErr(errno);
}

#endif /* MS_WINDOWS */

static PyObject *
path_error(path_t *path)
{
#ifdef MS_WINDOWS
    return PyErr_SetExcFromWindowsErrWithFilenameObject(PyExc_OSError,
                                                        0, path->object);
#else
    return PyErr_SetFromErrnoWithFilenameObject(PyExc_OSError, path->object);
#endif
}


static PyObject *
path_error2(path_t *path, path_t *path2)
{
#ifdef MS_WINDOWS
    return PyErr_SetExcFromWindowsErrWithFilenameObjects(PyExc_OSError,
            0, path->object, path2->object);
#else
    return PyErr_SetFromErrnoWithFilenameObjects(PyExc_OSError,
        path->object, path2->object);
#endif
}


/* POSIX generic methods */

static int
fildes_converter(PyObject *o, void *p)
{
    int fd;
    int *pointer = (int *)p;
    fd = PyObject_AsFileDescriptor(o);
    if (fd < 0)
        return 0;
    *pointer = fd;
    return 1;
}

static PyObject *
posix_fildes_fd(int fd, int (*func)(int))
{
    int res;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        _Py_BEGIN_SUPPRESS_IPH
        res = (*func)(fd);
        _Py_END_SUPPRESS_IPH
        Py_END_ALLOW_THREADS
    } while (res != 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res != 0)
        return (!async_err) ? posix_error() : NULL;
    Py_RETURN_NONE;
}


#ifdef MS_WINDOWS
/* This is a reimplementation of the C library's chdir function,
   but one that produces Win32 errors instead of DOS error codes.
   chdir is essentially a wrapper around SetCurrentDirectory; however,
   it also needs to set "magic" environment variables indicating
   the per-drive current directory, which are of the form =<drive>: */
static BOOL __stdcall
win32_wchdir(LPCWSTR path)
{
    wchar_t path_buf[MAX_PATH], *new_path = path_buf;
    int result;
    wchar_t env[4] = L"=x:";

    if(!SetCurrentDirectoryW(path))
        return FALSE;
    result = GetCurrentDirectoryW(Py_ARRAY_LENGTH(path_buf), new_path);
    if (!result)
        return FALSE;
    if (result > Py_ARRAY_LENGTH(path_buf)) {
        new_path = PyMem_RawMalloc(result * sizeof(wchar_t));
        if (!new_path) {
            SetLastError(ERROR_OUTOFMEMORY);
            return FALSE;
        }
        result = GetCurrentDirectoryW(result, new_path);
        if (!result) {
            PyMem_RawFree(new_path);
            return FALSE;
        }
    }
    if (wcsncmp(new_path, L"\\\\", 2) == 0 ||
        wcsncmp(new_path, L"//", 2) == 0)
        /* UNC path, nothing to do. */
        return TRUE;
    env[1] = new_path[0];
    result = SetEnvironmentVariableW(env, new_path);
    if (new_path != path_buf)
        PyMem_RawFree(new_path);
    return result;
}
#endif

#ifdef MS_WINDOWS
/* The CRT of Windows has a number of flaws wrt. its stat() implementation:
   - time stamps are restricted to second resolution
   - file modification times suffer from forth-and-back conversions between
     UTC and local time
   Therefore, we implement our own stat, based on the Win32 API directly.
*/
#define HAVE_STAT_NSEC 1
#define HAVE_STRUCT_STAT_ST_FILE_ATTRIBUTES 1

static void
find_data_to_file_info(WIN32_FIND_DATAW *pFileData,
                       BY_HANDLE_FILE_INFORMATION *info,
                       ULONG *reparse_tag)
{
    memset(info, 0, sizeof(*info));
    info->dwFileAttributes = pFileData->dwFileAttributes;
    info->ftCreationTime   = pFileData->ftCreationTime;
    info->ftLastAccessTime = pFileData->ftLastAccessTime;
    info->ftLastWriteTime  = pFileData->ftLastWriteTime;
    info->nFileSizeHigh    = pFileData->nFileSizeHigh;
    info->nFileSizeLow     = pFileData->nFileSizeLow;
/*  info->nNumberOfLinks   = 1; */
    if (pFileData->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        *reparse_tag = pFileData->dwReserved0;
    else
        *reparse_tag = 0;
}

static BOOL
attributes_from_dir(LPCWSTR pszFile, BY_HANDLE_FILE_INFORMATION *info, ULONG *reparse_tag)
{
    HANDLE hFindFile;
    WIN32_FIND_DATAW FileData;
    hFindFile = FindFirstFileW(pszFile, &FileData);
    if (hFindFile == INVALID_HANDLE_VALUE)
        return FALSE;
    FindClose(hFindFile);
    find_data_to_file_info(&FileData, info, reparse_tag);
    return TRUE;
}

static BOOL
get_target_path(HANDLE hdl, wchar_t **target_path)
{
    int buf_size, result_length;
    wchar_t *buf;

    /* We have a good handle to the target, use it to determine
       the target path name (then we'll call lstat on it). */
    buf_size = GetFinalPathNameByHandleW(hdl, 0, 0,
                                         VOLUME_NAME_DOS);
    if(!buf_size)
        return FALSE;

    buf = (wchar_t *)PyMem_RawMalloc((buf_size + 1) * sizeof(wchar_t));
    if (!buf) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }

    result_length = GetFinalPathNameByHandleW(hdl,
                       buf, buf_size, VOLUME_NAME_DOS);

    if(!result_length) {
        PyMem_RawFree(buf);
        return FALSE;
    }

    if(!CloseHandle(hdl)) {
        PyMem_RawFree(buf);
        return FALSE;
    }

    buf[result_length] = 0;

    *target_path = buf;
    return TRUE;
}

static int
win32_xstat_impl(const wchar_t *path, struct _Py_stat_struct *result,
                 BOOL traverse)
{
    int code;
    HANDLE hFile, hFile2;
    BY_HANDLE_FILE_INFORMATION info;
    ULONG reparse_tag = 0;
    wchar_t *target_path;
    const wchar_t *dot;

    hFile = CreateFileW(
        path,
        FILE_READ_ATTRIBUTES, /* desired access */
        0, /* share mode */
        NULL, /* security attributes */
        OPEN_EXISTING,
        /* FILE_FLAG_BACKUP_SEMANTICS is required to open a directory */
        /* FILE_FLAG_OPEN_REPARSE_POINT does not follow the symlink.
           Because of this, calls like GetFinalPathNameByHandle will return
           the symlink path again and not the actual final path. */
        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_BACKUP_SEMANTICS|
            FILE_FLAG_OPEN_REPARSE_POINT,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        /* Either the target doesn't exist, or we don't have access to
           get a handle to it. If the former, we need to return an error.
           If the latter, we can use attributes_from_dir. */
        if (GetLastError() != ERROR_SHARING_VIOLATION)
            return -1;
        /* Could not get attributes on open file. Fall back to
           reading the directory. */
        if (!attributes_from_dir(path, &info, &reparse_tag))
            /* Very strange. This should not fail now */
            return -1;
        if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            if (traverse) {
                /* Should traverse, but could not open reparse point handle */
                SetLastError(ERROR_SHARING_VIOLATION);
                return -1;
            }
        }
    } else {
        if (!GetFileInformationByHandle(hFile, &info)) {
            CloseHandle(hFile);
            return -1;
        }
        if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            if (!win32_get_reparse_tag(hFile, &reparse_tag))
                return -1;

            /* Close the outer open file handle now that we're about to
               reopen it with different flags. */
            if (!CloseHandle(hFile))
                return -1;

            if (traverse) {
                /* In order to call GetFinalPathNameByHandle we need to open
                   the file without the reparse handling flag set. */
                hFile2 = CreateFileW(
                           path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                           NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL|FILE_FLAG_BACKUP_SEMANTICS,
                           NULL);
                if (hFile2 == INVALID_HANDLE_VALUE)
                    return -1;

                if (!get_target_path(hFile2, &target_path))
                    return -1;

                code = win32_xstat_impl(target_path, result, FALSE);
                PyMem_RawFree(target_path);
                return code;
            }
        } else
            CloseHandle(hFile);
    }
    _Py_attribute_data_to_stat(&info, reparse_tag, result);

    /* Set S_IEXEC if it is an .exe, .bat, ... */
    dot = wcsrchr(path, '.');
    if (dot) {
        if (_wcsicmp(dot, L".bat") == 0 || _wcsicmp(dot, L".cmd") == 0 ||
            _wcsicmp(dot, L".exe") == 0 || _wcsicmp(dot, L".com") == 0)
            result->st_mode |= 0111;
    }
    return 0;
}

static int
win32_xstat(const wchar_t *path, struct _Py_stat_struct *result, BOOL traverse)
{
    /* Protocol violation: we explicitly clear errno, instead of
       setting it to a POSIX error. Callers should use GetLastError. */
    int code = win32_xstat_impl(path, result, traverse);
    errno = 0;
    return code;
}
/* About the following functions: win32_lstat_w, win32_stat, win32_stat_w

   In Posix, stat automatically traverses symlinks and returns the stat
   structure for the target.  In Windows, the equivalent GetFileAttributes by
   default does not traverse symlinks and instead returns attributes for
   the symlink.

   Therefore, win32_lstat will get the attributes traditionally, and
   win32_stat will first explicitly resolve the symlink target and then will
   call win32_lstat on that result. */

static int
win32_lstat(const wchar_t* path, struct _Py_stat_struct *result)
{
    return win32_xstat(path, result, FALSE);
}

static int
win32_stat(const wchar_t* path, struct _Py_stat_struct *result)
{
    return win32_xstat(path, result, TRUE);
}

#endif /* MS_WINDOWS */

PyDoc_STRVAR(stat_result__doc__,
"stat_result: Result from stat, fstat, or lstat.\n\n\
This object may be accessed either as a tuple of\n\
  (mode, ino, dev, nlink, uid, gid, size, atime, mtime, ctime)\n\
or via the attributes st_mode, st_ino, st_dev, st_nlink, st_uid, and so on.\n\
\n\
Posix/windows: If your platform supports st_blksize, st_blocks, st_rdev,\n\
or st_flags, they are available as attributes only.\n\
\n\
See os.stat for more information.");

static PyStructSequence_Field stat_result_fields[] = {
    {"st_mode",    "protection bits"},
    {"st_ino",     "inode"},
    {"st_dev",     "device"},
    {"st_nlink",   "number of hard links"},
    {"st_uid",     "user ID of owner"},
    {"st_gid",     "group ID of owner"},
    {"st_size",    "total size, in bytes"},
    /* The NULL is replaced with PyStructSequence_UnnamedField later. */
    {NULL,   "integer time of last access"},
    {NULL,   "integer time of last modification"},
    {NULL,   "integer time of last change"},
    {"st_atime",   "time of last access"},
    {"st_mtime",   "time of last modification"},
    {"st_ctime",   "time of last change"},
    {"st_atime_ns",   "time of last access in nanoseconds"},
    {"st_mtime_ns",   "time of last modification in nanoseconds"},
    {"st_ctime_ns",   "time of last change in nanoseconds"},
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
    {"st_blksize", "blocksize for filesystem I/O"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
    {"st_blocks",  "number of blocks allocated"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_RDEV
    {"st_rdev",    "device type (if inode device)"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_FLAGS
    {"st_flags",   "user defined flags for file"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_GEN
    {"st_gen",    "generation number"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
    {"st_birthtime",   "time of creation"},
#endif
#ifdef HAVE_STRUCT_STAT_ST_FILE_ATTRIBUTES
    {"st_file_attributes", "Windows file attribute bits"},
#endif
    {0}
};

#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
#define ST_BLKSIZE_IDX 16
#else
#define ST_BLKSIZE_IDX 15
#endif

#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
#define ST_BLOCKS_IDX (ST_BLKSIZE_IDX+1)
#else
#define ST_BLOCKS_IDX ST_BLKSIZE_IDX
#endif

#ifdef HAVE_STRUCT_STAT_ST_RDEV
#define ST_RDEV_IDX (ST_BLOCKS_IDX+1)
#else
#define ST_RDEV_IDX ST_BLOCKS_IDX
#endif

#ifdef HAVE_STRUCT_STAT_ST_FLAGS
#define ST_FLAGS_IDX (ST_RDEV_IDX+1)
#else
#define ST_FLAGS_IDX ST_RDEV_IDX
#endif

#ifdef HAVE_STRUCT_STAT_ST_GEN
#define ST_GEN_IDX (ST_FLAGS_IDX+1)
#else
#define ST_GEN_IDX ST_FLAGS_IDX
#endif

#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
#define ST_BIRTHTIME_IDX (ST_GEN_IDX+1)
#else
#define ST_BIRTHTIME_IDX ST_GEN_IDX
#endif

#ifdef HAVE_STRUCT_STAT_ST_FILE_ATTRIBUTES
#define ST_FILE_ATTRIBUTES_IDX (ST_BIRTHTIME_IDX+1)
#else
#define ST_FILE_ATTRIBUTES_IDX ST_BIRTHTIME_IDX
#endif

static PyStructSequence_Desc stat_result_desc = {
    "stat_result", /* name */
    stat_result__doc__, /* doc */
    stat_result_fields,
    10
};

PyDoc_STRVAR(statvfs_result__doc__,
"statvfs_result: Result from statvfs or fstatvfs.\n\n\
This object may be accessed either as a tuple of\n\
  (bsize, frsize, blocks, bfree, bavail, files, ffree, favail, flag, namemax),\n\
or via the attributes f_bsize, f_frsize, f_blocks, f_bfree, and so on.\n\
\n\
See os.statvfs for more information.");

static PyStructSequence_Field statvfs_result_fields[] = {
    {"f_bsize",  },
    {"f_frsize", },
    {"f_blocks", },
    {"f_bfree",  },
    {"f_bavail", },
    {"f_files",  },
    {"f_ffree",  },
    {"f_favail", },
    {"f_flag",   },
    {"f_namemax",},
    {0}
};

static PyStructSequence_Desc statvfs_result_desc = {
    "statvfs_result", /* name */
    statvfs_result__doc__, /* doc */
    statvfs_result_fields,
    10
};

#if defined(HAVE_WAITID) && !defined(__APPLE__)
PyDoc_STRVAR(waitid_result__doc__,
"waitid_result: Result from waitid.\n\n\
This object may be accessed either as a tuple of\n\
  (si_pid, si_uid, si_signo, si_status, si_code),\n\
or via the attributes si_pid, si_uid, and so on.\n\
\n\
See os.waitid for more information.");

static PyStructSequence_Field waitid_result_fields[] = {
    {"si_pid",  },
    {"si_uid", },
    {"si_signo", },
    {"si_status",  },
    {"si_code", },
    {0}
};

static PyStructSequence_Desc waitid_result_desc = {
    "waitid_result", /* name */
    waitid_result__doc__, /* doc */
    waitid_result_fields,
    5
};
static PyTypeObject WaitidResultType;
#endif

static int initialized;
static PyTypeObject StatResultType;
static PyTypeObject StatVFSResultType;
#if defined(HAVE_SCHED_SETPARAM) || defined(HAVE_SCHED_SETSCHEDULER)
static PyTypeObject SchedParamType;
#endif
static newfunc structseq_new;

static PyObject *
statresult_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyStructSequence *result;
    int i;

    result = (PyStructSequence*)structseq_new(type, args, kwds);
    if (!result)
        return NULL;
    /* If we have been initialized from a tuple,
       st_?time might be set to None. Initialize it
       from the int slots.  */
    for (i = 7; i <= 9; i++) {
        if (result->ob_item[i+3] == Py_None) {
            Py_DECREF(Py_None);
            Py_INCREF(result->ob_item[i]);
            result->ob_item[i+3] = result->ob_item[i];
        }
    }
    return (PyObject*)result;
}



/* If true, st_?time is float. */
static int _stat_float_times = 1;

PyDoc_STRVAR(stat_float_times__doc__,
"stat_float_times([newval]) -> oldval\n\n\
Determine whether os.[lf]stat represents time stamps as float objects.\n\
\n\
If value is True, future calls to stat() return floats; if it is False,\n\
future calls return ints.\n\
If value is omitted, return the current setting.\n");

/* AC 3.5: the public default value should be None, not ready for that yet */
static PyObject*
stat_float_times(PyObject* self, PyObject *args)
{
    int newval = -1;
    if (!PyArg_ParseTuple(args, "|i:stat_float_times", &newval))
        return NULL;
    if (PyErr_WarnEx(PyExc_DeprecationWarning,
                     "stat_float_times() is deprecated",
                     1))
        return NULL;
    if (newval == -1)
        /* Return old value */
        return PyBool_FromLong(_stat_float_times);
    _stat_float_times = newval;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *billion = NULL;

static void
fill_time(PyObject *v, int index, time_t sec, unsigned long nsec)
{
    PyObject *s = _PyLong_FromTime_t(sec);
    PyObject *ns_fractional = PyLong_FromUnsignedLong(nsec);
    PyObject *s_in_ns = NULL;
    PyObject *ns_total = NULL;
    PyObject *float_s = NULL;

    if (!(s && ns_fractional))
        goto exit;

    s_in_ns = PyNumber_Multiply(s, billion);
    if (!s_in_ns)
        goto exit;

    ns_total = PyNumber_Add(s_in_ns, ns_fractional);
    if (!ns_total)
        goto exit;

    if (_stat_float_times) {
        float_s = PyFloat_FromDouble(sec + 1e-9*nsec);
        if (!float_s)
            goto exit;
    }
    else {
        float_s = s;
        Py_INCREF(float_s);
    }

    PyStructSequence_SET_ITEM(v, index, s);
    PyStructSequence_SET_ITEM(v, index+3, float_s);
    PyStructSequence_SET_ITEM(v, index+6, ns_total);
    s = NULL;
    float_s = NULL;
    ns_total = NULL;
exit:
    Py_XDECREF(s);
    Py_XDECREF(ns_fractional);
    Py_XDECREF(s_in_ns);
    Py_XDECREF(ns_total);
    Py_XDECREF(float_s);
}

/* pack a system stat C structure into the Python stat tuple
   (used by posix_stat() and posix_fstat()) */
static PyObject*
_pystat_fromstructstat(STRUCT_STAT *st)
{
    unsigned long ansec, mnsec, cnsec;
    PyObject *v = PyStructSequence_New(&StatResultType);
    if (v == NULL)
        return NULL;

    PyStructSequence_SET_ITEM(v, 0, PyLong_FromLong((long)st->st_mode));
#ifdef HAVE_LARGEFILE_SUPPORT
    PyStructSequence_SET_ITEM(v, 1,
                              PyLong_FromLongLong((long long)st->st_ino));
#else
    PyStructSequence_SET_ITEM(v, 1, PyLong_FromLong((long)st->st_ino));
#endif
#ifdef MS_WINDOWS
    PyStructSequence_SET_ITEM(v, 2, PyLong_FromUnsignedLong(st->st_dev));
#else
    PyStructSequence_SET_ITEM(v, 2, _PyLong_FromDev(st->st_dev));
#endif
    PyStructSequence_SET_ITEM(v, 3, PyLong_FromLong((long)st->st_nlink));
#if defined(MS_WINDOWS)
    PyStructSequence_SET_ITEM(v, 4, PyLong_FromLong(0));
    PyStructSequence_SET_ITEM(v, 5, PyLong_FromLong(0));
#else
    PyStructSequence_SET_ITEM(v, 4, _PyLong_FromUid(st->st_uid));
    PyStructSequence_SET_ITEM(v, 5, _PyLong_FromGid(st->st_gid));
#endif
#ifdef HAVE_LARGEFILE_SUPPORT
    PyStructSequence_SET_ITEM(v, 6,
                              PyLong_FromLongLong((long long)st->st_size));
#else
    PyStructSequence_SET_ITEM(v, 6, PyLong_FromLong(st->st_size));
#endif

#if defined(HAVE_STAT_TV_NSEC)
    ansec = st->st_atim.tv_nsec;
    mnsec = st->st_mtim.tv_nsec;
    cnsec = st->st_ctim.tv_nsec;
#elif defined(HAVE_STAT_TV_NSEC2)
    ansec = st->st_atimespec.tv_nsec;
    mnsec = st->st_mtimespec.tv_nsec;
    cnsec = st->st_ctimespec.tv_nsec;
#elif defined(HAVE_STAT_NSEC)
    ansec = st->st_atime_nsec;
    mnsec = st->st_mtime_nsec;
    cnsec = st->st_ctime_nsec;
#else
    ansec = mnsec = cnsec = 0;
#endif
    fill_time(v, 7, st->st_atime, ansec);
    fill_time(v, 8, st->st_mtime, mnsec);
    fill_time(v, 9, st->st_ctime, cnsec);

#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
    PyStructSequence_SET_ITEM(v, ST_BLKSIZE_IDX,
                              PyLong_FromLong((long)st->st_blksize));
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
    PyStructSequence_SET_ITEM(v, ST_BLOCKS_IDX,
                              PyLong_FromLong((long)st->st_blocks));
#endif
#ifdef HAVE_STRUCT_STAT_ST_RDEV
    PyStructSequence_SET_ITEM(v, ST_RDEV_IDX,
                              PyLong_FromLong((long)st->st_rdev));
#endif
#ifdef HAVE_STRUCT_STAT_ST_GEN
    PyStructSequence_SET_ITEM(v, ST_GEN_IDX,
                              PyLong_FromLong((long)st->st_gen));
#endif
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
    {
      PyObject *val;
      unsigned long bsec,bnsec;
      bsec = (long)st->st_birthtime;
#ifdef HAVE_STAT_TV_NSEC2
      bnsec = st->st_birthtimespec.tv_nsec;
#else
      bnsec = 0;
#endif
      if (_stat_float_times) {
        val = PyFloat_FromDouble(bsec + 1e-9*bnsec);
      } else {
        val = PyLong_FromLong((long)bsec);
      }
      PyStructSequence_SET_ITEM(v, ST_BIRTHTIME_IDX,
                                val);
    }
#endif
#ifdef HAVE_STRUCT_STAT_ST_FLAGS
    PyStructSequence_SET_ITEM(v, ST_FLAGS_IDX,
                              PyLong_FromLong((long)st->st_flags));
#endif
#ifdef HAVE_STRUCT_STAT_ST_FILE_ATTRIBUTES
    PyStructSequence_SET_ITEM(v, ST_FILE_ATTRIBUTES_IDX,
                              PyLong_FromUnsignedLong(st->st_file_attributes));
#endif

    if (PyErr_Occurred()) {
        Py_DECREF(v);
        return NULL;
    }

    return v;
}

/* POSIX methods */


static PyObject *
posix_do_stat(const char *function_name, path_t *path,
              int dir_fd, int follow_symlinks)
{
    STRUCT_STAT st;
    int result;

#if !defined(MS_WINDOWS) && !defined(HAVE_FSTATAT) && !defined(HAVE_LSTAT)
    if (follow_symlinks_specified(function_name, follow_symlinks))
        return NULL;
#endif

    if (path_and_dir_fd_invalid("stat", path, dir_fd) ||
        dir_fd_and_fd_invalid("stat", dir_fd, path->fd) ||
        fd_and_follow_symlinks_invalid("stat", path->fd, follow_symlinks))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    if (path->fd != -1)
        result = FSTAT(path->fd, &st);
#ifdef MS_WINDOWS
    else if (follow_symlinks)
        result = win32_stat(path->wide, &st);
    else
        result = win32_lstat(path->wide, &st);
#else
    else
#if defined(HAVE_LSTAT)
    if ((!follow_symlinks) && (dir_fd == DEFAULT_DIR_FD))
        result = LSTAT(path->narrow, &st);
    else
#endif /* HAVE_LSTAT */
#ifdef HAVE_FSTATAT
    if ((dir_fd != DEFAULT_DIR_FD) || !follow_symlinks)
        result = fstatat(dir_fd, path->narrow, &st,
                         follow_symlinks ? 0 : AT_SYMLINK_NOFOLLOW);
    else
#endif /* HAVE_FSTATAT */
        result = STAT(path->narrow, &st);
#endif /* MS_WINDOWS */
    Py_END_ALLOW_THREADS

    if (result != 0) {
        return path_error(path);
    }

    return _pystat_fromstructstat(&st);
}

/*[python input]

for s in """

FACCESSAT
FCHMODAT
FCHOWNAT
FSTATAT
LINKAT
MKDIRAT
MKFIFOAT
MKNODAT
OPENAT
READLINKAT
SYMLINKAT
UNLINKAT

""".strip().split():
    s = s.strip()
    print("""
#ifdef HAVE_{s}
    #define {s}_DIR_FD_CONVERTER dir_fd_converter
#else
    #define {s}_DIR_FD_CONVERTER dir_fd_unavailable
#endif
""".rstrip().format(s=s))

for s in """

FCHDIR
FCHMOD
FCHOWN
FDOPENDIR
FEXECVE
FPATHCONF
FSTATVFS
FTRUNCATE

""".strip().split():
    s = s.strip()
    print("""
#ifdef HAVE_{s}
    #define PATH_HAVE_{s} 1
#else
    #define PATH_HAVE_{s} 0
#endif

""".rstrip().format(s=s))
[python start generated code]*/

#ifdef HAVE_FACCESSAT
    #define FACCESSAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define FACCESSAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_FCHMODAT
    #define FCHMODAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define FCHMODAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_FCHOWNAT
    #define FCHOWNAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define FCHOWNAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_FSTATAT
    #define FSTATAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define FSTATAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_LINKAT
    #define LINKAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define LINKAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_MKDIRAT
    #define MKDIRAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define MKDIRAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_MKFIFOAT
    #define MKFIFOAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define MKFIFOAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_MKNODAT
    #define MKNODAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define MKNODAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_OPENAT
    #define OPENAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define OPENAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_READLINKAT
    #define READLINKAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define READLINKAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_SYMLINKAT
    #define SYMLINKAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define SYMLINKAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_UNLINKAT
    #define UNLINKAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define UNLINKAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#ifdef HAVE_FCHDIR
    #define PATH_HAVE_FCHDIR 1
#else
    #define PATH_HAVE_FCHDIR 0
#endif

#ifdef HAVE_FCHMOD
    #define PATH_HAVE_FCHMOD 1
#else
    #define PATH_HAVE_FCHMOD 0
#endif

#ifdef HAVE_FCHOWN
    #define PATH_HAVE_FCHOWN 1
#else
    #define PATH_HAVE_FCHOWN 0
#endif

#ifdef HAVE_FDOPENDIR
    #define PATH_HAVE_FDOPENDIR 1
#else
    #define PATH_HAVE_FDOPENDIR 0
#endif

#ifdef HAVE_FEXECVE
    #define PATH_HAVE_FEXECVE 1
#else
    #define PATH_HAVE_FEXECVE 0
#endif

#ifdef HAVE_FPATHCONF
    #define PATH_HAVE_FPATHCONF 1
#else
    #define PATH_HAVE_FPATHCONF 0
#endif

#ifdef HAVE_FSTATVFS
    #define PATH_HAVE_FSTATVFS 1
#else
    #define PATH_HAVE_FSTATVFS 0
#endif

#ifdef HAVE_FTRUNCATE
    #define PATH_HAVE_FTRUNCATE 1
#else
    #define PATH_HAVE_FTRUNCATE 0
#endif
/*[python end generated code: output=4bd4f6f7d41267f1 input=80b4c890b6774ea5]*/

#ifdef MS_WINDOWS
    #undef PATH_HAVE_FTRUNCATE
    #define PATH_HAVE_FTRUNCATE 1
#endif

/*[python input]

class path_t_converter(CConverter):

    type = "path_t"
    impl_by_reference = True
    parse_by_reference = True

    converter = 'path_converter'

    def converter_init(self, *, allow_fd=False, nullable=False):
        # right now path_t doesn't support default values.
        # to support a default value, you'll need to override initialize().
        if self.default not in (unspecified, None):
            fail("Can't specify a default to the path_t converter!")

        if self.c_default not in (None, 'Py_None'):
            raise RuntimeError("Can't specify a c_default to the path_t converter!")

        self.nullable = nullable
        self.allow_fd = allow_fd

    def pre_render(self):
        def strify(value):
            if isinstance(value, str):
                return value
            return str(int(bool(value)))

        # add self.py_name here when merging with posixmodule conversion
        self.c_default = 'PATH_T_INITIALIZE("{}", "{}", {}, {})'.format(
            self.function.name,
            self.name,
            strify(self.nullable),
            strify(self.allow_fd),
            )

    def cleanup(self):
        return "path_cleanup(&" + self.name + ");\n"


class dir_fd_converter(CConverter):
    type = 'int'

    def converter_init(self, requires=None):
        if self.default in (unspecified, None):
            self.c_default = 'DEFAULT_DIR_FD'
        if isinstance(requires, str):
            self.converter = requires.upper() + '_DIR_FD_CONVERTER'
        else:
            self.converter = 'dir_fd_converter'

class fildes_converter(CConverter):
    type = 'int'
    converter = 'fildes_converter'

class uid_t_converter(CConverter):
    type = "uid_t"
    converter = '_Py_Uid_Converter'

class gid_t_converter(CConverter):
    type = "gid_t"
    converter = '_Py_Gid_Converter'

class dev_t_converter(CConverter):
    type = 'dev_t'
    converter = '_Py_Dev_Converter'

class dev_t_return_converter(unsigned_long_return_converter):
    type = 'dev_t'
    conversion_fn = '_PyLong_FromDev'
    unsigned_cast = '(dev_t)'

class FSConverter_converter(CConverter):
    type = 'PyObject *'
    converter = 'PyUnicode_FSConverter'
    def converter_init(self):
        if self.default is not unspecified:
            fail("FSConverter_converter does not support default values")
        self.c_default = 'NULL'

    def cleanup(self):
        return "Py_XDECREF(" + self.name + ");\n"

class pid_t_converter(CConverter):
    type = 'pid_t'
    format_unit = '" _Py_PARSE_PID "'

class idtype_t_converter(int_converter):
    type = 'idtype_t'

class id_t_converter(CConverter):
    type = 'id_t'
    format_unit = '" _Py_PARSE_PID "'

class intptr_t_converter(CConverter):
    type = 'intptr_t'
    format_unit = '" _Py_PARSE_INTPTR "'

class Py_off_t_converter(CConverter):
    type = 'Py_off_t'
    converter = 'Py_off_t_converter'

class Py_off_t_return_converter(long_return_converter):
    type = 'Py_off_t'
    conversion_fn = 'PyLong_FromPy_off_t'

class path_confname_converter(CConverter):
    type="int"
    converter="conv_path_confname"

class confstr_confname_converter(path_confname_converter):
    converter='conv_confstr_confname'

class sysconf_confname_converter(path_confname_converter):
    converter="conv_sysconf_confname"

class sched_param_converter(CConverter):
    type = 'struct sched_param'
    converter = 'convert_sched_param'
    impl_by_reference = True;

[python start generated code]*/
/*[python end generated code: output=da39a3ee5e6b4b0d input=418fce0e01144461]*/

/*[clinic input]

os.stat

    path : path_t(allow_fd=True)
        Path to be examined; can be string, bytes, or open-file-descriptor int.

    *

    dir_fd : dir_fd(requires='fstatat') = None
        If not None, it should be a file descriptor open to a directory,
        and path should be a relative string; path will then be relative to
        that directory.

    follow_symlinks: bool = True
        If False, and the last element of the path is a symbolic link,
        stat will examine the symbolic link itself instead of the file
        the link points to.

Perform a stat system call on the given path.

dir_fd and follow_symlinks may not be implemented
  on your platform.  If they are unavailable, using them will raise a
  NotImplementedError.

It's an error to use dir_fd or follow_symlinks when specifying path as
  an open file descriptor.

[clinic start generated code]*/

static PyObject *
os_stat_impl(PyObject *module, path_t *path, int dir_fd, int follow_symlinks)
/*[clinic end generated code: output=7d4976e6f18a59c5 input=099d356c306fa24a]*/
{
    return posix_do_stat("stat", path, dir_fd, follow_symlinks);
}


/*[clinic input]
os.lstat

    path : path_t

    *

    dir_fd : dir_fd(requires='fstatat') = None

Perform a stat system call on the given path, without following symbolic links.

Like stat(), but do not follow symbolic links.
Equivalent to stat(path, follow_symlinks=False).
[clinic start generated code]*/

static PyObject *
os_lstat_impl(PyObject *module, path_t *path, int dir_fd)
/*[clinic end generated code: output=ef82a5d35ce8ab37 input=0b7474765927b925]*/
{
    int follow_symlinks = 0;
    return posix_do_stat("lstat", path, dir_fd, follow_symlinks);
}


/*[clinic input]
os.access -> bool

    path: path_t
        Path to be tested; can be string or bytes

    mode: int
        Operating-system mode bitfield.  Can be F_OK to test existence,
        or the inclusive-OR of R_OK, W_OK, and X_OK.

    *

    dir_fd : dir_fd(requires='faccessat') = None
        If not None, it should be a file descriptor open to a directory,
        and path should be relative; path will then be relative to that
        directory.

    effective_ids: bool = False
        If True, access will use the effective uid/gid instead of
        the real uid/gid.

    follow_symlinks: bool = True
        If False, and the last element of the path is a symbolic link,
        access will examine the symbolic link itself instead of the file
        the link points to.

Use the real uid/gid to test for access to a path.

{parameters}
dir_fd, effective_ids, and follow_symlinks may not be implemented
  on your platform.  If they are unavailable, using them will raise a
  NotImplementedError.

Note that most operations will use the effective uid/gid, therefore this
  routine can be used in a suid/sgid environment to test if the invoking user
  has the specified access to the path.

[clinic start generated code]*/

static int
os_access_impl(PyObject *module, path_t *path, int mode, int dir_fd,
               int effective_ids, int follow_symlinks)
/*[clinic end generated code: output=cf84158bc90b1a77 input=8e8c3a6ba791fee3]*/
{
    int return_value;

#ifdef MS_WINDOWS
    DWORD attr;
#else
    int result;
#endif

#ifndef HAVE_FACCESSAT
    if (follow_symlinks_specified("access", follow_symlinks))
        return -1;

    if (effective_ids) {
        argument_unavailable_error("access", "effective_ids");
        return -1;
    }
#endif

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    attr = GetFileAttributesW(path->wide);
    Py_END_ALLOW_THREADS

    /*
     * Access is possible if
     *   * we didn't get a -1, and
     *     * write access wasn't requested,
     *     * or the file isn't read-only,
     *     * or it's a directory.
     * (Directories cannot be read-only on Windows.)
    */
    return_value = (attr != INVALID_FILE_ATTRIBUTES) &&
            (!(mode & 2) ||
            !(attr & FILE_ATTRIBUTE_READONLY) ||
            (attr & FILE_ATTRIBUTE_DIRECTORY));
#else

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_FACCESSAT
    if ((dir_fd != DEFAULT_DIR_FD) ||
        effective_ids ||
        !follow_symlinks) {
        int flags = 0;
        if (!follow_symlinks)
            flags |= AT_SYMLINK_NOFOLLOW;
        if (effective_ids)
            flags |= AT_EACCESS;
        result = faccessat(dir_fd, path->narrow, mode, flags);
    }
    else
#endif
        result = access(path->narrow, mode);
    Py_END_ALLOW_THREADS
    return_value = !result;
#endif

    return return_value;
}

#ifndef F_OK
#define F_OK 0
#endif
#ifndef R_OK
#define R_OK 4
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef X_OK
#define X_OK 1
#endif


#ifdef HAVE_TTYNAME
/*[clinic input]
os.ttyname -> DecodeFSDefault

    fd: int
        Integer file descriptor handle.

    /

Return the name of the terminal device connected to 'fd'.
[clinic start generated code]*/

static char *
os_ttyname_impl(PyObject *module, int fd)
/*[clinic end generated code: output=ed16ad216d813591 input=5f72ca83e76b3b45]*/
{
    char *ret;

    ret = ttyname(fd);
    if (ret == NULL)
        posix_error();
    return ret;
}
#endif

#ifdef HAVE_CTERMID
/*[clinic input]
os.ctermid

Return the name of the controlling terminal for this process.
[clinic start generated code]*/

static PyObject *
os_ctermid_impl(PyObject *module)
/*[clinic end generated code: output=02f017e6c9e620db input=3b87fdd52556382d]*/
{
    char *ret;
    char buffer[L_ctermid];

#ifdef USE_CTERMID_R
    ret = ctermid_r(buffer);
#else
    ret = ctermid(buffer);
#endif
    if (ret == NULL)
        return posix_error();
    return PyUnicode_DecodeFSDefault(buffer);
}
#endif /* HAVE_CTERMID */


/*[clinic input]
os.chdir

    path: path_t(allow_fd='PATH_HAVE_FCHDIR')

Change the current working directory to the specified path.

path may always be specified as a string.
On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.
[clinic start generated code]*/

static PyObject *
os_chdir_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=3be6400eee26eaae input=1a4a15b4d12cb15d]*/
{
    int result;

    Py_BEGIN_ALLOW_THREADS
#ifdef MS_WINDOWS
    /* on unix, success = 0, on windows, success = !0 */
    result = !win32_wchdir(path->wide);
#else
#ifdef HAVE_FCHDIR
    if (path->fd != -1)
        result = fchdir(path->fd);
    else
#endif
        result = chdir(path->narrow);
#endif
    Py_END_ALLOW_THREADS

    if (result) {
        return path_error(path);
    }

    Py_RETURN_NONE;
}


#ifdef HAVE_FCHDIR
/*[clinic input]
os.fchdir

    fd: fildes

Change to the directory of the given file descriptor.

fd must be opened on a directory, not a file.
Equivalent to os.chdir(fd).

[clinic start generated code]*/

static PyObject *
os_fchdir_impl(PyObject *module, int fd)
/*[clinic end generated code: output=42e064ec4dc00ab0 input=18e816479a2fa985]*/
{
    return posix_fildes_fd(fd, fchdir);
}
#endif /* HAVE_FCHDIR */


/*[clinic input]
os.chmod

    path: path_t(allow_fd='PATH_HAVE_FCHMOD')
        Path to be modified.  May always be specified as a str or bytes.
        On some platforms, path may also be specified as an open file descriptor.
        If this functionality is unavailable, using it raises an exception.

    mode: int
        Operating-system mode bitfield.

    *

    dir_fd : dir_fd(requires='fchmodat') = None
        If not None, it should be a file descriptor open to a directory,
        and path should be relative; path will then be relative to that
        directory.

    follow_symlinks: bool = True
        If False, and the last element of the path is a symbolic link,
        chmod will modify the symbolic link itself instead of the file
        the link points to.

Change the access permissions of a file.

It is an error to use dir_fd or follow_symlinks when specifying path as
  an open file descriptor.
dir_fd and follow_symlinks may not be implemented on your platform.
  If they are unavailable, using them will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_chmod_impl(PyObject *module, path_t *path, int mode, int dir_fd,
              int follow_symlinks)
/*[clinic end generated code: output=5cf6a94915cc7bff input=7f1618e5e15cc196]*/
{
    int result;

#ifdef MS_WINDOWS
    DWORD attr;
#endif

#ifdef HAVE_FCHMODAT
    int fchmodat_nofollow_unsupported = 0;
#endif

#if !(defined(HAVE_FCHMODAT) || defined(HAVE_LCHMOD))
    if (follow_symlinks_specified("chmod", follow_symlinks))
        return NULL;
#endif

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    attr = GetFileAttributesW(path->wide);
    if (attr == INVALID_FILE_ATTRIBUTES)
        result = 0;
    else {
        if (mode & _S_IWRITE)
            attr &= ~FILE_ATTRIBUTE_READONLY;
        else
            attr |= FILE_ATTRIBUTE_READONLY;
        result = SetFileAttributesW(path->wide, attr);
    }
    Py_END_ALLOW_THREADS

    if (!result) {
        return path_error(path);
    }
#else /* MS_WINDOWS */
    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_FCHMOD
    if (path->fd != -1)
        result = fchmod(path->fd, mode);
    else
#endif
#ifdef HAVE_LCHMOD
    if ((!follow_symlinks) && (dir_fd == DEFAULT_DIR_FD))
        result = lchmod(path->narrow, mode);
    else
#endif
#ifdef HAVE_FCHMODAT
    if ((dir_fd != DEFAULT_DIR_FD) || !follow_symlinks) {
        /*
         * fchmodat() doesn't currently support AT_SYMLINK_NOFOLLOW!
         * The documentation specifically shows how to use it,
         * and then says it isn't implemented yet.
         * (true on linux with glibc 2.15, and openindiana 3.x)
         *
         * Once it is supported, os.chmod will automatically
         * support dir_fd and follow_symlinks=False.  (Hopefully.)
         * Until then, we need to be careful what exception we raise.
         */
        result = fchmodat(dir_fd, path->narrow, mode,
                          follow_symlinks ? 0 : AT_SYMLINK_NOFOLLOW);
        /*
         * But wait!  We can't throw the exception without allowing threads,
         * and we can't do that in this nested scope.  (Macro trickery, sigh.)
         */
        fchmodat_nofollow_unsupported =
                         result &&
                         ((errno == ENOTSUP) || (errno == EOPNOTSUPP)) &&
                         !follow_symlinks;
    }
    else
#endif
        result = chmod(path->narrow, mode);
    Py_END_ALLOW_THREADS

    if (result) {
#ifdef HAVE_FCHMODAT
        if (fchmodat_nofollow_unsupported) {
            if (dir_fd != DEFAULT_DIR_FD)
                dir_fd_and_follow_symlinks_invalid("chmod",
                                                   dir_fd, follow_symlinks);
            else
                follow_symlinks_specified("chmod", follow_symlinks);
        }
        else
#endif
        return path_error(path);
    }
#endif

    Py_RETURN_NONE;
}


#ifdef HAVE_FCHMOD
/*[clinic input]
os.fchmod

    fd: int
    mode: int

Change the access permissions of the file given by file descriptor fd.

Equivalent to os.chmod(fd, mode).
[clinic start generated code]*/

static PyObject *
os_fchmod_impl(PyObject *module, int fd, int mode)
/*[clinic end generated code: output=afd9bc05b4e426b3 input=8ab11975ca01ee5b]*/
{
    int res;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = fchmod(fd, mode);
        Py_END_ALLOW_THREADS
    } while (res != 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res != 0)
        return (!async_err) ? posix_error() : NULL;

    Py_RETURN_NONE;
}
#endif /* HAVE_FCHMOD */


#ifdef HAVE_LCHMOD
/*[clinic input]
os.lchmod

    path: path_t
    mode: int

Change the access permissions of a file, without following symbolic links.

If path is a symlink, this affects the link itself rather than the target.
Equivalent to chmod(path, mode, follow_symlinks=False)."
[clinic start generated code]*/

static PyObject *
os_lchmod_impl(PyObject *module, path_t *path, int mode)
/*[clinic end generated code: output=082344022b51a1d5 input=90c5663c7465d24f]*/
{
    int res;
    Py_BEGIN_ALLOW_THREADS
    res = lchmod(path->narrow, mode);
    Py_END_ALLOW_THREADS
    if (res < 0) {
        path_error(path);
        return NULL;
    }
    Py_RETURN_NONE;
}
#endif /* HAVE_LCHMOD */


#ifdef HAVE_CHFLAGS
/*[clinic input]
os.chflags

    path: path_t
    flags: unsigned_long(bitwise=True)
    follow_symlinks: bool=True

Set file flags.

If follow_symlinks is False, and the last element of the path is a symbolic
  link, chflags will change flags on the symbolic link itself instead of the
  file the link points to.
follow_symlinks may not be implemented on your platform.  If it is
unavailable, using it will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_chflags_impl(PyObject *module, path_t *path, unsigned long flags,
                int follow_symlinks)
/*[clinic end generated code: output=85571c6737661ce9 input=0327e29feb876236]*/
{
    int result;

#ifndef HAVE_LCHFLAGS
    if (follow_symlinks_specified("chflags", follow_symlinks))
        return NULL;
#endif

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_LCHFLAGS
    if (!follow_symlinks)
        result = lchflags(path->narrow, flags);
    else
#endif
        result = chflags(path->narrow, flags);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error(path);

    Py_RETURN_NONE;
}
#endif /* HAVE_CHFLAGS */


#ifdef HAVE_LCHFLAGS
/*[clinic input]
os.lchflags

    path: path_t
    flags: unsigned_long(bitwise=True)

Set file flags.

This function will not follow symbolic links.
Equivalent to chflags(path, flags, follow_symlinks=False).
[clinic start generated code]*/

static PyObject *
os_lchflags_impl(PyObject *module, path_t *path, unsigned long flags)
/*[clinic end generated code: output=30ae958695c07316 input=f9f82ea8b585ca9d]*/
{
    int res;
    Py_BEGIN_ALLOW_THREADS
    res = lchflags(path->narrow, flags);
    Py_END_ALLOW_THREADS
    if (res < 0) {
        return path_error(path);
    }
    Py_RETURN_NONE;
}
#endif /* HAVE_LCHFLAGS */


#ifdef HAVE_CHROOT
/*[clinic input]
os.chroot
    path: path_t

Change root directory to path.

[clinic start generated code]*/

static PyObject *
os_chroot_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=de80befc763a4475 input=14822965652c3dc3]*/
{
    int res;
    Py_BEGIN_ALLOW_THREADS
    res = chroot(path->narrow);
    Py_END_ALLOW_THREADS
    if (res < 0)
        return path_error(path);
    Py_RETURN_NONE;
}
#endif /* HAVE_CHROOT */


#ifdef HAVE_FSYNC
/*[clinic input]
os.fsync

    fd: fildes

Force write of fd to disk.
[clinic start generated code]*/

static PyObject *
os_fsync_impl(PyObject *module, int fd)
/*[clinic end generated code: output=4a10d773f52b3584 input=21c3645c056967f2]*/
{
    return posix_fildes_fd(fd, fsync);
}
#endif /* HAVE_FSYNC */


#ifdef HAVE_SYNC
/*[clinic input]
os.sync

Force write of everything to disk.
[clinic start generated code]*/

static PyObject *
os_sync_impl(PyObject *module)
/*[clinic end generated code: output=2796b1f0818cd71c input=84749fe5e9b404ff]*/
{
    Py_BEGIN_ALLOW_THREADS
    sync();
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}
#endif /* HAVE_SYNC */


#ifdef HAVE_FDATASYNC
#ifdef __hpux
extern int fdatasync(int); /* On HP-UX, in libc but not in unistd.h */
#endif

/*[clinic input]
os.fdatasync

    fd: fildes

Force write of fd to disk without forcing update of metadata.
[clinic start generated code]*/

static PyObject *
os_fdatasync_impl(PyObject *module, int fd)
/*[clinic end generated code: output=b4b9698b5d7e26dd input=bc74791ee54dd291]*/
{
    return posix_fildes_fd(fd, fdatasync);
}
#endif /* HAVE_FDATASYNC */


#ifdef HAVE_CHOWN
/*[clinic input]
os.chown

    path : path_t(allow_fd='PATH_HAVE_FCHOWN')
        Path to be examined; can be string, bytes, or open-file-descriptor int.

    uid: uid_t

    gid: gid_t

    *

    dir_fd : dir_fd(requires='fchownat') = None
        If not None, it should be a file descriptor open to a directory,
        and path should be relative; path will then be relative to that
        directory.

    follow_symlinks: bool = True
        If False, and the last element of the path is a symbolic link,
        stat will examine the symbolic link itself instead of the file
        the link points to.

Change the owner and group id of path to the numeric uid and gid.\

path may always be specified as a string.
On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.
If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, chown will modify the symbolic link itself instead of the file the
  link points to.
It is an error to use dir_fd or follow_symlinks when specifying path as
  an open file descriptor.
dir_fd and follow_symlinks may not be implemented on your platform.
  If they are unavailable, using them will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_chown_impl(PyObject *module, path_t *path, uid_t uid, gid_t gid,
              int dir_fd, int follow_symlinks)
/*[clinic end generated code: output=4beadab0db5f70cd input=a61cc35574814d5d]*/
{
    int result;

#if !(defined(HAVE_LCHOWN) || defined(HAVE_FCHOWNAT))
    if (follow_symlinks_specified("chown", follow_symlinks))
        return NULL;
#endif
    if (dir_fd_and_fd_invalid("chown", dir_fd, path->fd) ||
        fd_and_follow_symlinks_invalid("chown", path->fd, follow_symlinks))
        return NULL;

#ifdef __APPLE__
    /*
     * This is for Mac OS X 10.3, which doesn't have lchown.
     * (But we still have an lchown symbol because of weak-linking.)
     * It doesn't have fchownat either.  So there's no possibility
     * of a graceful failover.
     */
    if ((!follow_symlinks) && (lchown == NULL)) {
        follow_symlinks_specified("chown", follow_symlinks);
        return NULL;
    }
#endif

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_FCHOWN
    if (path->fd != -1)
        result = fchown(path->fd, uid, gid);
    else
#endif
#ifdef HAVE_LCHOWN
    if ((!follow_symlinks) && (dir_fd == DEFAULT_DIR_FD))
        result = lchown(path->narrow, uid, gid);
    else
#endif
#ifdef HAVE_FCHOWNAT
    if ((dir_fd != DEFAULT_DIR_FD) || (!follow_symlinks))
        result = fchownat(dir_fd, path->narrow, uid, gid,
                          follow_symlinks ? 0 : AT_SYMLINK_NOFOLLOW);
    else
#endif
        result = chown(path->narrow, uid, gid);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error(path);

    Py_RETURN_NONE;
}
#endif /* HAVE_CHOWN */


#ifdef HAVE_FCHOWN
/*[clinic input]
os.fchown

    fd: int
    uid: uid_t
    gid: gid_t

Change the owner and group id of the file specified by file descriptor.

Equivalent to os.chown(fd, uid, gid).

[clinic start generated code]*/

static PyObject *
os_fchown_impl(PyObject *module, int fd, uid_t uid, gid_t gid)
/*[clinic end generated code: output=97d21cbd5a4350a6 input=3af544ba1b13a0d7]*/
{
    int res;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = fchown(fd, uid, gid);
        Py_END_ALLOW_THREADS
    } while (res != 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res != 0)
        return (!async_err) ? posix_error() : NULL;

    Py_RETURN_NONE;
}
#endif /* HAVE_FCHOWN */


#ifdef HAVE_LCHOWN
/*[clinic input]
os.lchown

    path : path_t
    uid: uid_t
    gid: gid_t

Change the owner and group id of path to the numeric uid and gid.

This function will not follow symbolic links.
Equivalent to os.chown(path, uid, gid, follow_symlinks=False).
[clinic start generated code]*/

static PyObject *
os_lchown_impl(PyObject *module, path_t *path, uid_t uid, gid_t gid)
/*[clinic end generated code: output=25eaf6af412fdf2f input=b1c6014d563a7161]*/
{
    int res;
    Py_BEGIN_ALLOW_THREADS
    res = lchown(path->narrow, uid, gid);
    Py_END_ALLOW_THREADS
    if (res < 0) {
        return path_error(path);
    }
    Py_RETURN_NONE;
}
#endif /* HAVE_LCHOWN */


static PyObject *
posix_getcwd(int use_bytes)
{
    char *buf, *tmpbuf;
    char *cwd;
    const size_t chunk = 1024;
    size_t buflen = 0;
    PyObject *obj;

#ifdef MS_WINDOWS
    if (!use_bytes) {
        wchar_t wbuf[MAXPATHLEN];
        wchar_t *wbuf2 = wbuf;
        PyObject *resobj;
        DWORD len;
        Py_BEGIN_ALLOW_THREADS
        len = GetCurrentDirectoryW(Py_ARRAY_LENGTH(wbuf), wbuf);
        /* If the buffer is large enough, len does not include the
           terminating \0. If the buffer is too small, len includes
           the space needed for the terminator. */
        if (len >= Py_ARRAY_LENGTH(wbuf)) {
            wbuf2 = PyMem_RawMalloc(len * sizeof(wchar_t));
            if (wbuf2)
                len = GetCurrentDirectoryW(len, wbuf2);
        }
        Py_END_ALLOW_THREADS
        if (!wbuf2) {
            PyErr_NoMemory();
            return NULL;
        }
        if (!len) {
            if (wbuf2 != wbuf)
                PyMem_RawFree(wbuf2);
            return PyErr_SetFromWindowsErr(0);
        }
        resobj = PyUnicode_FromWideChar(wbuf2, len);
        if (wbuf2 != wbuf)
            PyMem_RawFree(wbuf2);
        return resobj;
    }

    if (win32_warn_bytes_api())
        return NULL;
#endif

    buf = cwd = NULL;
    Py_BEGIN_ALLOW_THREADS
    do {
        buflen += chunk;
#ifdef MS_WINDOWS
        if (buflen > INT_MAX) {
            PyErr_NoMemory();
            break;
        }
#endif
        tmpbuf = PyMem_RawRealloc(buf, buflen);
        if (tmpbuf == NULL)
            break;

        buf = tmpbuf;
#ifdef MS_WINDOWS
        cwd = getcwd(buf, (int)buflen);
#else
        cwd = getcwd(buf, buflen);
#endif
    } while (cwd == NULL && errno == ERANGE);
    Py_END_ALLOW_THREADS

    if (cwd == NULL) {
        PyMem_RawFree(buf);
        return posix_error();
    }

    if (use_bytes)
        obj = PyBytes_FromStringAndSize(buf, strlen(buf));
    else
        obj = PyUnicode_DecodeFSDefault(buf);
    PyMem_RawFree(buf);

    return obj;
}


/*[clinic input]
os.getcwd

Return a unicode string representing the current working directory.
[clinic start generated code]*/

static PyObject *
os_getcwd_impl(PyObject *module)
/*[clinic end generated code: output=21badfae2ea99ddc input=f069211bb70e3d39]*/
{
    return posix_getcwd(0);
}


/*[clinic input]
os.getcwdb

Return a bytes string representing the current working directory.
[clinic start generated code]*/

static PyObject *
os_getcwdb_impl(PyObject *module)
/*[clinic end generated code: output=3dd47909480e4824 input=f6f6a378dad3d9cb]*/
{
    return posix_getcwd(1);
}


#if ((!defined(HAVE_LINK)) && defined(MS_WINDOWS))
#define HAVE_LINK 1
#endif

#ifdef HAVE_LINK
/*[clinic input]

os.link

    src : path_t
    dst : path_t
    *
    src_dir_fd : dir_fd = None
    dst_dir_fd : dir_fd = None
    follow_symlinks: bool = True

Create a hard link to a file.

If either src_dir_fd or dst_dir_fd is not None, it should be a file
  descriptor open to a directory, and the respective path string (src or dst)
  should be relative; the path will then be relative to that directory.
If follow_symlinks is False, and the last element of src is a symbolic
  link, link will create a link to the symbolic link itself instead of the
  file the link points to.
src_dir_fd, dst_dir_fd, and follow_symlinks may not be implemented on your
  platform.  If they are unavailable, using them will raise a
  NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_link_impl(PyObject *module, path_t *src, path_t *dst, int src_dir_fd,
             int dst_dir_fd, int follow_symlinks)
/*[clinic end generated code: output=7f00f6007fd5269a input=b0095ebbcbaa7e04]*/
{
#ifdef MS_WINDOWS
    BOOL result = FALSE;
#else
    int result;
#endif

#ifndef HAVE_LINKAT
    if ((src_dir_fd != DEFAULT_DIR_FD) || (dst_dir_fd != DEFAULT_DIR_FD)) {
        argument_unavailable_error("link", "src_dir_fd and dst_dir_fd");
        return NULL;
    }
#endif

#ifndef MS_WINDOWS
    if ((src->narrow && dst->wide) || (src->wide && dst->narrow)) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "link: src and dst must be the same type");
        return NULL;
    }
#endif

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    if (src->wide)
        result = CreateHardLinkW(dst->wide, src->wide, NULL);
    Py_END_ALLOW_THREADS

    if (!result)
        return path_error2(src, dst);
#else
    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_LINKAT
    if ((src_dir_fd != DEFAULT_DIR_FD) ||
        (dst_dir_fd != DEFAULT_DIR_FD) ||
        (!follow_symlinks))
        result = linkat(src_dir_fd, src->narrow,
            dst_dir_fd, dst->narrow,
            follow_symlinks ? AT_SYMLINK_FOLLOW : 0);
    else
#endif /* HAVE_LINKAT */
        result = link(src->narrow, dst->narrow);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error2(src, dst);
#endif /* MS_WINDOWS */

    Py_RETURN_NONE;
}
#endif


#if defined(MS_WINDOWS) && !defined(HAVE_OPENDIR)
static PyObject *
_listdir_windows_no_opendir(path_t *path, PyObject *list)
{
    PyObject *v;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    BOOL result;
    wchar_t namebuf[MAX_PATH+4]; /* Overallocate for "\*.*" */
    /* only claim to have space for MAX_PATH */
    Py_ssize_t len = Py_ARRAY_LENGTH(namebuf)-4;
    wchar_t *wnamebuf = NULL;

    WIN32_FIND_DATAW wFileData;
    const wchar_t *po_wchars;

    if (!path->wide) { /* Default arg: "." */
        po_wchars = L".";
        len = 1;
    } else {
        po_wchars = path->wide;
        len = wcslen(path->wide);
    }
    /* The +5 is so we can append "\\*.*\0" */
    wnamebuf = PyMem_New(wchar_t, len + 5);
    if (!wnamebuf) {
        PyErr_NoMemory();
        goto exit;
    }
    wcscpy(wnamebuf, po_wchars);
    if (len > 0) {
        wchar_t wch = wnamebuf[len-1];
        if (wch != SEP && wch != ALTSEP && wch != L':')
            wnamebuf[len++] = SEP;
        wcscpy(wnamebuf + len, L"*.*");
    }
    if ((list = PyList_New(0)) == NULL) {
        goto exit;
    }
    Py_BEGIN_ALLOW_THREADS
    hFindFile = FindFirstFileW(wnamebuf, &wFileData);
    Py_END_ALLOW_THREADS
    if (hFindFile == INVALID_HANDLE_VALUE) {
        int error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND)
            goto exit;
        Py_DECREF(list);
        list = path_error(path);
        goto exit;
    }
    do {
        /* Skip over . and .. */
        if (wcscmp(wFileData.cFileName, L".") != 0 &&
            wcscmp(wFileData.cFileName, L"..") != 0) {
            v = PyUnicode_FromWideChar(wFileData.cFileName,
                                       wcslen(wFileData.cFileName));
            if (path->narrow && v) {
                Py_SETREF(v, PyUnicode_EncodeFSDefault(v));
            }
            if (v == NULL) {
                Py_DECREF(list);
                list = NULL;
                break;
            }
            if (PyList_Append(list, v) != 0) {
                Py_DECREF(v);
                Py_DECREF(list);
                list = NULL;
                break;
            }
            Py_DECREF(v);
        }
        Py_BEGIN_ALLOW_THREADS
        result = FindNextFileW(hFindFile, &wFileData);
        Py_END_ALLOW_THREADS
        /* FindNextFile sets error to ERROR_NO_MORE_FILES if
           it got to the end of the directory. */
        if (!result && GetLastError() != ERROR_NO_MORE_FILES) {
            Py_DECREF(list);
            list = path_error(path);
            goto exit;
        }
    } while (result == TRUE);

exit:
    if (hFindFile != INVALID_HANDLE_VALUE) {
        if (FindClose(hFindFile) == FALSE) {
            if (list != NULL) {
                Py_DECREF(list);
                list = path_error(path);
            }
        }
    }
    PyMem_Free(wnamebuf);

    return list;
}  /* end of _listdir_windows_no_opendir */

#else  /* thus POSIX, ie: not (MS_WINDOWS and not HAVE_OPENDIR) */

static PyObject *
_posix_listdir(path_t *path, PyObject *list)
{
    PyObject *v;
    DIR *dirp = NULL;
    struct dirent *ep;
    int return_str; /* if false, return bytes */
#ifdef HAVE_FDOPENDIR
    int fd = -1;
#endif

    errno = 0;
#ifdef HAVE_FDOPENDIR
    if (path->fd != -1) {
        /* closedir() closes the FD, so we duplicate it */
        fd = _Py_dup(path->fd);
        if (fd == -1)
            return NULL;

        return_str = 1;

        Py_BEGIN_ALLOW_THREADS
        dirp = fdopendir(fd);
        Py_END_ALLOW_THREADS
    }
    else
#endif
    {
        const char *name;
        if (path->narrow) {
            name = path->narrow;
            /* only return bytes if they specified a bytes object */
            return_str = !(PyBytes_Check(path->object));
        }
        else {
            name = ".";
            return_str = 1;
        }

        Py_BEGIN_ALLOW_THREADS
        dirp = opendir(name);
        Py_END_ALLOW_THREADS
    }

    if (dirp == NULL) {
        list = path_error(path);
#ifdef HAVE_FDOPENDIR
        if (fd != -1) {
            Py_BEGIN_ALLOW_THREADS
            close(fd);
            Py_END_ALLOW_THREADS
        }
#endif
        goto exit;
    }
    if ((list = PyList_New(0)) == NULL) {
        goto exit;
    }
    for (;;) {
        errno = 0;
        Py_BEGIN_ALLOW_THREADS
        ep = readdir(dirp);
        Py_END_ALLOW_THREADS
        if (ep == NULL) {
            if (errno == 0) {
                break;
            } else {
                Py_DECREF(list);
                list = path_error(path);
                goto exit;
            }
        }
        if (ep->d_name[0] == '.' &&
            (NAMLEN(ep) == 1 ||
             (ep->d_name[1] == '.' && NAMLEN(ep) == 2)))
            continue;
        if (return_str)
            v = PyUnicode_DecodeFSDefaultAndSize(ep->d_name, NAMLEN(ep));
        else
            v = PyBytes_FromStringAndSize(ep->d_name, NAMLEN(ep));
        if (v == NULL) {
            Py_CLEAR(list);
            break;
        }
        if (PyList_Append(list, v) != 0) {
            Py_DECREF(v);
            Py_CLEAR(list);
            break;
        }
        Py_DECREF(v);
    }

exit:
    if (dirp != NULL) {
        Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_FDOPENDIR
        if (fd > -1)
            rewinddir(dirp);
#endif
        closedir(dirp);
        Py_END_ALLOW_THREADS
    }

    return list;
}  /* end of _posix_listdir */
#endif  /* which OS */


/*[clinic input]
os.listdir

    path : path_t(nullable=True, allow_fd='PATH_HAVE_FDOPENDIR') = None

Return a list containing the names of the files in the directory.

path can be specified as either str or bytes.  If path is bytes,
  the filenames returned will also be bytes; in all other circumstances
  the filenames returned will be str.
If path is None, uses the path='.'.
On some platforms, path may also be specified as an open file descriptor;\
  the file descriptor must refer to a directory.
  If this functionality is unavailable, using it raises NotImplementedError.

The list is in arbitrary order.  It does not include the special
entries '.' and '..' even if they are present in the directory.


[clinic start generated code]*/

static PyObject *
os_listdir_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=293045673fcd1a75 input=09e300416e3cd729]*/
{
#if defined(MS_WINDOWS) && !defined(HAVE_OPENDIR)
    return _listdir_windows_no_opendir(path, NULL);
#else
    return _posix_listdir(path, NULL);
#endif
}

#ifdef MS_WINDOWS
/* A helper function for abspath on win32 */
/*[clinic input]
os._getfullpathname

    path: path_t
    /

[clinic start generated code]*/

static PyObject *
os__getfullpathname_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=bb8679d56845bc9b input=332ed537c29d0a3e]*/
{
    wchar_t woutbuf[MAX_PATH], *woutbufp = woutbuf;
    wchar_t *wtemp;
    DWORD result;
    PyObject *v;

    result = GetFullPathNameW(path->wide,
                              Py_ARRAY_LENGTH(woutbuf),
                              woutbuf, &wtemp);
    if (result > Py_ARRAY_LENGTH(woutbuf)) {
        woutbufp = PyMem_New(wchar_t, result);
        if (!woutbufp)
            return PyErr_NoMemory();
        result = GetFullPathNameW(path->wide, result, woutbufp, &wtemp);
    }
    if (result) {
        v = PyUnicode_FromWideChar(woutbufp, wcslen(woutbufp));
        if (path->narrow)
            Py_SETREF(v, PyUnicode_EncodeFSDefault(v));
    } else
        v = win32_error_object("GetFullPathNameW", path->object);
    if (woutbufp != woutbuf)
        PyMem_Free(woutbufp);
    return v;
}


/*[clinic input]
os._getfinalpathname

    path: unicode
    /

A helper function for samepath on windows.
[clinic start generated code]*/

static PyObject *
os__getfinalpathname_impl(PyObject *module, PyObject *path)
/*[clinic end generated code: output=9bd78d0e52782e75 input=71d5e89334891bf4]*/
{
    HANDLE hFile;
    int buf_size;
    wchar_t *target_path;
    int result_length;
    PyObject *result;
    const wchar_t *path_wchar;

    path_wchar = PyUnicode_AsUnicode(path);
    if (path_wchar == NULL)
        return NULL;

    hFile = CreateFileW(
        path_wchar,
        0, /* desired access */
        0, /* share mode */
        NULL, /* security attributes */
        OPEN_EXISTING,
        /* FILE_FLAG_BACKUP_SEMANTICS is required to open a directory */
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
        return win32_error_object("CreateFileW", path);

    /* We have a good handle to the target, use it to determine the
       target path name. */
    buf_size = GetFinalPathNameByHandleW(hFile, 0, 0, VOLUME_NAME_NT);

    if(!buf_size)
        return win32_error_object("GetFinalPathNameByHandle", path);

    target_path = PyMem_New(wchar_t, buf_size+1);
    if(!target_path)
        return PyErr_NoMemory();

    result_length = GetFinalPathNameByHandleW(hFile, target_path,
                                              buf_size, VOLUME_NAME_DOS);
    if(!result_length)
        return win32_error_object("GetFinalPathNamyByHandle", path);

    if(!CloseHandle(hFile))
        return win32_error_object("CloseHandle", path);

    target_path[result_length] = 0;
    result = PyUnicode_FromWideChar(target_path, result_length);
    PyMem_Free(target_path);
    return result;
}

PyDoc_STRVAR(posix__isdir__doc__,
"Return true if the pathname refers to an existing directory.");

/*[clinic input]
os._isdir

    path: path_t
    /

[clinic start generated code]*/

static PyObject *
os__isdir_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=75f56f32720836cb input=e794f12faab62a2a]*/
{
    DWORD attributes;

    Py_BEGIN_ALLOW_THREADS
    attributes = GetFileAttributesW(path->wide);
    Py_END_ALLOW_THREADS

    if (attributes == INVALID_FILE_ATTRIBUTES)
        Py_RETURN_FALSE;

    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


/*[clinic input]
os._getvolumepathname

    path: unicode

A helper function for ismount on Win32.
[clinic start generated code]*/

static PyObject *
os__getvolumepathname_impl(PyObject *module, PyObject *path)
/*[clinic end generated code: output=cbdcbd1059ceef4c input=7eacadc40acbda6b]*/
{
    PyObject *result;
    const wchar_t *path_wchar;
    wchar_t *mountpath=NULL;
    size_t buflen;
    BOOL ret;

    path_wchar = PyUnicode_AsUnicodeAndSize(path, &buflen);
    if (path_wchar == NULL)
        return NULL;
    buflen += 1;

    /* Volume path should be shorter than entire path */
    buflen = Py_MAX(buflen, MAX_PATH);

    if (buflen > DWORD_MAX) {
        PyErr_SetString(PyExc_OverflowError, "path too long");
        return NULL;
    }

    mountpath = PyMem_New(wchar_t, buflen);
    if (mountpath == NULL)
        return PyErr_NoMemory();

    Py_BEGIN_ALLOW_THREADS
    ret = GetVolumePathNameW(path_wchar, mountpath,
                             Py_SAFE_DOWNCAST(buflen, size_t, DWORD));
    Py_END_ALLOW_THREADS

    if (!ret) {
        result = win32_error_object("_getvolumepathname", path);
        goto exit;
    }
    result = PyUnicode_FromWideChar(mountpath, wcslen(mountpath));

exit:
    PyMem_Free(mountpath);
    return result;
}

#endif /* MS_WINDOWS */


/*[clinic input]
os.mkdir

    path : path_t

    mode: int = 0o777

    *

    dir_fd : dir_fd(requires='mkdirat') = None

# "mkdir(path, mode=0o777, *, dir_fd=None)\n\n\

Create a directory.

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.

The mode argument is ignored on Windows.
[clinic start generated code]*/

static PyObject *
os_mkdir_impl(PyObject *module, path_t *path, int mode, int dir_fd)
/*[clinic end generated code: output=a70446903abe821f input=e965f68377e9b1ce]*/
{
    int result;

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    result = CreateDirectoryW(path->wide, NULL);
    Py_END_ALLOW_THREADS

    if (!result)
        return path_error(path);
#else
    Py_BEGIN_ALLOW_THREADS
#if HAVE_MKDIRAT
    if (dir_fd != DEFAULT_DIR_FD)
        result = mkdirat(dir_fd, path->narrow, mode);
    else
#endif
#if ( defined(__WATCOMC__) || defined(PYCC_VACPP) ) && !defined(__QNX__)
        result = mkdir(path->narrow);
#else
        result = mkdir(path->narrow, mode);
#endif
    Py_END_ALLOW_THREADS
    if (result < 0)
        return path_error(path);
#endif /* MS_WINDOWS */
    Py_RETURN_NONE;
}


/* sys/resource.h is needed for at least: wait3(), wait4(), broken nice. */
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif


#ifdef HAVE_NICE
/*[clinic input]
os.nice

    increment: int
    /

Add increment to the priority of process and return the new priority.
[clinic start generated code]*/

static PyObject *
os_nice_impl(PyObject *module, int increment)
/*[clinic end generated code: output=9dad8a9da8109943 input=864be2d402a21da2]*/
{
    int value;

    /* There are two flavours of 'nice': one that returns the new
       priority (as required by almost all standards out there) and the
       Linux/FreeBSD/BSDI one, which returns '0' on success and advices
       the use of getpriority() to get the new priority.

       If we are of the nice family that returns the new priority, we
       need to clear errno before the call, and check if errno is filled
       before calling posix_error() on a returnvalue of -1, because the
       -1 may be the actual new priority! */

    errno = 0;
    value = nice(increment);
#if defined(HAVE_BROKEN_NICE) && defined(HAVE_GETPRIORITY)
    if (value == 0)
        value = getpriority(PRIO_PROCESS, 0);
#endif
    if (value == -1 && errno != 0)
        /* either nice() or getpriority() returned an error */
        return posix_error();
    return PyLong_FromLong((long) value);
}
#endif /* HAVE_NICE */


#ifdef HAVE_GETPRIORITY
/*[clinic input]
os.getpriority

    which: int
    who: int

Return program scheduling priority.
[clinic start generated code]*/

static PyObject *
os_getpriority_impl(PyObject *module, int which, int who)
/*[clinic end generated code: output=c41b7b63c7420228 input=9be615d40e2544ef]*/
{
    int retval;

    errno = 0;
    retval = getpriority(which, who);
    if (errno != 0)
        return posix_error();
    return PyLong_FromLong((long)retval);
}
#endif /* HAVE_GETPRIORITY */


#ifdef HAVE_SETPRIORITY
/*[clinic input]
os.setpriority

    which: int
    who: int
    priority: int

Set program scheduling priority.
[clinic start generated code]*/

static PyObject *
os_setpriority_impl(PyObject *module, int which, int who, int priority)
/*[clinic end generated code: output=3d910d95a7771eb2 input=710ccbf65b9dc513]*/
{
    int retval;

    retval = setpriority(which, who, priority);
    if (retval == -1)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETPRIORITY */


static PyObject *
internal_rename(path_t *src, path_t *dst, int src_dir_fd, int dst_dir_fd, int is_replace)
{
    const char *function_name = is_replace ? "replace" : "rename";
    int dir_fd_specified;

#ifdef MS_WINDOWS
    BOOL result;
    int flags = is_replace ? MOVEFILE_REPLACE_EXISTING : 0;
#else
    int result;
#endif

    dir_fd_specified = (src_dir_fd != DEFAULT_DIR_FD) ||
                       (dst_dir_fd != DEFAULT_DIR_FD);
#ifndef HAVE_RENAMEAT
    if (dir_fd_specified) {
        argument_unavailable_error(function_name, "src_dir_fd and dst_dir_fd");
        return NULL;
    }
#endif

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    result = MoveFileExW(src->wide, dst->wide, flags);
    Py_END_ALLOW_THREADS

    if (!result)
        return path_error2(src, dst);

#else
    if ((src->narrow && dst->wide) || (src->wide && dst->narrow)) {
        PyErr_Format(PyExc_ValueError,
                     "%s: src and dst must be the same type", function_name);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_RENAMEAT
    if (dir_fd_specified)
        result = renameat(src_dir_fd, src->narrow, dst_dir_fd, dst->narrow);
    else
#endif
    result = rename(src->narrow, dst->narrow);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error2(src, dst);
#endif
    Py_RETURN_NONE;
}


/*[clinic input]
os.rename

    src : path_t
    dst : path_t
    *
    src_dir_fd : dir_fd = None
    dst_dir_fd : dir_fd = None

Rename a file or directory.

If either src_dir_fd or dst_dir_fd is not None, it should be a file
  descriptor open to a directory, and the respective path string (src or dst)
  should be relative; the path will then be relative to that directory.
src_dir_fd and dst_dir_fd, may not be implemented on your platform.
  If they are unavailable, using them will raise a NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_rename_impl(PyObject *module, path_t *src, path_t *dst, int src_dir_fd,
               int dst_dir_fd)
/*[clinic end generated code: output=59e803072cf41230 input=faa61c847912c850]*/
{
    return internal_rename(src, dst, src_dir_fd, dst_dir_fd, 0);
}


/*[clinic input]
os.replace = os.rename

Rename a file or directory, overwriting the destination.

If either src_dir_fd or dst_dir_fd is not None, it should be a file
  descriptor open to a directory, and the respective path string (src or dst)
  should be relative; the path will then be relative to that directory.
src_dir_fd and dst_dir_fd, may not be implemented on your platform.
  If they are unavailable, using them will raise a NotImplementedError."
[clinic start generated code]*/

static PyObject *
os_replace_impl(PyObject *module, path_t *src, path_t *dst, int src_dir_fd,
                int dst_dir_fd)
/*[clinic end generated code: output=1968c02e7857422b input=25515dfb107c8421]*/
{
    return internal_rename(src, dst, src_dir_fd, dst_dir_fd, 1);
}


/*[clinic input]
os.rmdir

    path: path_t
    *
    dir_fd: dir_fd(requires='unlinkat') = None

Remove a directory.

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_rmdir_impl(PyObject *module, path_t *path, int dir_fd)
/*[clinic end generated code: output=080eb54f506e8301 input=38c8b375ca34a7e2]*/
{
    int result;

    Py_BEGIN_ALLOW_THREADS
#ifdef MS_WINDOWS
    /* Windows, success=1, UNIX, success=0 */
    result = !RemoveDirectoryW(path->wide);
#else
#ifdef HAVE_UNLINKAT
    if (dir_fd != DEFAULT_DIR_FD)
        result = unlinkat(dir_fd, path->narrow, AT_REMOVEDIR);
    else
#endif
        result = rmdir(path->narrow);
#endif
    Py_END_ALLOW_THREADS

    if (result)
        return path_error(path);

    Py_RETURN_NONE;
}


#ifdef HAVE_SYSTEM
#ifdef MS_WINDOWS
/*[clinic input]
os.system -> long

    command: Py_UNICODE

Execute the command in a subshell.
[clinic start generated code]*/

static long
os_system_impl(PyObject *module, Py_UNICODE *command)
/*[clinic end generated code: output=96c4dffee36dfb48 input=303f5ce97df606b0]*/
{
    long result;
    Py_BEGIN_ALLOW_THREADS
    result = _wsystem(command);
    Py_END_ALLOW_THREADS
    return result;
}
#else /* MS_WINDOWS */
/*[clinic input]
os.system -> long

    command: FSConverter

Execute the command in a subshell.
[clinic start generated code]*/

static long
os_system_impl(PyObject *module, PyObject *command)
/*[clinic end generated code: output=290fc437dd4f33a0 input=86a58554ba6094af]*/
{
    long result;
    const char *bytes = PyBytes_AsString(command);
    Py_BEGIN_ALLOW_THREADS
    result = system(bytes);
    Py_END_ALLOW_THREADS
    return result;
}
#endif
#endif /* HAVE_SYSTEM */


/*[clinic input]
os.umask

    mask: int
    /

Set the current numeric umask and return the previous umask.
[clinic start generated code]*/

static PyObject *
os_umask_impl(PyObject *module, int mask)
/*[clinic end generated code: output=a2e33ce3bc1a6e33 input=ab6bfd9b24d8a7e8]*/
{
    int i = (int)umask(mask);
    if (i < 0)
        return posix_error();
    return PyLong_FromLong((long)i);
}

#ifdef MS_WINDOWS

/* override the default DeleteFileW behavior so that directory
symlinks can be removed with this function, the same as with
Unix symlinks */
BOOL WINAPI Py_DeleteFileW(LPCWSTR lpFileName)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    WIN32_FIND_DATAW find_data;
    HANDLE find_data_handle;
    int is_directory = 0;
    int is_link = 0;

    if (GetFileAttributesExW(lpFileName, GetFileExInfoStandard, &info)) {
        is_directory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

        /* Get WIN32_FIND_DATA structure for the path to determine if
           it is a symlink */
        if(is_directory &&
           info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            find_data_handle = FindFirstFileW(lpFileName, &find_data);

            if(find_data_handle != INVALID_HANDLE_VALUE) {
                /* IO_REPARSE_TAG_SYMLINK if it is a symlink and
                   IO_REPARSE_TAG_MOUNT_POINT if it is a junction point. */
                is_link = find_data.dwReserved0 == IO_REPARSE_TAG_SYMLINK ||
                          find_data.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT;
                FindClose(find_data_handle);
            }
        }
    }

    if (is_directory && is_link)
        return RemoveDirectoryW(lpFileName);

    return DeleteFileW(lpFileName);
}
#endif /* MS_WINDOWS */


/*[clinic input]
os.unlink

    path: path_t
    *
    dir_fd: dir_fd(requires='unlinkat')=None

Remove a file (same as remove()).

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_unlink_impl(PyObject *module, path_t *path, int dir_fd)
/*[clinic end generated code: output=621797807b9963b1 input=d7bcde2b1b2a2552]*/
{
    int result;

    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
#ifdef MS_WINDOWS
    /* Windows, success=1, UNIX, success=0 */
    result = !Py_DeleteFileW(path->wide);
#else
#ifdef HAVE_UNLINKAT
    if (dir_fd != DEFAULT_DIR_FD)
        result = unlinkat(dir_fd, path->narrow, 0);
    else
#endif /* HAVE_UNLINKAT */
        result = unlink(path->narrow);
#endif
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS

    if (result)
        return path_error(path);

    Py_RETURN_NONE;
}


/*[clinic input]
os.remove = os.unlink

Remove a file (same as unlink()).

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_remove_impl(PyObject *module, path_t *path, int dir_fd)
/*[clinic end generated code: output=a8535b28f0068883 input=e05c5ab55cd30983]*/
{
    return os_unlink_impl(module, path, dir_fd);
}


static PyStructSequence_Field uname_result_fields[] = {
    {"sysname",    "operating system name"},
    {"nodename",   "name of machine on network (implementation-defined)"},
    {"release",    "operating system release"},
    {"version",    "operating system version"},
    {"machine",    "hardware identifier"},
    {NULL}
};

PyDoc_STRVAR(uname_result__doc__,
"uname_result: Result from os.uname().\n\n\
This object may be accessed either as a tuple of\n\
  (sysname, nodename, release, version, machine),\n\
or via the attributes sysname, nodename, release, version, and machine.\n\
\n\
See os.uname for more information.");

static PyStructSequence_Desc uname_result_desc = {
    "uname_result", /* name */
    uname_result__doc__, /* doc */
    uname_result_fields,
    5
};

static PyTypeObject UnameResultType;


#ifdef HAVE_UNAME
/*[clinic input]
os.uname

Return an object identifying the current operating system.

The object behaves like a named tuple with the following fields:
  (sysname, nodename, release, version, machine)

[clinic start generated code]*/

static PyObject *
os_uname_impl(PyObject *module)
/*[clinic end generated code: output=e6a49cf1a1508a19 input=e68bd246db3043ed]*/
{
    struct utsname u;
    int res;
    PyObject *value;

    Py_BEGIN_ALLOW_THREADS
    res = uname(&u);
    Py_END_ALLOW_THREADS
    if (res < 0)
        return posix_error();

    value = PyStructSequence_New(&UnameResultType);
    if (value == NULL)
        return NULL;

#define SET(i, field) \
    { \
    PyObject *o = PyUnicode_DecodeFSDefault(field); \
    if (!o) { \
        Py_DECREF(value); \
        return NULL; \
    } \
    PyStructSequence_SET_ITEM(value, i, o); \
    } \

    SET(0, u.sysname);
    SET(1, u.nodename);
    SET(2, u.release);
    SET(3, u.version);
    SET(4, u.machine);

#undef SET

    return value;
}
#endif /* HAVE_UNAME */



typedef struct {
    int    now;
    time_t atime_s;
    long   atime_ns;
    time_t mtime_s;
    long   mtime_ns;
} utime_t;

/*
 * these macros assume that "ut" is a pointer to a utime_t
 * they also intentionally leak the declaration of a pointer named "time"
 */
#define UTIME_TO_TIMESPEC \
    struct timespec ts[2]; \
    struct timespec *time; \
    if (ut->now) \
        time = NULL; \
    else { \
        ts[0].tv_sec = ut->atime_s; \
        ts[0].tv_nsec = ut->atime_ns; \
        ts[1].tv_sec = ut->mtime_s; \
        ts[1].tv_nsec = ut->mtime_ns; \
        time = ts; \
    } \

#define UTIME_TO_TIMEVAL \
    struct timeval tv[2]; \
    struct timeval *time; \
    if (ut->now) \
        time = NULL; \
    else { \
        tv[0].tv_sec = ut->atime_s; \
        tv[0].tv_usec = ut->atime_ns / 1000; \
        tv[1].tv_sec = ut->mtime_s; \
        tv[1].tv_usec = ut->mtime_ns / 1000; \
        time = tv; \
    } \

#define UTIME_TO_UTIMBUF \
    struct utimbuf u; \
    struct utimbuf *time; \
    if (ut->now) \
        time = NULL; \
    else { \
        u.actime = ut->atime_s; \
        u.modtime = ut->mtime_s; \
        time = &u; \
    }

#define UTIME_TO_TIME_T \
    time_t timet[2]; \
    time_t *time; \
    if (ut->now) \
        time = NULL; \
    else { \
        timet[0] = ut->atime_s; \
        timet[1] = ut->mtime_s; \
        time = timet; \
    } \


#if defined(HAVE_FUTIMESAT) || defined(HAVE_UTIMENSAT)

static int
utime_dir_fd(utime_t *ut, int dir_fd, const char *path, int follow_symlinks)
{
#ifdef HAVE_UTIMENSAT
    int flags = follow_symlinks ? 0 : AT_SYMLINK_NOFOLLOW;
    UTIME_TO_TIMESPEC;
    return utimensat(dir_fd, path, time, flags);
#elif defined(HAVE_FUTIMESAT)
    UTIME_TO_TIMEVAL;
    /*
     * follow_symlinks will never be false here;
     * we only allow !follow_symlinks and dir_fd together
     * if we have utimensat()
     */
    assert(follow_symlinks);
    return futimesat(dir_fd, path, time);
#endif
}

    #define FUTIMENSAT_DIR_FD_CONVERTER dir_fd_converter
#else
    #define FUTIMENSAT_DIR_FD_CONVERTER dir_fd_unavailable
#endif

#if defined(HAVE_FUTIMES) || defined(HAVE_FUTIMENS)

static int
utime_fd(utime_t *ut, int fd)
{
#ifdef HAVE_FUTIMENS
    UTIME_TO_TIMESPEC;
    return futimens(fd, time);
#else
    UTIME_TO_TIMEVAL;
    return futimes(fd, time);
#endif
}

    #define PATH_UTIME_HAVE_FD 1
#else
    #define PATH_UTIME_HAVE_FD 0
#endif

#if defined(HAVE_UTIMENSAT) || defined(HAVE_LUTIMES)
#  define UTIME_HAVE_NOFOLLOW_SYMLINKS
#endif

#ifdef UTIME_HAVE_NOFOLLOW_SYMLINKS

static int
utime_nofollow_symlinks(utime_t *ut, const char *path)
{
#ifdef HAVE_UTIMENSAT
    UTIME_TO_TIMESPEC;
    return utimensat(DEFAULT_DIR_FD, path, time, AT_SYMLINK_NOFOLLOW);
#else
    UTIME_TO_TIMEVAL;
    return lutimes(path, time);
#endif
}

#endif

#ifndef MS_WINDOWS

static int
utime_default(utime_t *ut, const char *path)
{
#ifdef HAVE_UTIMENSAT
    UTIME_TO_TIMESPEC;
    return utimensat(DEFAULT_DIR_FD, path, time, 0);
#elif defined(HAVE_UTIMES)
    UTIME_TO_TIMEVAL;
    return utimes(path, time);
#elif defined(HAVE_UTIME_H)
    UTIME_TO_UTIMBUF;
    return utime(path, time);
#else
    UTIME_TO_TIME_T;
    return utime(path, time);
#endif
}

#endif

static int
split_py_long_to_s_and_ns(PyObject *py_long, time_t *s, long *ns)
{
    int result = 0;
    PyObject *divmod;
    divmod = PyNumber_Divmod(py_long, billion);
    if (!divmod)
        goto exit;
    *s = _PyLong_AsTime_t(PyTuple_GET_ITEM(divmod, 0));
    if ((*s == -1) && PyErr_Occurred())
        goto exit;
    *ns = PyLong_AsLong(PyTuple_GET_ITEM(divmod, 1));
    if ((*ns == -1) && PyErr_Occurred())
        goto exit;

    result = 1;
exit:
    Py_XDECREF(divmod);
    return result;
}


/*[clinic input]
os.utime

    path: path_t(allow_fd='PATH_UTIME_HAVE_FD')
    times: object = NULL
    *
    ns: object = NULL
    dir_fd: dir_fd(requires='futimensat') = None
    follow_symlinks: bool=True

# "utime(path, times=None, *[, ns], dir_fd=None, follow_symlinks=True)\n\

Set the access and modified time of path.

path may always be specified as a string.
On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.

If times is not None, it must be a tuple (atime, mtime);
    atime and mtime should be expressed as float seconds since the epoch.
If ns is specified, it must be a tuple (atime_ns, mtime_ns);
    atime_ns and mtime_ns should be expressed as integer nanoseconds
    since the epoch.
If times is None and ns is unspecified, utime uses the current time.
Specifying tuples for both times and ns is an error.

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, utime will modify the symbolic link itself instead of the file the
  link points to.
It is an error to use dir_fd or follow_symlinks when specifying path
  as an open file descriptor.
dir_fd and follow_symlinks may not be available on your platform.
  If they are unavailable, using them will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_utime_impl(PyObject *module, path_t *path, PyObject *times, PyObject *ns,
              int dir_fd, int follow_symlinks)
/*[clinic end generated code: output=cfcac69d027b82cf input=081cdc54ca685385]*/
{
#ifdef MS_WINDOWS
    HANDLE hFile;
    FILETIME atime, mtime;
#else
    int result;
#endif

    PyObject *return_value = NULL;
    utime_t utime;

    memset(&utime, 0, sizeof(utime_t));

    if (times && (times != Py_None) && ns) {
        PyErr_SetString(PyExc_ValueError,
                     "utime: you may specify either 'times'"
                     " or 'ns' but not both");
        goto exit;
    }

    if (times && (times != Py_None)) {
        time_t a_sec, m_sec;
        long a_nsec, m_nsec;
        if (!PyTuple_CheckExact(times) || (PyTuple_Size(times) != 2)) {
            PyErr_SetString(PyExc_TypeError,
                         "utime: 'times' must be either"
                         " a tuple of two ints or None");
            goto exit;
        }
        utime.now = 0;
        if (_PyTime_ObjectToTimespec(PyTuple_GET_ITEM(times, 0),
                                     &a_sec, &a_nsec, _PyTime_ROUND_FLOOR) == -1 ||
            _PyTime_ObjectToTimespec(PyTuple_GET_ITEM(times, 1),
                                     &m_sec, &m_nsec, _PyTime_ROUND_FLOOR) == -1) {
            goto exit;
        }
        utime.atime_s = a_sec;
        utime.atime_ns = a_nsec;
        utime.mtime_s = m_sec;
        utime.mtime_ns = m_nsec;
    }
    else if (ns) {
        if (!PyTuple_CheckExact(ns) || (PyTuple_Size(ns) != 2)) {
            PyErr_SetString(PyExc_TypeError,
                         "utime: 'ns' must be a tuple of two ints");
            goto exit;
        }
        utime.now = 0;
        if (!split_py_long_to_s_and_ns(PyTuple_GET_ITEM(ns, 0),
                                      &utime.atime_s, &utime.atime_ns) ||
            !split_py_long_to_s_and_ns(PyTuple_GET_ITEM(ns, 1),
                                       &utime.mtime_s, &utime.mtime_ns)) {
            goto exit;
        }
    }
    else {
        /* times and ns are both None/unspecified. use "now". */
        utime.now = 1;
    }

#if !defined(UTIME_HAVE_NOFOLLOW_SYMLINKS)
    if (follow_symlinks_specified("utime", follow_symlinks))
        goto exit;
#endif

    if (path_and_dir_fd_invalid("utime", path, dir_fd) ||
        dir_fd_and_fd_invalid("utime", dir_fd, path->fd) ||
        fd_and_follow_symlinks_invalid("utime", path->fd, follow_symlinks))
        goto exit;

#if !defined(HAVE_UTIMENSAT)
    if ((dir_fd != DEFAULT_DIR_FD) && (!follow_symlinks)) {
        PyErr_SetString(PyExc_ValueError,
                     "utime: cannot use dir_fd and follow_symlinks "
                     "together on this platform");
        goto exit;
    }
#endif

#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    hFile = CreateFileW(path->wide, FILE_WRITE_ATTRIBUTES, 0,
                        NULL, OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS, NULL);
    Py_END_ALLOW_THREADS
    if (hFile == INVALID_HANDLE_VALUE) {
        path_error(path);
        goto exit;
    }

    if (utime.now) {
        GetSystemTimeAsFileTime(&mtime);
        atime = mtime;
    }
    else {
        _Py_time_t_to_FILE_TIME(utime.atime_s, utime.atime_ns, &atime);
        _Py_time_t_to_FILE_TIME(utime.mtime_s, utime.mtime_ns, &mtime);
    }
    if (!SetFileTime(hFile, NULL, &atime, &mtime)) {
        /* Avoid putting the file name into the error here,
           as that may confuse the user into believing that
           something is wrong with the file, when it also
           could be the time stamp that gives a problem. */
        PyErr_SetFromWindowsErr(0);
        goto exit;
    }
#else /* MS_WINDOWS */
    Py_BEGIN_ALLOW_THREADS

#ifdef UTIME_HAVE_NOFOLLOW_SYMLINKS
    if ((!follow_symlinks) && (dir_fd == DEFAULT_DIR_FD))
        result = utime_nofollow_symlinks(&utime, path->narrow);
    else
#endif

#if defined(HAVE_FUTIMESAT) || defined(HAVE_UTIMENSAT)
    if ((dir_fd != DEFAULT_DIR_FD) || (!follow_symlinks))
        result = utime_dir_fd(&utime, dir_fd, path->narrow, follow_symlinks);
    else
#endif

#if defined(HAVE_FUTIMES) || defined(HAVE_FUTIMENS)
    if (path->fd != -1)
        result = utime_fd(&utime, path->fd);
    else
#endif

    result = utime_default(&utime, path->narrow);

    Py_END_ALLOW_THREADS

    if (result < 0) {
        /* see previous comment about not putting filename in error here */
        return_value = posix_error();
        goto exit;
    }

#endif /* MS_WINDOWS */

    Py_INCREF(Py_None);
    return_value = Py_None;

exit:
#ifdef MS_WINDOWS
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
#endif
    return return_value;
}

/* Process operations */


/*[clinic input]
os._exit

    status: int

Exit to the system with specified status, without normal exit processing.
[clinic start generated code]*/

static PyObject *
os__exit_impl(PyObject *module, int status)
/*[clinic end generated code: output=116e52d9c2260d54 input=5e6d57556b0c4a62]*/
{
    _exit(status);
    return NULL; /* Make gcc -Wall happy */
}

#if defined(HAVE_WEXECV) || defined(HAVE_WSPAWNV)
#define EXECV_CHAR wchar_t
#else
#define EXECV_CHAR char
#endif

#if defined(HAVE_EXECV) || defined(HAVE_SPAWNV)
static void
free_string_array(EXECV_CHAR **array, Py_ssize_t count)
{
    Py_ssize_t i;
    for (i = 0; i < count; i++)
        PyMem_Free(array[i]);
    PyMem_DEL(array);
}

static
int fsconvert_strdup(PyObject *o, EXECV_CHAR**out)
{
    Py_ssize_t size;
#if defined(HAVE_WEXECV) || defined(HAVE_WSPAWNV)
    *out = PyUnicode_AsWideCharString(o, &size);
    if (!*out)
        return 0;
#else
    PyObject *bytes;
    if (!PyUnicode_FSConverter(o, &bytes))
        return 0;
    size = PyBytes_GET_SIZE(bytes);
    *out = PyMem_Malloc(size+1);
    if (!*out) {
        PyErr_NoMemory();
        return 0;
    }
    memcpy(*out, PyBytes_AsString(bytes), size+1);
    Py_DECREF(bytes);
#endif
    return 1;
}
#endif

#if defined(HAVE_EXECV) || defined (HAVE_FEXECVE)
static EXECV_CHAR**
parse_envlist(PyObject* env, Py_ssize_t *envc_ptr)
{
    Py_ssize_t i, pos, envc;
    PyObject *keys=NULL, *vals=NULL;
    PyObject *key, *val, *keyval;
    EXECV_CHAR **envlist;

    i = PyMapping_Size(env);
    if (i < 0)
        return NULL;
    envlist = PyMem_NEW(EXECV_CHAR *, i + 1);
    if (envlist == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    envc = 0;
    keys = PyMapping_Keys(env);
    if (!keys)
        goto error;
    vals = PyMapping_Values(env);
    if (!vals)
        goto error;
    if (!PyList_Check(keys) || !PyList_Check(vals)) {
        PyErr_Format(PyExc_TypeError,
                     "env.keys() or env.values() is not a list");
        goto error;
    }

    for (pos = 0; pos < i; pos++) {
        key = PyList_GetItem(keys, pos);
        val = PyList_GetItem(vals, pos);
        if (!key || !val)
            goto error;

        keyval = PyUnicode_FromFormat("%U=%U", key, val);
        if (!keyval)
            goto error;

        if (!fsconvert_strdup(keyval, &envlist[envc++])) {
            Py_DECREF(keyval);
            goto error;
        }
        
        Py_DECREF(keyval);
    }
    Py_DECREF(vals);
    Py_DECREF(keys);

    envlist[envc] = 0;
    *envc_ptr = envc;
    return envlist;

error:
    Py_XDECREF(keys);
    Py_XDECREF(vals);
    free_string_array(envlist, envc);
    return NULL;
}

static EXECV_CHAR**
parse_arglist(PyObject* argv, Py_ssize_t *argc)
{
    int i;
    EXECV_CHAR **argvlist = PyMem_NEW(EXECV_CHAR *, *argc+1);
    if (argvlist == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    for (i = 0; i < *argc; i++) {
        PyObject* item = PySequence_ITEM(argv, i);
        if (item == NULL)
            goto fail;
        if (!fsconvert_strdup(item, &argvlist[i])) {
            Py_DECREF(item);
            goto fail;
        }
        Py_DECREF(item);
    }
    argvlist[*argc] = NULL;
    return argvlist;
fail:
    *argc = i;
    free_string_array(argvlist, *argc);
    return NULL;
}

#endif


#ifdef HAVE_EXECV
/*[clinic input]
os.execv

    path: path_t
        Path of executable file.
    argv: object
        Tuple or list of strings.
    /

Execute an executable path with arguments, replacing current process.
[clinic start generated code]*/

static PyObject *
os_execv_impl(PyObject *module, path_t *path, PyObject *argv)
/*[clinic end generated code: output=3b52fec34cd0dafd input=9bac31efae07dac7]*/
{
    EXECV_CHAR **argvlist;
    Py_ssize_t argc;

    /* execv has two arguments: (path, argv), where
       argv is a list or tuple of strings. */

    if (!PyList_Check(argv) && !PyTuple_Check(argv)) {
        PyErr_SetString(PyExc_TypeError,
                        "execv() arg 2 must be a tuple or list");
        return NULL;
    }
    argc = PySequence_Size(argv);
    if (argc < 1) {
        PyErr_SetString(PyExc_ValueError, "execv() arg 2 must not be empty");
        return NULL;
    }

    argvlist = parse_arglist(argv, &argc);
    if (argvlist == NULL) {
        return NULL;
    }

#ifdef HAVE_WEXECV
    _wexecv(path->wide, argvlist);
#else
    execv(path->narrow, argvlist);
#endif

    /* If we get here it's definitely an error */

    free_string_array(argvlist, argc);
    return posix_error();
}


/*[clinic input]
os.execve

    path: path_t(allow_fd='PATH_HAVE_FEXECVE')
        Path of executable file.
    argv: object
        Tuple or list of strings.
    env: object
        Dictionary of strings mapping to strings.

Execute an executable path with arguments, replacing current process.
[clinic start generated code]*/

static PyObject *
os_execve_impl(PyObject *module, path_t *path, PyObject *argv, PyObject *env)
/*[clinic end generated code: output=ff9fa8e4da8bde58 input=626804fa092606d9]*/
{
    EXECV_CHAR **argvlist = NULL;
    EXECV_CHAR **envlist;
    Py_ssize_t argc, envc;

    /* execve has three arguments: (path, argv, env), where
       argv is a list or tuple of strings and env is a dictionary
       like posix.environ. */

    if (!PyList_Check(argv) && !PyTuple_Check(argv)) {
        PyErr_SetString(PyExc_TypeError,
                        "execve: argv must be a tuple or list");
        goto fail;
    }
    argc = PySequence_Size(argv);
    if (!PyMapping_Check(env)) {
        PyErr_SetString(PyExc_TypeError,
                        "execve: environment must be a mapping object");
        goto fail;
    }

    argvlist = parse_arglist(argv, &argc);
    if (argvlist == NULL) {
        goto fail;
    }

    envlist = parse_envlist(env, &envc);
    if (envlist == NULL)
        goto fail;

#ifdef HAVE_FEXECVE
    if (path->fd > -1)
        fexecve(path->fd, argvlist, envlist);
    else
#endif
#ifdef HAVE_WEXECV
        _wexecve(path->wide, argvlist, envlist);
#else
        execve(path->narrow, argvlist, envlist);
#endif

    /* If we get here it's definitely an error */

    path_error(path);

    free_string_array(envlist, envc);
  fail:
    if (argvlist)
        free_string_array(argvlist, argc);
    return NULL;
}

#endif /* HAVE_EXECV */


#if defined(HAVE_SPAWNV) || defined(HAVE_WSPAWNV)
/*[clinic input]
os.spawnv

    mode: int
        Mode of process creation.
    path: path_t
        Path of executable file.
    argv: object
        Tuple or list of strings.
    /

Execute the program specified by path in a new process.
[clinic start generated code]*/

static PyObject *
os_spawnv_impl(PyObject *module, int mode, path_t *path, PyObject *argv)
/*[clinic end generated code: output=71cd037a9d96b816 input=43224242303291be]*/
{
    EXECV_CHAR **argvlist;
    int i;
    Py_ssize_t argc;
    intptr_t spawnval;
    PyObject *(*getitem)(PyObject *, Py_ssize_t);

    /* spawnv has three arguments: (mode, path, argv), where
       argv is a list or tuple of strings. */

    if (PyList_Check(argv)) {
        argc = PyList_Size(argv);
        getitem = PyList_GetItem;
    }
    else if (PyTuple_Check(argv)) {
        argc = PyTuple_Size(argv);
        getitem = PyTuple_GetItem;
    }
    else {
        PyErr_SetString(PyExc_TypeError,
                        "spawnv() arg 2 must be a tuple or list");
        return NULL;
    }

    argvlist = PyMem_NEW(EXECV_CHAR *, argc+1);
    if (argvlist == NULL) {
        return PyErr_NoMemory();
    }
    for (i = 0; i < argc; i++) {
        if (!fsconvert_strdup((*getitem)(argv, i),
                              &argvlist[i])) {
            free_string_array(argvlist, i);
            PyErr_SetString(
                PyExc_TypeError,
                "spawnv() arg 2 must contain only strings");
            return NULL;
        }
    }
    argvlist[argc] = NULL;

    if (mode == _OLD_P_OVERLAY)
        mode = _P_OVERLAY;

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_WSPAWNV
    spawnval = _wspawnv(mode, path->wide, argvlist);
#else
    spawnval = _spawnv(mode, path->narrow, argvlist);
#endif
    Py_END_ALLOW_THREADS

    free_string_array(argvlist, argc);

    if (spawnval == -1)
        return posix_error();
    else
        return Py_BuildValue(_Py_PARSE_INTPTR, spawnval);
}


/*[clinic input]
os.spawnve

    mode: int
        Mode of process creation.
    path: path_t
        Path of executable file.
    argv: object
        Tuple or list of strings.
    env: object
        Dictionary of strings mapping to strings.
    /

Execute the program specified by path in a new process.
[clinic start generated code]*/

static PyObject *
os_spawnve_impl(PyObject *module, int mode, path_t *path, PyObject *argv,
                PyObject *env)
/*[clinic end generated code: output=30fe85be56fe37ad input=3e40803ee7c4c586]*/
{
    EXECV_CHAR **argvlist;
    EXECV_CHAR **envlist;
    PyObject *res = NULL;
    Py_ssize_t argc, i, envc;
    intptr_t spawnval;
    PyObject *(*getitem)(PyObject *, Py_ssize_t);
    Py_ssize_t lastarg = 0;

    /* spawnve has four arguments: (mode, path, argv, env), where
       argv is a list or tuple of strings and env is a dictionary
       like posix.environ. */

    if (PyList_Check(argv)) {
        argc = PyList_Size(argv);
        getitem = PyList_GetItem;
    }
    else if (PyTuple_Check(argv)) {
        argc = PyTuple_Size(argv);
        getitem = PyTuple_GetItem;
    }
    else {
        PyErr_SetString(PyExc_TypeError,
                        "spawnve() arg 2 must be a tuple or list");
        goto fail_0;
    }
    if (!PyMapping_Check(env)) {
        PyErr_SetString(PyExc_TypeError,
                        "spawnve() arg 3 must be a mapping object");
        goto fail_0;
    }

    argvlist = PyMem_NEW(EXECV_CHAR *, argc+1);
    if (argvlist == NULL) {
        PyErr_NoMemory();
        goto fail_0;
    }
    for (i = 0; i < argc; i++) {
        if (!fsconvert_strdup((*getitem)(argv, i),
                              &argvlist[i]))
        {
            lastarg = i;
            goto fail_1;
        }
    }
    lastarg = argc;
    argvlist[argc] = NULL;

    envlist = parse_envlist(env, &envc);
    if (envlist == NULL)
        goto fail_1;

    if (mode == _OLD_P_OVERLAY)
        mode = _P_OVERLAY;

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_WSPAWNV
    spawnval = _wspawnve(mode, path->wide, argvlist, envlist);
#else
    spawnval = _spawnve(mode, path->narrow, argvlist, envlist);
#endif
    Py_END_ALLOW_THREADS

    if (spawnval == -1)
        (void) posix_error();
    else
        res = Py_BuildValue(_Py_PARSE_INTPTR, spawnval);

    while (--envc >= 0)
        PyMem_DEL(envlist[envc]);
    PyMem_DEL(envlist);
  fail_1:
    free_string_array(argvlist, lastarg);
  fail_0:
    return res;
}

#endif /* HAVE_SPAWNV */


#ifdef HAVE_FORK1
/*[clinic input]
os.fork1

Fork a child process with a single multiplexed (i.e., not bound) thread.

Return 0 to child process and PID of child to parent process.
[clinic start generated code]*/

static PyObject *
os_fork1_impl(PyObject *module)
/*[clinic end generated code: output=0de8e67ce2a310bc input=12db02167893926e]*/
{
    pid_t pid;
    int result = 0;
    _PyImport_AcquireLock();
    pid = fork1();
    if (pid == 0) {
        /* child: this clobbers and resets the import lock. */
        PyOS_AfterFork();
    } else {
        /* parent: release the import lock. */
        result = _PyImport_ReleaseLock();
    }
    if (pid == -1)
        return posix_error();
    if (result < 0) {
        /* Don't clobber the OSError if the fork failed. */
        PyErr_SetString(PyExc_RuntimeError,
                        "not holding the import lock");
        return NULL;
    }
    return PyLong_FromPid(pid);
}
#endif /* HAVE_FORK1 */


#ifdef HAVE_FORK
/*[clinic input]
os.fork

Fork a child process.

Return 0 to child process and PID of child to parent process.
[clinic start generated code]*/

static PyObject *
os_fork_impl(PyObject *module)
/*[clinic end generated code: output=3626c81f98985d49 input=13c956413110eeaa]*/
{
    pid_t pid;
    int result = 0;
    _PyImport_AcquireLock();
    pid = fork();
    if (pid == 0) {
        /* child: this clobbers and resets the import lock. */
        PyOS_AfterFork();
    } else {
        /* parent: release the import lock. */
        result = _PyImport_ReleaseLock();
    }
    if (pid == -1)
        return posix_error();
    if (result < 0) {
        /* Don't clobber the OSError if the fork failed. */
        PyErr_SetString(PyExc_RuntimeError,
                        "not holding the import lock");
        return NULL;
    }
    return PyLong_FromPid(pid);
}
#endif /* HAVE_FORK */


#ifdef HAVE_SCHED_H
#ifdef HAVE_SCHED_GET_PRIORITY_MAX
/*[clinic input]
os.sched_get_priority_max

    policy: int

Get the maximum scheduling priority for policy.
[clinic start generated code]*/

static PyObject *
os_sched_get_priority_max_impl(PyObject *module, int policy)
/*[clinic end generated code: output=9e465c6e43130521 input=2097b7998eca6874]*/
{
    int max;

    max = sched_get_priority_max(policy);
    if (max < 0)
        return posix_error();
    return PyLong_FromLong(max);
}


/*[clinic input]
os.sched_get_priority_min

    policy: int

Get the minimum scheduling priority for policy.
[clinic start generated code]*/

static PyObject *
os_sched_get_priority_min_impl(PyObject *module, int policy)
/*[clinic end generated code: output=7595c1138cc47a6d input=21bc8fa0d70983bf]*/
{
    int min = sched_get_priority_min(policy);
    if (min < 0)
        return posix_error();
    return PyLong_FromLong(min);
}
#endif /* HAVE_SCHED_GET_PRIORITY_MAX */


#ifdef HAVE_SCHED_SETSCHEDULER
/*[clinic input]
os.sched_getscheduler
    pid: pid_t
    /

Get the scheduling policy for the process identifiedy by pid.

Passing 0 for pid returns the scheduling policy for the calling process.
[clinic start generated code]*/

static PyObject *
os_sched_getscheduler_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=dce4c0bd3f1b34c8 input=5f14cfd1f189e1a0]*/
{
    int policy;

    policy = sched_getscheduler(pid);
    if (policy < 0)
        return posix_error();
    return PyLong_FromLong(policy);
}
#endif /* HAVE_SCHED_SETSCHEDULER */


#if defined(HAVE_SCHED_SETSCHEDULER) || defined(HAVE_SCHED_SETPARAM)
/*[clinic input]
class os.sched_param "PyObject *" "&SchedParamType"

@classmethod
os.sched_param.__new__

    sched_priority: object
        A scheduling parameter.

Current has only one field: sched_priority");
[clinic start generated code]*/

static PyObject *
os_sched_param_impl(PyTypeObject *type, PyObject *sched_priority)
/*[clinic end generated code: output=48f4067d60f48c13 input=73a4c22f7071fc62]*/
{
    PyObject *res;

    res = PyStructSequence_New(type);
    if (!res)
        return NULL;
    Py_INCREF(sched_priority);
    PyStructSequence_SET_ITEM(res, 0, sched_priority);
    return res;
}


PyDoc_VAR(os_sched_param__doc__);

static PyStructSequence_Field sched_param_fields[] = {
    {"sched_priority", "the scheduling priority"},
    {0}
};

static PyStructSequence_Desc sched_param_desc = {
    "sched_param", /* name */
    os_sched_param__doc__, /* doc */
    sched_param_fields,
    1
};

static int
convert_sched_param(PyObject *param, struct sched_param *res)
{
    long priority;

    if (Py_TYPE(param) != &SchedParamType) {
        PyErr_SetString(PyExc_TypeError, "must have a sched_param object");
        return 0;
    }
    priority = PyLong_AsLong(PyStructSequence_GET_ITEM(param, 0));
    if (priority == -1 && PyErr_Occurred())
        return 0;
    if (priority > INT_MAX || priority < INT_MIN) {
        PyErr_SetString(PyExc_OverflowError, "sched_priority out of range");
        return 0;
    }
    res->sched_priority = Py_SAFE_DOWNCAST(priority, long, int);
    return 1;
}
#endif /* defined(HAVE_SCHED_SETSCHEDULER) || defined(HAVE_SCHED_SETPARAM) */


#ifdef HAVE_SCHED_SETSCHEDULER
/*[clinic input]
os.sched_setscheduler

    pid: pid_t
    policy: int
    param: sched_param
    /

Set the scheduling policy for the process identified by pid.

If pid is 0, the calling process is changed.
param is an instance of sched_param.
[clinic start generated code]*/

static PyObject *
os_sched_setscheduler_impl(PyObject *module, pid_t pid, int policy,
                           struct sched_param *param)
/*[clinic end generated code: output=b0ac0a70d3b1d705 input=c581f9469a5327dd]*/
{
    /*
    ** sched_setscheduler() returns 0 in Linux, but the previous
    ** scheduling policy under Solaris/Illumos, and others.
    ** On error, -1 is returned in all Operating Systems.
    */
    if (sched_setscheduler(pid, policy, param) == -1)
        return posix_error();
    Py_RETURN_NONE;
}
#endif  /* HAVE_SCHED_SETSCHEDULER*/


#ifdef HAVE_SCHED_SETPARAM
/*[clinic input]
os.sched_getparam
    pid: pid_t
    /

Returns scheduling parameters for the process identified by pid.

If pid is 0, returns parameters for the calling process.
Return value is an instance of sched_param.
[clinic start generated code]*/

static PyObject *
os_sched_getparam_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=b194e8708dcf2db8 input=18a1ef9c2efae296]*/
{
    struct sched_param param;
    PyObject *result;
    PyObject *priority;

    if (sched_getparam(pid, &param))
        return posix_error();
    result = PyStructSequence_New(&SchedParamType);
    if (!result)
        return NULL;
    priority = PyLong_FromLong(param.sched_priority);
    if (!priority) {
        Py_DECREF(result);
        return NULL;
    }
    PyStructSequence_SET_ITEM(result, 0, priority);
    return result;
}


/*[clinic input]
os.sched_setparam
    pid: pid_t
    param: sched_param
    /

Set scheduling parameters for the process identified by pid.

If pid is 0, sets parameters for the calling process.
param should be an instance of sched_param.
[clinic start generated code]*/

static PyObject *
os_sched_setparam_impl(PyObject *module, pid_t pid,
                       struct sched_param *param)
/*[clinic end generated code: output=8af013f78a32b591 input=6b8d6dfcecdc21bd]*/
{
    if (sched_setparam(pid, param))
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SCHED_SETPARAM */


#ifdef HAVE_SCHED_RR_GET_INTERVAL
/*[clinic input]
os.sched_rr_get_interval -> double
    pid: pid_t
    /

Return the round-robin quantum for the process identified by pid, in seconds.

Value returned is a float.
[clinic start generated code]*/

static double
os_sched_rr_get_interval_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=7e2d935833ab47dc input=2a973da15cca6fae]*/
{
    struct timespec interval;
    if (sched_rr_get_interval(pid, &interval)) {
        posix_error();
        return -1.0;
    }
    return (double)interval.tv_sec + 1e-9*interval.tv_nsec;
}
#endif /* HAVE_SCHED_RR_GET_INTERVAL */


/*[clinic input]
os.sched_yield

Voluntarily relinquish the CPU.
[clinic start generated code]*/

static PyObject *
os_sched_yield_impl(PyObject *module)
/*[clinic end generated code: output=902323500f222cac input=e54d6f98189391d4]*/
{
    if (sched_yield())
        return posix_error();
    Py_RETURN_NONE;
}

#ifdef HAVE_SCHED_SETAFFINITY
/* The minimum number of CPUs allocated in a cpu_set_t */
static const int NCPUS_START = sizeof(unsigned long) * CHAR_BIT;

/*[clinic input]
os.sched_setaffinity
    pid: pid_t
    mask : object
    /

Set the CPU affinity of the process identified by pid to mask.

mask should be an iterable of integers identifying CPUs.
[clinic start generated code]*/

static PyObject *
os_sched_setaffinity_impl(PyObject *module, pid_t pid, PyObject *mask)
/*[clinic end generated code: output=882d7dd9a229335b input=a0791a597c7085ba]*/
{
    int ncpus;
    size_t setsize;
    cpu_set_t *cpu_set = NULL;
    PyObject *iterator = NULL, *item;

    iterator = PyObject_GetIter(mask);
    if (iterator == NULL)
        return NULL;

    ncpus = NCPUS_START;
    setsize = CPU_ALLOC_SIZE(ncpus);
    cpu_set = CPU_ALLOC(ncpus);
    if (cpu_set == NULL) {
        PyErr_NoMemory();
        goto error;
    }
    CPU_ZERO_S(setsize, cpu_set);

    while ((item = PyIter_Next(iterator))) {
        long cpu;
        if (!PyLong_Check(item)) {
            PyErr_Format(PyExc_TypeError,
                        "expected an iterator of ints, "
                        "but iterator yielded %R",
                        Py_TYPE(item));
            Py_DECREF(item);
            goto error;
        }
        cpu = PyLong_AsLong(item);
        Py_DECREF(item);
        if (cpu < 0) {
            if (!PyErr_Occurred())
                PyErr_SetString(PyExc_ValueError, "negative CPU number");
            goto error;
        }
        if (cpu > INT_MAX - 1) {
            PyErr_SetString(PyExc_OverflowError, "CPU number too large");
            goto error;
        }
        if (cpu >= ncpus) {
            /* Grow CPU mask to fit the CPU number */
            int newncpus = ncpus;
            cpu_set_t *newmask;
            size_t newsetsize;
            while (newncpus <= cpu) {
                if (newncpus > INT_MAX / 2)
                    newncpus = cpu + 1;
                else
                    newncpus = newncpus * 2;
            }
            newmask = CPU_ALLOC(newncpus);
            if (newmask == NULL) {
                PyErr_NoMemory();
                goto error;
            }
            newsetsize = CPU_ALLOC_SIZE(newncpus);
            CPU_ZERO_S(newsetsize, newmask);
            memcpy(newmask, cpu_set, setsize);
            CPU_FREE(cpu_set);
            setsize = newsetsize;
            cpu_set = newmask;
            ncpus = newncpus;
        }
        CPU_SET_S(cpu, setsize, cpu_set);
    }
    Py_CLEAR(iterator);

    if (sched_setaffinity(pid, setsize, cpu_set)) {
        posix_error();
        goto error;
    }
    CPU_FREE(cpu_set);
    Py_RETURN_NONE;

error:
    if (cpu_set)
        CPU_FREE(cpu_set);
    Py_XDECREF(iterator);
    return NULL;
}


/*[clinic input]
os.sched_getaffinity
    pid: pid_t
    /

Return the affinity of the process identified by pid (or the current process if zero).

The affinity is returned as a set of CPU identifiers.
[clinic start generated code]*/

static PyObject *
os_sched_getaffinity_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=f726f2c193c17a4f input=983ce7cb4a565980]*/
{
    int cpu, ncpus, count;
    size_t setsize;
    cpu_set_t *mask = NULL;
    PyObject *res = NULL;

    ncpus = NCPUS_START;
    while (1) {
        setsize = CPU_ALLOC_SIZE(ncpus);
        mask = CPU_ALLOC(ncpus);
        if (mask == NULL)
            return PyErr_NoMemory();
        if (sched_getaffinity(pid, setsize, mask) == 0)
            break;
        CPU_FREE(mask);
        if (errno != EINVAL)
            return posix_error();
        if (ncpus > INT_MAX / 2) {
            PyErr_SetString(PyExc_OverflowError, "could not allocate "
                            "a large enough CPU set");
            return NULL;
        }
        ncpus = ncpus * 2;
    }

    res = PySet_New(NULL);
    if (res == NULL)
        goto error;
    for (cpu = 0, count = CPU_COUNT_S(setsize, mask); count; cpu++) {
        if (CPU_ISSET_S(cpu, setsize, mask)) {
            PyObject *cpu_num = PyLong_FromLong(cpu);
            --count;
            if (cpu_num == NULL)
                goto error;
            if (PySet_Add(res, cpu_num)) {
                Py_DECREF(cpu_num);
                goto error;
            }
            Py_DECREF(cpu_num);
        }
    }
    CPU_FREE(mask);
    return res;

error:
    if (mask)
        CPU_FREE(mask);
    Py_XDECREF(res);
    return NULL;
}

#endif /* HAVE_SCHED_SETAFFINITY */

#endif /* HAVE_SCHED_H */


/* AIX uses /dev/ptc but is otherwise the same as /dev/ptmx */
/* IRIX has both /dev/ptc and /dev/ptmx, use ptmx */
#if defined(HAVE_DEV_PTC) && !defined(HAVE_DEV_PTMX)
#define DEV_PTY_FILE "/dev/ptc"
#define HAVE_DEV_PTMX
#else
#define DEV_PTY_FILE "/dev/ptmx"
#endif

#if defined(HAVE_OPENPTY) || defined(HAVE_FORKPTY) || defined(HAVE_DEV_PTMX)
#ifdef HAVE_PTY_H
#include <pty.h>
#else
#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#else
#ifdef HAVE_UTIL_H
#include <util.h>
#endif /* HAVE_UTIL_H */
#endif /* HAVE_LIBUTIL_H */
#endif /* HAVE_PTY_H */
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#endif /* defined(HAVE_OPENPTY) || defined(HAVE_FORKPTY) || defined(HAVE_DEV_PTMX */


#if defined(HAVE_OPENPTY) || defined(HAVE__GETPTY) || defined(HAVE_DEV_PTMX)
/*[clinic input]
os.openpty

Open a pseudo-terminal.

Return a tuple of (master_fd, slave_fd) containing open file descriptors
for both the master and slave ends.
[clinic start generated code]*/

static PyObject *
os_openpty_impl(PyObject *module)
/*[clinic end generated code: output=98841ce5ec9cef3c input=f3d99fd99e762907]*/
{
    int master_fd = -1, slave_fd = -1;
#ifndef HAVE_OPENPTY
    char * slave_name;
#endif
#if defined(HAVE_DEV_PTMX) && !defined(HAVE_OPENPTY) && !defined(HAVE__GETPTY)
    PyOS_sighandler_t sig_saved;
#ifdef sun
    extern char *ptsname(int fildes);
#endif
#endif

#ifdef HAVE_OPENPTY
    if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) != 0)
        goto posix_error;

    if (_Py_set_inheritable(master_fd, 0, NULL) < 0)
        goto error;
    if (_Py_set_inheritable(slave_fd, 0, NULL) < 0)
        goto error;

#elif defined(HAVE__GETPTY)
    slave_name = _getpty(&master_fd, O_RDWR, 0666, 0);
    if (slave_name == NULL)
        goto posix_error;
    if (_Py_set_inheritable(master_fd, 0, NULL) < 0)
        goto error;

    slave_fd = _Py_open(slave_name, O_RDWR);
    if (slave_fd < 0)
        goto error;

#else
    master_fd = open(DEV_PTY_FILE, O_RDWR | O_NOCTTY); /* open master */
    if (master_fd < 0)
        goto posix_error;

    sig_saved = PyOS_setsig(SIGCHLD, SIG_DFL);

    /* change permission of slave */
    if (grantpt(master_fd) < 0) {
        PyOS_setsig(SIGCHLD, sig_saved);
        goto posix_error;
    }

    /* unlock slave */
    if (unlockpt(master_fd) < 0) {
        PyOS_setsig(SIGCHLD, sig_saved);
        goto posix_error;
    }

    PyOS_setsig(SIGCHLD, sig_saved);

    slave_name = ptsname(master_fd); /* get name of slave */
    if (slave_name == NULL)
        goto posix_error;

    slave_fd = _Py_open(slave_name, O_RDWR | O_NOCTTY); /* open slave */
    if (slave_fd == -1)
        goto error;

    if (_Py_set_inheritable(master_fd, 0, NULL) < 0)
        goto posix_error;

#if !defined(__CYGWIN__) && !defined(__ANDROID__) && !defined(HAVE_DEV_PTC)
    ioctl(slave_fd, I_PUSH, "ptem"); /* push ptem */
    ioctl(slave_fd, I_PUSH, "ldterm"); /* push ldterm */
#ifndef __hpux
    ioctl(slave_fd, I_PUSH, "ttcompat"); /* push ttcompat */
#endif /* __hpux */
#endif /* HAVE_CYGWIN */
#endif /* HAVE_OPENPTY */

    return Py_BuildValue("(ii)", master_fd, slave_fd);

posix_error:
    posix_error();
error:
    if (master_fd != -1)
        close(master_fd);
    if (slave_fd != -1)
        close(slave_fd);
    return NULL;
}
#endif /* defined(HAVE_OPENPTY) || defined(HAVE__GETPTY) || defined(HAVE_DEV_PTMX) */


#ifdef HAVE_FORKPTY
/*[clinic input]
os.forkpty

Fork a new process with a new pseudo-terminal as controlling tty.

Returns a tuple of (pid, master_fd).
Like fork(), return pid of 0 to the child process,
and pid of child to the parent process.
To both, return fd of newly opened pseudo-terminal.
[clinic start generated code]*/

static PyObject *
os_forkpty_impl(PyObject *module)
/*[clinic end generated code: output=60d0a5c7512e4087 input=f1f7f4bae3966010]*/
{
    int master_fd = -1, result = 0;
    pid_t pid;

    _PyImport_AcquireLock();
    pid = forkpty(&master_fd, NULL, NULL, NULL);
    if (pid == 0) {
        /* child: this clobbers and resets the import lock. */
        PyOS_AfterFork();
    } else {
        /* parent: release the import lock. */
        result = _PyImport_ReleaseLock();
    }
    if (pid == -1)
        return posix_error();
    if (result < 0) {
        /* Don't clobber the OSError if the fork failed. */
        PyErr_SetString(PyExc_RuntimeError,
                        "not holding the import lock");
        return NULL;
    }
    return Py_BuildValue("(Ni)", PyLong_FromPid(pid), master_fd);
}
#endif /* HAVE_FORKPTY */


#ifdef HAVE_GETEGID
/*[clinic input]
os.getegid

Return the current process's effective group id.
[clinic start generated code]*/

static PyObject *
os_getegid_impl(PyObject *module)
/*[clinic end generated code: output=67d9be7ac68898a2 input=1596f79ad1107d5d]*/
{
    return _PyLong_FromGid(getegid());
}
#endif /* HAVE_GETEGID */


#ifdef HAVE_GETEUID
/*[clinic input]
os.geteuid

Return the current process's effective user id.
[clinic start generated code]*/

static PyObject *
os_geteuid_impl(PyObject *module)
/*[clinic end generated code: output=ea1b60f0d6abb66e input=4644c662d3bd9f19]*/
{
    return _PyLong_FromUid(geteuid());
}
#endif /* HAVE_GETEUID */


#ifdef HAVE_GETGID
/*[clinic input]
os.getgid

Return the current process's group id.
[clinic start generated code]*/

static PyObject *
os_getgid_impl(PyObject *module)
/*[clinic end generated code: output=4f28ebc9d3e5dfcf input=58796344cd87c0f6]*/
{
    return _PyLong_FromGid(getgid());
}
#endif /* HAVE_GETGID */


/*[clinic input]
os.getpid

Return the current process id.
[clinic start generated code]*/

static PyObject *
os_getpid_impl(PyObject *module)
/*[clinic end generated code: output=9ea6fdac01ed2b3c input=5a9a00f0ab68aa00]*/
{
    return PyLong_FromPid(getpid());
}

#ifdef HAVE_GETGROUPLIST

/* AC 3.5: funny apple logic below */
PyDoc_STRVAR(posix_getgrouplist__doc__,
"getgrouplist(user, group) -> list of groups to which a user belongs\n\n\
Returns a list of groups to which a user belongs.\n\n\
    user: username to lookup\n\
    group: base group id of the user");

static PyObject *
posix_getgrouplist(PyObject *self, PyObject *args)
{
#ifdef NGROUPS_MAX
#define MAX_GROUPS NGROUPS_MAX
#else
    /* defined to be 16 on Solaris7, so this should be a small number */
#define MAX_GROUPS 64
#endif

    const char *user;
    int i, ngroups;
    PyObject *list;
#ifdef __APPLE__
    int *groups, basegid;
#else
    gid_t *groups, basegid;
#endif
    ngroups = MAX_GROUPS;

#ifdef __APPLE__
    if (!PyArg_ParseTuple(args, "si:getgrouplist", &user, &basegid))
        return NULL;
#else
    if (!PyArg_ParseTuple(args, "sO&:getgrouplist", &user,
                          _Py_Gid_Converter, &basegid))
        return NULL;
#endif

#ifdef __APPLE__
    groups = PyMem_New(int, ngroups);
#else
    groups = PyMem_New(gid_t, ngroups);
#endif
    if (groups == NULL)
        return PyErr_NoMemory();

    if (getgrouplist(user, basegid, groups, &ngroups) == -1) {
        PyMem_Del(groups);
        return posix_error();
    }

    list = PyList_New(ngroups);
    if (list == NULL) {
        PyMem_Del(groups);
        return NULL;
    }

    for (i = 0; i < ngroups; i++) {
#ifdef __APPLE__
        PyObject *o = PyLong_FromUnsignedLong((unsigned long)groups[i]);
#else
        PyObject *o = _PyLong_FromGid(groups[i]);
#endif
        if (o == NULL) {
            Py_DECREF(list);
            PyMem_Del(groups);
            return NULL;
        }
        PyList_SET_ITEM(list, i, o);
    }

    PyMem_Del(groups);

    return list;
}
#endif /* HAVE_GETGROUPLIST */


#ifdef HAVE_GETGROUPS
/*[clinic input]
os.getgroups

Return list of supplemental group IDs for the process.
[clinic start generated code]*/

static PyObject *
os_getgroups_impl(PyObject *module)
/*[clinic end generated code: output=42b0c17758561b56 input=d3f109412e6a155c]*/
{
    PyObject *result = NULL;

#ifdef NGROUPS_MAX
#define MAX_GROUPS NGROUPS_MAX
#else
    /* defined to be 16 on Solaris7, so this should be a small number */
#define MAX_GROUPS 64
#endif
    gid_t grouplist[MAX_GROUPS];

    /* On MacOSX getgroups(2) can return more than MAX_GROUPS results
     * This is a helper variable to store the intermediate result when
     * that happens.
     *
     * To keep the code readable the OSX behaviour is unconditional,
     * according to the POSIX spec this should be safe on all unix-y
     * systems.
     */
    gid_t* alt_grouplist = grouplist;
    int n;

#ifdef __APPLE__
    /* Issue #17557: As of OS X 10.8, getgroups(2) no longer raises EINVAL if
     * there are more groups than can fit in grouplist.  Therefore, on OS X
     * always first call getgroups with length 0 to get the actual number
     * of groups.
     */
    n = getgroups(0, NULL);
    if (n < 0) {
        return posix_error();
    } else if (n <= MAX_GROUPS) {
        /* groups will fit in existing array */
        alt_grouplist = grouplist;
    } else {
        alt_grouplist = PyMem_New(gid_t, n);
        if (alt_grouplist == NULL) {
            errno = EINVAL;
            return posix_error();
        }
    }

    n = getgroups(n, alt_grouplist);
    if (n == -1) {
        if (alt_grouplist != grouplist) {
            PyMem_Free(alt_grouplist);
        }
        return posix_error();
    }
#else
    n = getgroups(MAX_GROUPS, grouplist);
    if (n < 0) {
        if (errno == EINVAL) {
            n = getgroups(0, NULL);
            if (n == -1) {
                return posix_error();
            }
            if (n == 0) {
                /* Avoid malloc(0) */
                alt_grouplist = grouplist;
            } else {
                alt_grouplist = PyMem_New(gid_t, n);
                if (alt_grouplist == NULL) {
                    errno = EINVAL;
                    return posix_error();
                }
                n = getgroups(n, alt_grouplist);
                if (n == -1) {
                    PyMem_Free(alt_grouplist);
                    return posix_error();
                }
            }
        } else {
            return posix_error();
        }
    }
#endif

    result = PyList_New(n);
    if (result != NULL) {
        int i;
        for (i = 0; i < n; ++i) {
            PyObject *o = _PyLong_FromGid(alt_grouplist[i]);
            if (o == NULL) {
                Py_DECREF(result);
                result = NULL;
                break;
            }
            PyList_SET_ITEM(result, i, o);
        }
    }

    if (alt_grouplist != grouplist) {
        PyMem_Free(alt_grouplist);
    }

    return result;
}
#endif /* HAVE_GETGROUPS */

#ifdef HAVE_INITGROUPS
PyDoc_STRVAR(posix_initgroups__doc__,
"initgroups(username, gid) -> None\n\n\
Call the system initgroups() to initialize the group access list with all of\n\
the groups of which the specified username is a member, plus the specified\n\
group id.");

/* AC 3.5: funny apple logic */
static PyObject *
posix_initgroups(PyObject *self, PyObject *args)
{
    PyObject *oname;
    const char *username;
    int res;
#ifdef __APPLE__
    int gid;
#else
    gid_t gid;
#endif

#ifdef __APPLE__
    if (!PyArg_ParseTuple(args, "O&i:initgroups",
                          PyUnicode_FSConverter, &oname,
                          &gid))
#else
    if (!PyArg_ParseTuple(args, "O&O&:initgroups",
                          PyUnicode_FSConverter, &oname,
                          _Py_Gid_Converter, &gid))
#endif
        return NULL;
    username = PyBytes_AS_STRING(oname);

    res = initgroups(username, gid);
    Py_DECREF(oname);
    if (res == -1)
        return PyErr_SetFromErrno(PyExc_OSError);

    Py_INCREF(Py_None);
    return Py_None;
}
#endif /* HAVE_INITGROUPS */


#ifdef HAVE_GETPGID
/*[clinic input]
os.getpgid

    pid: pid_t

Call the system call getpgid(), and return the result.
[clinic start generated code]*/

static PyObject *
os_getpgid_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=1db95a97be205d18 input=39d710ae3baaf1c7]*/
{
    pid_t pgid = getpgid(pid);
    if (pgid < 0)
        return posix_error();
    return PyLong_FromPid(pgid);
}
#endif /* HAVE_GETPGID */


#ifdef HAVE_GETPGRP
/*[clinic input]
os.getpgrp

Return the current process group id.
[clinic start generated code]*/

static PyObject *
os_getpgrp_impl(PyObject *module)
/*[clinic end generated code: output=c4fc381e51103cf3 input=6846fb2bb9a3705e]*/
{
#ifdef GETPGRP_HAVE_ARG
    return PyLong_FromPid(getpgrp(0));
#else /* GETPGRP_HAVE_ARG */
    return PyLong_FromPid(getpgrp());
#endif /* GETPGRP_HAVE_ARG */
}
#endif /* HAVE_GETPGRP */


#ifdef HAVE_SETPGRP
/*[clinic input]
os.setpgrp

Make the current process the leader of its process group.
[clinic start generated code]*/

static PyObject *
os_setpgrp_impl(PyObject *module)
/*[clinic end generated code: output=2554735b0a60f0a0 input=1f0619fcb5731e7e]*/
{
#ifdef SETPGRP_HAVE_ARG
    if (setpgrp(0, 0) < 0)
#else /* SETPGRP_HAVE_ARG */
    if (setpgrp() < 0)
#endif /* SETPGRP_HAVE_ARG */
        return posix_error();
    Py_INCREF(Py_None);
    return Py_None;
}
#endif /* HAVE_SETPGRP */

#ifdef HAVE_GETPPID

#ifdef MS_WINDOWS
#include <tlhelp32.h>

static PyObject*
win32_getppid()
{
    HANDLE snapshot;
    pid_t mypid;
    PyObject* result = NULL;
    BOOL have_record;
    PROCESSENTRY32 pe;

    mypid = getpid(); /* This function never fails */

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return PyErr_SetFromWindowsErr(GetLastError());

    pe.dwSize = sizeof(pe);
    have_record = Process32First(snapshot, &pe);
    while (have_record) {
        if (mypid == (pid_t)pe.th32ProcessID) {
            /* We could cache the ulong value in a static variable. */
            result = PyLong_FromPid((pid_t)pe.th32ParentProcessID);
            break;
        }

        have_record = Process32Next(snapshot, &pe);
    }

    /* If our loop exits and our pid was not found (result will be NULL)
     * then GetLastError will return ERROR_NO_MORE_FILES. This is an
     * error anyway, so let's raise it. */
    if (!result)
        result = PyErr_SetFromWindowsErr(GetLastError());

    CloseHandle(snapshot);

    return result;
}
#endif /*MS_WINDOWS*/


/*[clinic input]
os.getppid

Return the parent's process id.

If the parent process has already exited, Windows machines will still
return its id; others systems will return the id of the 'init' process (1).
[clinic start generated code]*/

static PyObject *
os_getppid_impl(PyObject *module)
/*[clinic end generated code: output=43b2a946a8c603b4 input=e637cb87539c030e]*/
{
#ifdef MS_WINDOWS
    return win32_getppid();
#else
    return PyLong_FromPid(getppid());
#endif
}
#endif /* HAVE_GETPPID */


#ifdef HAVE_GETLOGIN
/*[clinic input]
os.getlogin

Return the actual login name.
[clinic start generated code]*/

static PyObject *
os_getlogin_impl(PyObject *module)
/*[clinic end generated code: output=a32e66a7e5715dac input=2a21ab1e917163df]*/
{
    PyObject *result = NULL;
#ifdef MS_WINDOWS
    wchar_t user_name[UNLEN + 1];
    DWORD num_chars = Py_ARRAY_LENGTH(user_name);

    if (GetUserNameW(user_name, &num_chars)) {
        /* num_chars is the number of unicode chars plus null terminator */
        result = PyUnicode_FromWideChar(user_name, num_chars - 1);
    }
    else
        result = PyErr_SetFromWindowsErr(GetLastError());
#else
    char *name;
    int old_errno = errno;

    errno = 0;
    name = getlogin();
    if (name == NULL) {
        if (errno)
            posix_error();
        else
            PyErr_SetString(PyExc_OSError, "unable to determine login name");
    }
    else
        result = PyUnicode_DecodeFSDefault(name);
    errno = old_errno;
#endif
    return result;
}
#endif /* HAVE_GETLOGIN */


#ifdef HAVE_GETUID
/*[clinic input]
os.getuid

Return the current process's user id.
[clinic start generated code]*/

static PyObject *
os_getuid_impl(PyObject *module)
/*[clinic end generated code: output=415c0b401ebed11a input=b53c8b35f110a516]*/
{
    return _PyLong_FromUid(getuid());
}
#endif /* HAVE_GETUID */


#ifdef MS_WINDOWS
#define HAVE_KILL
#endif /* MS_WINDOWS */

#ifdef HAVE_KILL
/*[clinic input]
os.kill

    pid: pid_t
    signal: Py_ssize_t
    /

Kill a process with a signal.
[clinic start generated code]*/

static PyObject *
os_kill_impl(PyObject *module, pid_t pid, Py_ssize_t signal)
/*[clinic end generated code: output=8e346a6701c88568 input=61a36b86ca275ab9]*/
#ifndef MS_WINDOWS
{
    if (kill(pid, (int)signal) == -1)
        return posix_error();
    Py_RETURN_NONE;
}
#else /* !MS_WINDOWS */
{
    PyObject *result;
    DWORD sig = (DWORD)signal;
    DWORD err;
    HANDLE handle;

    /* Console processes which share a common console can be sent CTRL+C or
       CTRL+BREAK events, provided they handle said events. */
    if (sig == CTRL_C_EVENT || sig == CTRL_BREAK_EVENT) {
        if (GenerateConsoleCtrlEvent(sig, (DWORD)pid) == 0) {
            err = GetLastError();
            PyErr_SetFromWindowsErr(err);
        }
        else
            Py_RETURN_NONE;
    }

    /* If the signal is outside of what GenerateConsoleCtrlEvent can use,
       attempt to open and terminate the process. */
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
    if (handle == NULL) {
        err = GetLastError();
        return PyErr_SetFromWindowsErr(err);
    }

    if (TerminateProcess(handle, sig) == 0) {
        err = GetLastError();
        result = PyErr_SetFromWindowsErr(err);
    } else {
        Py_INCREF(Py_None);
        result = Py_None;
    }

    CloseHandle(handle);
    return result;
}
#endif /* !MS_WINDOWS */
#endif /* HAVE_KILL */


#ifdef HAVE_KILLPG
/*[clinic input]
os.killpg

    pgid: pid_t
    signal: int
    /

Kill a process group with a signal.
[clinic start generated code]*/

static PyObject *
os_killpg_impl(PyObject *module, pid_t pgid, int signal)
/*[clinic end generated code: output=6dbcd2f1fdf5fdba input=38b5449eb8faec19]*/
{
    /* XXX some man pages make the `pgid` parameter an int, others
       a pid_t. Since getpgrp() returns a pid_t, we assume killpg should
       take the same type. Moreover, pid_t is always at least as wide as
       int (else compilation of this module fails), which is safe. */
    if (killpg(pgid, signal) == -1)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_KILLPG */


#ifdef HAVE_PLOCK
#ifdef HAVE_SYS_LOCK_H
#include <sys/lock.h>
#endif

/*[clinic input]
os.plock
    op: int
    /

Lock program segments into memory.");
[clinic start generated code]*/

static PyObject *
os_plock_impl(PyObject *module, int op)
/*[clinic end generated code: output=81424167033b168e input=e6e5e348e1525f60]*/
{
    if (plock(op) == -1)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_PLOCK */


#ifdef HAVE_SETUID
/*[clinic input]
os.setuid

    uid: uid_t
    /

Set the current process's user id.
[clinic start generated code]*/

static PyObject *
os_setuid_impl(PyObject *module, uid_t uid)
/*[clinic end generated code: output=a0a41fd0d1ec555f input=c921a3285aa22256]*/
{
    if (setuid(uid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETUID */


#ifdef HAVE_SETEUID
/*[clinic input]
os.seteuid

    euid: uid_t
    /

Set the current process's effective user id.
[clinic start generated code]*/

static PyObject *
os_seteuid_impl(PyObject *module, uid_t euid)
/*[clinic end generated code: output=102e3ad98361519a input=ba93d927e4781aa9]*/
{
    if (seteuid(euid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETEUID */


#ifdef HAVE_SETEGID
/*[clinic input]
os.setegid

    egid: gid_t
    /

Set the current process's effective group id.
[clinic start generated code]*/

static PyObject *
os_setegid_impl(PyObject *module, gid_t egid)
/*[clinic end generated code: output=4e4b825a6a10258d input=4080526d0ccd6ce3]*/
{
    if (setegid(egid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETEGID */


#ifdef HAVE_SETREUID
/*[clinic input]
os.setreuid

    ruid: uid_t
    euid: uid_t
    /

Set the current process's real and effective user ids.
[clinic start generated code]*/

static PyObject *
os_setreuid_impl(PyObject *module, uid_t ruid, uid_t euid)
/*[clinic end generated code: output=62d991210006530a input=0ca8978de663880c]*/
{
    if (setreuid(ruid, euid) < 0) {
        return posix_error();
    } else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
#endif /* HAVE_SETREUID */


#ifdef HAVE_SETREGID
/*[clinic input]
os.setregid

    rgid: gid_t
    egid: gid_t
    /

Set the current process's real and effective group ids.
[clinic start generated code]*/

static PyObject *
os_setregid_impl(PyObject *module, gid_t rgid, gid_t egid)
/*[clinic end generated code: output=aa803835cf5342f3 input=c59499f72846db78]*/
{
    if (setregid(rgid, egid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETREGID */


#ifdef HAVE_SETGID
/*[clinic input]
os.setgid
    gid: gid_t
    /

Set the current process's group id.
[clinic start generated code]*/

static PyObject *
os_setgid_impl(PyObject *module, gid_t gid)
/*[clinic end generated code: output=bdccd7403f6ad8c3 input=27d30c4059045dc6]*/
{
    if (setgid(gid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETGID */


#ifdef HAVE_SETGROUPS
/*[clinic input]
os.setgroups

    groups: object
    /

Set the groups of the current process to list.
[clinic start generated code]*/

static PyObject *
os_setgroups(PyObject *module, PyObject *groups)
/*[clinic end generated code: output=3fcb32aad58c5ecd input=fa742ca3daf85a7e]*/
{
    int i, len;
    gid_t grouplist[MAX_GROUPS];

    if (!PySequence_Check(groups)) {
        PyErr_SetString(PyExc_TypeError, "setgroups argument must be a sequence");
        return NULL;
    }
    len = PySequence_Size(groups);
    if (len > MAX_GROUPS) {
        PyErr_SetString(PyExc_ValueError, "too many groups");
        return NULL;
    }
    for(i = 0; i < len; i++) {
        PyObject *elem;
        elem = PySequence_GetItem(groups, i);
        if (!elem)
            return NULL;
        if (!PyLong_Check(elem)) {
            PyErr_SetString(PyExc_TypeError,
                            "groups must be integers");
            Py_DECREF(elem);
            return NULL;
        } else {
            if (!_Py_Gid_Converter(elem, &grouplist[i])) {
                Py_DECREF(elem);
                return NULL;
            }
        }
        Py_DECREF(elem);
    }

    if (setgroups(len, grouplist) < 0)
        return posix_error();
    Py_INCREF(Py_None);
    return Py_None;
}
#endif /* HAVE_SETGROUPS */

#if defined(HAVE_WAIT3) || defined(HAVE_WAIT4)
static PyObject *
wait_helper(pid_t pid, int status, struct rusage *ru)
{
    PyObject *result;
    static PyObject *struct_rusage;
    _Py_IDENTIFIER(struct_rusage);

    if (pid == -1)
        return posix_error();

    if (struct_rusage == NULL) {
        PyObject *m = PyImport_ImportModuleNoBlock("resource");
        if (m == NULL)
            return NULL;
        struct_rusage = _PyObject_GetAttrId(m, &PyId_struct_rusage);
        Py_DECREF(m);
        if (struct_rusage == NULL)
            return NULL;
    }

    /* XXX(nnorwitz): Copied (w/mods) from resource.c, there should be only one. */
    result = PyStructSequence_New((PyTypeObject*) struct_rusage);
    if (!result)
        return NULL;

#ifndef doubletime
#define doubletime(TV) ((double)(TV).tv_sec + (TV).tv_usec * 0.000001)
#endif

    PyStructSequence_SET_ITEM(result, 0,
                              PyFloat_FromDouble(doubletime(ru->ru_utime)));
    PyStructSequence_SET_ITEM(result, 1,
                              PyFloat_FromDouble(doubletime(ru->ru_stime)));
#define SET_INT(result, index, value)\
        PyStructSequence_SET_ITEM(result, index, PyLong_FromLong(value))
    SET_INT(result, 2, ru->ru_maxrss);
    SET_INT(result, 3, ru->ru_ixrss);
    SET_INT(result, 4, ru->ru_idrss);
    SET_INT(result, 5, ru->ru_isrss);
    SET_INT(result, 6, ru->ru_minflt);
    SET_INT(result, 7, ru->ru_majflt);
    SET_INT(result, 8, ru->ru_nswap);
    SET_INT(result, 9, ru->ru_inblock);
    SET_INT(result, 10, ru->ru_oublock);
    SET_INT(result, 11, ru->ru_msgsnd);
    SET_INT(result, 12, ru->ru_msgrcv);
    SET_INT(result, 13, ru->ru_nsignals);
    SET_INT(result, 14, ru->ru_nvcsw);
    SET_INT(result, 15, ru->ru_nivcsw);
#undef SET_INT

    if (PyErr_Occurred()) {
        Py_DECREF(result);
        return NULL;
    }

    return Py_BuildValue("NiN", PyLong_FromPid(pid), status, result);
}
#endif /* HAVE_WAIT3 || HAVE_WAIT4 */


#ifdef HAVE_WAIT3
/*[clinic input]
os.wait3

    options: int
Wait for completion of a child process.

Returns a tuple of information about the child process:
  (pid, status, rusage)
[clinic start generated code]*/

static PyObject *
os_wait3_impl(PyObject *module, int options)
/*[clinic end generated code: output=92c3224e6f28217a input=8ac4c56956b61710]*/
{
    pid_t pid;
    struct rusage ru;
    int async_err = 0;
    WAIT_TYPE status;
    WAIT_STATUS_INT(status) = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        pid = wait3(&status, options, &ru);
        Py_END_ALLOW_THREADS
    } while (pid < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (pid < 0)
        return (!async_err) ? posix_error() : NULL;

    return wait_helper(pid, WAIT_STATUS_INT(status), &ru);
}
#endif /* HAVE_WAIT3 */


#ifdef HAVE_WAIT4
/*[clinic input]

os.wait4

    pid: pid_t
    options: int

Wait for completion of a specific child process.

Returns a tuple of information about the child process:
  (pid, status, rusage)
[clinic start generated code]*/

static PyObject *
os_wait4_impl(PyObject *module, pid_t pid, int options)
/*[clinic end generated code: output=66195aa507b35f70 input=d11deed0750600ba]*/
{
    pid_t res;
    struct rusage ru;
    int async_err = 0;
    WAIT_TYPE status;
    WAIT_STATUS_INT(status) = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = wait4(pid, &status, options, &ru);
        Py_END_ALLOW_THREADS
    } while (res < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res < 0)
        return (!async_err) ? posix_error() : NULL;

    return wait_helper(res, WAIT_STATUS_INT(status), &ru);
}
#endif /* HAVE_WAIT4 */


#if defined(HAVE_WAITID) && !defined(__APPLE__)
/*[clinic input]
os.waitid

    idtype: idtype_t
        Must be one of be P_PID, P_PGID or P_ALL.
    id: id_t
        The id to wait on.
    options: int
        Constructed from the ORing of one or more of WEXITED, WSTOPPED
        or WCONTINUED and additionally may be ORed with WNOHANG or WNOWAIT.
    /

Returns the result of waiting for a process or processes.

Returns either waitid_result or None if WNOHANG is specified and there are
no children in a waitable state.
[clinic start generated code]*/

static PyObject *
os_waitid_impl(PyObject *module, idtype_t idtype, id_t id, int options)
/*[clinic end generated code: output=5d2e1c0bde61f4d8 input=d8e7f76e052b7920]*/
{
    PyObject *result;
    int res;
    int async_err = 0;
    siginfo_t si;
    si.si_pid = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = waitid(idtype, id, &si, options);
        Py_END_ALLOW_THREADS
    } while (res < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res < 0)
        return (!async_err) ? posix_error() : NULL;

    if (si.si_pid == 0)
        Py_RETURN_NONE;

    result = PyStructSequence_New(&WaitidResultType);
    if (!result)
        return NULL;

    PyStructSequence_SET_ITEM(result, 0, PyLong_FromPid(si.si_pid));
    PyStructSequence_SET_ITEM(result, 1, _PyLong_FromUid(si.si_uid));
    PyStructSequence_SET_ITEM(result, 2, PyLong_FromLong((long)(si.si_signo)));
    PyStructSequence_SET_ITEM(result, 3, PyLong_FromLong((long)(si.si_status)));
    PyStructSequence_SET_ITEM(result, 4, PyLong_FromLong((long)(si.si_code)));
    if (PyErr_Occurred()) {
        Py_DECREF(result);
        return NULL;
    }

    return result;
}
#endif /* defined(HAVE_WAITID) && !defined(__APPLE__) */


#if defined(HAVE_WAITPID)
/*[clinic input]
os.waitpid
    pid: pid_t
    options: int
    /

Wait for completion of a given child process.

Returns a tuple of information regarding the child process:
    (pid, status)

The options argument is ignored on Windows.
[clinic start generated code]*/

static PyObject *
os_waitpid_impl(PyObject *module, pid_t pid, int options)
/*[clinic end generated code: output=5c37c06887a20270 input=0bf1666b8758fda3]*/
{
    pid_t res;
    int async_err = 0;
    WAIT_TYPE status;
    WAIT_STATUS_INT(status) = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = waitpid(pid, &status, options);
        Py_END_ALLOW_THREADS
    } while (res < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res < 0)
        return (!async_err) ? posix_error() : NULL;

    return Py_BuildValue("Ni", PyLong_FromPid(res), WAIT_STATUS_INT(status));
}
#elif defined(HAVE_CWAIT)
/* MS C has a variant of waitpid() that's usable for most purposes. */
/*[clinic input]
os.waitpid
    pid: intptr_t
    options: int
    /

Wait for completion of a given process.

Returns a tuple of information regarding the process:
    (pid, status << 8)

The options argument is ignored on Windows.
[clinic start generated code]*/

static PyObject *
os_waitpid_impl(PyObject *module, intptr_t pid, int options)
/*[clinic end generated code: output=be836b221271d538 input=40f2440c515410f8]*/
{
    int status;
    intptr_t res;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = _cwait(&status, pid, options);
        Py_END_ALLOW_THREADS
    } while (res < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res < 0)
        return (!async_err) ? posix_error() : NULL;

    /* shift the status left a byte so this is more like the POSIX waitpid */
    return Py_BuildValue(_Py_PARSE_INTPTR "i", res, status << 8);
}
#endif


#ifdef HAVE_WAIT
/*[clinic input]
os.wait

Wait for completion of a child process.

Returns a tuple of information about the child process:
    (pid, status)
[clinic start generated code]*/

static PyObject *
os_wait_impl(PyObject *module)
/*[clinic end generated code: output=6bc419ac32fb364b input=03b0182d4a4700ce]*/
{
    pid_t pid;
    int async_err = 0;
    WAIT_TYPE status;
    WAIT_STATUS_INT(status) = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        pid = wait(&status);
        Py_END_ALLOW_THREADS
    } while (pid < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (pid < 0)
        return (!async_err) ? posix_error() : NULL;

    return Py_BuildValue("Ni", PyLong_FromPid(pid), WAIT_STATUS_INT(status));
}
#endif /* HAVE_WAIT */


#if defined(HAVE_READLINK) || defined(MS_WINDOWS)
PyDoc_STRVAR(readlink__doc__,
"readlink(path, *, dir_fd=None) -> path\n\n\
Return a string representing the path to which the symbolic link points.\n\
\n\
If dir_fd is not None, it should be a file descriptor open to a directory,\n\
  and path should be relative; path will then be relative to that directory.\n\
dir_fd may not be implemented on your platform.\n\
  If it is unavailable, using it will raise a NotImplementedError.");
#endif

#ifdef HAVE_READLINK

/* AC 3.5: merge win32 and not together */
static PyObject *
posix_readlink(PyObject *self, PyObject *args, PyObject *kwargs)
{
    path_t path;
    int dir_fd = DEFAULT_DIR_FD;
    char buffer[MAXPATHLEN];
    ssize_t length;
    PyObject *return_value = NULL;
    static char *keywords[] = {"path", "dir_fd", NULL};

    memset(&path, 0, sizeof(path));
    path.function_name = "readlink";
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|$O&:readlink", keywords,
                          path_converter, &path,
                          READLINKAT_DIR_FD_CONVERTER, &dir_fd))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_READLINKAT
    if (dir_fd != DEFAULT_DIR_FD)
        length = readlinkat(dir_fd, path.narrow, buffer, sizeof(buffer));
    else
#endif
        length = readlink(path.narrow, buffer, sizeof(buffer));
    Py_END_ALLOW_THREADS

    if (length < 0) {
        return_value = path_error(&path);
        goto exit;
    }

    if (PyUnicode_Check(path.object))
        return_value = PyUnicode_DecodeFSDefaultAndSize(buffer, length);
    else
        return_value = PyBytes_FromStringAndSize(buffer, length);
exit:
    path_cleanup(&path);
    return return_value;
}

#endif /* HAVE_READLINK */

#if !defined(HAVE_READLINK) && defined(MS_WINDOWS)

static PyObject *
win_readlink(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const wchar_t *path;
    DWORD n_bytes_returned;
    DWORD io_result;
    PyObject *po, *result;
    int dir_fd;
    HANDLE reparse_point_handle;

    char target_buffer[_Py_MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    _Py_REPARSE_DATA_BUFFER *rdb = (_Py_REPARSE_DATA_BUFFER *)target_buffer;
    const wchar_t *print_name;

    static char *keywords[] = {"path", "dir_fd", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|$O&:readlink", keywords,
                          &po,
                          dir_fd_unavailable, &dir_fd
                          ))
        return NULL;

    path = PyUnicode_AsUnicode(po);
    if (path == NULL)
        return NULL;

    /* First get a handle to the reparse point */
    Py_BEGIN_ALLOW_THREADS
    reparse_point_handle = CreateFileW(
        path,
        0,
        0,
        0,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS,
        0);
    Py_END_ALLOW_THREADS

    if (reparse_point_handle==INVALID_HANDLE_VALUE)
        return win32_error_object("readlink", po);

    Py_BEGIN_ALLOW_THREADS
    /* New call DeviceIoControl to read the reparse point */
    io_result = DeviceIoControl(
        reparse_point_handle,
        FSCTL_GET_REPARSE_POINT,
        0, 0, /* in buffer */
        target_buffer, sizeof(target_buffer),
        &n_bytes_returned,
        0 /* we're not using OVERLAPPED_IO */
        );
    CloseHandle(reparse_point_handle);
    Py_END_ALLOW_THREADS

    if (io_result==0)
        return win32_error_object("readlink", po);

    if (rdb->ReparseTag != IO_REPARSE_TAG_SYMLINK)
    {
        PyErr_SetString(PyExc_ValueError,
                "not a symbolic link");
        return NULL;
    }
    print_name = rdb->SymbolicLinkReparseBuffer.PathBuffer +
                 rdb->SymbolicLinkReparseBuffer.PrintNameOffset;

    result = PyUnicode_FromWideChar(print_name,
                    rdb->SymbolicLinkReparseBuffer.PrintNameLength/2);
    return result;
}

#endif /* !defined(HAVE_READLINK) && defined(MS_WINDOWS) */



#ifdef HAVE_SYMLINK

#if defined(MS_WINDOWS)

/* Grab CreateSymbolicLinkW dynamically from kernel32 */
static DWORD (CALLBACK *Py_CreateSymbolicLinkW)(LPCWSTR, LPCWSTR, DWORD) = NULL;

static int
check_CreateSymbolicLink(void)
{
    HINSTANCE hKernel32;
    /* only recheck */
    if (Py_CreateSymbolicLinkW)
        return 1;
    hKernel32 = GetModuleHandleW(L"KERNEL32");
    *(FARPROC*)&Py_CreateSymbolicLinkW = GetProcAddress(hKernel32,
                                                        "CreateSymbolicLinkW");
    return Py_CreateSymbolicLinkW != NULL;
}

/* Remove the last portion of the path */
static void
_dirnameW(WCHAR *path)
{
    WCHAR *ptr;

    /* walk the path from the end until a backslash is encountered */
    for(ptr = path + wcslen(path); ptr != path; ptr--) {
        if (*ptr == L'\\' || *ptr == L'/')
            break;
    }
    *ptr = 0;
}

/* Is this path absolute? */
static int
_is_absW(const WCHAR *path)
{
    return path[0] == L'\\' || path[0] == L'/' || path[1] == L':';

}

/* join root and rest with a backslash */
static void
_joinW(WCHAR *dest_path, const WCHAR *root, const WCHAR *rest)
{
    size_t root_len;

    if (_is_absW(rest)) {
        wcscpy(dest_path, rest);
        return;
    }

    root_len = wcslen(root);

    wcscpy(dest_path, root);
    if(root_len) {
        dest_path[root_len] = L'\\';
        root_len++;
    }
    wcscpy(dest_path+root_len, rest);
}

/* Return True if the path at src relative to dest is a directory */
static int
_check_dirW(LPCWSTR src, LPCWSTR dest)
{
    WIN32_FILE_ATTRIBUTE_DATA src_info;
    WCHAR dest_parent[MAX_PATH];
    WCHAR src_resolved[MAX_PATH] = L"";

    /* dest_parent = os.path.dirname(dest) */
    wcscpy(dest_parent, dest);
    _dirnameW(dest_parent);
    /* src_resolved = os.path.join(dest_parent, src) */
    _joinW(src_resolved, dest_parent, src);
    return (
        GetFileAttributesExW(src_resolved, GetFileExInfoStandard, &src_info)
        && src_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
    );
}
#endif


/*[clinic input]
os.symlink
    src: path_t
    dst: path_t
    target_is_directory: bool = False
    *
    dir_fd: dir_fd(requires='symlinkat')=None

# "symlink(src, dst, target_is_directory=False, *, dir_fd=None)\n\n\

Create a symbolic link pointing to src named dst.

target_is_directory is required on Windows if the target is to be
  interpreted as a directory.  (On Windows, symlink requires
  Windows 6.0 or greater, and raises a NotImplementedError otherwise.)
  target_is_directory is ignored on non-Windows platforms.

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.

[clinic start generated code]*/

static PyObject *
os_symlink_impl(PyObject *module, path_t *src, path_t *dst,
                int target_is_directory, int dir_fd)
/*[clinic end generated code: output=08ca9f3f3cf960f6 input=e820ec4472547bc3]*/
{
#ifdef MS_WINDOWS
    DWORD result;
#else
    int result;
#endif

#ifdef MS_WINDOWS
    if (!check_CreateSymbolicLink()) {
        PyErr_SetString(PyExc_NotImplementedError,
            "CreateSymbolicLink functions not found");
        return NULL;
        }
    if (!win32_can_symlink) {
        PyErr_SetString(PyExc_OSError, "symbolic link privilege not held");
        return NULL;
        }
#endif

    if ((src->narrow && dst->wide) || (src->wide && dst->narrow)) {
        PyErr_SetString(PyExc_ValueError,
            "symlink: src and dst must be the same type");
        return NULL;
    }

#ifdef MS_WINDOWS

    Py_BEGIN_ALLOW_THREADS
    /* if src is a directory, ensure target_is_directory==1 */
    target_is_directory |= _check_dirW(src->wide, dst->wide);
    result = Py_CreateSymbolicLinkW(dst->wide, src->wide,
                                    target_is_directory);
    Py_END_ALLOW_THREADS

    if (!result)
        return path_error2(src, dst);

#else

    Py_BEGIN_ALLOW_THREADS
#if HAVE_SYMLINKAT
    if (dir_fd != DEFAULT_DIR_FD)
        result = symlinkat(src->narrow, dir_fd, dst->narrow);
    else
#endif
        result = symlink(src->narrow, dst->narrow);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error2(src, dst);
#endif

    Py_RETURN_NONE;
}
#endif /* HAVE_SYMLINK */




static PyStructSequence_Field times_result_fields[] = {
    {"user",    "user time"},
    {"system",   "system time"},
    {"children_user",    "user time of children"},
    {"children_system",    "system time of children"},
    {"elapsed",    "elapsed time since an arbitrary point in the past"},
    {NULL}
};

PyDoc_STRVAR(times_result__doc__,
"times_result: Result from os.times().\n\n\
This object may be accessed either as a tuple of\n\
  (user, system, children_user, children_system, elapsed),\n\
or via the attributes user, system, children_user, children_system,\n\
and elapsed.\n\
\n\
See os.times for more information.");

static PyStructSequence_Desc times_result_desc = {
    "times_result", /* name */
    times_result__doc__, /* doc */
    times_result_fields,
    5
};

static PyTypeObject TimesResultType;

#ifdef MS_WINDOWS
#define HAVE_TIMES  /* mandatory, for the method table */
#endif

#ifdef HAVE_TIMES

static PyObject *
build_times_result(double user, double system,
    double children_user, double children_system,
    double elapsed)
{
    PyObject *value = PyStructSequence_New(&TimesResultType);
    if (value == NULL)
        return NULL;

#define SET(i, field) \
    { \
    PyObject *o = PyFloat_FromDouble(field); \
    if (!o) { \
        Py_DECREF(value); \
        return NULL; \
    } \
    PyStructSequence_SET_ITEM(value, i, o); \
    } \

    SET(0, user);
    SET(1, system);
    SET(2, children_user);
    SET(3, children_system);
    SET(4, elapsed);

#undef SET

    return value;
}


#ifndef MS_WINDOWS
#define NEED_TICKS_PER_SECOND
static long ticks_per_second = -1;
#endif /* MS_WINDOWS */

/*[clinic input]
os.times

Return a collection containing process timing information.

The object returned behaves like a named tuple with these fields:
  (utime, stime, cutime, cstime, elapsed_time)
All fields are floating point numbers.
[clinic start generated code]*/

static PyObject *
os_times_impl(PyObject *module)
/*[clinic end generated code: output=35f640503557d32a input=2bf9df3d6ab2e48b]*/
#ifdef MS_WINDOWS
{
    FILETIME create, exit, kernel, user;
    HANDLE hProc;
    hProc = GetCurrentProcess();
    GetProcessTimes(hProc, &create, &exit, &kernel, &user);
    /* The fields of a FILETIME structure are the hi and lo part
       of a 64-bit value expressed in 100 nanosecond units.
       1e7 is one second in such units; 1e-7 the inverse.
       429.4967296 is 2**32 / 1e7 or 2**32 * 1e-7.
    */
    return build_times_result(
        (double)(user.dwHighDateTime*429.4967296 +
                 user.dwLowDateTime*1e-7),
        (double)(kernel.dwHighDateTime*429.4967296 +
                 kernel.dwLowDateTime*1e-7),
        (double)0,
        (double)0,
        (double)0);
}
#else /* MS_WINDOWS */
{


    struct tms t;
    clock_t c;
    errno = 0;
    c = times(&t);
    if (c == (clock_t) -1)
        return posix_error();
    return build_times_result(
                         (double)t.tms_utime / ticks_per_second,
                         (double)t.tms_stime / ticks_per_second,
                         (double)t.tms_cutime / ticks_per_second,
                         (double)t.tms_cstime / ticks_per_second,
                         (double)c / ticks_per_second);
}
#endif /* MS_WINDOWS */
#endif /* HAVE_TIMES */


#ifdef HAVE_GETSID
/*[clinic input]
os.getsid

    pid: pid_t
    /

Call the system call getsid(pid) and return the result.
[clinic start generated code]*/

static PyObject *
os_getsid_impl(PyObject *module, pid_t pid)
/*[clinic end generated code: output=112deae56b306460 input=eeb2b923a30ce04e]*/
{
    int sid;
    sid = getsid(pid);
    if (sid < 0)
        return posix_error();
    return PyLong_FromLong((long)sid);
}
#endif /* HAVE_GETSID */


#ifdef HAVE_SETSID
/*[clinic input]
os.setsid

Call the system call setsid().
[clinic start generated code]*/

static PyObject *
os_setsid_impl(PyObject *module)
/*[clinic end generated code: output=e2ddedd517086d77 input=5fff45858e2f0776]*/
{
    if (setsid() < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETSID */


#ifdef HAVE_SETPGID
/*[clinic input]
os.setpgid

    pid: pid_t
    pgrp: pid_t
    /

Call the system call setpgid(pid, pgrp).
[clinic start generated code]*/

static PyObject *
os_setpgid_impl(PyObject *module, pid_t pid, pid_t pgrp)
/*[clinic end generated code: output=6461160319a43d6a input=fceb395eca572e1a]*/
{
    if (setpgid(pid, pgrp) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETPGID */


#ifdef HAVE_TCGETPGRP
/*[clinic input]
os.tcgetpgrp

    fd: int
    /

Return the process group associated with the terminal specified by fd.
[clinic start generated code]*/

static PyObject *
os_tcgetpgrp_impl(PyObject *module, int fd)
/*[clinic end generated code: output=f865e88be86c272b input=7f6c18eac10ada86]*/
{
    pid_t pgid = tcgetpgrp(fd);
    if (pgid < 0)
        return posix_error();
    return PyLong_FromPid(pgid);
}
#endif /* HAVE_TCGETPGRP */


#ifdef HAVE_TCSETPGRP
/*[clinic input]
os.tcsetpgrp

    fd: int
    pgid: pid_t
    /

Set the process group associated with the terminal specified by fd.
[clinic start generated code]*/

static PyObject *
os_tcsetpgrp_impl(PyObject *module, int fd, pid_t pgid)
/*[clinic end generated code: output=f1821a381b9daa39 input=5bdc997c6a619020]*/
{
    if (tcsetpgrp(fd, pgid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_TCSETPGRP */

/* Functions acting on file descriptors */

#ifdef O_CLOEXEC
extern int _Py_open_cloexec_works;
#endif


/*[clinic input]
os.open -> int
    path: path_t
    flags: int
    mode: int = 0o777
    *
    dir_fd: dir_fd(requires='openat') = None

# "open(path, flags, mode=0o777, *, dir_fd=None)\n\n\

Open a file for low level IO.  Returns a file descriptor (integer).

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.
[clinic start generated code]*/

static int
os_open_impl(PyObject *module, path_t *path, int flags, int mode, int dir_fd)
/*[clinic end generated code: output=abc7227888c8bc73 input=ad8623b29acd2934]*/
{
    int fd;
    int async_err = 0;

#ifdef O_CLOEXEC
    int *atomic_flag_works = &_Py_open_cloexec_works;
#elif !defined(MS_WINDOWS)
    int *atomic_flag_works = NULL;
#endif

#ifdef MS_WINDOWS
    flags |= O_NOINHERIT;
#elif defined(O_CLOEXEC)
    flags |= O_CLOEXEC;
#endif

    _Py_BEGIN_SUPPRESS_IPH
    do {
        Py_BEGIN_ALLOW_THREADS
#ifdef MS_WINDOWS
        fd = _wopen(path->wide, flags, mode);
#else
#ifdef HAVE_OPENAT
        if (dir_fd != DEFAULT_DIR_FD)
            fd = openat(dir_fd, path->narrow, flags, mode);
        else
#endif /* HAVE_OPENAT */
            fd = open(path->narrow, flags, mode);
#endif /* !MS_WINDOWS */
        Py_END_ALLOW_THREADS
    } while (fd < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    _Py_END_SUPPRESS_IPH

    if (fd < 0) {
        if (!async_err)
            PyErr_SetFromErrnoWithFilenameObject(PyExc_OSError, path->object);
        return -1;
    }

#ifndef MS_WINDOWS
    if (_Py_set_inheritable(fd, 0, atomic_flag_works) < 0) {
        close(fd);
        return -1;
    }
#endif

    return fd;
}


/*[clinic input]
os.close

    fd: int

Close a file descriptor.
[clinic start generated code]*/

static PyObject *
os_close_impl(PyObject *module, int fd)
/*[clinic end generated code: output=2fe4e93602822c14 input=2bc42451ca5c3223]*/
{
    int res;
    /* We do not want to retry upon EINTR: see http://lwn.net/Articles/576478/
     * and http://linux.derkeiler.com/Mailing-Lists/Kernel/2005-09/3000.html
     * for more details.
     */
    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
    res = close(fd);
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS
    if (res < 0)
        return posix_error();
    Py_RETURN_NONE;
}


/*[clinic input]
os.closerange

    fd_low: int
    fd_high: int
    /

Closes all file descriptors in [fd_low, fd_high), ignoring errors.
[clinic start generated code]*/

static PyObject *
os_closerange_impl(PyObject *module, int fd_low, int fd_high)
/*[clinic end generated code: output=0ce5c20fcda681c2 input=5855a3d053ebd4ec]*/
{
    int i;
    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
    for (i = Py_MAX(fd_low, 0); i < fd_high; i++)
        close(i);
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}


/*[clinic input]
os.dup -> int

    fd: int
    /

Return a duplicate of a file descriptor.
[clinic start generated code]*/

static int
os_dup_impl(PyObject *module, int fd)
/*[clinic end generated code: output=486f4860636b2a9f input=6f10f7ea97f7852a]*/
{
    return _Py_dup(fd);
}


/*[clinic input]
os.dup2
    fd: int
    fd2: int
    inheritable: bool=True

Duplicate file descriptor.
[clinic start generated code]*/

static PyObject *
os_dup2_impl(PyObject *module, int fd, int fd2, int inheritable)
/*[clinic end generated code: output=db832a2d872ccc5f input=76e96f511be0352f]*/
{
    int res;
#if defined(HAVE_DUP3) && \
    !(defined(HAVE_FCNTL_H) && defined(F_DUP2FD_CLOEXEC))
    /* dup3() is available on Linux 2.6.27+ and glibc 2.9 */
    int dup3_works = -1;
#endif

    if (fd < 0 || fd2 < 0)
        return posix_error();

    /* dup2() can fail with EINTR if the target FD is already open, because it
     * then has to be closed. See os_close_impl() for why we don't handle EINTR
     * upon close(), and therefore below.
     */
#ifdef MS_WINDOWS
    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
    res = dup2(fd, fd2);
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS
    if (res < 0)
        return posix_error();

    /* Character files like console cannot be make non-inheritable */
    if (!inheritable && _Py_set_inheritable(fd2, 0, NULL) < 0) {
        close(fd2);
        return NULL;
    }

#elif defined(HAVE_FCNTL_H) && defined(F_DUP2FD_CLOEXEC)
    Py_BEGIN_ALLOW_THREADS
    if (!inheritable)
        res = fcntl(fd, F_DUP2FD_CLOEXEC, fd2);
    else
        res = dup2(fd, fd2);
    Py_END_ALLOW_THREADS
    if (res < 0)
        return posix_error();

#else

#ifdef HAVE_DUP3
    if (!inheritable && dup3_works != 0) {
        Py_BEGIN_ALLOW_THREADS
        res = dup3(fd, fd2, O_CLOEXEC);
        Py_END_ALLOW_THREADS
        if (res < 0) {
            if (dup3_works == -1)
                dup3_works = (errno != ENOSYS);
            if (dup3_works)
                return posix_error();
        }
    }

    if (inheritable || dup3_works == 0)
    {
#endif
        Py_BEGIN_ALLOW_THREADS
        res = dup2(fd, fd2);
        Py_END_ALLOW_THREADS
        if (res < 0)
            return posix_error();

        if (!inheritable && _Py_set_inheritable(fd2, 0, NULL) < 0) {
            close(fd2);
            return NULL;
        }
#ifdef HAVE_DUP3
    }
#endif

#endif

    Py_RETURN_NONE;
}


#ifdef HAVE_LOCKF
/*[clinic input]
os.lockf

    fd: int
        An open file descriptor.
    command: int
        One of F_LOCK, F_TLOCK, F_ULOCK or F_TEST.
    length: Py_off_t
        The number of bytes to lock, starting at the current position.
    /

Apply, test or remove a POSIX lock on an open file descriptor.

[clinic start generated code]*/

static PyObject *
os_lockf_impl(PyObject *module, int fd, int command, Py_off_t length)
/*[clinic end generated code: output=af7051f3e7c29651 input=65da41d2106e9b79]*/
{
    int res;

    Py_BEGIN_ALLOW_THREADS
    res = lockf(fd, command, length);
    Py_END_ALLOW_THREADS

    if (res < 0)
        return posix_error();

    Py_RETURN_NONE;
}
#endif /* HAVE_LOCKF */


/*[clinic input]
os.lseek -> Py_off_t

    fd: int
    position: Py_off_t
    how: int
    /

Set the position of a file descriptor.  Return the new position.

Return the new cursor position in number of bytes
relative to the beginning of the file.
[clinic start generated code]*/

static Py_off_t
os_lseek_impl(PyObject *module, int fd, Py_off_t position, int how)
/*[clinic end generated code: output=971e1efb6b30bd2f input=902654ad3f96a6d3]*/
{
    Py_off_t result;

#ifdef SEEK_SET
    /* Turn 0, 1, 2 into SEEK_{SET,CUR,END} */
    switch (how) {
        case 0: how = SEEK_SET; break;
        case 1: how = SEEK_CUR; break;
        case 2: how = SEEK_END; break;
    }
#endif /* SEEK_END */

    if (PyErr_Occurred())
        return -1;

    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
#ifdef MS_WINDOWS
    result = _lseeki64(fd, position, how);
#else
    result = lseek(fd, position, how);
#endif
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS
    if (result < 0)
        posix_error();

    return result;
}


/*[clinic input]
os.read
    fd: int
    length: Py_ssize_t
    /

Read from a file descriptor.  Returns a bytes object.
[clinic start generated code]*/

static PyObject *
os_read_impl(PyObject *module, int fd, Py_ssize_t length)
/*[clinic end generated code: output=dafbe9a5cddb987b input=1df2eaa27c0bf1d3]*/
{
    Py_ssize_t n;
    PyObject *buffer;

    if (length < 0) {
        errno = EINVAL;
        return posix_error();
    }

#ifdef MS_WINDOWS
    /* On Windows, the count parameter of read() is an int */
    if (length > INT_MAX)
        length = INT_MAX;
#endif

    buffer = PyBytes_FromStringAndSize((char *)NULL, length);
    if (buffer == NULL)
        return NULL;

    n = _Py_read(fd, PyBytes_AS_STRING(buffer), length);
    if (n == -1) {
        Py_DECREF(buffer);
        return NULL;
    }

    if (n != length)
        _PyBytes_Resize(&buffer, n);

    return buffer;
}

#if (defined(HAVE_SENDFILE) && (defined(__FreeBSD__) || defined(__DragonFly__) \
    || defined(__APPLE__))) || defined(HAVE_READV) || defined(HAVE_WRITEV)
static Py_ssize_t
iov_setup(struct iovec **iov, Py_buffer **buf, PyObject *seq, int cnt, int type)
{
    int i, j;
    Py_ssize_t blen, total = 0;

    *iov = PyMem_New(struct iovec, cnt);
    if (*iov == NULL) {
        PyErr_NoMemory();
        return -1;
    }

    *buf = PyMem_New(Py_buffer, cnt);
    if (*buf == NULL) {
        PyMem_Del(*iov);
        PyErr_NoMemory();
        return -1;
    }

    for (i = 0; i < cnt; i++) {
        PyObject *item = PySequence_GetItem(seq, i);
        if (item == NULL)
            goto fail;
        if (PyObject_GetBuffer(item, &(*buf)[i], type) == -1) {
            Py_DECREF(item);
            goto fail;
        }
        Py_DECREF(item);
        (*iov)[i].iov_base = (*buf)[i].buf;
        blen = (*buf)[i].len;
        (*iov)[i].iov_len = blen;
        total += blen;
    }
    return total;

fail:
    PyMem_Del(*iov);
    for (j = 0; j < i; j++) {
        PyBuffer_Release(&(*buf)[j]);
    }
    PyMem_Del(*buf);
    return -1;
}

static void
iov_cleanup(struct iovec *iov, Py_buffer *buf, int cnt)
{
    int i;
    PyMem_Del(iov);
    for (i = 0; i < cnt; i++) {
        PyBuffer_Release(&buf[i]);
    }
    PyMem_Del(buf);
}
#endif


#ifdef HAVE_READV
/*[clinic input]
os.readv -> Py_ssize_t

    fd: int
    buffers: object
    /

Read from a file descriptor fd into an iterable of buffers.

The buffers should be mutable buffers accepting bytes.
readv will transfer data into each buffer until it is full
and then move on to the next buffer in the sequence to hold
the rest of the data.

readv returns the total number of bytes read,
which may be less than the total capacity of all the buffers.
[clinic start generated code]*/

static Py_ssize_t
os_readv_impl(PyObject *module, int fd, PyObject *buffers)
/*[clinic end generated code: output=792da062d3fcebdb input=e679eb5dbfa0357d]*/
{
    int cnt;
    Py_ssize_t n;
    int async_err = 0;
    struct iovec *iov;
    Py_buffer *buf;

    if (!PySequence_Check(buffers)) {
        PyErr_SetString(PyExc_TypeError,
            "readv() arg 2 must be a sequence");
        return -1;
    }

    cnt = PySequence_Size(buffers);

    if (iov_setup(&iov, &buf, buffers, cnt, PyBUF_WRITABLE) < 0)
        return -1;

    do {
        Py_BEGIN_ALLOW_THREADS
        n = readv(fd, iov, cnt);
        Py_END_ALLOW_THREADS
    } while (n < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));

    iov_cleanup(iov, buf, cnt);
    if (n < 0) {
        if (!async_err)
            posix_error();
        return -1;
    }

    return n;
}
#endif /* HAVE_READV */


#ifdef HAVE_PREAD
/*[clinic input]
# TODO length should be size_t!  but Python doesn't support parsing size_t yet.
os.pread

    fd: int
    length: int
    offset: Py_off_t
    /

Read a number of bytes from a file descriptor starting at a particular offset.

Read length bytes from file descriptor fd, starting at offset bytes from
the beginning of the file.  The file offset remains unchanged.
[clinic start generated code]*/

static PyObject *
os_pread_impl(PyObject *module, int fd, int length, Py_off_t offset)
/*[clinic end generated code: output=435b29ee32b54a78 input=084948dcbaa35d4c]*/
{
    Py_ssize_t n;
    int async_err = 0;
    PyObject *buffer;

    if (length < 0) {
        errno = EINVAL;
        return posix_error();
    }
    buffer = PyBytes_FromStringAndSize((char *)NULL, length);
    if (buffer == NULL)
        return NULL;

    do {
        Py_BEGIN_ALLOW_THREADS
        _Py_BEGIN_SUPPRESS_IPH
        n = pread(fd, PyBytes_AS_STRING(buffer), length, offset);
        _Py_END_SUPPRESS_IPH
        Py_END_ALLOW_THREADS
    } while (n < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));

    if (n < 0) {
        Py_DECREF(buffer);
        return (!async_err) ? posix_error() : NULL;
    }
    if (n != length)
        _PyBytes_Resize(&buffer, n);
    return buffer;
}
#endif /* HAVE_PREAD */


/*[clinic input]
os.write -> Py_ssize_t

    fd: int
    data: Py_buffer
    /

Write a bytes object to a file descriptor.
[clinic start generated code]*/

static Py_ssize_t
os_write_impl(PyObject *module, int fd, Py_buffer *data)
/*[clinic end generated code: output=e4ef5bc904b58ef9 input=3207e28963234f3c]*/
{
    return _Py_write(fd, data->buf, data->len);
}

#ifdef HAVE_SENDFILE
PyDoc_STRVAR(posix_sendfile__doc__,
"sendfile(out, in, offset, count) -> byteswritten\n\
sendfile(out, in, offset, count[, headers][, trailers], flags=0)\n\
            -> byteswritten\n\
Copy count bytes from file descriptor in to file descriptor out.");

/* AC 3.5: don't bother converting, has optional group*/
static PyObject *
posix_sendfile(PyObject *self, PyObject *args, PyObject *kwdict)
{
    int in, out;
    Py_ssize_t ret;
    int async_err = 0;
    off_t offset;

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__APPLE__)
#ifndef __APPLE__
    Py_ssize_t len;
#endif
    PyObject *headers = NULL, *trailers = NULL;
    Py_buffer *hbuf, *tbuf;
    off_t sbytes;
    struct sf_hdtr sf;
    int flags = 0;
    /* Beware that "in" clashes with Python's own "in" operator keyword */
    static char *keywords[] = {"out", "in",
                                "offset", "count",
                                "headers", "trailers", "flags", NULL};

    sf.headers = NULL;
    sf.trailers = NULL;

#ifdef __APPLE__
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "iiO&O&|OOi:sendfile",
        keywords, &out, &in, Py_off_t_converter, &offset, Py_off_t_converter, &sbytes,
#else
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "iiO&n|OOi:sendfile",
        keywords, &out, &in, Py_off_t_converter, &offset, &len,
#endif
                &headers, &trailers, &flags))
            return NULL;
    if (headers != NULL) {
        if (!PySequence_Check(headers)) {
            PyErr_SetString(PyExc_TypeError,
                "sendfile() headers must be a sequence");
            return NULL;
        } else {
            Py_ssize_t i = 0; /* Avoid uninitialized warning */
            sf.hdr_cnt = PySequence_Size(headers);
            if (sf.hdr_cnt > 0 &&
                (i = iov_setup(&(sf.headers), &hbuf,
                                headers, sf.hdr_cnt, PyBUF_SIMPLE)) < 0)
                return NULL;
#ifdef __APPLE__
            sbytes += i;
#endif
        }
    }
    if (trailers != NULL) {
        if (!PySequence_Check(trailers)) {
            PyErr_SetString(PyExc_TypeError,
                "sendfile() trailers must be a sequence");
            return NULL;
        } else {
            Py_ssize_t i = 0; /* Avoid uninitialized warning */
            sf.trl_cnt = PySequence_Size(trailers);
            if (sf.trl_cnt > 0 &&
                (i = iov_setup(&(sf.trailers), &tbuf,
                                trailers, sf.trl_cnt, PyBUF_SIMPLE)) < 0)
                return NULL;
#ifdef __APPLE__
            sbytes += i;
#endif
        }
    }

    _Py_BEGIN_SUPPRESS_IPH
    do {
        Py_BEGIN_ALLOW_THREADS
#ifdef __APPLE__
        ret = sendfile(in, out, offset, &sbytes, &sf, flags);
#else
        ret = sendfile(in, out, offset, len, &sf, &sbytes, flags);
#endif
        Py_END_ALLOW_THREADS
    } while (ret < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    _Py_END_SUPPRESS_IPH

    if (sf.headers != NULL)
        iov_cleanup(sf.headers, hbuf, sf.hdr_cnt);
    if (sf.trailers != NULL)
        iov_cleanup(sf.trailers, tbuf, sf.trl_cnt);

    if (ret < 0) {
        if ((errno == EAGAIN) || (errno == EBUSY)) {
            if (sbytes != 0) {
                // some data has been sent
                goto done;
            }
            else {
                // no data has been sent; upper application is supposed
                // to retry on EAGAIN or EBUSY
                return posix_error();
            }
        }
        return (!async_err) ? posix_error() : NULL;
    }
    goto done;

done:
    #if !defined(HAVE_LARGEFILE_SUPPORT)
        return Py_BuildValue("l", sbytes);
    #else
        return Py_BuildValue("L", sbytes);
    #endif

#else
    Py_ssize_t count;
    PyObject *offobj;
    static char *keywords[] = {"out", "in",
                                "offset", "count", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "iiOn:sendfile",
            keywords, &out, &in, &offobj, &count))
        return NULL;
#ifdef __linux__
    if (offobj == Py_None) {
        do {
            Py_BEGIN_ALLOW_THREADS
            ret = sendfile(out, in, NULL, count);
            Py_END_ALLOW_THREADS
        } while (ret < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
        if (ret < 0)
            return (!async_err) ? posix_error() : NULL;
        return Py_BuildValue("n", ret);
    }
#endif
    if (!Py_off_t_converter(offobj, &offset))
        return NULL;

    do {
        Py_BEGIN_ALLOW_THREADS
        ret = sendfile(out, in, &offset, count);
        Py_END_ALLOW_THREADS
    } while (ret < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (ret < 0)
        return (!async_err) ? posix_error() : NULL;
    return Py_BuildValue("n", ret);
#endif
}
#endif /* HAVE_SENDFILE */


/*[clinic input]
os.fstat

    fd : int

Perform a stat system call on the given file descriptor.

Like stat(), but for an open file descriptor.
Equivalent to os.stat(fd).
[clinic start generated code]*/

static PyObject *
os_fstat_impl(PyObject *module, int fd)
/*[clinic end generated code: output=efc038cb5f654492 input=27e0e0ebbe5600c9]*/
{
    STRUCT_STAT st;
    int res;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        res = FSTAT(fd, &st);
        Py_END_ALLOW_THREADS
    } while (res != 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));
    if (res != 0) {
#ifdef MS_WINDOWS
        return PyErr_SetFromWindowsErr(0);
#else
        return (!async_err) ? posix_error() : NULL;
#endif
    }

    return _pystat_fromstructstat(&st);
}


/*[clinic input]
os.isatty -> bool
    fd: int
    /

Return True if the fd is connected to a terminal.

Return True if the file descriptor is an open file descriptor
connected to the slave end of a terminal.
[clinic start generated code]*/

static int
os_isatty_impl(PyObject *module, int fd)
/*[clinic end generated code: output=6a48c8b4e644ca00 input=08ce94aa1eaf7b5e]*/
{
    int return_value;
    _Py_BEGIN_SUPPRESS_IPH
    return_value = isatty(fd);
    _Py_END_SUPPRESS_IPH
    return return_value;
}


#ifdef HAVE_PIPE
/*[clinic input]
os.pipe

Create a pipe.

Returns a tuple of two file descriptors:
  (read_fd, write_fd)
[clinic start generated code]*/

static PyObject *
os_pipe_impl(PyObject *module)
/*[clinic end generated code: output=ff9b76255793b440 input=02535e8c8fa6c4d4]*/
{
    int fds[2];
#ifdef MS_WINDOWS
    HANDLE read, write;
    SECURITY_ATTRIBUTES attr;
    BOOL ok;
#else
    int res;
#endif

#ifdef MS_WINDOWS
    attr.nLength = sizeof(attr);
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = FALSE;

    Py_BEGIN_ALLOW_THREADS
    ok = CreatePipe(&read, &write, &attr, 0);
    if (ok) {
        fds[0] = _open_osfhandle((intptr_t)read, _O_RDONLY);
        fds[1] = _open_osfhandle((intptr_t)write, _O_WRONLY);
        if (fds[0] == -1 || fds[1] == -1) {
            CloseHandle(read);
            CloseHandle(write);
            ok = 0;
        }
    }
    Py_END_ALLOW_THREADS

    if (!ok)
        return PyErr_SetFromWindowsErr(0);
#else

#ifdef HAVE_PIPE2
    Py_BEGIN_ALLOW_THREADS
    res = pipe2(fds, O_CLOEXEC);
    Py_END_ALLOW_THREADS

    if (res != 0 && errno == ENOSYS)
    {
#endif
        Py_BEGIN_ALLOW_THREADS
        res = pipe(fds);
        Py_END_ALLOW_THREADS

        if (res == 0) {
            if (_Py_set_inheritable(fds[0], 0, NULL) < 0) {
                close(fds[0]);
                close(fds[1]);
                return NULL;
            }
            if (_Py_set_inheritable(fds[1], 0, NULL) < 0) {
                close(fds[0]);
                close(fds[1]);
                return NULL;
            }
        }
#ifdef HAVE_PIPE2
    }
#endif

    if (res != 0)
        return PyErr_SetFromErrno(PyExc_OSError);
#endif /* !MS_WINDOWS */
    return Py_BuildValue("(ii)", fds[0], fds[1]);
}
#endif  /* HAVE_PIPE */


#ifdef HAVE_PIPE2
/*[clinic input]
os.pipe2

    flags: int
    /

Create a pipe with flags set atomically.

Returns a tuple of two file descriptors:
  (read_fd, write_fd)

flags can be constructed by ORing together one or more of these values:
O_NONBLOCK, O_CLOEXEC.
[clinic start generated code]*/

static PyObject *
os_pipe2_impl(PyObject *module, int flags)
/*[clinic end generated code: output=25751fb43a45540f input=f261b6e7e63c6817]*/
{
    int fds[2];
    int res;

    res = pipe2(fds, flags);
    if (res != 0)
        return posix_error();
    return Py_BuildValue("(ii)", fds[0], fds[1]);
}
#endif /* HAVE_PIPE2 */


#ifdef HAVE_WRITEV
/*[clinic input]
os.writev -> Py_ssize_t
    fd: int
    buffers: object
    /

Iterate over buffers, and write the contents of each to a file descriptor.

Returns the total number of bytes written.
buffers must be a sequence of bytes-like objects.
[clinic start generated code]*/

static Py_ssize_t
os_writev_impl(PyObject *module, int fd, PyObject *buffers)
/*[clinic end generated code: output=56565cfac3aac15b input=5b8d17fe4189d2fe]*/
{
    int cnt;
    Py_ssize_t result;
    int async_err = 0;
    struct iovec *iov;
    Py_buffer *buf;

    if (!PySequence_Check(buffers)) {
        PyErr_SetString(PyExc_TypeError,
            "writev() arg 2 must be a sequence");
        return -1;
    }
    cnt = PySequence_Size(buffers);

    if (iov_setup(&iov, &buf, buffers, cnt, PyBUF_SIMPLE) < 0) {
        return -1;
    }

    do {
        Py_BEGIN_ALLOW_THREADS
        result = writev(fd, iov, cnt);
        Py_END_ALLOW_THREADS
    } while (result < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));

    iov_cleanup(iov, buf, cnt);
    if (result < 0 && !async_err)
        posix_error();

    return result;
}
#endif /* HAVE_WRITEV */


#ifdef HAVE_PWRITE
/*[clinic input]
os.pwrite -> Py_ssize_t

    fd: int
    buffer: Py_buffer
    offset: Py_off_t
    /

Write bytes to a file descriptor starting at a particular offset.

Write buffer to fd, starting at offset bytes from the beginning of
the file.  Returns the number of bytes writte.  Does not change the
current file offset.
[clinic start generated code]*/

static Py_ssize_t
os_pwrite_impl(PyObject *module, int fd, Py_buffer *buffer, Py_off_t offset)
/*[clinic end generated code: output=c74da630758ee925 input=19903f1b3dd26377]*/
{
    Py_ssize_t size;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        _Py_BEGIN_SUPPRESS_IPH
        size = pwrite(fd, buffer->buf, (size_t)buffer->len, offset);
        _Py_END_SUPPRESS_IPH
        Py_END_ALLOW_THREADS
    } while (size < 0 && errno == EINTR && !(async_err = PyErr_CheckSignals()));

    if (size < 0 && !async_err)
        posix_error();
    return size;
}
#endif /* HAVE_PWRITE */


#ifdef HAVE_MKFIFO
/*[clinic input]
os.mkfifo

    path: path_t
    mode: int=0o666
    *
    dir_fd: dir_fd(requires='mkfifoat')=None

Create a "fifo" (a POSIX named pipe).

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_mkfifo_impl(PyObject *module, path_t *path, int mode, int dir_fd)
/*[clinic end generated code: output=ce41cfad0e68c940 input=73032e98a36e0e19]*/
{
    int result;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_MKFIFOAT
        if (dir_fd != DEFAULT_DIR_FD)
            result = mkfifoat(dir_fd, path->narrow, mode);
        else
#endif
            result = mkfifo(path->narrow, mode);
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;

    Py_RETURN_NONE;
}
#endif /* HAVE_MKFIFO */


#if defined(HAVE_MKNOD) && defined(HAVE_MAKEDEV)
/*[clinic input]
os.mknod

    path: path_t
    mode: int=0o600
    device: dev_t=0
    *
    dir_fd: dir_fd(requires='mknodat')=None

Create a node in the file system.

Create a node in the file system (file, device special file or named pipe)
at path.  mode specifies both the permissions to use and the
type of node to be created, being combined (bitwise OR) with one of
S_IFREG, S_IFCHR, S_IFBLK, and S_IFIFO.  If S_IFCHR or S_IFBLK is set on mode,
device defines the newly created device special file (probably using
os.makedev()).  Otherwise device is ignored.

If dir_fd is not None, it should be a file descriptor open to a directory,
  and path should be relative; path will then be relative to that directory.
dir_fd may not be implemented on your platform.
  If it is unavailable, using it will raise a NotImplementedError.
[clinic start generated code]*/

static PyObject *
os_mknod_impl(PyObject *module, path_t *path, int mode, dev_t device,
              int dir_fd)
/*[clinic end generated code: output=92e55d3ca8917461 input=ee44531551a4d83b]*/
{
    int result;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_MKNODAT
        if (dir_fd != DEFAULT_DIR_FD)
            result = mknodat(dir_fd, path->narrow, mode, device);
        else
#endif
            result = mknod(path->narrow, mode, device);
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;

    Py_RETURN_NONE;
}
#endif /* defined(HAVE_MKNOD) && defined(HAVE_MAKEDEV) */


#ifdef HAVE_DEVICE_MACROS
/*[clinic input]
os.major -> unsigned_int

    device: dev_t
    /

Extracts a device major number from a raw device number.
[clinic start generated code]*/

static unsigned int
os_major_impl(PyObject *module, dev_t device)
/*[clinic end generated code: output=5b3b2589bafb498e input=1e16a4d30c4d4462]*/
{
    return major(device);
}


/*[clinic input]
os.minor -> unsigned_int

    device: dev_t
    /

Extracts a device minor number from a raw device number.
[clinic start generated code]*/

static unsigned int
os_minor_impl(PyObject *module, dev_t device)
/*[clinic end generated code: output=5e1a25e630b0157d input=0842c6d23f24c65e]*/
{
    return minor(device);
}


/*[clinic input]
os.makedev -> dev_t

    major: int
    minor: int
    /

Composes a raw device number from the major and minor device numbers.
[clinic start generated code]*/

static dev_t
os_makedev_impl(PyObject *module, int major, int minor)
/*[clinic end generated code: output=881aaa4aba6f6a52 input=4b9fd8fc73cbe48f]*/
{
    return makedev(major, minor);
}
#endif /* HAVE_DEVICE_MACROS */


#if defined HAVE_FTRUNCATE || defined MS_WINDOWS
/*[clinic input]
os.ftruncate

    fd: int
    length: Py_off_t
    /

Truncate a file, specified by file descriptor, to a specific length.
[clinic start generated code]*/

static PyObject *
os_ftruncate_impl(PyObject *module, int fd, Py_off_t length)
/*[clinic end generated code: output=fba15523721be7e4 input=63b43641e52818f2]*/
{
    int result;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        _Py_BEGIN_SUPPRESS_IPH
#ifdef MS_WINDOWS
        result = _chsize_s(fd, length);
#else
        result = ftruncate(fd, length);
#endif
        _Py_END_SUPPRESS_IPH
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;
    Py_RETURN_NONE;
}
#endif /* HAVE_FTRUNCATE || MS_WINDOWS */


#if defined HAVE_TRUNCATE || defined MS_WINDOWS
/*[clinic input]
os.truncate
    path: path_t(allow_fd='PATH_HAVE_FTRUNCATE')
    length: Py_off_t

Truncate a file, specified by path, to a specific length.

On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.
[clinic start generated code]*/

static PyObject *
os_truncate_impl(PyObject *module, path_t *path, Py_off_t length)
/*[clinic end generated code: output=43009c8df5c0a12b input=77229cf0b50a9b77]*/
{
    int result;
#ifdef MS_WINDOWS
    int fd;
#endif

    if (path->fd != -1)
        return os_ftruncate_impl(module, path->fd, length);

    Py_BEGIN_ALLOW_THREADS
    _Py_BEGIN_SUPPRESS_IPH
#ifdef MS_WINDOWS
    fd = _wopen(path->wide, _O_WRONLY | _O_BINARY | _O_NOINHERIT);
    if (fd < 0)
        result = -1;
    else {
        result = _chsize_s(fd, length);
        close(fd);
        if (result < 0)
            errno = result;
    }
#else
    result = truncate(path->narrow, length);
#endif
    _Py_END_SUPPRESS_IPH
    Py_END_ALLOW_THREADS
    if (result < 0)
        return path_error(path);

    Py_RETURN_NONE;
}
#endif /* HAVE_TRUNCATE || MS_WINDOWS */


/* Issue #22396: On 32-bit AIX platform, the prototypes of os.posix_fadvise()
   and os.posix_fallocate() in system headers are wrong if _LARGE_FILES is
   defined, which is the case in Python on AIX. AIX bug report:
   http://www-01.ibm.com/support/docview.wss?uid=isg1IV56170 */
#if defined(_AIX) && defined(_LARGE_FILES) && !defined(__64BIT__)
#  define POSIX_FADVISE_AIX_BUG
#endif


#if defined(HAVE_POSIX_FALLOCATE) && !defined(POSIX_FADVISE_AIX_BUG)
/*[clinic input]
os.posix_fallocate

    fd: int
    offset: Py_off_t
    length: Py_off_t
    /

Ensure a file has allocated at least a particular number of bytes on disk.

Ensure that the file specified by fd encompasses a range of bytes
starting at offset bytes from the beginning and continuing for length bytes.
[clinic start generated code]*/

static PyObject *
os_posix_fallocate_impl(PyObject *module, int fd, Py_off_t offset,
                        Py_off_t length)
/*[clinic end generated code: output=73f107139564aa9d input=d7a2ef0ab2ca52fb]*/
{
    int result;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        result = posix_fallocate(fd, offset, length);
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;
    Py_RETURN_NONE;
}
#endif /* HAVE_POSIX_FALLOCATE) && !POSIX_FADVISE_AIX_BUG */


#if defined(HAVE_POSIX_FADVISE) && !defined(POSIX_FADVISE_AIX_BUG)
/*[clinic input]
os.posix_fadvise

    fd: int
    offset: Py_off_t
    length: Py_off_t
    advice: int
    /

Announce an intention to access data in a specific pattern.

Announce an intention to access data in a specific pattern, thus allowing
the kernel to make optimizations.
The advice applies to the region of the file specified by fd starting at
offset and continuing for length bytes.
advice is one of POSIX_FADV_NORMAL, POSIX_FADV_SEQUENTIAL,
POSIX_FADV_RANDOM, POSIX_FADV_NOREUSE, POSIX_FADV_WILLNEED, or
POSIX_FADV_DONTNEED.
[clinic start generated code]*/

static PyObject *
os_posix_fadvise_impl(PyObject *module, int fd, Py_off_t offset,
                      Py_off_t length, int advice)
/*[clinic end generated code: output=412ef4aa70c98642 input=0fbe554edc2f04b5]*/
{
    int result;
    int async_err = 0;

    do {
        Py_BEGIN_ALLOW_THREADS
        result = posix_fadvise(fd, offset, length, advice);
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;
    Py_RETURN_NONE;
}
#endif /* HAVE_POSIX_FADVISE && !POSIX_FADVISE_AIX_BUG */

#ifdef HAVE_PUTENV

/* Save putenv() parameters as values here, so we can collect them when they
 * get re-set with another call for the same key. */
static PyObject *posix_putenv_garbage;

static void
posix_putenv_garbage_setitem(PyObject *name, PyObject *value)
{
    /* Install the first arg and newstr in posix_putenv_garbage;
     * this will cause previous value to be collected.  This has to
     * happen after the real putenv() call because the old value
     * was still accessible until then. */
    if (PyDict_SetItem(posix_putenv_garbage, name, value))
        /* really not much we can do; just leak */
        PyErr_Clear();
    else
        Py_DECREF(value);
}


#ifdef MS_WINDOWS
/*[clinic input]
os.putenv

    name: unicode
    value: unicode
    /

Change or add an environment variable.
[clinic start generated code]*/

static PyObject *
os_putenv_impl(PyObject *module, PyObject *name, PyObject *value)
/*[clinic end generated code: output=d29a567d6b2327d2 input=ba586581c2e6105f]*/
{
    const wchar_t *env;

    PyObject *unicode = PyUnicode_FromFormat("%U=%U", name, value);
    if (unicode == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    if (_MAX_ENV < PyUnicode_GET_LENGTH(unicode)) {
        PyErr_Format(PyExc_ValueError,
                     "the environment variable is longer than %u characters",
                     _MAX_ENV);
        goto error;
    }

    env = PyUnicode_AsUnicode(unicode);
    if (env == NULL)
        goto error;
    if (_wputenv(env)) {
        posix_error();
        goto error;
    }

    posix_putenv_garbage_setitem(name, unicode);
    Py_RETURN_NONE;

error:
    Py_DECREF(unicode);
    return NULL;
}
#else /* MS_WINDOWS */
/*[clinic input]
os.putenv

    name: FSConverter
    value: FSConverter
    /

Change or add an environment variable.
[clinic start generated code]*/

static PyObject *
os_putenv_impl(PyObject *module, PyObject *name, PyObject *value)
/*[clinic end generated code: output=d29a567d6b2327d2 input=a97bc6152f688d31]*/
{
    PyObject *bytes = NULL;
    char *env;
    const char *name_string = PyBytes_AsString(name);
    const char *value_string = PyBytes_AsString(value);

    bytes = PyBytes_FromFormat("%s=%s", name_string, value_string);
    if (bytes == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    env = PyBytes_AS_STRING(bytes);
    if (putenv(env)) {
        Py_DECREF(bytes);
        return posix_error();
    }

    posix_putenv_garbage_setitem(name, bytes);
    Py_RETURN_NONE;
}
#endif /* MS_WINDOWS */
#endif /* HAVE_PUTENV */


#ifdef HAVE_UNSETENV
/*[clinic input]
os.unsetenv
    name: FSConverter
    /

Delete an environment variable.
[clinic start generated code]*/

static PyObject *
os_unsetenv_impl(PyObject *module, PyObject *name)
/*[clinic end generated code: output=54c4137ab1834f02 input=2bb5288a599c7107]*/
{
#ifndef HAVE_BROKEN_UNSETENV
    int err;
#endif

#ifdef HAVE_BROKEN_UNSETENV
    unsetenv(PyBytes_AS_STRING(name));
#else
    err = unsetenv(PyBytes_AS_STRING(name));
    if (err)
        return posix_error();
#endif

    /* Remove the key from posix_putenv_garbage;
     * this will cause it to be collected.  This has to
     * happen after the real unsetenv() call because the
     * old value was still accessible until then.
     */
    if (PyDict_DelItem(posix_putenv_garbage, name)) {
        /* really not much we can do; just leak */
        PyErr_Clear();
    }
    Py_RETURN_NONE;
}
#endif /* HAVE_UNSETENV */


/*[clinic input]
os.strerror

    code: int
    /

Translate an error code to a message string.
[clinic start generated code]*/

static PyObject *
os_strerror_impl(PyObject *module, int code)
/*[clinic end generated code: output=baebf09fa02a78f2 input=75a8673d97915a91]*/
{
    char *message = strerror(code);
    if (message == NULL) {
        PyErr_SetString(PyExc_ValueError,
                        "strerror() argument out of range");
        return NULL;
    }
    return PyUnicode_DecodeLocale(message, "surrogateescape");
}


#ifdef HAVE_SYS_WAIT_H
#ifdef WCOREDUMP
/*[clinic input]
os.WCOREDUMP -> bool

    status: int
    /

Return True if the process returning status was dumped to a core file.
[clinic start generated code]*/

static int
os_WCOREDUMP_impl(PyObject *module, int status)
/*[clinic end generated code: output=1a584b147b16bd18 input=8b05e7ab38528d04]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WCOREDUMP(wait_status);
}
#endif /* WCOREDUMP */


#ifdef WIFCONTINUED
/*[clinic input]
os.WIFCONTINUED -> bool

    status: int

Return True if a particular process was continued from a job control stop.

Return True if the process returning status was continued from a
job control stop.
[clinic start generated code]*/

static int
os_WIFCONTINUED_impl(PyObject *module, int status)
/*[clinic end generated code: output=1e35295d844364bd input=e777e7d38eb25bd9]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WIFCONTINUED(wait_status);
}
#endif /* WIFCONTINUED */


#ifdef WIFSTOPPED
/*[clinic input]
os.WIFSTOPPED -> bool

    status: int

Return True if the process returning status was stopped.
[clinic start generated code]*/

static int
os_WIFSTOPPED_impl(PyObject *module, int status)
/*[clinic end generated code: output=fdb57122a5c9b4cb input=043cb7f1289ef904]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WIFSTOPPED(wait_status);
}
#endif /* WIFSTOPPED */


#ifdef WIFSIGNALED
/*[clinic input]
os.WIFSIGNALED -> bool

    status: int

Return True if the process returning status was terminated by a signal.
[clinic start generated code]*/

static int
os_WIFSIGNALED_impl(PyObject *module, int status)
/*[clinic end generated code: output=d1dde4dcc819a5f5 input=d55ba7cc9ce5dc43]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WIFSIGNALED(wait_status);
}
#endif /* WIFSIGNALED */


#ifdef WIFEXITED
/*[clinic input]
os.WIFEXITED -> bool

    status: int

Return True if the process returning status exited via the exit() system call.
[clinic start generated code]*/

static int
os_WIFEXITED_impl(PyObject *module, int status)
/*[clinic end generated code: output=01c09d6ebfeea397 input=d63775a6791586c0]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WIFEXITED(wait_status);
}
#endif /* WIFEXITED */


#ifdef WEXITSTATUS
/*[clinic input]
os.WEXITSTATUS -> int

    status: int

Return the process return code from status.
[clinic start generated code]*/

static int
os_WEXITSTATUS_impl(PyObject *module, int status)
/*[clinic end generated code: output=6e3efbba11f6488d input=e1fb4944e377585b]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WEXITSTATUS(wait_status);
}
#endif /* WEXITSTATUS */


#ifdef WTERMSIG
/*[clinic input]
os.WTERMSIG -> int

    status: int

Return the signal that terminated the process that provided the status value.
[clinic start generated code]*/

static int
os_WTERMSIG_impl(PyObject *module, int status)
/*[clinic end generated code: output=172f7dfc8dcfc3ad input=727fd7f84ec3f243]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WTERMSIG(wait_status);
}
#endif /* WTERMSIG */


#ifdef WSTOPSIG
/*[clinic input]
os.WSTOPSIG -> int

    status: int

Return the signal that stopped the process that provided the status value.
[clinic start generated code]*/

static int
os_WSTOPSIG_impl(PyObject *module, int status)
/*[clinic end generated code: output=0ab7586396f5d82b input=46ebf1d1b293c5c1]*/
{
    WAIT_TYPE wait_status;
    WAIT_STATUS_INT(wait_status) = status;
    return WSTOPSIG(wait_status);
}
#endif /* WSTOPSIG */
#endif /* HAVE_SYS_WAIT_H */


#if defined(HAVE_FSTATVFS) && defined(HAVE_SYS_STATVFS_H)
#ifdef _SCO_DS
/* SCO OpenServer 5.0 and later requires _SVID3 before it reveals the
   needed definitions in sys/statvfs.h */
#define _SVID3
#endif
#include <sys/statvfs.h>

static PyObject*
_pystatvfs_fromstructstatvfs(struct statvfs st) {
    PyObject *v = PyStructSequence_New(&StatVFSResultType);
    if (v == NULL)
        return NULL;

#if !defined(HAVE_LARGEFILE_SUPPORT)
    PyStructSequence_SET_ITEM(v, 0, PyLong_FromLong((long) st.f_bsize));
    PyStructSequence_SET_ITEM(v, 1, PyLong_FromLong((long) st.f_frsize));
    PyStructSequence_SET_ITEM(v, 2, PyLong_FromLong((long) st.f_blocks));
    PyStructSequence_SET_ITEM(v, 3, PyLong_FromLong((long) st.f_bfree));
    PyStructSequence_SET_ITEM(v, 4, PyLong_FromLong((long) st.f_bavail));
    PyStructSequence_SET_ITEM(v, 5, PyLong_FromLong((long) st.f_files));
    PyStructSequence_SET_ITEM(v, 6, PyLong_FromLong((long) st.f_ffree));
    PyStructSequence_SET_ITEM(v, 7, PyLong_FromLong((long) st.f_favail));
    PyStructSequence_SET_ITEM(v, 8, PyLong_FromLong((long) st.f_flag));
    PyStructSequence_SET_ITEM(v, 9, PyLong_FromLong((long) st.f_namemax));
#else
    PyStructSequence_SET_ITEM(v, 0, PyLong_FromLong((long) st.f_bsize));
    PyStructSequence_SET_ITEM(v, 1, PyLong_FromLong((long) st.f_frsize));
    PyStructSequence_SET_ITEM(v, 2,
                              PyLong_FromLongLong((long long) st.f_blocks));
    PyStructSequence_SET_ITEM(v, 3,
                              PyLong_FromLongLong((long long) st.f_bfree));
    PyStructSequence_SET_ITEM(v, 4,
                              PyLong_FromLongLong((long long) st.f_bavail));
    PyStructSequence_SET_ITEM(v, 5,
                              PyLong_FromLongLong((long long) st.f_files));
    PyStructSequence_SET_ITEM(v, 6,
                              PyLong_FromLongLong((long long) st.f_ffree));
    PyStructSequence_SET_ITEM(v, 7,
                              PyLong_FromLongLong((long long) st.f_favail));
    PyStructSequence_SET_ITEM(v, 8, PyLong_FromLong((long) st.f_flag));
    PyStructSequence_SET_ITEM(v, 9, PyLong_FromLong((long) st.f_namemax));
#endif
    if (PyErr_Occurred()) {
        Py_DECREF(v);
        return NULL;
    }

    return v;
}


/*[clinic input]
os.fstatvfs
    fd: int
    /

Perform an fstatvfs system call on the given fd.

Equivalent to statvfs(fd).
[clinic start generated code]*/

static PyObject *
os_fstatvfs_impl(PyObject *module, int fd)
/*[clinic end generated code: output=53547cf0cc55e6c5 input=d8122243ac50975e]*/
{
    int result;
    int async_err = 0;
    struct statvfs st;

    do {
        Py_BEGIN_ALLOW_THREADS
        result = fstatvfs(fd, &st);
        Py_END_ALLOW_THREADS
    } while (result != 0 && errno == EINTR &&
             !(async_err = PyErr_CheckSignals()));
    if (result != 0)
        return (!async_err) ? posix_error() : NULL;

    return _pystatvfs_fromstructstatvfs(st);
}
#endif /* defined(HAVE_FSTATVFS) && defined(HAVE_SYS_STATVFS_H) */


#if defined(HAVE_STATVFS) && defined(HAVE_SYS_STATVFS_H)
#include <sys/statvfs.h>
/*[clinic input]
os.statvfs

    path: path_t(allow_fd='PATH_HAVE_FSTATVFS')

Perform a statvfs system call on the given path.

path may always be specified as a string.
On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.
[clinic start generated code]*/

static PyObject *
os_statvfs_impl(PyObject *module, path_t *path)
/*[clinic end generated code: output=87106dd1beb8556e input=3f5c35791c669bd9]*/
{
    int result;
    struct statvfs st;

    Py_BEGIN_ALLOW_THREADS
#ifdef HAVE_FSTATVFS
    if (path->fd != -1) {
#ifdef __APPLE__
        /* handle weak-linking on Mac OS X 10.3 */
        if (fstatvfs == NULL) {
            fd_specified("statvfs", path->fd);
            return NULL;
        }
#endif
        result = fstatvfs(path->fd, &st);
    }
    else
#endif
        result = statvfs(path->narrow, &st);
    Py_END_ALLOW_THREADS

    if (result) {
        return path_error(path);
    }

    return _pystatvfs_fromstructstatvfs(st);
}
#endif /* defined(HAVE_STATVFS) && defined(HAVE_SYS_STATVFS_H) */


#ifdef MS_WINDOWS
/*[clinic input]
os._getdiskusage

    path: Py_UNICODE

Return disk usage statistics about the given path as a (total, free) tuple.
[clinic start generated code]*/

static PyObject *
os__getdiskusage_impl(PyObject *module, Py_UNICODE *path)
/*[clinic end generated code: output=76d6adcd86b1db0b input=6458133aed893c78]*/
{
    BOOL retval;
    ULARGE_INTEGER _, total, free;

    Py_BEGIN_ALLOW_THREADS
    retval = GetDiskFreeSpaceExW(path, &_, &total, &free);
    Py_END_ALLOW_THREADS
    if (retval == 0)
        return PyErr_SetFromWindowsErr(0);

    return Py_BuildValue("(LL)", total.QuadPart, free.QuadPart);
}
#endif /* MS_WINDOWS */


/* This is used for fpathconf(), pathconf(), confstr() and sysconf().
 * It maps strings representing configuration variable names to
 * integer values, allowing those functions to be called with the
 * magic names instead of polluting the module's namespace with tons of
 * rarely-used constants.  There are three separate tables that use
 * these definitions.
 *
 * This code is always included, even if none of the interfaces that
 * need it are included.  The #if hackery needed to avoid it would be
 * sufficiently pervasive that it's not worth the loss of readability.
 */
struct constdef {
    const char *name;
    int value;
};

static int
conv_confname(PyObject *arg, int *valuep, struct constdef *table,
              size_t tablesize)
{
    if (PyLong_Check(arg)) {
        int value = _PyLong_AsInt(arg);
        if (value == -1 && PyErr_Occurred())
            return 0;
        *valuep = value;
        return 1;
    }
    else {
        /* look up the value in the table using a binary search */
        size_t lo = 0;
        size_t mid;
        size_t hi = tablesize;
        int cmp;
        const char *confname;
        if (!PyUnicode_Check(arg)) {
            PyErr_SetString(PyExc_TypeError,
                "configuration names must be strings or integers");
            return 0;
        }
        confname = _PyUnicode_AsString(arg);
        if (confname == NULL)
            return 0;
        while (lo < hi) {
            mid = (lo + hi) / 2;
            cmp = strcmp(confname, table[mid].name);
            if (cmp < 0)
                hi = mid;
            else if (cmp > 0)
                lo = mid + 1;
            else {
                *valuep = table[mid].value;
                return 1;
            }
        }
        PyErr_SetString(PyExc_ValueError, "unrecognized configuration name");
        return 0;
    }
}


#if defined(HAVE_FPATHCONF) || defined(HAVE_PATHCONF)
static struct constdef  posix_constants_pathconf[] = {
#ifdef _PC_ABI_AIO_XFER_MAX
    {"PC_ABI_AIO_XFER_MAX",     _PC_ABI_AIO_XFER_MAX},
#endif
#ifdef _PC_ABI_ASYNC_IO
    {"PC_ABI_ASYNC_IO", _PC_ABI_ASYNC_IO},
#endif
#ifdef _PC_ASYNC_IO
    {"PC_ASYNC_IO",     _PC_ASYNC_IO},
#endif
#ifdef _PC_CHOWN_RESTRICTED
    {"PC_CHOWN_RESTRICTED",     _PC_CHOWN_RESTRICTED},
#endif
#ifdef _PC_FILESIZEBITS
    {"PC_FILESIZEBITS", _PC_FILESIZEBITS},
#endif
#ifdef _PC_LAST
    {"PC_LAST", _PC_LAST},
#endif
#ifdef _PC_LINK_MAX
    {"PC_LINK_MAX",     _PC_LINK_MAX},
#endif
#ifdef _PC_MAX_CANON
    {"PC_MAX_CANON",    _PC_MAX_CANON},
#endif
#ifdef _PC_MAX_INPUT
    {"PC_MAX_INPUT",    _PC_MAX_INPUT},
#endif
#ifdef _PC_NAME_MAX
    {"PC_NAME_MAX",     _PC_NAME_MAX},
#endif
#ifdef _PC_NO_TRUNC
    {"PC_NO_TRUNC",     _PC_NO_TRUNC},
#endif
#ifdef _PC_PATH_MAX
    {"PC_PATH_MAX",     _PC_PATH_MAX},
#endif
#ifdef _PC_PIPE_BUF
    {"PC_PIPE_BUF",     _PC_PIPE_BUF},
#endif
#ifdef _PC_PRIO_IO
    {"PC_PRIO_IO",      _PC_PRIO_IO},
#endif
#ifdef _PC_SOCK_MAXBUF
    {"PC_SOCK_MAXBUF",  _PC_SOCK_MAXBUF},
#endif
#ifdef _PC_SYNC_IO
    {"PC_SYNC_IO",      _PC_SYNC_IO},
#endif
#ifdef _PC_VDISABLE
    {"PC_VDISABLE",     _PC_VDISABLE},
#endif
#ifdef _PC_ACL_ENABLED
    {"PC_ACL_ENABLED",  _PC_ACL_ENABLED},
#endif
#ifdef _PC_MIN_HOLE_SIZE
    {"PC_MIN_HOLE_SIZE",    _PC_MIN_HOLE_SIZE},
#endif
#ifdef _PC_ALLOC_SIZE_MIN
    {"PC_ALLOC_SIZE_MIN",   _PC_ALLOC_SIZE_MIN},
#endif
#ifdef _PC_REC_INCR_XFER_SIZE
    {"PC_REC_INCR_XFER_SIZE",   _PC_REC_INCR_XFER_SIZE},
#endif
#ifdef _PC_REC_MAX_XFER_SIZE
    {"PC_REC_MAX_XFER_SIZE",    _PC_REC_MAX_XFER_SIZE},
#endif
#ifdef _PC_REC_MIN_XFER_SIZE
    {"PC_REC_MIN_XFER_SIZE",    _PC_REC_MIN_XFER_SIZE},
#endif
#ifdef _PC_REC_XFER_ALIGN
    {"PC_REC_XFER_ALIGN",   _PC_REC_XFER_ALIGN},
#endif
#ifdef _PC_SYMLINK_MAX
    {"PC_SYMLINK_MAX",  _PC_SYMLINK_MAX},
#endif
#ifdef _PC_XATTR_ENABLED
    {"PC_XATTR_ENABLED",    _PC_XATTR_ENABLED},
#endif
#ifdef _PC_XATTR_EXISTS
    {"PC_XATTR_EXISTS", _PC_XATTR_EXISTS},
#endif
#ifdef _PC_TIMESTAMP_RESOLUTION
    {"PC_TIMESTAMP_RESOLUTION", _PC_TIMESTAMP_RESOLUTION},
#endif
};

static int
conv_path_confname(PyObject *arg, int *valuep)
{
    return conv_confname(arg, valuep, posix_constants_pathconf,
                         sizeof(posix_constants_pathconf)
                           / sizeof(struct constdef));
}
#endif


#ifdef HAVE_FPATHCONF
/*[clinic input]
os.fpathconf -> long

    fd: int
    name: path_confname
    /

Return the configuration limit name for the file descriptor fd.

If there is no limit, return -1.
[clinic start generated code]*/

static long
os_fpathconf_impl(PyObject *module, int fd, int name)
/*[clinic end generated code: output=d5b7042425fc3e21 input=5942a024d3777810]*/
{
    long limit;

    errno = 0;
    limit = fpathconf(fd, name);
    if (limit == -1 && errno != 0)
        posix_error();

    return limit;
}
#endif /* HAVE_FPATHCONF */


#ifdef HAVE_PATHCONF
/*[clinic input]
os.pathconf -> long
    path: path_t(allow_fd='PATH_HAVE_FPATHCONF')
    name: path_confname

Return the configuration limit name for the file or directory path.

If there is no limit, return -1.
On some platforms, path may also be specified as an open file descriptor.
  If this functionality is unavailable, using it raises an exception.
[clinic start generated code]*/

static long
os_pathconf_impl(PyObject *module, path_t *path, int name)
/*[clinic end generated code: output=5bedee35b293a089 input=bc3e2a985af27e5e]*/
{
    long limit;

    errno = 0;
#ifdef HAVE_FPATHCONF
    if (path->fd != -1)
        limit = fpathconf(path->fd, name);
    else
#endif
        limit = pathconf(path->narrow, name);
    if (limit == -1 && errno != 0) {
        if (errno == EINVAL)
            /* could be a path or name problem */
            posix_error();
        else
            path_error(path);
    }

    return limit;
}
#endif /* HAVE_PATHCONF */

#ifdef HAVE_CONFSTR
static struct constdef posix_constants_confstr[] = {
#ifdef _CS_ARCHITECTURE
    {"CS_ARCHITECTURE", _CS_ARCHITECTURE},
#endif
#ifdef _CS_GNU_LIBC_VERSION
    {"CS_GNU_LIBC_VERSION",     _CS_GNU_LIBC_VERSION},
#endif
#ifdef _CS_GNU_LIBPTHREAD_VERSION
    {"CS_GNU_LIBPTHREAD_VERSION",       _CS_GNU_LIBPTHREAD_VERSION},
#endif
#ifdef _CS_HOSTNAME
    {"CS_HOSTNAME",     _CS_HOSTNAME},
#endif
#ifdef _CS_HW_PROVIDER
    {"CS_HW_PROVIDER",  _CS_HW_PROVIDER},
#endif
#ifdef _CS_HW_SERIAL
    {"CS_HW_SERIAL",    _CS_HW_SERIAL},
#endif
#ifdef _CS_INITTAB_NAME
    {"CS_INITTAB_NAME", _CS_INITTAB_NAME},
#endif
#ifdef _CS_LFS64_CFLAGS
    {"CS_LFS64_CFLAGS", _CS_LFS64_CFLAGS},
#endif
#ifdef _CS_LFS64_LDFLAGS
    {"CS_LFS64_LDFLAGS",        _CS_LFS64_LDFLAGS},
#endif
#ifdef _CS_LFS64_LIBS
    {"CS_LFS64_LIBS",   _CS_LFS64_LIBS},
#endif
#ifdef _CS_LFS64_LINTFLAGS
    {"CS_LFS64_LINTFLAGS",      _CS_LFS64_LINTFLAGS},
#endif
#ifdef _CS_LFS_CFLAGS
    {"CS_LFS_CFLAGS",   _CS_LFS_CFLAGS},
#endif
#ifdef _CS_LFS_LDFLAGS
    {"CS_LFS_LDFLAGS",  _CS_LFS_LDFLAGS},
#endif
#ifdef _CS_LFS_LIBS
    {"CS_LFS_LIBS",     _CS_LFS_LIBS},
#endif
#ifdef _CS_LFS_LINTFLAGS
    {"CS_LFS_LINTFLAGS",        _CS_LFS_LINTFLAGS},
#endif
#ifdef _CS_MACHINE
    {"CS_MACHINE",      _CS_MACHINE},
#endif
#ifdef _CS_PATH
    {"CS_PATH", _CS_PATH},
#endif
#ifdef _CS_RELEASE
    {"CS_RELEASE",      _CS_RELEASE},
#endif
#ifdef _CS_SRPC_DOMAIN
    {"CS_SRPC_DOMAIN",  _CS_SRPC_DOMAIN},
#endif
#ifdef _CS_SYSNAME
    {"CS_SYSNAME",      _CS_SYSNAME},
#endif
#ifdef _CS_VERSION
    {"CS_VERSION",      _CS_VERSION},
#endif
#ifdef _CS_XBS5_ILP32_OFF32_CFLAGS
    {"CS_XBS5_ILP32_OFF32_CFLAGS",      _CS_XBS5_ILP32_OFF32_CFLAGS},
#endif
#ifdef _CS_XBS5_ILP32_OFF32_LDFLAGS
    {"CS_XBS5_ILP32_OFF32_LDFLAGS",     _CS_XBS5_ILP32_OFF32_LDFLAGS},
#endif
#ifdef _CS_XBS5_ILP32_OFF32_LIBS
    {"CS_XBS5_ILP32_OFF32_LIBS",        _CS_XBS5_ILP32_OFF32_LIBS},
#endif
#ifdef _CS_XBS5_ILP32_OFF32_LINTFLAGS
    {"CS_XBS5_ILP32_OFF32_LINTFLAGS",   _CS_XBS5_ILP32_OFF32_LINTFLAGS},
#endif
#ifdef _CS_XBS5_ILP32_OFFBIG_CFLAGS
    {"CS_XBS5_ILP32_OFFBIG_CFLAGS",     _CS_XBS5_ILP32_OFFBIG_CFLAGS},
#endif
#ifdef _CS_XBS5_ILP32_OFFBIG_LDFLAGS
    {"CS_XBS5_ILP32_OFFBIG_LDFLAGS",    _CS_XBS5_ILP32_OFFBIG_LDFLAGS},
#endif
#ifdef _CS_XBS5_ILP32_OFFBIG_LIBS
    {"CS_XBS5_ILP32_OFFBIG_LIBS",       _CS_XBS5_ILP32_OFFBIG_LIBS},
#endif
#ifdef _CS_XBS5_ILP32_OFFBIG_LINTFLAGS
    {"CS_XBS5_ILP32_OFFBIG_LINTFLAGS",  _CS_XBS5_ILP32_OFFBIG_LINTFLAGS},
#endif
#ifdef _CS_XBS5_LP64_OFF64_CFLAGS
    {"CS_XBS5_LP64_OFF64_CFLAGS",       _CS_XBS5_LP64_OFF64_CFLAGS},
#endif
#ifdef _CS_XBS5_LP64_OFF64_LDFLAGS
    {"CS_XBS5_LP64_OFF64_LDFLAGS",      _CS_XBS5_LP64_OFF64_LDFLAGS},
#endif
#ifdef _CS_XBS5_LP64_OFF64_LIBS
    {"CS_XBS5_LP64_OFF64_LIBS", _CS_XBS5_LP64_OFF64_LIBS},
#endif
#ifdef _CS_XBS5_LP64_OFF64_LINTFLAGS
    {"CS_XBS5_LP64_OFF64_LINTFLAGS",    _CS_XBS5_LP64_OFF64_LINTFLAGS},
#endif
#ifdef _CS_XBS5_LPBIG_OFFBIG_CFLAGS
    {"CS_XBS5_LPBIG_OFFBIG_CFLAGS",     _CS_XBS5_LPBIG_OFFBIG_CFLAGS},
#endif
#ifdef _CS_XBS5_LPBIG_OFFBIG_LDFLAGS
    {"CS_XBS5_LPBIG_OFFBIG_LDFLAGS",    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS},
#endif
#ifdef _CS_XBS5_LPBIG_OFFBIG_LIBS
    {"CS_XBS5_LPBIG_OFFBIG_LIBS",       _CS_XBS5_LPBIG_OFFBIG_LIBS},
#endif
#ifdef _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS
    {"CS_XBS5_LPBIG_OFFBIG_LINTFLAGS",  _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS},
#endif
#ifdef _MIPS_CS_AVAIL_PROCESSORS
    {"MIPS_CS_AVAIL_PROCESSORS",        _MIPS_CS_AVAIL_PROCESSORS},
#endif
#ifdef _MIPS_CS_BASE
    {"MIPS_CS_BASE",    _MIPS_CS_BASE},
#endif
#ifdef _MIPS_CS_HOSTID
    {"MIPS_CS_HOSTID",  _MIPS_CS_HOSTID},
#endif
#ifdef _MIPS_CS_HW_NAME
    {"MIPS_CS_HW_NAME", _MIPS_CS_HW_NAME},
#endif
#ifdef _MIPS_CS_NUM_PROCESSORS
    {"MIPS_CS_NUM_PROCESSORS",  _MIPS_CS_NUM_PROCESSORS},
#endif
#ifdef _MIPS_CS_OSREL_MAJ
    {"MIPS_CS_OSREL_MAJ",       _MIPS_CS_OSREL_MAJ},
#endif
#ifdef _MIPS_CS_OSREL_MIN
    {"MIPS_CS_OSREL_MIN",       _MIPS_CS_OSREL_MIN},
#endif
#ifdef _MIPS_CS_OSREL_PATCH
    {"MIPS_CS_OSREL_PATCH",     _MIPS_CS_OSREL_PATCH},
#endif
#ifdef _MIPS_CS_OS_NAME
    {"MIPS_CS_OS_NAME", _MIPS_CS_OS_NAME},
#endif
#ifdef _MIPS_CS_OS_PROVIDER
    {"MIPS_CS_OS_PROVIDER",     _MIPS_CS_OS_PROVIDER},
#endif
#ifdef _MIPS_CS_PROCESSORS
    {"MIPS_CS_PROCESSORS",      _MIPS_CS_PROCESSORS},
#endif
#ifdef _MIPS_CS_SERIAL
    {"MIPS_CS_SERIAL",  _MIPS_CS_SERIAL},
#endif
#ifdef _MIPS_CS_VENDOR
    {"MIPS_CS_VENDOR",  _MIPS_CS_VENDOR},
#endif
};

static int
conv_confstr_confname(PyObject *arg, int *valuep)
{
    return conv_confname(arg, valuep, posix_constants_confstr,
                         sizeof(posix_constants_confstr)
                           / sizeof(struct constdef));
}


/*[clinic input]
os.confstr

    name: confstr_confname
    /

Return a string-valued system configuration variable.
[clinic start generated code]*/

static PyObject *
os_confstr_impl(PyObject *module, int name)
/*[clinic end generated code: output=bfb0b1b1e49b9383 input=18fb4d0567242e65]*/
{
    PyObject *result = NULL;
    char buffer[255];
    size_t len;

    errno = 0;
    len = confstr(name, buffer, sizeof(buffer));
    if (len == 0) {
        if (errno) {
            posix_error();
            return NULL;
        }
        else {
            Py_RETURN_NONE;
        }
    }

    if (len >= sizeof(buffer)) {
        size_t len2;
        char *buf = PyMem_Malloc(len);
        if (buf == NULL)
            return PyErr_NoMemory();
        len2 = confstr(name, buf, len);
        assert(len == len2);
        result = PyUnicode_DecodeFSDefaultAndSize(buf, len2-1);
        PyMem_Free(buf);
    }
    else
        result = PyUnicode_DecodeFSDefaultAndSize(buffer, len-1);
    return result;
}
#endif /* HAVE_CONFSTR */


#ifdef HAVE_SYSCONF
static struct constdef posix_constants_sysconf[] = {
#ifdef _SC_2_CHAR_TERM
    {"SC_2_CHAR_TERM",  _SC_2_CHAR_TERM},
#endif
#ifdef _SC_2_C_BIND
    {"SC_2_C_BIND",     _SC_2_C_BIND},
#endif
#ifdef _SC_2_C_DEV
    {"SC_2_C_DEV",      _SC_2_C_DEV},
#endif
#ifdef _SC_2_C_VERSION
    {"SC_2_C_VERSION",  _SC_2_C_VERSION},
#endif
#ifdef _SC_2_FORT_DEV
    {"SC_2_FORT_DEV",   _SC_2_FORT_DEV},
#endif
#ifdef _SC_2_FORT_RUN
    {"SC_2_FORT_RUN",   _SC_2_FORT_RUN},
#endif
#ifdef _SC_2_LOCALEDEF
    {"SC_2_LOCALEDEF",  _SC_2_LOCALEDEF},
#endif
#ifdef _SC_2_SW_DEV
    {"SC_2_SW_DEV",     _SC_2_SW_DEV},
#endif
#ifdef _SC_2_UPE
    {"SC_2_UPE",        _SC_2_UPE},
#endif
#ifdef _SC_2_VERSION
    {"SC_2_VERSION",    _SC_2_VERSION},
#endif
#ifdef _SC_ABI_ASYNCHRONOUS_IO
    {"SC_ABI_ASYNCHRONOUS_IO",  _SC_ABI_ASYNCHRONOUS_IO},
#endif
#ifdef _SC_ACL
    {"SC_ACL",  _SC_ACL},
#endif
#ifdef _SC_AIO_LISTIO_MAX
    {"SC_AIO_LISTIO_MAX",       _SC_AIO_LISTIO_MAX},
#endif
#ifdef _SC_AIO_MAX
    {"SC_AIO_MAX",      _SC_AIO_MAX},
#endif
#ifdef _SC_AIO_PRIO_DELTA_MAX
    {"SC_AIO_PRIO_DELTA_MAX",   _SC_AIO_PRIO_DELTA_MAX},
#endif
#ifdef _SC_ARG_MAX
    {"SC_ARG_MAX",      _SC_ARG_MAX},
#endif
#ifdef _SC_ASYNCHRONOUS_IO
    {"SC_ASYNCHRONOUS_IO",      _SC_ASYNCHRONOUS_IO},
#endif
#ifdef _SC_ATEXIT_MAX
    {"SC_ATEXIT_MAX",   _SC_ATEXIT_MAX},
#endif
#ifdef _SC_AUDIT
    {"SC_AUDIT",        _SC_AUDIT},
#endif
#ifdef _SC_AVPHYS_PAGES
    {"SC_AVPHYS_PAGES", _SC_AVPHYS_PAGES},
#endif
#ifdef _SC_BC_BASE_MAX
    {"SC_BC_BASE_MAX",  _SC_BC_BASE_MAX},
#endif
#ifdef _SC_BC_DIM_MAX
    {"SC_BC_DIM_MAX",   _SC_BC_DIM_MAX},
#endif
#ifdef _SC_BC_SCALE_MAX
    {"SC_BC_SCALE_MAX", _SC_BC_SCALE_MAX},
#endif
#ifdef _SC_BC_STRING_MAX
    {"SC_BC_STRING_MAX",        _SC_BC_STRING_MAX},
#endif
#ifdef _SC_CAP
    {"SC_CAP",  _SC_CAP},
#endif
#ifdef _SC_CHARCLASS_NAME_MAX
    {"SC_CHARCLASS_NAME_MAX",   _SC_CHARCLASS_NAME_MAX},
#endif
#ifdef _SC_CHAR_BIT
    {"SC_CHAR_BIT",     _SC_CHAR_BIT},
#endif
#ifdef _SC_CHAR_MAX
    {"SC_CHAR_MAX",     _SC_CHAR_MAX},
#endif
#ifdef _SC_CHAR_MIN
    {"SC_CHAR_MIN",     _SC_CHAR_MIN},
#endif
#ifdef _SC_CHILD_MAX
    {"SC_CHILD_MAX",    _SC_CHILD_MAX},
#endif
#ifdef _SC_CLK_TCK
    {"SC_CLK_TCK",      _SC_CLK_TCK},
#endif
#ifdef _SC_COHER_BLKSZ
    {"SC_COHER_BLKSZ",  _SC_COHER_BLKSZ},
#endif
#ifdef _SC_COLL_WEIGHTS_MAX
    {"SC_COLL_WEIGHTS_MAX",     _SC_COLL_WEIGHTS_MAX},
#endif
#ifdef _SC_DCACHE_ASSOC
    {"SC_DCACHE_ASSOC", _SC_DCACHE_ASSOC},
#endif
#ifdef _SC_DCACHE_BLKSZ
    {"SC_DCACHE_BLKSZ", _SC_DCACHE_BLKSZ},
#endif
#ifdef _SC_DCACHE_LINESZ
    {"SC_DCACHE_LINESZ",        _SC_DCACHE_LINESZ},
#endif
#ifdef _SC_DCACHE_SZ
    {"SC_DCACHE_SZ",    _SC_DCACHE_SZ},
#endif
#ifdef _SC_DCACHE_TBLKSZ
    {"SC_DCACHE_TBLKSZ",        _SC_DCACHE_TBLKSZ},
#endif
#ifdef _SC_DELAYTIMER_MAX
    {"SC_DELAYTIMER_MAX",       _SC_DELAYTIMER_MAX},
#endif
#ifdef _SC_EQUIV_CLASS_MAX
    {"SC_EQUIV_CLASS_MAX",      _SC_EQUIV_CLASS_MAX},
#endif
#ifdef _SC_EXPR_NEST_MAX
    {"SC_EXPR_NEST_MAX",        _SC_EXPR_NEST_MAX},
#endif
#ifdef _SC_FSYNC
    {"SC_FSYNC",        _SC_FSYNC},
#endif
#ifdef _SC_GETGR_R_SIZE_MAX
    {"SC_GETGR_R_SIZE_MAX",     _SC_GETGR_R_SIZE_MAX},
#endif
#ifdef _SC_GETPW_R_SIZE_MAX
    {"SC_GETPW_R_SIZE_MAX",     _SC_GETPW_R_SIZE_MAX},
#endif
#ifdef _SC_ICACHE_ASSOC
    {"SC_ICACHE_ASSOC", _SC_ICACHE_ASSOC},
#endif
#ifdef _SC_ICACHE_BLKSZ
    {"SC_ICACHE_BLKSZ", _SC_ICACHE_BLKSZ},
#endif
#ifdef _SC_ICACHE_LINESZ
    {"SC_ICACHE_LINESZ",        _SC_ICACHE_LINESZ},
#endif
#ifdef _SC_ICACHE_SZ
    {"SC_ICACHE_SZ",    _SC_ICACHE_SZ},
#endif
#ifdef _SC_INF
    {"SC_INF",  _SC_INF},
#endif
#ifdef _SC_INT_MAX
    {"SC_INT_MAX",      _SC_INT_MAX},
#endif
#ifdef _SC_INT_MIN
    {"SC_INT_MIN",      _SC_INT_MIN},
#endif
#ifdef _SC_IOV_MAX
    {"SC_IOV_MAX",      _SC_IOV_MAX},
#endif
#ifdef _SC_IP_SECOPTS
    {"SC_IP_SECOPTS",   _SC_IP_SECOPTS},
#endif
#ifdef _SC_JOB_CONTROL
    {"SC_JOB_CONTROL",  _SC_JOB_CONTROL},
#endif
#ifdef _SC_KERN_POINTERS
    {"SC_KERN_POINTERS",        _SC_KERN_POINTERS},
#endif
#ifdef _SC_KERN_SIM
    {"SC_KERN_SIM",     _SC_KERN_SIM},
#endif
#ifdef _SC_LINE_MAX
    {"SC_LINE_MAX",     _SC_LINE_MAX},
#endif
#ifdef _SC_LOGIN_NAME_MAX
    {"SC_LOGIN_NAME_MAX",       _SC_LOGIN_NAME_MAX},
#endif
#ifdef _SC_LOGNAME_MAX
    {"SC_LOGNAME_MAX",  _SC_LOGNAME_MAX},
#endif
#ifdef _SC_LONG_BIT
    {"SC_LONG_BIT",     _SC_LONG_BIT},
#endif
#ifdef _SC_MAC
    {"SC_MAC",  _SC_MAC},
#endif
#ifdef _SC_MAPPED_FILES
    {"SC_MAPPED_FILES", _SC_MAPPED_FILES},
#endif
#ifdef _SC_MAXPID
    {"SC_MAXPID",       _SC_MAXPID},
#endif
#ifdef _SC_MB_LEN_MAX
    {"SC_MB_LEN_MAX",   _SC_MB_LEN_MAX},
#endif
#ifdef _SC_MEMLOCK
    {"SC_MEMLOCK",      _SC_MEMLOCK},
#endif
#ifdef _SC_MEMLOCK_RANGE
    {"SC_MEMLOCK_RANGE",        _SC_MEMLOCK_RANGE},
#endif
#ifdef _SC_MEMORY_PROTECTION
    {"SC_MEMORY_PROTECTION",    _SC_MEMORY_PROTECTION},
#endif
#ifdef _SC_MESSAGE_PASSING
    {"SC_MESSAGE_PASSING",      _SC_MESSAGE_PASSING},
#endif
#ifdef _SC_MMAP_FIXED_ALIGNMENT
    {"SC_MMAP_FIXED_ALIGNMENT", _SC_MMAP_FIXED_ALIGNMENT},
#endif
#ifdef _SC_MQ_OPEN_MAX
    {"SC_MQ_OPEN_MAX",  _SC_MQ_OPEN_MAX},
#endif
#ifdef _SC_MQ_PRIO_MAX
    {"SC_MQ_PRIO_MAX",  _SC_MQ_PRIO_MAX},
#endif
#ifdef _SC_NACLS_MAX
    {"SC_NACLS_MAX",    _SC_NACLS_MAX},
#endif
#ifdef _SC_NGROUPS_MAX
    {"SC_NGROUPS_MAX",  _SC_NGROUPS_MAX},
#endif
#ifdef _SC_NL_ARGMAX
    {"SC_NL_ARGMAX",    _SC_NL_ARGMAX},
#endif
#ifdef _SC_NL_LANGMAX
    {"SC_NL_LANGMAX",   _SC_NL_LANGMAX},
#endif
#ifdef _SC_NL_MSGMAX
    {"SC_NL_MSGMAX",    _SC_NL_MSGMAX},
#endif
#ifdef _SC_NL_NMAX
    {"SC_NL_NMAX",      _SC_NL_NMAX},
#endif
#ifdef _SC_NL_SETMAX
    {"SC_NL_SETMAX",    _SC_NL_SETMAX},
#endif
#ifdef _SC_NL_TEXTMAX
    {"SC_NL_TEXTMAX",   _SC_NL_TEXTMAX},
#endif
#ifdef _SC_NPROCESSORS_CONF
    {"SC_NPROCESSORS_CONF",     _SC_NPROCESSORS_CONF},
#endif
#ifdef _SC_NPROCESSORS_ONLN
    {"SC_NPROCESSORS_ONLN",     _SC_NPROCESSORS_ONLN},
#endif
#ifdef _SC_NPROC_CONF
    {"SC_NPROC_CONF",   _SC_NPROC_CONF},
#endif
#ifdef _SC_NPROC_ONLN
    {"SC_NPROC_ONLN",   _SC_NPROC_ONLN},
#endif
#ifdef _SC_NZERO
    {"SC_NZERO",        _SC_NZERO},
#endif
#ifdef _SC_OPEN_MAX
    {"SC_OPEN_MAX",     _SC_OPEN_MAX},
#endif
#ifdef _SC_PAGESIZE
    {"SC_PAGESIZE",     _SC_PAGESIZE},
#endif
#ifdef _SC_PAGE_SIZE
    {"SC_PAGE_SIZE",    _SC_PAGE_SIZE},
#endif
#ifdef _SC_PASS_MAX
    {"SC_PASS_MAX",     _SC_PASS_MAX},
#endif
#ifdef _SC_PHYS_PAGES
    {"SC_PHYS_PAGES",   _SC_PHYS_PAGES},
#endif
#ifdef _SC_PII
    {"SC_PII",  _SC_PII},
#endif
#ifdef _SC_PII_INTERNET
    {"SC_PII_INTERNET", _SC_PII_INTERNET},
#endif
#ifdef _SC_PII_INTERNET_DGRAM
    {"SC_PII_INTERNET_DGRAM",   _SC_PII_INTERNET_DGRAM},
#endif
#ifdef _SC_PII_INTERNET_STREAM
    {"SC_PII_INTERNET_STREAM",  _SC_PII_INTERNET_STREAM},
#endif
#ifdef _SC_PII_OSI
    {"SC_PII_OSI",      _SC_PII_OSI},
#endif
#ifdef _SC_PII_OSI_CLTS
    {"SC_PII_OSI_CLTS", _SC_PII_OSI_CLTS},
#endif
#ifdef _SC_PII_OSI_COTS
    {"SC_PII_OSI_COTS", _SC_PII_OSI_COTS},
#endif
#ifdef _SC_PII_OSI_M
    {"SC_PII_OSI_M",    _SC_PII_OSI_M},
#endif
#ifdef _SC_PII_SOCKET
    {"SC_PII_SOCKET",   _SC_PII_SOCKET},
#endif
#ifdef _SC_PII_XTI
    {"SC_PII_XTI",      _SC_PII_XTI},
#endif
#ifdef _SC_POLL
    {"SC_POLL", _SC_POLL},
#endif
#ifdef _SC_PRIORITIZED_IO
    {"SC_PRIORITIZED_IO",       _SC_PRIORITIZED_IO},
#endif
#ifdef _SC_PRIORITY_SCHEDULING
    {"SC_PRIORITY_SCHEDULING",  _SC_PRIORITY_SCHEDULING},
#endif
#ifdef _SC_REALTIME_SIGNALS
    {"SC_REALTIME_SIGNALS",     _SC_REALTIME_SIGNALS},
#endif
#ifdef _SC_RE_DUP_MAX
    {"SC_RE_DUP_MAX",   _SC_RE_DUP_MAX},
#endif
#ifdef _SC_RTSIG_MAX
    {"SC_RTSIG_MAX",    _SC_RTSIG_MAX},
#endif
#ifdef _SC_SAVED_IDS
    {"SC_SAVED_IDS",    _SC_SAVED_IDS},
#endif
#ifdef _SC_SCHAR_MAX
    {"SC_SCHAR_MAX",    _SC_SCHAR_MAX},
#endif
#ifdef _SC_SCHAR_MIN
    {"SC_SCHAR_MIN",    _SC_SCHAR_MIN},
#endif
#ifdef _SC_SELECT
    {"SC_SELECT",       _SC_SELECT},
#endif
#ifdef _SC_SEMAPHORES
    {"SC_SEMAPHORES",   _SC_SEMAPHORES},
#endif
#ifdef _SC_SEM_NSEMS_MAX
    {"SC_SEM_NSEMS_MAX",        _SC_SEM_NSEMS_MAX},
#endif
#ifdef _SC_SEM_VALUE_MAX
    {"SC_SEM_VALUE_MAX",        _SC_SEM_VALUE_MAX},
#endif
#ifdef _SC_SHARED_MEMORY_OBJECTS
    {"SC_SHARED_MEMORY_OBJECTS",        _SC_SHARED_MEMORY_OBJECTS},
#endif
#ifdef _SC_SHRT_MAX
    {"SC_SHRT_MAX",     _SC_SHRT_MAX},
#endif
#ifdef _SC_SHRT_MIN
    {"SC_SHRT_MIN",     _SC_SHRT_MIN},
#endif
#ifdef _SC_SIGQUEUE_MAX
    {"SC_SIGQUEUE_MAX", _SC_SIGQUEUE_MAX},
#endif
#ifdef _SC_SIGRT_MAX
    {"SC_SIGRT_MAX",    _SC_SIGRT_MAX},
#endif
#ifdef _SC_SIGRT_MIN
    {"SC_SIGRT_MIN",    _SC_SIGRT_MIN},
#endif
#ifdef _SC_SOFTPOWER
    {"SC_SOFTPOWER",    _SC_SOFTPOWER},
#endif
#ifdef _SC_SPLIT_CACHE
    {"SC_SPLIT_CACHE",  _SC_SPLIT_CACHE},
#endif
#ifdef _SC_SSIZE_MAX
    {"SC_SSIZE_MAX",    _SC_SSIZE_MAX},
#endif
#ifdef _SC_STACK_PROT
    {"SC_STACK_PROT",   _SC_STACK_PROT},
#endif
#ifdef _SC_STREAM_MAX
    {"SC_STREAM_MAX",   _SC_STREAM_MAX},
#endif
#ifdef _SC_SYNCHRONIZED_IO
    {"SC_SYNCHRONIZED_IO",      _SC_SYNCHRONIZED_IO},
#endif
#ifdef _SC_THREADS
    {"SC_THREADS",      _SC_THREADS},
#endif
#ifdef _SC_THREAD_ATTR_STACKADDR
    {"SC_THREAD_ATTR_STACKADDR",        _SC_THREAD_ATTR_STACKADDR},
#endif
#ifdef _SC_THREAD_ATTR_STACKSIZE
    {"SC_THREAD_ATTR_STACKSIZE",        _SC_THREAD_ATTR_STACKSIZE},
#endif
#ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
    {"SC_THREAD_DESTRUCTOR_ITERATIONS", _SC_THREAD_DESTRUCTOR_ITERATIONS},
#endif
#ifdef _SC_THREAD_KEYS_MAX
    {"SC_THREAD_KEYS_MAX",      _SC_THREAD_KEYS_MAX},
#endif
#ifdef _SC_THREAD_PRIORITY_SCHEDULING
    {"SC_THREAD_PRIORITY_SCHEDULING",   _SC_THREAD_PRIORITY_SCHEDULING},
#endif
#ifdef _SC_THREAD_PRIO_INHERIT
    {"SC_THREAD_PRIO_INHERIT",  _SC_THREAD_PRIO_INHERIT},
#endif
#ifdef _SC_THREAD_PRIO_PROTECT
    {"SC_THREAD_PRIO_PROTECT",  _SC_THREAD_PRIO_PROTECT},
#endif
#ifdef _SC_THREAD_PROCESS_SHARED
    {"SC_THREAD_PROCESS_SHARED",        _SC_THREAD_PROCESS_SHARED},
#endif
#ifdef _SC_THREAD_SAFE_FUNCTIONS
    {"SC_THREAD_SAFE_FUNCTIONS",        _SC_THREAD_SAFE_FUNCTIONS},
#endif
#ifdef _SC_THREAD_STACK_MIN
    {"SC_THREAD_STACK_MIN",     _SC_THREAD_STACK_MIN},
#endif
#ifdef _SC_THREAD_THREADS_MAX
    {"SC_THREAD_THREADS_MAX",   _SC_THREAD_THREADS_MAX},
#endif
#ifdef _SC_TIMERS
    {"SC_TIMERS",       _SC_TIMERS},
#endif
#ifdef _SC_TIMER_MAX
    {"SC_TIMER_MAX",    _SC_TIMER_MAX},
#endif
#ifdef _SC_TTY_NAME_MAX
    {"SC_TTY_NAME_MAX", _SC_TTY_NAME_MAX},
#endif
#ifdef _SC_TZNAME_MAX
    {"SC_TZNAME_MAX",   _SC_TZNAME_MAX},
#endif
#ifdef _SC_T_IOV_MAX
    {"SC_T_IOV_MAX",    _SC_T_IOV_MAX},
#endif
#ifdef _SC_UCHAR_MAX
    {"SC_UCHAR_MAX",    _SC_UCHAR_MAX},
#endif
#ifdef _SC_UINT_MAX
    {"SC_UINT_MAX",     _SC_UINT_MAX},
#endif
#ifdef _SC_UIO_MAXIOV
    {"SC_UIO_MAXIOV",   _SC_UIO_MAXIOV},
#endif
#ifdef _SC_ULONG_MAX
    {"SC_ULONG_MAX",    _SC_ULONG_MAX},
#endif
#ifdef _SC_USHRT_MAX
    {"SC_USHRT_MAX",    _SC_USHRT_MAX},
#endif
#ifdef _SC_VERSION
    {"SC_VERSION",      _SC_VERSION},
#endif
#ifdef _SC_WORD_BIT
    {"SC_WORD_BIT",     _SC_WORD_BIT},
#endif
#ifdef _SC_XBS5_ILP32_OFF32
    {"SC_XBS5_ILP32_OFF32",     _SC_XBS5_ILP32_OFF32},
#endif
#ifdef _SC_XBS5_ILP32_OFFBIG
    {"SC_XBS5_ILP32_OFFBIG",    _SC_XBS5_ILP32_OFFBIG},
#endif
#ifdef _SC_XBS5_LP64_OFF64
    {"SC_XBS5_LP64_OFF64",      _SC_XBS5_LP64_OFF64},
#endif
#ifdef _SC_XBS5_LPBIG_OFFBIG
    {"SC_XBS5_LPBIG_OFFBIG",    _SC_XBS5_LPBIG_OFFBIG},
#endif
#ifdef _SC_XOPEN_CRYPT
    {"SC_XOPEN_CRYPT",  _SC_XOPEN_CRYPT},
#endif
#ifdef _SC_XOPEN_ENH_I18N
    {"SC_XOPEN_ENH_I18N",       _SC_XOPEN_ENH_I18N},
#endif
#ifdef _SC_XOPEN_LEGACY
    {"SC_XOPEN_LEGACY", _SC_XOPEN_LEGACY},
#endif
#ifdef _SC_XOPEN_REALTIME
    {"SC_XOPEN_REALTIME",       _SC_XOPEN_REALTIME},
#endif
#ifdef _SC_XOPEN_REALTIME_THREADS
    {"SC_XOPEN_REALTIME_THREADS",       _SC_XOPEN_REALTIME_THREADS},
#endif
#ifdef _SC_XOPEN_SHM
    {"SC_XOPEN_SHM",    _SC_XOPEN_SHM},
#endif
#ifdef _SC_XOPEN_UNIX
    {"SC_XOPEN_UNIX",   _SC_XOPEN_UNIX},
#endif
#ifdef _SC_XOPEN_VERSION
    {"SC_XOPEN_VERSION",        _SC_XOPEN_VERSION},
#endif
#ifdef _SC_XOPEN_XCU_VERSION
    {"SC_XOPEN_XCU_VERSION",    _SC_XOPEN_XCU_VERSION},
#endif
#ifdef _SC_XOPEN_XPG2
    {"SC_XOPEN_XPG2",   _SC_XOPEN_XPG2},
#endif
#ifdef _SC_XOPEN_XPG3
    {"SC_XOPEN_XPG3",   _SC_XOPEN_XPG3},
#endif
#ifdef _SC_XOPEN_XPG4
    {"SC_XOPEN_XPG4",   _SC_XOPEN_XPG4},
#endif
};

static int
conv_sysconf_confname(PyObject *arg, int *valuep)
{
    return conv_confname(arg, valuep, posix_constants_sysconf,
                         sizeof(posix_constants_sysconf)
                           / sizeof(struct constdef));
}


/*[clinic input]
os.sysconf -> long
    name: sysconf_confname
    /

Return an integer-valued system configuration variable.
[clinic start generated code]*/

static long
os_sysconf_impl(PyObject *module, int name)
/*[clinic end generated code: output=3662f945fc0cc756 input=279e3430a33f29e4]*/
{
    long value;

    errno = 0;
    value = sysconf(name);
    if (value == -1 && errno != 0)
        posix_error();
    return value;
}
#endif /* HAVE_SYSCONF */


/* This code is used to ensure that the tables of configuration value names
 * are in sorted order as required by conv_confname(), and also to build
 * the exported dictionaries that are used to publish information about the
 * names available on the host platform.
 *
 * Sorting the table at runtime ensures that the table is properly ordered
 * when used, even for platforms we're not able to test on.  It also makes
 * it easier to add additional entries to the tables.
 */

static int
cmp_constdefs(const void *v1,  const void *v2)
{
    const struct constdef *c1 =
    (const struct constdef *) v1;
    const struct constdef *c2 =
    (const struct constdef *) v2;

    return strcmp(c1->name, c2->name);
}

static int
setup_confname_table(struct constdef *table, size_t tablesize,
                     const char *tablename, PyObject *module)
{
    PyObject *d = NULL;
    size_t i;

    qsort(table, tablesize, sizeof(struct constdef), cmp_constdefs);
    d = PyDict_New();
    if (d == NULL)
        return -1;

    for (i=0; i < tablesize; ++i) {
        PyObject *o = PyLong_FromLong(table[i].value);
        if (o == NULL || PyDict_SetItemString(d, table[i].name, o) == -1) {
            Py_XDECREF(o);
            Py_DECREF(d);
            return -1;
        }
        Py_DECREF(o);
    }
    return PyModule_AddObject(module, tablename, d);
}

/* Return -1 on failure, 0 on success. */
static int
setup_confname_tables(PyObject *module)
{
#if defined(HAVE_FPATHCONF) || defined(HAVE_PATHCONF)
    if (setup_confname_table(posix_constants_pathconf,
                             sizeof(posix_constants_pathconf)
                               / sizeof(struct constdef),
                             "pathconf_names", module))
        return -1;
#endif
#ifdef HAVE_CONFSTR
    if (setup_confname_table(posix_constants_confstr,
                             sizeof(posix_constants_confstr)
                               / sizeof(struct constdef),
                             "confstr_names", module))
        return -1;
#endif
#ifdef HAVE_SYSCONF
    if (setup_confname_table(posix_constants_sysconf,
                             sizeof(posix_constants_sysconf)
                               / sizeof(struct constdef),
                             "sysconf_names", module))
        return -1;
#endif
    return 0;
}


/*[clinic input]
os.abort

Abort the interpreter immediately.

This function 'dumps core' or otherwise fails in the hardest way possible
on the hosting operating system.  This function never returns.
[clinic start generated code]*/

static PyObject *
os_abort_impl(PyObject *module)
/*[clinic end generated code: output=dcf52586dad2467c input=cf2c7d98bc504047]*/
{
    abort();
    /*NOTREACHED*/
    Py_FatalError("abort() called from Python code didn't abort!");
    return NULL;
}

#ifdef MS_WINDOWS
/* Grab ShellExecute dynamically from shell32 */
static int has_ShellExecute = -1;
static HINSTANCE (CALLBACK *Py_ShellExecuteW)(HWND, LPCWSTR, LPCWSTR, LPCWSTR,
                                              LPCWSTR, INT);
static int
check_ShellExecute()
{
    HINSTANCE hShell32;

    /* only recheck */
    if (-1 == has_ShellExecute) {
        Py_BEGIN_ALLOW_THREADS
        hShell32 = LoadLibraryW(L"SHELL32");
        Py_END_ALLOW_THREADS
        if (hShell32) {
            *(FARPROC*)&Py_ShellExecuteW = GetProcAddress(hShell32,
                                            "ShellExecuteW");
            has_ShellExecute = Py_ShellExecuteW != NULL;
        } else {
            has_ShellExecute = 0;
        }
    }
    return has_ShellExecute;
}


/*[clinic input]
os.startfile
    filepath: path_t
    operation: Py_UNICODE = NULL

startfile(filepath [, operation])

Start a file with its associated application.

When "operation" is not specified or "open", this acts like
double-clicking the file in Explorer, or giving the file name as an
argument to the DOS "start" command: the file is opened with whatever
application (if any) its extension is associated.
When another "operation" is given, it specifies what should be done with
the file.  A typical operation is "print".

startfile returns as soon as the associated application is launched.
There is no option to wait for the application to close, and no way
to retrieve the application's exit status.

The filepath is relative to the current directory.  If you want to use
an absolute path, make sure the first character is not a slash ("/");
the underlying Win32 ShellExecute function doesn't work if it is.
[clinic start generated code]*/

static PyObject *
os_startfile_impl(PyObject *module, path_t *filepath, Py_UNICODE *operation)
/*[clinic end generated code: output=912ceba79acfa1c9 input=63950bf2986380d0]*/
{
    HINSTANCE rc;

    if(!check_ShellExecute()) {
        /* If the OS doesn't have ShellExecute, return a
           NotImplementedError. */
        return PyErr_Format(PyExc_NotImplementedError,
            "startfile not available on this platform");
    }

    Py_BEGIN_ALLOW_THREADS
    rc = Py_ShellExecuteW((HWND)0, operation, filepath->wide,
                          NULL, NULL, SW_SHOWNORMAL);
    Py_END_ALLOW_THREADS

    if (rc <= (HINSTANCE)32) {
        win32_error_object("startfile", filepath->object);
        return NULL;
    }
    Py_RETURN_NONE;
}
#endif /* MS_WINDOWS */


#ifdef HAVE_GETLOADAVG
/*[clinic input]
os.getloadavg

Return average recent system load information.

Return the number of processes in the system run queue averaged over
the last 1, 5, and 15 minutes as a tuple of three floats.
Raises OSError if the load average was unobtainable.
[clinic start generated code]*/

static PyObject *
os_getloadavg_impl(PyObject *module)
/*[clinic end generated code: output=9ad3a11bfb4f4bd2 input=3d6d826b76d8a34e]*/
{
    double loadavg[3];
    if (getloadavg(loadavg, 3)!=3) {
        PyErr_SetString(PyExc_OSError, "Load averages are unobtainable");
        return NULL;
    } else
        return Py_BuildValue("ddd", loadavg[0], loadavg[1], loadavg[2]);
}
#endif /* HAVE_GETLOADAVG */


/*[clinic input]
os.device_encoding
    fd: int

Return a string describing the encoding of a terminal's file descriptor.

The file descriptor must be attached to a terminal.
If the device is not a terminal, return None.
[clinic start generated code]*/

static PyObject *
os_device_encoding_impl(PyObject *module, int fd)
/*[clinic end generated code: output=e0d294bbab7e8c2b input=9e1d4a42b66df312]*/
{
    return _Py_device_encoding(fd);
}


#ifdef HAVE_SETRESUID
/*[clinic input]
os.setresuid

    ruid: uid_t
    euid: uid_t
    suid: uid_t
    /

Set the current process's real, effective, and saved user ids.
[clinic start generated code]*/

static PyObject *
os_setresuid_impl(PyObject *module, uid_t ruid, uid_t euid, uid_t suid)
/*[clinic end generated code: output=834a641e15373e97 input=9e33cb79a82792f3]*/
{
    if (setresuid(ruid, euid, suid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETRESUID */


#ifdef HAVE_SETRESGID
/*[clinic input]
os.setresgid

    rgid: gid_t
    egid: gid_t
    sgid: gid_t
    /

Set the current process's real, effective, and saved group ids.
[clinic start generated code]*/

static PyObject *
os_setresgid_impl(PyObject *module, gid_t rgid, gid_t egid, gid_t sgid)
/*[clinic end generated code: output=6aa402f3d2e514a9 input=33e9e0785ef426b1]*/
{
    if (setresgid(rgid, egid, sgid) < 0)
        return posix_error();
    Py_RETURN_NONE;
}
#endif /* HAVE_SETRESGID */


#ifdef HAVE_GETRESUID
/*[clinic input]
os.getresuid

Return a tuple of the current process's real, effective, and saved user ids.
[clinic start generated code]*/

static PyObject *
os_getresuid_impl(PyObject *module)
/*[clinic end generated code: output=8e0becff5dece5bf input=41ccfa8e1f6517ad]*/
{
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) < 0)
        return posix_error();
    return Py_BuildValue("(NNN)", _PyLong_FromUid(ruid),
                                  _PyLong_FromUid(euid),
                                  _PyLong_FromUid(suid));
}
#endif /* HAVE_GETRESUID */


#ifdef HAVE_GETRESGID
/*[clinic input]
os.getresgid

Return a tuple of the current process's real, effective, and saved group ids.
[clinic start generated code]*/

static PyObject *
os_getresgid_impl(PyObject *module)
/*[clinic end generated code: output=2719c4bfcf27fb9f input=517e68db9ca32df6]*/
{
    gid_t rgid, egid, sgid;
    if (getresgid(&rgid, &egid, &sgid) < 0)
        return posix_error();
    return Py_BuildValue("(NNN)", _PyLong_FromGid(rgid),
                                  _PyLong_FromGid(egid),
                                  _PyLong_FromGid(sgid));
}
#endif /* HAVE_GETRESGID */


#ifdef USE_XATTRS
/*[clinic input]
os.getxattr

    path: path_t(allow_fd=True)
    attribute: path_t
    *
    follow_symlinks: bool = True

Return the value of extended attribute attribute on path.

path may be either a string or an open file descriptor.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, getxattr will examine the symbolic link itself instead of the file
  the link points to.

[clinic start generated code]*/

static PyObject *
os_getxattr_impl(PyObject *module, path_t *path, path_t *attribute,
                 int follow_symlinks)
/*[clinic end generated code: output=5f2f44200a43cff2 input=8c8ea3bab78d89c2]*/
{
    Py_ssize_t i;
    PyObject *buffer = NULL;

    if (fd_and_follow_symlinks_invalid("getxattr", path->fd, follow_symlinks))
        return NULL;

    for (i = 0; ; i++) {
        void *ptr;
        ssize_t result;
        static const Py_ssize_t buffer_sizes[] = {128, XATTR_SIZE_MAX, 0};
        Py_ssize_t buffer_size = buffer_sizes[i];
        if (!buffer_size) {
            path_error(path);
            return NULL;
        }
        buffer = PyBytes_FromStringAndSize(NULL, buffer_size);
        if (!buffer)
            return NULL;
        ptr = PyBytes_AS_STRING(buffer);

        Py_BEGIN_ALLOW_THREADS;
        if (path->fd >= 0)
            result = fgetxattr(path->fd, attribute->narrow, ptr, buffer_size);
        else if (follow_symlinks)
            result = getxattr(path->narrow, attribute->narrow, ptr, buffer_size);
        else
            result = lgetxattr(path->narrow, attribute->narrow, ptr, buffer_size);
        Py_END_ALLOW_THREADS;

        if (result < 0) {
            Py_DECREF(buffer);
            if (errno == ERANGE)
                continue;
            path_error(path);
            return NULL;
        }

        if (result != buffer_size) {
            /* Can only shrink. */
            _PyBytes_Resize(&buffer, result);
        }
        break;
    }

    return buffer;
}


/*[clinic input]
os.setxattr

    path: path_t(allow_fd=True)
    attribute: path_t
    value: Py_buffer
    flags: int = 0
    *
    follow_symlinks: bool = True

Set extended attribute attribute on path to value.

path may be either a string or an open file descriptor.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, setxattr will modify the symbolic link itself instead of the file
  the link points to.

[clinic start generated code]*/

static PyObject *
os_setxattr_impl(PyObject *module, path_t *path, path_t *attribute,
                 Py_buffer *value, int flags, int follow_symlinks)
/*[clinic end generated code: output=98b83f63fdde26bb input=f0d26833992015c2]*/
{
    ssize_t result;

    if (fd_and_follow_symlinks_invalid("setxattr", path->fd, follow_symlinks))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    if (path->fd > -1)
        result = fsetxattr(path->fd, attribute->narrow,
                           value->buf, value->len, flags);
    else if (follow_symlinks)
        result = setxattr(path->narrow, attribute->narrow,
                           value->buf, value->len, flags);
    else
        result = lsetxattr(path->narrow, attribute->narrow,
                           value->buf, value->len, flags);
    Py_END_ALLOW_THREADS;

    if (result) {
        path_error(path);
        return NULL;
    }

    Py_RETURN_NONE;
}


/*[clinic input]
os.removexattr

    path: path_t(allow_fd=True)
    attribute: path_t
    *
    follow_symlinks: bool = True

Remove extended attribute attribute on path.

path may be either a string or an open file descriptor.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, removexattr will modify the symbolic link itself instead of the file
  the link points to.

[clinic start generated code]*/

static PyObject *
os_removexattr_impl(PyObject *module, path_t *path, path_t *attribute,
                    int follow_symlinks)
/*[clinic end generated code: output=521a51817980cda6 input=cdb54834161e3329]*/
{
    ssize_t result;

    if (fd_and_follow_symlinks_invalid("removexattr", path->fd, follow_symlinks))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    if (path->fd > -1)
        result = fremovexattr(path->fd, attribute->narrow);
    else if (follow_symlinks)
        result = removexattr(path->narrow, attribute->narrow);
    else
        result = lremovexattr(path->narrow, attribute->narrow);
    Py_END_ALLOW_THREADS;

    if (result) {
        return path_error(path);
    }

    Py_RETURN_NONE;
}


/*[clinic input]
os.listxattr

    path: path_t(allow_fd=True, nullable=True) = None
    *
    follow_symlinks: bool = True

Return a list of extended attributes on path.

path may be either None, a string, or an open file descriptor.
if path is None, listxattr will examine the current directory.
If follow_symlinks is False, and the last element of the path is a symbolic
  link, listxattr will examine the symbolic link itself instead of the file
  the link points to.
[clinic start generated code]*/

static PyObject *
os_listxattr_impl(PyObject *module, path_t *path, int follow_symlinks)
/*[clinic end generated code: output=bebdb4e2ad0ce435 input=08cca53ac0b07c13]*/
{
    Py_ssize_t i;
    PyObject *result = NULL;
    const char *name;
    char *buffer = NULL;

    if (fd_and_follow_symlinks_invalid("listxattr", path->fd, follow_symlinks))
        goto exit;

    name = path->narrow ? path->narrow : ".";

    for (i = 0; ; i++) {
        const char *start, *trace, *end;
        ssize_t length;
        static const Py_ssize_t buffer_sizes[] = { 256, XATTR_LIST_MAX, 0 };
        Py_ssize_t buffer_size = buffer_sizes[i];
        if (!buffer_size) {
            /* ERANGE */
            path_error(path);
            break;
        }
        buffer = PyMem_MALLOC(buffer_size);
        if (!buffer) {
            PyErr_NoMemory();
            break;
        }

        Py_BEGIN_ALLOW_THREADS;
        if (path->fd > -1)
            length = flistxattr(path->fd, buffer, buffer_size);
        else if (follow_symlinks)
            length = listxattr(name, buffer, buffer_size);
        else
            length = llistxattr(name, buffer, buffer_size);
        Py_END_ALLOW_THREADS;

        if (length < 0) {
            if (errno == ERANGE) {
                PyMem_FREE(buffer);
                buffer = NULL;
                continue;
            }
            path_error(path);
            break;
        }

        result = PyList_New(0);
        if (!result) {
            goto exit;
        }

        end = buffer + length;
        for (trace = start = buffer; trace != end; trace++) {
            if (!*trace) {
                int error;
                PyObject *attribute = PyUnicode_DecodeFSDefaultAndSize(start,
                                                                 trace - start);
                if (!attribute) {
                    Py_DECREF(result);
                    result = NULL;
                    goto exit;
                }
                error = PyList_Append(result, attribute);
                Py_DECREF(attribute);
                if (error) {
                    Py_DECREF(result);
                    result = NULL;
                    goto exit;
                }
                start = trace + 1;
            }
        }
    break;
    }
exit:
    if (buffer)
        PyMem_FREE(buffer);
    return result;
}
#endif /* USE_XATTRS */


/*[clinic input]
os.urandom

    size: Py_ssize_t
    /

Return a bytes object containing random bytes suitable for cryptographic use.
[clinic start generated code]*/

static PyObject *
os_urandom_impl(PyObject *module, Py_ssize_t size)
/*[clinic end generated code: output=42c5cca9d18068e9 input=4067cdb1b6776c29]*/
{
    PyObject *bytes;
    int result;

    if (size < 0)
        return PyErr_Format(PyExc_ValueError,
                            "negative argument not allowed");
    bytes = PyBytes_FromStringAndSize(NULL, size);
    if (bytes == NULL)
        return NULL;

    result = _PyOS_URandom(PyBytes_AS_STRING(bytes), PyBytes_GET_SIZE(bytes));
    if (result == -1) {
        Py_DECREF(bytes);
        return NULL;
    }
    return bytes;
}

/* Terminal size querying */

static PyTypeObject TerminalSizeType;

PyDoc_STRVAR(TerminalSize_docstring,
    "A tuple of (columns, lines) for holding terminal window size");

static PyStructSequence_Field TerminalSize_fields[] = {
    {"columns", "width of the terminal window in characters"},
    {"lines", "height of the terminal window in characters"},
    {NULL, NULL}
};

static PyStructSequence_Desc TerminalSize_desc = {
    "os.terminal_size",
    TerminalSize_docstring,
    TerminalSize_fields,
    2,
};

#if defined(TERMSIZE_USE_CONIO) || defined(TERMSIZE_USE_IOCTL)
/* AC 3.5: fd should accept None */
PyDoc_STRVAR(termsize__doc__,
    "Return the size of the terminal window as (columns, lines).\n"        \
    "\n"                                                                   \
    "The optional argument fd (default standard output) specifies\n"       \
    "which file descriptor should be queried.\n"                           \
    "\n"                                                                   \
    "If the file descriptor is not connected to a terminal, an OSError\n"  \
    "is thrown.\n"                                                         \
    "\n"                                                                   \
    "This function will only be defined if an implementation is\n"         \
    "available for this system.\n"                                         \
    "\n"                                                                   \
    "shutil.get_terminal_size is the high-level function which should \n"  \
    "normally be used, os.get_terminal_size is the low-level implementation.");

static PyObject*
get_terminal_size(PyObject *self, PyObject *args)
{
    int columns, lines;
    PyObject *termsize;

    int fd = fileno(stdout);
    /* Under some conditions stdout may not be connected and
     * fileno(stdout) may point to an invalid file descriptor. For example
     * GUI apps don't have valid standard streams by default.
     *
     * If this happens, and the optional fd argument is not present,
     * the ioctl below will fail returning EBADF. This is what we want.
     */

    if (!PyArg_ParseTuple(args, "|i", &fd))
        return NULL;

#ifdef TERMSIZE_USE_IOCTL
    {
        struct winsize w;
        if (ioctl(fd, TIOCGWINSZ, &w))
            return PyErr_SetFromErrno(PyExc_OSError);
        columns = w.ws_col;
        lines = w.ws_row;
    }
#endif /* TERMSIZE_USE_IOCTL */

#ifdef TERMSIZE_USE_CONIO
    {
        DWORD nhandle;
        HANDLE handle;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        switch (fd) {
        case 0: nhandle = STD_INPUT_HANDLE;
            break;
        case 1: nhandle = STD_OUTPUT_HANDLE;
            break;
        case 2: nhandle = STD_ERROR_HANDLE;
            break;
        default:
            return PyErr_Format(PyExc_ValueError, "bad file descriptor");
        }
        handle = GetStdHandle(nhandle);
        if (handle == NULL)
            return PyErr_Format(PyExc_OSError, "handle cannot be retrieved");
        if (handle == INVALID_HANDLE_VALUE)
            return PyErr_SetFromWindowsErr(0);

        if (!GetConsoleScreenBufferInfo(handle, &csbi))
            return PyErr_SetFromWindowsErr(0);

        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif /* TERMSIZE_USE_CONIO */

    termsize = PyStructSequence_New(&TerminalSizeType);
    if (termsize == NULL)
        return NULL;
    PyStructSequence_SET_ITEM(termsize, 0, PyLong_FromLong(columns));
    PyStructSequence_SET_ITEM(termsize, 1, PyLong_FromLong(lines));
    if (PyErr_Occurred()) {
        Py_DECREF(termsize);
        return NULL;
    }
    return termsize;
}
#endif /* defined(TERMSIZE_USE_CONIO) || defined(TERMSIZE_USE_IOCTL) */


/*[clinic input]
os.cpu_count

Return the number of CPUs in the system; return None if indeterminable.

This number is not equivalent to the number of CPUs the current process can
use.  The number of usable CPUs can be obtained with
``len(os.sched_getaffinity(0))``
[clinic start generated code]*/

static PyObject *
os_cpu_count_impl(PyObject *module)
/*[clinic end generated code: output=5fc29463c3936a9c input=e7c8f4ba6dbbadd3]*/
{
    int ncpu = 0;
#ifdef MS_WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    ncpu = sysinfo.dwNumberOfProcessors;
#elif defined(__hpux)
    ncpu = mpctl(MPC_GETNUMSPUS, NULL, NULL);
#elif defined(HAVE_SYSCONF) && defined(_SC_NPROCESSORS_ONLN)
    ncpu = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(__DragonFly__) || \
      defined(__OpenBSD__)   || \
      defined(__FreeBSD__)   || \
      defined(__NetBSD__)    || \
      defined(__APPLE__)
    int mib[2];
    size_t len = sizeof(ncpu);
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    if (sysctl(mib, 2, &ncpu, &len, NULL, 0) != 0)
        ncpu = 0;
#endif
    if (ncpu >= 1)
        return PyLong_FromLong(ncpu);
    else
        Py_RETURN_NONE;
}


/*[clinic input]
os.get_inheritable -> bool

    fd: int
    /

Get the close-on-exe flag of the specified file descriptor.
[clinic start generated code]*/

static int
os_get_inheritable_impl(PyObject *module, int fd)
/*[clinic end generated code: output=0445e20e149aa5b8 input=89ac008dc9ab6b95]*/
{
    int return_value;
    _Py_BEGIN_SUPPRESS_IPH
    return_value = _Py_get_inheritable(fd);
    _Py_END_SUPPRESS_IPH
    return return_value;
}


/*[clinic input]
os.set_inheritable
    fd: int
    inheritable: int
    /

Set the inheritable flag of the specified file descriptor.
[clinic start generated code]*/

static PyObject *
os_set_inheritable_impl(PyObject *module, int fd, int inheritable)
/*[clinic end generated code: output=f1b1918a2f3c38c2 input=9ceaead87a1e2402]*/
{
    int result;

    _Py_BEGIN_SUPPRESS_IPH
    result = _Py_set_inheritable(fd, inheritable, NULL);
    _Py_END_SUPPRESS_IPH
    if (result < 0)
        return NULL;
    Py_RETURN_NONE;
}


#ifdef MS_WINDOWS
/*[clinic input]
os.get_handle_inheritable -> bool
    handle: intptr_t
    /

Get the close-on-exe flag of the specified file descriptor.
[clinic start generated code]*/

static int
os_get_handle_inheritable_impl(PyObject *module, intptr_t handle)
/*[clinic end generated code: output=36be5afca6ea84d8 input=cfe99f9c05c70ad1]*/
{
    DWORD flags;

    if (!GetHandleInformation((HANDLE)handle, &flags)) {
        PyErr_SetFromWindowsErr(0);
        return -1;
    }

    return flags & HANDLE_FLAG_INHERIT;
}


/*[clinic input]
os.set_handle_inheritable
    handle: intptr_t
    inheritable: bool
    /

Set the inheritable flag of the specified handle.
[clinic start generated code]*/

static PyObject *
os_set_handle_inheritable_impl(PyObject *module, intptr_t handle,
                               int inheritable)
/*[clinic end generated code: output=021d74fe6c96baa3 input=7a7641390d8364fc]*/
{
    DWORD flags = inheritable ? HANDLE_FLAG_INHERIT : 0;
    if (!SetHandleInformation((HANDLE)handle, HANDLE_FLAG_INHERIT, flags)) {
        PyErr_SetFromWindowsErr(0);
        return NULL;
    }
    Py_RETURN_NONE;
}
#endif /* MS_WINDOWS */

#ifndef MS_WINDOWS
PyDoc_STRVAR(get_blocking__doc__,
    "get_blocking(fd) -> bool\n" \
    "\n" \
    "Get the blocking mode of the file descriptor:\n" \
    "False if the O_NONBLOCK flag is set, True if the flag is cleared.");

static PyObject*
posix_get_blocking(PyObject *self, PyObject *args)
{
    int fd;
    int blocking;

    if (!PyArg_ParseTuple(args, "i:get_blocking", &fd))
        return NULL;

    _Py_BEGIN_SUPPRESS_IPH
    blocking = _Py_get_blocking(fd);
    _Py_END_SUPPRESS_IPH
    if (blocking < 0)
        return NULL;
    return PyBool_FromLong(blocking);
}

PyDoc_STRVAR(set_blocking__doc__,
    "set_blocking(fd, blocking)\n" \
    "\n" \
    "Set the blocking mode of the specified file descriptor.\n" \
    "Set the O_NONBLOCK flag if blocking is False,\n" \
    "clear the O_NONBLOCK flag otherwise.");

static PyObject*
posix_set_blocking(PyObject *self, PyObject *args)
{
    int fd, blocking, result;

    if (!PyArg_ParseTuple(args, "ii:set_blocking", &fd, &blocking))
        return NULL;

    _Py_BEGIN_SUPPRESS_IPH
    result = _Py_set_blocking(fd, blocking);
    _Py_END_SUPPRESS_IPH
    if (result < 0)
        return NULL;
    Py_RETURN_NONE;
}
#endif   /* !MS_WINDOWS */


PyDoc_STRVAR(posix_scandir__doc__,
"scandir(path='.') -> iterator of DirEntry objects for given path");

static char *follow_symlinks_keywords[] = {"follow_symlinks", NULL};

typedef struct {
    PyObject_HEAD
    PyObject *name;
    PyObject *path;
    PyObject *stat;
    PyObject *lstat;
#ifdef MS_WINDOWS
    struct _Py_stat_struct win32_lstat;
    __int64 win32_file_index;
    int got_file_index;
#else /* POSIX */
#ifdef HAVE_DIRENT_D_TYPE
    unsigned char d_type;
#endif
    ino_t d_ino;
#endif
} DirEntry;

static void
DirEntry_dealloc(DirEntry *entry)
{
    Py_XDECREF(entry->name);
    Py_XDECREF(entry->path);
    Py_XDECREF(entry->stat);
    Py_XDECREF(entry->lstat);
    Py_TYPE(entry)->tp_free((PyObject *)entry);
}

/* Forward reference */
static int
DirEntry_test_mode(DirEntry *self, int follow_symlinks, unsigned short mode_bits);

/* Set exception and return -1 on error, 0 for False, 1 for True */
static int
DirEntry_is_symlink(DirEntry *self)
{
#ifdef MS_WINDOWS
    return (self->win32_lstat.st_mode & S_IFMT) == S_IFLNK;
#elif defined(HAVE_DIRENT_D_TYPE)
    /* POSIX */
    if (self->d_type != DT_UNKNOWN)
        return self->d_type == DT_LNK;
    else
        return DirEntry_test_mode(self, 0, S_IFLNK);
#else
    /* POSIX without d_type */
    return DirEntry_test_mode(self, 0, S_IFLNK);
#endif
}

static PyObject *
DirEntry_py_is_symlink(DirEntry *self)
{
    int result;

    result = DirEntry_is_symlink(self);
    if (result == -1)
        return NULL;
    return PyBool_FromLong(result);
}

static PyObject *
DirEntry_fetch_stat(DirEntry *self, int follow_symlinks)
{
    int result;
    struct _Py_stat_struct st;

#ifdef MS_WINDOWS
    const wchar_t *path;

    path = PyUnicode_AsUnicode(self->path);
    if (!path)
        return NULL;

    if (follow_symlinks)
        result = win32_stat(path, &st);
    else
        result = win32_lstat(path, &st);

    if (result != 0) {
        return PyErr_SetExcFromWindowsErrWithFilenameObject(PyExc_OSError,
                                                            0, self->path);
    }
#else /* POSIX */
    PyObject *bytes;
    const char *path;

    if (!PyUnicode_FSConverter(self->path, &bytes))
        return NULL;
    path = PyBytes_AS_STRING(bytes);

    if (follow_symlinks)
        result = STAT(path, &st);
    else
        result = LSTAT(path, &st);
    Py_DECREF(bytes);

    if (result != 0)
        return PyErr_SetFromErrnoWithFilenameObject(PyExc_OSError, self->path);
#endif

    return _pystat_fromstructstat(&st);
}

static PyObject *
DirEntry_get_lstat(DirEntry *self)
{
    if (!self->lstat) {
#ifdef MS_WINDOWS
        self->lstat = _pystat_fromstructstat(&self->win32_lstat);
#else /* POSIX */
        self->lstat = DirEntry_fetch_stat(self, 0);
#endif
    }
    Py_XINCREF(self->lstat);
    return self->lstat;
}

static PyObject *
DirEntry_get_stat(DirEntry *self, int follow_symlinks)
{
    if (!follow_symlinks)
        return DirEntry_get_lstat(self);

    if (!self->stat) {
        int result = DirEntry_is_symlink(self);
        if (result == -1)
            return NULL;
        else if (result)
            self->stat = DirEntry_fetch_stat(self, 1);
        else
            self->stat = DirEntry_get_lstat(self);
    }

    Py_XINCREF(self->stat);
    return self->stat;
}

static PyObject *
DirEntry_stat(DirEntry *self, PyObject *args, PyObject *kwargs)
{
    int follow_symlinks = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$p:DirEntry.stat",
                                     follow_symlinks_keywords, &follow_symlinks))
        return NULL;

    return DirEntry_get_stat(self, follow_symlinks);
}

/* Set exception and return -1 on error, 0 for False, 1 for True */
static int
DirEntry_test_mode(DirEntry *self, int follow_symlinks, unsigned short mode_bits)
{
    PyObject *stat = NULL;
    PyObject *st_mode = NULL;
    long mode;
    int result;
#if defined(MS_WINDOWS) || defined(HAVE_DIRENT_D_TYPE)
    int is_symlink;
    int need_stat;
#endif
#ifdef MS_WINDOWS
    unsigned long dir_bits;
#endif
    _Py_IDENTIFIER(st_mode);

#ifdef MS_WINDOWS
    is_symlink = (self->win32_lstat.st_mode & S_IFMT) == S_IFLNK;
    need_stat = follow_symlinks && is_symlink;
#elif defined(HAVE_DIRENT_D_TYPE)
    is_symlink = self->d_type == DT_LNK;
    need_stat = self->d_type == DT_UNKNOWN || (follow_symlinks && is_symlink);
#endif

#if defined(MS_WINDOWS) || defined(HAVE_DIRENT_D_TYPE)
    if (need_stat) {
#endif
        stat = DirEntry_get_stat(self, follow_symlinks);
        if (!stat) {
            if (PyErr_ExceptionMatches(PyExc_FileNotFoundError)) {
                /* If file doesn't exist (anymore), then return False
                   (i.e., say it's not a file/directory) */
                PyErr_Clear();
                return 0;
            }
            goto error;
        }
        st_mode = _PyObject_GetAttrId(stat, &PyId_st_mode);
        if (!st_mode)
            goto error;

        mode = PyLong_AsLong(st_mode);
        if (mode == -1 && PyErr_Occurred())
            goto error;
        Py_CLEAR(st_mode);
        Py_CLEAR(stat);
        result = (mode & S_IFMT) == mode_bits;
#if defined(MS_WINDOWS) || defined(HAVE_DIRENT_D_TYPE)
    }
    else if (is_symlink) {
        assert(mode_bits != S_IFLNK);
        result = 0;
    }
    else {
        assert(mode_bits == S_IFDIR || mode_bits == S_IFREG);
#ifdef MS_WINDOWS
        dir_bits = self->win32_lstat.st_file_attributes & FILE_ATTRIBUTE_DIRECTORY;
        if (mode_bits == S_IFDIR)
            result = dir_bits != 0;
        else
            result = dir_bits == 0;
#else /* POSIX */
        if (mode_bits == S_IFDIR)
            result = self->d_type == DT_DIR;
        else
            result = self->d_type == DT_REG;
#endif
    }
#endif

    return result;

error:
    Py_XDECREF(st_mode);
    Py_XDECREF(stat);
    return -1;
}

static PyObject *
DirEntry_py_test_mode(DirEntry *self, int follow_symlinks, unsigned short mode_bits)
{
    int result;

    result = DirEntry_test_mode(self, follow_symlinks, mode_bits);
    if (result == -1)
        return NULL;
    return PyBool_FromLong(result);
}

static PyObject *
DirEntry_is_dir(DirEntry *self, PyObject *args, PyObject *kwargs)
{
    int follow_symlinks = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$p:DirEntry.is_dir",
                                     follow_symlinks_keywords, &follow_symlinks))
        return NULL;

    return DirEntry_py_test_mode(self, follow_symlinks, S_IFDIR);
}

static PyObject *
DirEntry_is_file(DirEntry *self, PyObject *args, PyObject *kwargs)
{
    int follow_symlinks = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$p:DirEntry.is_file",
                                     follow_symlinks_keywords, &follow_symlinks))
        return NULL;

    return DirEntry_py_test_mode(self, follow_symlinks, S_IFREG);
}

static PyObject *
DirEntry_inode(DirEntry *self)
{
#ifdef MS_WINDOWS
    if (!self->got_file_index) {
        const wchar_t *path;
        struct _Py_stat_struct stat;

        path = PyUnicode_AsUnicode(self->path);
        if (!path)
            return NULL;

        if (win32_lstat(path, &stat) != 0) {
            return PyErr_SetExcFromWindowsErrWithFilenameObject(PyExc_OSError,
                                                                0, self->path);
        }

        self->win32_file_index = stat.st_ino;
        self->got_file_index = 1;
    }
    return PyLong_FromLongLong((long long)self->win32_file_index);
#else /* POSIX */
#ifdef HAVE_LARGEFILE_SUPPORT
    return PyLong_FromLongLong((long long)self->d_ino);
#else
    return PyLong_FromLong((long)self->d_ino);
#endif
#endif
}

static PyObject *
DirEntry_repr(DirEntry *self)
{
    return PyUnicode_FromFormat("<DirEntry %R>", self->name);
}

static PyObject *
DirEntry_fspath(DirEntry *self)
{
    Py_INCREF(self->path);
    return self->path;
}

static PyMemberDef DirEntry_members[] = {
    {"name", T_OBJECT_EX, offsetof(DirEntry, name), READONLY,
     "the entry's base filename, relative to scandir() \"path\" argument"},
    {"path", T_OBJECT_EX, offsetof(DirEntry, path), READONLY,
     "the entry's full path name; equivalent to os.path.join(scandir_path, entry.name)"},
    {NULL}
};

static PyMethodDef DirEntry_methods[] = {
    {"is_dir", (PyCFunction)DirEntry_is_dir, METH_VARARGS | METH_KEYWORDS,
     "return True if the entry is a directory; cached per entry"
    },
    {"is_file", (PyCFunction)DirEntry_is_file, METH_VARARGS | METH_KEYWORDS,
     "return True if the entry is a file; cached per entry"
    },
    {"is_symlink", (PyCFunction)DirEntry_py_is_symlink, METH_NOARGS,
     "return True if the entry is a symbolic link; cached per entry"
    },
    {"stat", (PyCFunction)DirEntry_stat, METH_VARARGS | METH_KEYWORDS,
     "return stat_result object for the entry; cached per entry"
    },
    {"inode", (PyCFunction)DirEntry_inode, METH_NOARGS,
     "return inode of the entry; cached per entry",
    },
    {"__fspath__", (PyCFunction)DirEntry_fspath, METH_NOARGS,
     "returns the path for the entry",
    },
    {NULL}
};

static PyTypeObject DirEntryType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    MODNAME ".DirEntry",                    /* tp_name */
    sizeof(DirEntry),                       /* tp_basicsize */
    0,                                      /* tp_itemsize */
    /* methods */
    (destructor)DirEntry_dealloc,           /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    (reprfunc)DirEntry_repr,                /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    0,                                      /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    DirEntry_methods,                       /* tp_methods */
    DirEntry_members,                       /* tp_members */
};

#ifdef MS_WINDOWS

static wchar_t *
join_path_filenameW(const wchar_t *path_wide, const wchar_t *filename)
{
    Py_ssize_t path_len;
    Py_ssize_t size;
    wchar_t *result;
    wchar_t ch;

    if (!path_wide) { /* Default arg: "." */
        path_wide = L".";
        path_len = 1;
    }
    else {
        path_len = wcslen(path_wide);
    }

    /* The +1's are for the path separator and the NUL */
    size = path_len + 1 + wcslen(filename) + 1;
    result = PyMem_New(wchar_t, size);
    if (!result) {
        PyErr_NoMemory();
        return NULL;
    }
    wcscpy(result, path_wide);
    if (path_len > 0) {
        ch = result[path_len - 1];
        if (ch != SEP && ch != ALTSEP && ch != L':')
            result[path_len++] = SEP;
        wcscpy(result + path_len, filename);
    }
    return result;
}

static PyObject *
DirEntry_from_find_data(path_t *path, WIN32_FIND_DATAW *dataW)
{
    DirEntry *entry;
    BY_HANDLE_FILE_INFORMATION file_info;
    ULONG reparse_tag;
    wchar_t *joined_path;

    entry = PyObject_New(DirEntry, &DirEntryType);
    if (!entry)
        return NULL;
    entry->name = NULL;
    entry->path = NULL;
    entry->stat = NULL;
    entry->lstat = NULL;
    entry->got_file_index = 0;

    entry->name = PyUnicode_FromWideChar(dataW->cFileName, -1);
    if (!entry->name)
        goto error;
    if (path->narrow) {
        Py_SETREF(entry->name, PyUnicode_EncodeFSDefault(entry->name));
        if (!entry->name)
            goto error;
    }

    joined_path = join_path_filenameW(path->wide, dataW->cFileName);
    if (!joined_path)
        goto error;

    entry->path = PyUnicode_FromWideChar(joined_path, -1);
    PyMem_Free(joined_path);
    if (!entry->path)
        goto error;
    if (path->narrow) {
        Py_SETREF(entry->path, PyUnicode_EncodeFSDefault(entry->path));
        if (!entry->path)
            goto error;
    }

    find_data_to_file_info(dataW, &file_info, &reparse_tag);
    _Py_attribute_data_to_stat(&file_info, reparse_tag, &entry->win32_lstat);

    return (PyObject *)entry;

error:
    Py_DECREF(entry);
    return NULL;
}

#else /* POSIX */

static char *
join_path_filename(const char *path_narrow, const char* filename, Py_ssize_t filename_len)
{
    Py_ssize_t path_len;
    Py_ssize_t size;
    char *result;

    if (!path_narrow) { /* Default arg: "." */
        path_narrow = ".";
        path_len = 1;
    }
    else {
        path_len = strlen(path_narrow);
    }

    if (filename_len == -1)
        filename_len = strlen(filename);

    /* The +1's are for the path separator and the NUL */
    size = path_len + 1 + filename_len + 1;
    result = PyMem_New(char, size);
    if (!result) {
        PyErr_NoMemory();
        return NULL;
    }
    strcpy(result, path_narrow);
    if (path_len > 0 && result[path_len - 1] != '/')
        result[path_len++] = '/';
    strcpy(result + path_len, filename);
    return result;
}

static PyObject *
DirEntry_from_posix_info(path_t *path, const char *name, Py_ssize_t name_len,
                         ino_t d_ino
#ifdef HAVE_DIRENT_D_TYPE
                         , unsigned char d_type
#endif
                         )
{
    DirEntry *entry;
    char *joined_path;

    entry = PyObject_New(DirEntry, &DirEntryType);
    if (!entry)
        return NULL;
    entry->name = NULL;
    entry->path = NULL;
    entry->stat = NULL;
    entry->lstat = NULL;

    joined_path = join_path_filename(path->narrow, name, name_len);
    if (!joined_path)
        goto error;

    if (!path->narrow || !PyBytes_Check(path->object)) {
        entry->name = PyUnicode_DecodeFSDefaultAndSize(name, name_len);
        entry->path = PyUnicode_DecodeFSDefault(joined_path);
    }
    else {
        entry->name = PyBytes_FromStringAndSize(name, name_len);
        entry->path = PyBytes_FromString(joined_path);
    }
    PyMem_Free(joined_path);
    if (!entry->name || !entry->path)
        goto error;

#ifdef HAVE_DIRENT_D_TYPE
    entry->d_type = d_type;
#endif
    entry->d_ino = d_ino;

    return (PyObject *)entry;

error:
    Py_XDECREF(entry);
    return NULL;
}

#endif


typedef struct {
    PyObject_HEAD
    path_t path;
#ifdef MS_WINDOWS
    HANDLE handle;
    WIN32_FIND_DATAW file_data;
    int first_time;
#else /* POSIX */
    DIR *dirp;
#endif
} ScandirIterator;

#ifdef MS_WINDOWS

static int
ScandirIterator_is_closed(ScandirIterator *iterator)
{
    return iterator->handle == INVALID_HANDLE_VALUE;
}

static void
ScandirIterator_closedir(ScandirIterator *iterator)
{
    HANDLE handle = iterator->handle;

    if (handle == INVALID_HANDLE_VALUE)
        return;

    iterator->handle = INVALID_HANDLE_VALUE;
    Py_BEGIN_ALLOW_THREADS
    FindClose(handle);
    Py_END_ALLOW_THREADS
}

static PyObject *
ScandirIterator_iternext(ScandirIterator *iterator)
{
    WIN32_FIND_DATAW *file_data = &iterator->file_data;
    BOOL success;
    PyObject *entry;

    /* Happens if the iterator is iterated twice, or closed explicitly */
    if (iterator->handle == INVALID_HANDLE_VALUE)
        return NULL;

    while (1) {
        if (!iterator->first_time) {
            Py_BEGIN_ALLOW_THREADS
            success = FindNextFileW(iterator->handle, file_data);
            Py_END_ALLOW_THREADS
            if (!success) {
                /* Error or no more files */
                if (GetLastError() != ERROR_NO_MORE_FILES)
                    path_error(&iterator->path);
                break;
            }
        }
        iterator->first_time = 0;

        /* Skip over . and .. */
        if (wcscmp(file_data->cFileName, L".") != 0 &&
            wcscmp(file_data->cFileName, L"..") != 0) {
            entry = DirEntry_from_find_data(&iterator->path, file_data);
            if (!entry)
                break;
            return entry;
        }

        /* Loop till we get a non-dot directory or finish iterating */
    }

    /* Error or no more files */
    ScandirIterator_closedir(iterator);
    return NULL;
}

#else /* POSIX */

static int
ScandirIterator_is_closed(ScandirIterator *iterator)
{
    return !iterator->dirp;
}

static void
ScandirIterator_closedir(ScandirIterator *iterator)
{
    DIR *dirp = iterator->dirp;

    if (!dirp)
        return;

    iterator->dirp = NULL;
    Py_BEGIN_ALLOW_THREADS
    closedir(dirp);
    Py_END_ALLOW_THREADS
    return;
}

static PyObject *
ScandirIterator_iternext(ScandirIterator *iterator)
{
    struct dirent *direntp;
    Py_ssize_t name_len;
    int is_dot;
    PyObject *entry;

    /* Happens if the iterator is iterated twice, or closed explicitly */
    if (!iterator->dirp)
        return NULL;

    while (1) {
        errno = 0;
        Py_BEGIN_ALLOW_THREADS
        direntp = readdir(iterator->dirp);
        Py_END_ALLOW_THREADS

        if (!direntp) {
            /* Error or no more files */
            if (errno != 0)
                path_error(&iterator->path);
            break;
        }

        /* Skip over . and .. */
        name_len = NAMLEN(direntp);
        is_dot = direntp->d_name[0] == '.' &&
                 (name_len == 1 || (direntp->d_name[1] == '.' && name_len == 2));
        if (!is_dot) {
            entry = DirEntry_from_posix_info(&iterator->path, direntp->d_name,
                                            name_len, direntp->d_ino
#ifdef HAVE_DIRENT_D_TYPE
                                            , direntp->d_type
#endif
                                            );
            if (!entry)
                break;
            return entry;
        }

        /* Loop till we get a non-dot directory or finish iterating */
    }

    /* Error or no more files */
    ScandirIterator_closedir(iterator);
    return NULL;
}

#endif

static PyObject *
ScandirIterator_close(ScandirIterator *self, PyObject *args)
{
    ScandirIterator_closedir(self);
    Py_RETURN_NONE;
}

static PyObject *
ScandirIterator_enter(PyObject *self, PyObject *args)
{
    Py_INCREF(self);
    return self;
}

static PyObject *
ScandirIterator_exit(ScandirIterator *self, PyObject *args)
{
    ScandirIterator_closedir(self);
    Py_RETURN_NONE;
}

static void
ScandirIterator_finalize(ScandirIterator *iterator)
{
    PyObject *error_type, *error_value, *error_traceback;

    /* Save the current exception, if any. */
    PyErr_Fetch(&error_type, &error_value, &error_traceback);

    if (!ScandirIterator_is_closed(iterator)) {
        ScandirIterator_closedir(iterator);

        if (PyErr_ResourceWarning((PyObject *)iterator, 1,
                                  "unclosed scandir iterator %R", iterator)) {
            /* Spurious errors can appear at shutdown */
            if (PyErr_ExceptionMatches(PyExc_Warning)) {
                PyErr_WriteUnraisable((PyObject *) iterator);
            }
        }
    }

    Py_CLEAR(iterator->path.object);
    path_cleanup(&iterator->path);

    /* Restore the saved exception. */
    PyErr_Restore(error_type, error_value, error_traceback);
}

static void
ScandirIterator_dealloc(ScandirIterator *iterator)
{
    if (PyObject_CallFinalizerFromDealloc((PyObject *)iterator) < 0)
        return;

    Py_TYPE(iterator)->tp_free((PyObject *)iterator);
}

static PyMethodDef ScandirIterator_methods[] = {
    {"__enter__", (PyCFunction)ScandirIterator_enter, METH_NOARGS},
    {"__exit__", (PyCFunction)ScandirIterator_exit, METH_VARARGS},
    {"close", (PyCFunction)ScandirIterator_close, METH_NOARGS},
    {NULL}
};

static PyTypeObject ScandirIteratorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    MODNAME ".ScandirIterator",             /* tp_name */
    sizeof(ScandirIterator),                /* tp_basicsize */
    0,                                      /* tp_itemsize */
    /* methods */
    (destructor)ScandirIterator_dealloc,    /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT
        | Py_TPFLAGS_HAVE_FINALIZE,         /* tp_flags */
    0,                                      /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    PyObject_SelfIter,                      /* tp_iter */
    (iternextfunc)ScandirIterator_iternext, /* tp_iternext */
    ScandirIterator_methods,                /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    0,                                      /* tp_new */
    0,                                      /* tp_free */
    0,                                      /* tp_is_gc */
    0,                                      /* tp_bases */
    0,                                      /* tp_mro */
    0,                                      /* tp_cache */
    0,                                      /* tp_subclasses */
    0,                                      /* tp_weaklist */
    0,                                      /* tp_del */
    0,                                      /* tp_version_tag */
    (destructor)ScandirIterator_finalize,   /* tp_finalize */
};

static PyObject *
posix_scandir(PyObject *self, PyObject *args, PyObject *kwargs)
{
    ScandirIterator *iterator;
    static char *keywords[] = {"path", NULL};
#ifdef MS_WINDOWS
    wchar_t *path_strW;
#else
    const char *path;
#endif

    iterator = PyObject_New(ScandirIterator, &ScandirIteratorType);
    if (!iterator)
        return NULL;
    memset(&iterator->path, 0, sizeof(path_t));
    iterator->path.function_name = "scandir";
    iterator->path.nullable = 1;

#ifdef MS_WINDOWS
    iterator->handle = INVALID_HANDLE_VALUE;
#else
    iterator->dirp = NULL;
#endif

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O&:scandir", keywords,
                                     path_converter, &iterator->path))
        goto error;

    /* path_converter doesn't keep path.object around, so do it
       manually for the lifetime of the iterator here (the refcount
       is decremented in ScandirIterator_dealloc)
    */
    Py_XINCREF(iterator->path.object);

#ifdef MS_WINDOWS
    iterator->first_time = 1;

    path_strW = join_path_filenameW(iterator->path.wide, L"*.*");
    if (!path_strW)
        goto error;

    Py_BEGIN_ALLOW_THREADS
    iterator->handle = FindFirstFileW(path_strW, &iterator->file_data);
    Py_END_ALLOW_THREADS

    PyMem_Free(path_strW);

    if (iterator->handle == INVALID_HANDLE_VALUE) {
        path_error(&iterator->path);
        goto error;
    }
#else /* POSIX */
    if (iterator->path.narrow)
        path = iterator->path.narrow;
    else
        path = ".";

    errno = 0;
    Py_BEGIN_ALLOW_THREADS
    iterator->dirp = opendir(path);
    Py_END_ALLOW_THREADS

    if (!iterator->dirp) {
        path_error(&iterator->path);
        goto error;
    }
#endif

    return (PyObject *)iterator;

error:
    Py_DECREF(iterator);
    return NULL;
}

/*
    Return the file system path representation of the object.

    If the object is str or bytes, then allow it to pass through with
    an incremented refcount. If the object defines __fspath__(), then
    return the result of that method. All other types raise a TypeError.
*/
PyObject *
PyOS_FSPath(PyObject *path)
{
    /* For error message reasons, this function is manually inlined in
       path_converter(). */
    _Py_IDENTIFIER(__fspath__);
    PyObject *func = NULL;
    PyObject *path_repr = NULL;

    if (PyUnicode_Check(path) || PyBytes_Check(path)) {
        Py_INCREF(path);
        return path;
    }

    func = _PyObject_LookupSpecial(path, &PyId___fspath__);
    if (NULL == func) {
        return PyErr_Format(PyExc_TypeError,
                            "expected str, bytes or os.PathLike object, "
                            "not %.200s",
                            Py_TYPE(path)->tp_name);
    }

    path_repr = PyObject_CallFunctionObjArgs(func, NULL);
    Py_DECREF(func);
    if (NULL == path_repr) {
        return NULL;
    }

    if (!(PyUnicode_Check(path_repr) || PyBytes_Check(path_repr))) {
        PyErr_Format(PyExc_TypeError,
                     "expected %.200s.__fspath__() to return str or bytes, "
                     "not %.200s", Py_TYPE(path)->tp_name,
                     Py_TYPE(path_repr)->tp_name);
        Py_DECREF(path_repr);
        return NULL;
    }

    return path_repr;
}

/*[clinic input]
os.fspath

    path: object

Return the file system path representation of the object.

If the object is str or bytes, then allow it to pass through as-is. If the
object defines __fspath__(), then return the result of that method. All other
types raise a TypeError.
[clinic start generated code]*/

static PyObject *
os_fspath_impl(PyObject *module, PyObject *path)
/*[clinic end generated code: output=c3c3b78ecff2914f input=e357165f7b22490f]*/
{
    return PyOS_FSPath(path);
}

#ifdef HAVE_GETRANDOM_SYSCALL
/*[clinic input]
os.getrandom

    size: Py_ssize_t
    flags: int=0

Obtain a series of random bytes.
[clinic start generated code]*/

static PyObject *
os_getrandom_impl(PyObject *module, Py_ssize_t size, int flags)
/*[clinic end generated code: output=b3a618196a61409c input=59bafac39c594947]*/
{
    char *buffer;
    Py_ssize_t n;
    PyObject *bytes;

    if (size < 0) {
        errno = EINVAL;
        return posix_error();
    }

    buffer = PyMem_Malloc(size);
    if (buffer == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    while (1) {
        n = syscall(SYS_getrandom, buffer, size, flags);
        if (n < 0 && errno == EINTR) {
            if (PyErr_CheckSignals() < 0) {
                return NULL;
            }
            continue;
        }
        break;
    }

    if (n < 0) {
        PyMem_Free(buffer);
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }

    bytes = PyBytes_FromStringAndSize(buffer, n);
    PyMem_Free(buffer);

    return bytes;
}
#endif   /* HAVE_GETRANDOM_SYSCALL */

#include "clinic/posixmodule.c.h"

/*[clinic input]
dump buffer
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=524ce2e021e4eba6]*/


static PyMethodDef posix_methods[] = {

    OS_STAT_METHODDEF
    OS_ACCESS_METHODDEF
    OS_TTYNAME_METHODDEF
    OS_CHDIR_METHODDEF
    OS_CHFLAGS_METHODDEF
    OS_CHMOD_METHODDEF
    OS_FCHMOD_METHODDEF
    OS_LCHMOD_METHODDEF
    OS_CHOWN_METHODDEF
    OS_FCHOWN_METHODDEF
    OS_LCHOWN_METHODDEF
    OS_LCHFLAGS_METHODDEF
    OS_CHROOT_METHODDEF
    OS_CTERMID_METHODDEF
    OS_GETCWD_METHODDEF
    OS_GETCWDB_METHODDEF
    OS_LINK_METHODDEF
    OS_LISTDIR_METHODDEF
    OS_LSTAT_METHODDEF
    OS_MKDIR_METHODDEF
    OS_NICE_METHODDEF
    OS_GETPRIORITY_METHODDEF
    OS_SETPRIORITY_METHODDEF
#ifdef HAVE_READLINK
    {"readlink",        (PyCFunction)posix_readlink,
                        METH_VARARGS | METH_KEYWORDS,
                        readlink__doc__},
#endif /* HAVE_READLINK */
#if !defined(HAVE_READLINK) && defined(MS_WINDOWS)
    {"readlink",        (PyCFunction)win_readlink,
                        METH_VARARGS | METH_KEYWORDS,
                        readlink__doc__},
#endif /* !defined(HAVE_READLINK) && defined(MS_WINDOWS) */
    OS_RENAME_METHODDEF
    OS_REPLACE_METHODDEF
    OS_RMDIR_METHODDEF
    {"stat_float_times", stat_float_times, METH_VARARGS, stat_float_times__doc__},
    OS_SYMLINK_METHODDEF
    OS_SYSTEM_METHODDEF
    OS_UMASK_METHODDEF
    OS_UNAME_METHODDEF
    OS_UNLINK_METHODDEF
    OS_REMOVE_METHODDEF
    OS_UTIME_METHODDEF
    OS_TIMES_METHODDEF
    OS__EXIT_METHODDEF
    OS_EXECV_METHODDEF
    OS_EXECVE_METHODDEF
    OS_SPAWNV_METHODDEF
    OS_SPAWNVE_METHODDEF
    OS_FORK1_METHODDEF
    OS_FORK_METHODDEF
    OS_SCHED_GET_PRIORITY_MAX_METHODDEF
    OS_SCHED_GET_PRIORITY_MIN_METHODDEF
    OS_SCHED_GETPARAM_METHODDEF
    OS_SCHED_GETSCHEDULER_METHODDEF
    OS_SCHED_RR_GET_INTERVAL_METHODDEF
    OS_SCHED_SETPARAM_METHODDEF
    OS_SCHED_SETSCHEDULER_METHODDEF
    OS_SCHED_YIELD_METHODDEF
    OS_SCHED_SETAFFINITY_METHODDEF
    OS_SCHED_GETAFFINITY_METHODDEF
    OS_OPENPTY_METHODDEF
    OS_FORKPTY_METHODDEF
    OS_GETEGID_METHODDEF
    OS_GETEUID_METHODDEF
    OS_GETGID_METHODDEF
#ifdef HAVE_GETGROUPLIST
    {"getgrouplist",    posix_getgrouplist, METH_VARARGS, posix_getgrouplist__doc__},
#endif
    OS_GETGROUPS_METHODDEF
    OS_GETPID_METHODDEF
    OS_GETPGRP_METHODDEF
    OS_GETPPID_METHODDEF
    OS_GETUID_METHODDEF
    OS_GETLOGIN_METHODDEF
    OS_KILL_METHODDEF
    OS_KILLPG_METHODDEF
    OS_PLOCK_METHODDEF
#ifdef MS_WINDOWS
    OS_STARTFILE_METHODDEF
#endif
    OS_SETUID_METHODDEF
    OS_SETEUID_METHODDEF
    OS_SETREUID_METHODDEF
    OS_SETGID_METHODDEF
    OS_SETEGID_METHODDEF
    OS_SETREGID_METHODDEF
    OS_SETGROUPS_METHODDEF
#ifdef HAVE_INITGROUPS
    {"initgroups",      posix_initgroups, METH_VARARGS, posix_initgroups__doc__},
#endif /* HAVE_INITGROUPS */
    OS_GETPGID_METHODDEF
    OS_SETPGRP_METHODDEF
    OS_WAIT_METHODDEF
    OS_WAIT3_METHODDEF
    OS_WAIT4_METHODDEF
    OS_WAITID_METHODDEF
    OS_WAITPID_METHODDEF
    OS_GETSID_METHODDEF
    OS_SETSID_METHODDEF
    OS_SETPGID_METHODDEF
    OS_TCGETPGRP_METHODDEF
    OS_TCSETPGRP_METHODDEF
    OS_OPEN_METHODDEF
    OS_CLOSE_METHODDEF
    OS_CLOSERANGE_METHODDEF
    OS_DEVICE_ENCODING_METHODDEF
    OS_DUP_METHODDEF
    OS_DUP2_METHODDEF
    OS_LOCKF_METHODDEF
    OS_LSEEK_METHODDEF
    OS_READ_METHODDEF
    OS_READV_METHODDEF
    OS_PREAD_METHODDEF
    OS_WRITE_METHODDEF
    OS_WRITEV_METHODDEF
    OS_PWRITE_METHODDEF
#ifdef HAVE_SENDFILE
    {"sendfile",        (PyCFunction)posix_sendfile, METH_VARARGS | METH_KEYWORDS,
                            posix_sendfile__doc__},
#endif
    OS_FSTAT_METHODDEF
    OS_ISATTY_METHODDEF
    OS_PIPE_METHODDEF
    OS_PIPE2_METHODDEF
    OS_MKFIFO_METHODDEF
    OS_MKNOD_METHODDEF
    OS_MAJOR_METHODDEF
    OS_MINOR_METHODDEF
    OS_MAKEDEV_METHODDEF
    OS_FTRUNCATE_METHODDEF
    OS_TRUNCATE_METHODDEF
    OS_POSIX_FALLOCATE_METHODDEF
    OS_POSIX_FADVISE_METHODDEF
    OS_PUTENV_METHODDEF
    OS_UNSETENV_METHODDEF
    OS_STRERROR_METHODDEF
    OS_FCHDIR_METHODDEF
    OS_FSYNC_METHODDEF
    OS_SYNC_METHODDEF
    OS_FDATASYNC_METHODDEF
    OS_WCOREDUMP_METHODDEF
    OS_WIFCONTINUED_METHODDEF
    OS_WIFSTOPPED_METHODDEF
    OS_WIFSIGNALED_METHODDEF
    OS_WIFEXITED_METHODDEF
    OS_WEXITSTATUS_METHODDEF
    OS_WTERMSIG_METHODDEF
    OS_WSTOPSIG_METHODDEF
    OS_FSTATVFS_METHODDEF
    OS_STATVFS_METHODDEF
    OS_CONFSTR_METHODDEF
    OS_SYSCONF_METHODDEF
    OS_FPATHCONF_METHODDEF
    OS_PATHCONF_METHODDEF
    OS_ABORT_METHODDEF
    OS__GETFULLPATHNAME_METHODDEF
    OS__ISDIR_METHODDEF
    OS__GETDISKUSAGE_METHODDEF
    OS__GETFINALPATHNAME_METHODDEF
    OS__GETVOLUMEPATHNAME_METHODDEF
    OS_GETLOADAVG_METHODDEF
    OS_URANDOM_METHODDEF
    OS_SETRESUID_METHODDEF
    OS_SETRESGID_METHODDEF
    OS_GETRESUID_METHODDEF
    OS_GETRESGID_METHODDEF

    OS_GETXATTR_METHODDEF
    OS_SETXATTR_METHODDEF
    OS_REMOVEXATTR_METHODDEF
    OS_LISTXATTR_METHODDEF

#if defined(TERMSIZE_USE_CONIO) || defined(TERMSIZE_USE_IOCTL)
    {"get_terminal_size", get_terminal_size, METH_VARARGS, termsize__doc__},
#endif
    OS_CPU_COUNT_METHODDEF
    OS_GET_INHERITABLE_METHODDEF
    OS_SET_INHERITABLE_METHODDEF
    OS_GET_HANDLE_INHERITABLE_METHODDEF
    OS_SET_HANDLE_INHERITABLE_METHODDEF
#ifndef MS_WINDOWS
    {"get_blocking", posix_get_blocking, METH_VARARGS, get_blocking__doc__},
    {"set_blocking", posix_set_blocking, METH_VARARGS, set_blocking__doc__},
#endif
    {"scandir",         (PyCFunction)posix_scandir,
                        METH_VARARGS | METH_KEYWORDS,
                        posix_scandir__doc__},
    OS_FSPATH_METHODDEF
    OS_GETRANDOM_METHODDEF
    {NULL,              NULL}            /* Sentinel */
};


#if defined(HAVE_SYMLINK) && defined(MS_WINDOWS)
static int
enable_symlink()
{
    HANDLE tok;
    TOKEN_PRIVILEGES tok_priv;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &tok))
        return 0;

    if (!LookupPrivilegeValue(NULL, SE_CREATE_SYMBOLIC_LINK_NAME, &luid))
        return 0;

    tok_priv.PrivilegeCount = 1;
    tok_priv.Privileges[0].Luid = luid;
    tok_priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(tok, FALSE, &tok_priv,
                               sizeof(TOKEN_PRIVILEGES),
                               (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL))
        return 0;

    /* ERROR_NOT_ALL_ASSIGNED returned when the privilege can't be assigned. */
    return GetLastError() == ERROR_NOT_ALL_ASSIGNED ? 0 : 1;
}
#endif /* defined(HAVE_SYMLINK) && defined(MS_WINDOWS) */

static int
all_ins(PyObject *m)
{
#ifdef F_OK
    if (PyModule_AddIntMacro(m, F_OK)) return -1;
#endif
#ifdef R_OK
    if (PyModule_AddIntMacro(m, R_OK)) return -1;
#endif
#ifdef W_OK
    if (PyModule_AddIntMacro(m, W_OK)) return -1;
#endif
#ifdef X_OK
    if (PyModule_AddIntMacro(m, X_OK)) return -1;
#endif
#ifdef NGROUPS_MAX
    if (PyModule_AddIntMacro(m, NGROUPS_MAX)) return -1;
#endif
#ifdef TMP_MAX
    if (PyModule_AddIntMacro(m, TMP_MAX)) return -1;
#endif
#ifdef WCONTINUED
    if (PyModule_AddIntMacro(m, WCONTINUED)) return -1;
#endif
#ifdef WNOHANG
    if (PyModule_AddIntMacro(m, WNOHANG)) return -1;
#endif
#ifdef WUNTRACED
    if (PyModule_AddIntMacro(m, WUNTRACED)) return -1;
#endif
#ifdef O_RDONLY
    if (PyModule_AddIntMacro(m, O_RDONLY)) return -1;
#endif
#ifdef O_WRONLY
    if (PyModule_AddIntMacro(m, O_WRONLY)) return -1;
#endif
#ifdef O_RDWR
    if (PyModule_AddIntMacro(m, O_RDWR)) return -1;
#endif
#ifdef O_NDELAY
    if (PyModule_AddIntMacro(m, O_NDELAY)) return -1;
#endif
#ifdef O_NONBLOCK
    if (PyModule_AddIntMacro(m, O_NONBLOCK)) return -1;
#endif
#ifdef O_APPEND
    if (PyModule_AddIntMacro(m, O_APPEND)) return -1;
#endif
#ifdef O_DSYNC
    if (PyModule_AddIntMacro(m, O_DSYNC)) return -1;
#endif
#ifdef O_RSYNC
    if (PyModule_AddIntMacro(m, O_RSYNC)) return -1;
#endif
#ifdef O_SYNC
    if (PyModule_AddIntMacro(m, O_SYNC)) return -1;
#endif
#ifdef O_NOCTTY
    if (PyModule_AddIntMacro(m, O_NOCTTY)) return -1;
#endif
#ifdef O_CREAT
    if (PyModule_AddIntMacro(m, O_CREAT)) return -1;
#endif
#ifdef O_EXCL
    if (PyModule_AddIntMacro(m, O_EXCL)) return -1;
#endif
#ifdef O_TRUNC
    if (PyModule_AddIntMacro(m, O_TRUNC)) return -1;
#endif
#ifdef O_BINARY
    if (PyModule_AddIntMacro(m, O_BINARY)) return -1;
#endif
#ifdef O_TEXT
    if (PyModule_AddIntMacro(m, O_TEXT)) return -1;
#endif
#ifdef O_XATTR
    if (PyModule_AddIntMacro(m, O_XATTR)) return -1;
#endif
#ifdef O_LARGEFILE
    if (PyModule_AddIntMacro(m, O_LARGEFILE)) return -1;
#endif
#ifndef __GNU__
#ifdef O_SHLOCK
    if (PyModule_AddIntMacro(m, O_SHLOCK)) return -1;
#endif
#ifdef O_EXLOCK
    if (PyModule_AddIntMacro(m, O_EXLOCK)) return -1;
#endif
#endif
#ifdef O_EXEC
    if (PyModule_AddIntMacro(m, O_EXEC)) return -1;
#endif
#ifdef O_SEARCH
    if (PyModule_AddIntMacro(m, O_SEARCH)) return -1;
#endif
#ifdef O_PATH
    if (PyModule_AddIntMacro(m, O_PATH)) return -1;
#endif
#ifdef O_TTY_INIT
    if (PyModule_AddIntMacro(m, O_TTY_INIT)) return -1;
#endif
#ifdef O_TMPFILE
    if (PyModule_AddIntMacro(m, O_TMPFILE)) return -1;
#endif
#ifdef PRIO_PROCESS
    if (PyModule_AddIntMacro(m, PRIO_PROCESS)) return -1;
#endif
#ifdef PRIO_PGRP
    if (PyModule_AddIntMacro(m, PRIO_PGRP)) return -1;
#endif
#ifdef PRIO_USER
    if (PyModule_AddIntMacro(m, PRIO_USER)) return -1;
#endif
#ifdef O_CLOEXEC
    if (PyModule_AddIntMacro(m, O_CLOEXEC)) return -1;
#endif
#ifdef O_ACCMODE
    if (PyModule_AddIntMacro(m, O_ACCMODE)) return -1;
#endif


#ifdef SEEK_HOLE
    if (PyModule_AddIntMacro(m, SEEK_HOLE)) return -1;
#endif
#ifdef SEEK_DATA
    if (PyModule_AddIntMacro(m, SEEK_DATA)) return -1;
#endif

/* MS Windows */
#ifdef O_NOINHERIT
    /* Don't inherit in child processes. */
    if (PyModule_AddIntMacro(m, O_NOINHERIT)) return -1;
#endif
#ifdef _O_SHORT_LIVED
    /* Optimize for short life (keep in memory). */
    /* MS forgot to define this one with a non-underscore form too. */
    if (PyModule_AddIntConstant(m, "O_SHORT_LIVED", _O_SHORT_LIVED)) return -1;
#endif
#ifdef O_TEMPORARY
    /* Automatically delete when last handle is closed. */
    if (PyModule_AddIntMacro(m, O_TEMPORARY)) return -1;
#endif
#ifdef O_RANDOM
    /* Optimize for random access. */
    if (PyModule_AddIntMacro(m, O_RANDOM)) return -1;
#endif
#ifdef O_SEQUENTIAL
    /* Optimize for sequential access. */
    if (PyModule_AddIntMacro(m, O_SEQUENTIAL)) return -1;
#endif

/* GNU extensions. */
#ifdef O_ASYNC
    /* Send a SIGIO signal whenever input or output
       becomes available on file descriptor */
    if (PyModule_AddIntMacro(m, O_ASYNC)) return -1;
#endif
#ifdef O_DIRECT
    /* Direct disk access. */
    if (PyModule_AddIntMacro(m, O_DIRECT)) return -1;
#endif
#ifdef O_DIRECTORY
    /* Must be a directory.      */
    if (PyModule_AddIntMacro(m, O_DIRECTORY)) return -1;
#endif
#ifdef O_NOFOLLOW
    /* Do not follow links.      */
    if (PyModule_AddIntMacro(m, O_NOFOLLOW)) return -1;
#endif
#ifdef O_NOLINKS
    /* Fails if link count of the named file is greater than 1 */
    if (PyModule_AddIntMacro(m, O_NOLINKS)) return -1;
#endif
#ifdef O_NOATIME
    /* Do not update the access time. */
    if (PyModule_AddIntMacro(m, O_NOATIME)) return -1;
#endif

    /* These come from sysexits.h */
#ifdef EX_OK
    if (PyModule_AddIntMacro(m, EX_OK)) return -1;
#endif /* EX_OK */
#ifdef EX_USAGE
    if (PyModule_AddIntMacro(m, EX_USAGE)) return -1;
#endif /* EX_USAGE */
#ifdef EX_DATAERR
    if (PyModule_AddIntMacro(m, EX_DATAERR)) return -1;
#endif /* EX_DATAERR */
#ifdef EX_NOINPUT
    if (PyModule_AddIntMacro(m, EX_NOINPUT)) return -1;
#endif /* EX_NOINPUT */
#ifdef EX_NOUSER
    if (PyModule_AddIntMacro(m, EX_NOUSER)) return -1;
#endif /* EX_NOUSER */
#ifdef EX_NOHOST
    if (PyModule_AddIntMacro(m, EX_NOHOST)) return -1;
#endif /* EX_NOHOST */
#ifdef EX_UNAVAILABLE
    if (PyModule_AddIntMacro(m, EX_UNAVAILABLE)) return -1;
#endif /* EX_UNAVAILABLE */
#ifdef EX_SOFTWARE
    if (PyModule_AddIntMacro(m, EX_SOFTWARE)) return -1;
#endif /* EX_SOFTWARE */
#ifdef EX_OSERR
    if (PyModule_AddIntMacro(m, EX_OSERR)) return -1;
#endif /* EX_OSERR */
#ifdef EX_OSFILE
    if (PyModule_AddIntMacro(m, EX_OSFILE)) return -1;
#endif /* EX_OSFILE */
#ifdef EX_CANTCREAT
    if (PyModule_AddIntMacro(m, EX_CANTCREAT)) return -1;
#endif /* EX_CANTCREAT */
#ifdef EX_IOERR
    if (PyModule_AddIntMacro(m, EX_IOERR)) return -1;
#endif /* EX_IOERR */
#ifdef EX_TEMPFAIL
    if (PyModule_AddIntMacro(m, EX_TEMPFAIL)) return -1;
#endif /* EX_TEMPFAIL */
#ifdef EX_PROTOCOL
    if (PyModule_AddIntMacro(m, EX_PROTOCOL)) return -1;
#endif /* EX_PROTOCOL */
#ifdef EX_NOPERM
    if (PyModule_AddIntMacro(m, EX_NOPERM)) return -1;
#endif /* EX_NOPERM */
#ifdef EX_CONFIG
    if (PyModule_AddIntMacro(m, EX_CONFIG)) return -1;
#endif /* EX_CONFIG */
#ifdef EX_NOTFOUND
    if (PyModule_AddIntMacro(m, EX_NOTFOUND)) return -1;
#endif /* EX_NOTFOUND */

    /* statvfs */
#ifdef ST_RDONLY
    if (PyModule_AddIntMacro(m, ST_RDONLY)) return -1;
#endif /* ST_RDONLY */
#ifdef ST_NOSUID
    if (PyModule_AddIntMacro(m, ST_NOSUID)) return -1;
#endif /* ST_NOSUID */

       /* GNU extensions */
#ifdef ST_NODEV
    if (PyModule_AddIntMacro(m, ST_NODEV)) return -1;
#endif /* ST_NODEV */
#ifdef ST_NOEXEC
    if (PyModule_AddIntMacro(m, ST_NOEXEC)) return -1;
#endif /* ST_NOEXEC */
#ifdef ST_SYNCHRONOUS
    if (PyModule_AddIntMacro(m, ST_SYNCHRONOUS)) return -1;
#endif /* ST_SYNCHRONOUS */
#ifdef ST_MANDLOCK
    if (PyModule_AddIntMacro(m, ST_MANDLOCK)) return -1;
#endif /* ST_MANDLOCK */
#ifdef ST_WRITE
    if (PyModule_AddIntMacro(m, ST_WRITE)) return -1;
#endif /* ST_WRITE */
#ifdef ST_APPEND
    if (PyModule_AddIntMacro(m, ST_APPEND)) return -1;
#endif /* ST_APPEND */
#ifdef ST_NOATIME
    if (PyModule_AddIntMacro(m, ST_NOATIME)) return -1;
#endif /* ST_NOATIME */
#ifdef ST_NODIRATIME
    if (PyModule_AddIntMacro(m, ST_NODIRATIME)) return -1;
#endif /* ST_NODIRATIME */
#ifdef ST_RELATIME
    if (PyModule_AddIntMacro(m, ST_RELATIME)) return -1;
#endif /* ST_RELATIME */

    /* FreeBSD sendfile() constants */
#ifdef SF_NODISKIO
    if (PyModule_AddIntMacro(m, SF_NODISKIO)) return -1;
#endif
#ifdef SF_MNOWAIT
    if (PyModule_AddIntMacro(m, SF_MNOWAIT)) return -1;
#endif
#ifdef SF_SYNC
    if (PyModule_AddIntMacro(m, SF_SYNC)) return -1;
#endif

    /* constants for posix_fadvise */
#ifdef POSIX_FADV_NORMAL
    if (PyModule_AddIntMacro(m, POSIX_FADV_NORMAL)) return -1;
#endif
#ifdef POSIX_FADV_SEQUENTIAL
    if (PyModule_AddIntMacro(m, POSIX_FADV_SEQUENTIAL)) return -1;
#endif
#ifdef POSIX_FADV_RANDOM
    if (PyModule_AddIntMacro(m, POSIX_FADV_RANDOM)) return -1;
#endif
#ifdef POSIX_FADV_NOREUSE
    if (PyModule_AddIntMacro(m, POSIX_FADV_NOREUSE)) return -1;
#endif
#ifdef POSIX_FADV_WILLNEED
    if (PyModule_AddIntMacro(m, POSIX_FADV_WILLNEED)) return -1;
#endif
#ifdef POSIX_FADV_DONTNEED
    if (PyModule_AddIntMacro(m, POSIX_FADV_DONTNEED)) return -1;
#endif

    /* constants for waitid */
#if defined(HAVE_SYS_WAIT_H) && defined(HAVE_WAITID)
    if (PyModule_AddIntMacro(m, P_PID)) return -1;
    if (PyModule_AddIntMacro(m, P_PGID)) return -1;
    if (PyModule_AddIntMacro(m, P_ALL)) return -1;
#endif
#ifdef WEXITED
    if (PyModule_AddIntMacro(m, WEXITED)) return -1;
#endif
#ifdef WNOWAIT
    if (PyModule_AddIntMacro(m, WNOWAIT)) return -1;
#endif
#ifdef WSTOPPED
    if (PyModule_AddIntMacro(m, WSTOPPED)) return -1;
#endif
#ifdef CLD_EXITED
    if (PyModule_AddIntMacro(m, CLD_EXITED)) return -1;
#endif
#ifdef CLD_DUMPED
    if (PyModule_AddIntMacro(m, CLD_DUMPED)) return -1;
#endif
#ifdef CLD_TRAPPED
    if (PyModule_AddIntMacro(m, CLD_TRAPPED)) return -1;
#endif
#ifdef CLD_CONTINUED
    if (PyModule_AddIntMacro(m, CLD_CONTINUED)) return -1;
#endif

    /* constants for lockf */
#ifdef F_LOCK
    if (PyModule_AddIntMacro(m, F_LOCK)) return -1;
#endif
#ifdef F_TLOCK
    if (PyModule_AddIntMacro(m, F_TLOCK)) return -1;
#endif
#ifdef F_ULOCK
    if (PyModule_AddIntMacro(m, F_ULOCK)) return -1;
#endif
#ifdef F_TEST
    if (PyModule_AddIntMacro(m, F_TEST)) return -1;
#endif

#ifdef HAVE_SPAWNV
    if (PyModule_AddIntConstant(m, "P_WAIT", _P_WAIT)) return -1;
    if (PyModule_AddIntConstant(m, "P_NOWAIT", _P_NOWAIT)) return -1;
    if (PyModule_AddIntConstant(m, "P_OVERLAY", _OLD_P_OVERLAY)) return -1;
    if (PyModule_AddIntConstant(m, "P_NOWAITO", _P_NOWAITO)) return -1;
    if (PyModule_AddIntConstant(m, "P_DETACH", _P_DETACH)) return -1;
#endif

#ifdef HAVE_SCHED_H
#ifdef SCHED_OTHER
    if (PyModule_AddIntMacro(m, SCHED_OTHER)) return -1;
#endif
#ifdef SCHED_FIFO
    if (PyModule_AddIntMacro(m, SCHED_FIFO)) return -1;
#endif
#ifdef SCHED_RR
    if (PyModule_AddIntMacro(m, SCHED_RR)) return -1;
#endif
#ifdef SCHED_SPORADIC
    if (PyModule_AddIntMacro(m, SCHED_SPORADIC) return -1;
#endif
#ifdef SCHED_BATCH
    if (PyModule_AddIntMacro(m, SCHED_BATCH)) return -1;
#endif
#ifdef SCHED_IDLE
    if (PyModule_AddIntMacro(m, SCHED_IDLE)) return -1;
#endif
#ifdef SCHED_RESET_ON_FORK
    if (PyModule_AddIntMacro(m, SCHED_RESET_ON_FORK)) return -1;
#endif
#ifdef SCHED_SYS
    if (PyModule_AddIntMacro(m, SCHED_SYS)) return -1;
#endif
#ifdef SCHED_IA
    if (PyModule_AddIntMacro(m, SCHED_IA)) return -1;
#endif
#ifdef SCHED_FSS
    if (PyModule_AddIntMacro(m, SCHED_FSS)) return -1;
#endif
#ifdef SCHED_FX
    if (PyModule_AddIntConstant(m, "SCHED_FX", SCHED_FSS)) return -1;
#endif
#endif

#ifdef USE_XATTRS
    if (PyModule_AddIntMacro(m, XATTR_CREATE)) return -1;
    if (PyModule_AddIntMacro(m, XATTR_REPLACE)) return -1;
    if (PyModule_AddIntMacro(m, XATTR_SIZE_MAX)) return -1;
#endif

#if HAVE_DECL_RTLD_LAZY
    if (PyModule_AddIntMacro(m, RTLD_LAZY)) return -1;
#endif
#if HAVE_DECL_RTLD_NOW
    if (PyModule_AddIntMacro(m, RTLD_NOW)) return -1;
#endif
#if HAVE_DECL_RTLD_GLOBAL
    if (PyModule_AddIntMacro(m, RTLD_GLOBAL)) return -1;
#endif
#if HAVE_DECL_RTLD_LOCAL
    if (PyModule_AddIntMacro(m, RTLD_LOCAL)) return -1;
#endif
#if HAVE_DECL_RTLD_NODELETE
    if (PyModule_AddIntMacro(m, RTLD_NODELETE)) return -1;
#endif
#if HAVE_DECL_RTLD_NOLOAD
    if (PyModule_AddIntMacro(m, RTLD_NOLOAD)) return -1;
#endif
#if HAVE_DECL_RTLD_DEEPBIND
    if (PyModule_AddIntMacro(m, RTLD_DEEPBIND)) return -1;
#endif

#ifdef HAVE_GETRANDOM_SYSCALL
    if (PyModule_AddIntMacro(m, GRND_RANDOM)) return -1;
    if (PyModule_AddIntMacro(m, GRND_NONBLOCK)) return -1;
#endif

    return 0;
}


static struct PyModuleDef posixmodule = {
    PyModuleDef_HEAD_INIT,
    MODNAME,
    posix__doc__,
    -1,
    posix_methods,
    NULL,
    NULL,
    NULL,
    NULL
};


static const char * const have_functions[] = {

#ifdef HAVE_FACCESSAT
    "HAVE_FACCESSAT",
#endif

#ifdef HAVE_FCHDIR
    "HAVE_FCHDIR",
#endif

#ifdef HAVE_FCHMOD
    "HAVE_FCHMOD",
#endif

#ifdef HAVE_FCHMODAT
    "HAVE_FCHMODAT",
#endif

#ifdef HAVE_FCHOWN
    "HAVE_FCHOWN",
#endif

#ifdef HAVE_FCHOWNAT
    "HAVE_FCHOWNAT",
#endif

#ifdef HAVE_FEXECVE
    "HAVE_FEXECVE",
#endif

#ifdef HAVE_FDOPENDIR
    "HAVE_FDOPENDIR",
#endif

#ifdef HAVE_FPATHCONF
    "HAVE_FPATHCONF",
#endif

#ifdef HAVE_FSTATAT
    "HAVE_FSTATAT",
#endif

#ifdef HAVE_FSTATVFS
    "HAVE_FSTATVFS",
#endif

#if defined HAVE_FTRUNCATE || defined MS_WINDOWS
    "HAVE_FTRUNCATE",
#endif

#ifdef HAVE_FUTIMENS
    "HAVE_FUTIMENS",
#endif

#ifdef HAVE_FUTIMES
    "HAVE_FUTIMES",
#endif

#ifdef HAVE_FUTIMESAT
    "HAVE_FUTIMESAT",
#endif

#ifdef HAVE_LINKAT
    "HAVE_LINKAT",
#endif

#ifdef HAVE_LCHFLAGS
    "HAVE_LCHFLAGS",
#endif

#ifdef HAVE_LCHMOD
    "HAVE_LCHMOD",
#endif

#ifdef HAVE_LCHOWN
    "HAVE_LCHOWN",
#endif

#ifdef HAVE_LSTAT
    "HAVE_LSTAT",
#endif

#ifdef HAVE_LUTIMES
    "HAVE_LUTIMES",
#endif

#ifdef HAVE_MKDIRAT
    "HAVE_MKDIRAT",
#endif

#ifdef HAVE_MKFIFOAT
    "HAVE_MKFIFOAT",
#endif

#ifdef HAVE_MKNODAT
    "HAVE_MKNODAT",
#endif

#ifdef HAVE_OPENAT
    "HAVE_OPENAT",
#endif

#ifdef HAVE_READLINKAT
    "HAVE_READLINKAT",
#endif

#ifdef HAVE_RENAMEAT
    "HAVE_RENAMEAT",
#endif

#ifdef HAVE_SYMLINKAT
    "HAVE_SYMLINKAT",
#endif

#ifdef HAVE_UNLINKAT
    "HAVE_UNLINKAT",
#endif

#ifdef HAVE_UTIMENSAT
    "HAVE_UTIMENSAT",
#endif

#ifdef MS_WINDOWS
    "MS_WINDOWS",
#endif

    NULL
};


PyMODINIT_FUNC
INITFUNC(void)
{
    PyObject *m, *v;
    PyObject *list;
    const char * const *trace;

#if defined(HAVE_SYMLINK) && defined(MS_WINDOWS)
    win32_can_symlink = enable_symlink();
#endif

    m = PyModule_Create(&posixmodule);
    if (m == NULL)
        return NULL;

    /* Initialize environ dictionary */
    v = convertenviron();
    Py_XINCREF(v);
    if (v == NULL || PyModule_AddObject(m, "environ", v) != 0)
        return NULL;
    Py_DECREF(v);

    if (all_ins(m))
        return NULL;

    if (setup_confname_tables(m))
        return NULL;

    Py_INCREF(PyExc_OSError);
    PyModule_AddObject(m, "error", PyExc_OSError);

#ifdef HAVE_PUTENV
    if (posix_putenv_garbage == NULL)
        posix_putenv_garbage = PyDict_New();
#endif

    if (!initialized) {
#if defined(HAVE_WAITID) && !defined(__APPLE__)
        waitid_result_desc.name = MODNAME ".waitid_result";
        if (PyStructSequence_InitType2(&WaitidResultType, &waitid_result_desc) < 0)
            return NULL;
#endif

        stat_result_desc.name = "os.stat_result"; /* see issue #19209 */
        stat_result_desc.fields[7].name = PyStructSequence_UnnamedField;
        stat_result_desc.fields[8].name = PyStructSequence_UnnamedField;
        stat_result_desc.fields[9].name = PyStructSequence_UnnamedField;
        if (PyStructSequence_InitType2(&StatResultType, &stat_result_desc) < 0)
            return NULL;
        structseq_new = StatResultType.tp_new;
        StatResultType.tp_new = statresult_new;

        statvfs_result_desc.name = "os.statvfs_result"; /* see issue #19209 */
        if (PyStructSequence_InitType2(&StatVFSResultType,
                                       &statvfs_result_desc) < 0)
            return NULL;
#ifdef NEED_TICKS_PER_SECOND
#  if defined(HAVE_SYSCONF) && defined(_SC_CLK_TCK)
        ticks_per_second = sysconf(_SC_CLK_TCK);
#  elif defined(HZ)
        ticks_per_second = HZ;
#  else
        ticks_per_second = 60; /* magic fallback value; may be bogus */
#  endif
#endif

#if defined(HAVE_SCHED_SETPARAM) || defined(HAVE_SCHED_SETSCHEDULER)
        sched_param_desc.name = MODNAME ".sched_param";
        if (PyStructSequence_InitType2(&SchedParamType, &sched_param_desc) < 0)
            return NULL;
        SchedParamType.tp_new = os_sched_param;
#endif

        /* initialize TerminalSize_info */
        if (PyStructSequence_InitType2(&TerminalSizeType,
                                       &TerminalSize_desc) < 0)
            return NULL;

        /* initialize scandir types */
        if (PyType_Ready(&ScandirIteratorType) < 0)
            return NULL;
        if (PyType_Ready(&DirEntryType) < 0)
            return NULL;
    }
#if defined(HAVE_WAITID) && !defined(__APPLE__)
    Py_INCREF((PyObject*) &WaitidResultType);
    PyModule_AddObject(m, "waitid_result", (PyObject*) &WaitidResultType);
#endif
    Py_INCREF((PyObject*) &StatResultType);
    PyModule_AddObject(m, "stat_result", (PyObject*) &StatResultType);
    Py_INCREF((PyObject*) &StatVFSResultType);
    PyModule_AddObject(m, "statvfs_result",
                       (PyObject*) &StatVFSResultType);

#if defined(HAVE_SCHED_SETPARAM) || defined(HAVE_SCHED_SETSCHEDULER)
    Py_INCREF(&SchedParamType);
    PyModule_AddObject(m, "sched_param", (PyObject *)&SchedParamType);
#endif

    times_result_desc.name = MODNAME ".times_result";
    if (PyStructSequence_InitType2(&TimesResultType, &times_result_desc) < 0)
        return NULL;
    PyModule_AddObject(m, "times_result", (PyObject *)&TimesResultType);

    uname_result_desc.name = MODNAME ".uname_result";
    if (PyStructSequence_InitType2(&UnameResultType, &uname_result_desc) < 0)
        return NULL;
    PyModule_AddObject(m, "uname_result", (PyObject *)&UnameResultType);

#ifdef __APPLE__
    /*
     * Step 2 of weak-linking support on Mac OS X.
     *
     * The code below removes functions that are not available on the
     * currently active platform.
     *
     * This block allow one to use a python binary that was build on
     * OSX 10.4 on OSX 10.3, without losing access to new APIs on
     * OSX 10.4.
     */
#ifdef HAVE_FSTATVFS
    if (fstatvfs == NULL) {
        if (PyObject_DelAttrString(m, "fstatvfs") == -1) {
            return NULL;
        }
    }
#endif /* HAVE_FSTATVFS */

#ifdef HAVE_STATVFS
    if (statvfs == NULL) {
        if (PyObject_DelAttrString(m, "statvfs") == -1) {
            return NULL;
        }
    }
#endif /* HAVE_STATVFS */

# ifdef HAVE_LCHOWN
    if (lchown == NULL) {
        if (PyObject_DelAttrString(m, "lchown") == -1) {
            return NULL;
        }
    }
#endif /* HAVE_LCHOWN */


#endif /* __APPLE__ */

    Py_INCREF(&TerminalSizeType);
    PyModule_AddObject(m, "terminal_size", (PyObject*) &TerminalSizeType);

    billion = PyLong_FromLong(1000000000);
    if (!billion)
        return NULL;

    /* suppress "function not used" warnings */
    {
    int ignored;
    fd_specified("", -1);
    follow_symlinks_specified("", 1);
    dir_fd_and_follow_symlinks_invalid("chmod", DEFAULT_DIR_FD, 1);
    dir_fd_converter(Py_None, &ignored);
    dir_fd_unavailable(Py_None, &ignored);
    }

    /*
     * provide list of locally available functions
     * so os.py can populate support_* lists
     */
    list = PyList_New(0);
    if (!list)
        return NULL;
    for (trace = have_functions; *trace; trace++) {
        PyObject *unicode = PyUnicode_DecodeASCII(*trace, strlen(*trace), NULL);
        if (!unicode)
            return NULL;
        if (PyList_Append(list, unicode))
            return NULL;
        Py_DECREF(unicode);
    }
    PyModule_AddObject(m, "_have_functions", list);

    Py_INCREF((PyObject *) &DirEntryType);
    PyModule_AddObject(m, "DirEntry", (PyObject *)&DirEntryType);

    initialized = 1;

    return m;
}

#ifdef __cplusplus
}
#endif
