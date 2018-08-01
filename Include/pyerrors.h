#ifndef Py_ERRORS_H
#define Py_ERRORS_H
#ifdef __cplusplus
extern "C" {
#endif

/* Error objects */

#ifndef Py_LIMITED_API
/* PyException_HEAD defines the initial segment of every exception class. */
#define PyException_HEAD PyObject_HEAD PyObject *dict;\
             PyObject *args; PyObject *traceback;\
             PyObject *context; PyObject *cause;\
             char suppress_context;

typedef struct {
    PyException_HEAD
} PyBaseExceptionObject;

typedef struct {
    PyException_HEAD
    PyObject *msg;
    PyObject *filename;
    PyObject *lineno;
    PyObject *offset;
    PyObject *text;
    PyObject *print_file_and_line;
} PySyntaxErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *msg;
    PyObject *name;
    PyObject *path;
} PyImportErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *encoding;
    PyObject *object;
    Py_ssize_t start;
    Py_ssize_t end;
    PyObject *reason;
} PyUnicodeErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *code;
} PySystemExitObject;

typedef struct {
    PyException_HEAD
    PyObject *myerrno;
    PyObject *strerror;
    PyObject *filename;
    PyObject *filename2;
#ifdef MS_WINDOWS
    PyObject *winerror;
#endif
    Py_ssize_t written;   /* only for BlockingIOError, -1 otherwise */
} PyOSErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *value;
} PyStopIterationObject;

/* Compatibility typedefs */
typedef PyOSErrorObject PyEnvironmentErrorObject;
#ifdef MS_WINDOWS
typedef PyOSErrorObject PyWindowsErrorObject;
#endif
#endif /* !Py_LIMITED_API */

/* Error handling definitions */

PyAPI_FUNC(void) PyErr_SetNone(PyObject *);
PyAPI_FUNC(void) PyErr_SetObject(PyObject *, PyObject *);
#ifndef Py_LIMITED_API
PyAPI_FUNC(void) _PyErr_SetKeyError(PyObject *);
_PyErr_StackItem *_PyErr_GetTopmostException(PyThreadState *tstate);
#endif
PyAPI_FUNC(void) PyErr_SetString(
    PyObject *exception,
    const char *string   /* decoded from utf-8 */
    );
PyAPI_FUNC(PyObject *) PyErr_Occurred(void);
PyAPI_FUNC(void) PyErr_Clear(void);
PyAPI_FUNC(void) PyErr_Fetch(PyObject **, PyObject **, PyObject **);
PyAPI_FUNC(void) PyErr_Restore(PyObject *, PyObject *, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyAPI_FUNC(void) PyErr_GetExcInfo(PyObject **, PyObject **, PyObject **);
PyAPI_FUNC(void) PyErr_SetExcInfo(PyObject *, PyObject *, PyObject *);
#endif

#if defined(__clang__) || \
    (defined(__GNUC_MAJOR__) && \
     ((__GNUC_MAJOR__ >= 3) || \
      (__GNUC_MAJOR__ == 2) && (__GNUC_MINOR__ >= 5)))
#  define _Py_NO_RETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#  define _Py_NO_RETURN __declspec(noreturn)
#else
#  define _Py_NO_RETURN
#endif

/* Defined in Python/pylifecycle.c */
PyAPI_FUNC(void) _Py_NO_RETURN Py_FatalError(const char *message);

#if defined(Py_DEBUG) || defined(Py_LIMITED_API)
#define _PyErr_OCCURRED() PyErr_Occurred()
#else
#define _PyErr_OCCURRED() (PyThreadState_GET()->curexc_type)
#endif

/* Error testing and normalization */
PyAPI_FUNC(int) PyErr_GivenExceptionMatches(PyObject *, PyObject *);
PyAPI_FUNC(int) PyErr_ExceptionMatches(PyObject *);
PyAPI_FUNC(void) PyErr_NormalizeException(PyObject**, PyObject**, PyObject**);

/* Traceback manipulation (PEP 3134) */
PyAPI_FUNC(int) PyException_SetTraceback(PyObject *, PyObject *);
PyAPI_FUNC(PyObject *) PyException_GetTraceback(PyObject *);

/* Cause manipulation (PEP 3134) */
PyAPI_FUNC(PyObject *) PyException_GetCause(PyObject *);
PyAPI_FUNC(void) PyException_SetCause(PyObject *, PyObject *);

/* Context manipulation (PEP 3134) */
PyAPI_FUNC(PyObject *) PyException_GetContext(PyObject *);
PyAPI_FUNC(void) PyException_SetContext(PyObject *, PyObject *);
#ifndef Py_LIMITED_API
PyAPI_FUNC(void) _PyErr_ChainExceptions(PyObject *, PyObject *, PyObject *);
#endif

/* */

#define PyExceptionClass_Check(x)                                       \
    (PyType_Check((x)) &&                                               \
     PyType_FastSubclass((PyTypeObject*)(x), Py_TPFLAGS_BASE_EXC_SUBCLASS))

#define PyExceptionInstance_Check(x)                    \
    PyType_FastSubclass((x)->ob_type, Py_TPFLAGS_BASE_EXC_SUBCLASS)

PyAPI_FUNC(const char *) PyExceptionClass_Name(PyObject *);
#ifndef Py_LIMITED_API
#define PyExceptionClass_Name(x)  (((PyTypeObject*)(x))->tp_name)
#endif

#define PyExceptionInstance_Class(x) ((PyObject*)((x)->ob_type))


/* Predefined exceptions */

PyAPI_DATA(PyObject *) PyExc_BaseException;
PyAPI_DATA(PyObject *) PyExc_Exception;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
PyAPI_DATA(PyObject *) PyExc_StopAsyncIteration;
#endif
PyAPI_DATA(PyObject *) PyExc_StopIteration;
PyAPI_DATA(PyObject *) PyExc_GeneratorExit;
PyAPI_DATA(PyObject *) PyExc_ArithmeticError;
PyAPI_DATA(PyObject *) PyExc_LookupError;

PyAPI_DATA(PyObject *) PyExc_AssertionError;
PyAPI_DATA(PyObject *) PyExc_AttributeError;
PyAPI_DATA(PyObject *) PyExc_BufferError;
PyAPI_DATA(PyObject *) PyExc_EOFError;
PyAPI_DATA(PyObject *) PyExc_FloatingPointError;
PyAPI_DATA(PyObject *) PyExc_OSError;
PyAPI_DATA(PyObject *) PyExc_ImportError;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
PyAPI_DATA(PyObject *) PyExc_ModuleNotFoundError;
#endif
PyAPI_DATA(PyObject *) PyExc_IndexError;
PyAPI_DATA(PyObject *) PyExc_KeyError;
PyAPI_DATA(PyObject *) PyExc_KeyboardInterrupt;
PyAPI_DATA(PyObject *) PyExc_MemoryError;
PyAPI_DATA(PyObject *) PyExc_NameError;
PyAPI_DATA(PyObject *) PyExc_OverflowError;
PyAPI_DATA(PyObject *) PyExc_RuntimeError;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
PyAPI_DATA(PyObject *) PyExc_RecursionError;
#endif
PyAPI_DATA(PyObject *) PyExc_NotImplementedError;
PyAPI_DATA(PyObject *) PyExc_SyntaxError;
PyAPI_DATA(PyObject *) PyExc_IndentationError;
PyAPI_DATA(PyObject *) PyExc_TabError;
PyAPI_DATA(PyObject *) PyExc_ReferenceError;
PyAPI_DATA(PyObject *) PyExc_SystemError;
PyAPI_DATA(PyObject *) PyExc_SystemExit;
PyAPI_DATA(PyObject *) PyExc_TypeError;
PyAPI_DATA(PyObject *) PyExc_UnboundLocalError;
PyAPI_DATA(PyObject *) PyExc_UnicodeError;
PyAPI_DATA(PyObject *) PyExc_UnicodeEncodeError;
PyAPI_DATA(PyObject *) PyExc_UnicodeDecodeError;
PyAPI_DATA(PyObject *) PyExc_UnicodeTranslateError;
PyAPI_DATA(PyObject *) PyExc_ValueError;
PyAPI_DATA(PyObject *) PyExc_ZeroDivisionError;

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyAPI_DATA(PyObject *) PyExc_BlockingIOError;
PyAPI_DATA(PyObject *) PyExc_BrokenPipeError;
PyAPI_DATA(PyObject *) PyExc_ChildProcessError;
PyAPI_DATA(PyObject *) PyExc_ConnectionError;
PyAPI_DATA(PyObject *) PyExc_ConnectionAbortedError;
PyAPI_DATA(PyObject *) PyExc_ConnectionRefusedError;
PyAPI_DATA(PyObject *) PyExc_ConnectionResetError;
PyAPI_DATA(PyObject *) PyExc_FileExistsError;
PyAPI_DATA(PyObject *) PyExc_FileNotFoundError;
PyAPI_DATA(PyObject *) PyExc_InterruptedError;
PyAPI_DATA(PyObject *) PyExc_IsADirectoryError;
PyAPI_DATA(PyObject *) PyExc_NotADirectoryError;
PyAPI_DATA(PyObject *) PyExc_PermissionError;
PyAPI_DATA(PyObject *) PyExc_ProcessLookupError;
PyAPI_DATA(PyObject *) PyExc_TimeoutError;
#endif


/* Compatibility aliases */
PyAPI_DATA(PyObject *) PyExc_EnvironmentError;
PyAPI_DATA(PyObject *) PyExc_IOError;
#ifdef MS_WINDOWS
PyAPI_DATA(PyObject *) PyExc_WindowsError;
#endif

/* Predefined warning categories */
PyAPI_DATA(PyObject *) PyExc_Warning;
PyAPI_DATA(PyObject *) PyExc_UserWarning;
PyAPI_DATA(PyObject *) PyExc_DeprecationWarning;
PyAPI_DATA(PyObject *) PyExc_PendingDeprecationWarning;
PyAPI_DATA(PyObject *) PyExc_SyntaxWarning;
PyAPI_DATA(PyObject *) PyExc_RuntimeWarning;
PyAPI_DATA(PyObject *) PyExc_FutureWarning;
PyAPI_DATA(PyObject *) PyExc_ImportWarning;
PyAPI_DATA(PyObject *) PyExc_UnicodeWarning;
PyAPI_DATA(PyObject *) PyExc_BytesWarning;
PyAPI_DATA(PyObject *) PyExc_ResourceWarning;


/* Convenience functions */

PyAPI_FUNC(int) PyErr_BadArgument(void);
PyAPI_FUNC(PyObject *) PyErr_NoMemory(void);
PyAPI_FUNC(PyObject *) PyErr_SetFromErrno(PyObject *);
PyAPI_FUNC(PyObject *) PyErr_SetFromErrnoWithFilenameObject(
    PyObject *, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03040000
PyAPI_FUNC(PyObject *) PyErr_SetFromErrnoWithFilenameObjects(
    PyObject *, PyObject *, PyObject *);
#endif
PyAPI_FUNC(PyObject *) PyErr_SetFromErrnoWithFilename(
    PyObject *exc,
    const char *filename   /* decoded from the filesystem encoding */
    );
#if defined(MS_WINDOWS) && !defined(Py_LIMITED_API)
PyAPI_FUNC(PyObject *) PyErr_SetFromErrnoWithUnicodeFilename(
    PyObject *, const Py_UNICODE *) Py_DEPRECATED(3.3);
#endif /* MS_WINDOWS */

PyAPI_FUNC(PyObject *) PyErr_Format(
    PyObject *exception,
    const char *format,   /* ASCII-encoded string  */
    ...
    );
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
PyAPI_FUNC(PyObject *) PyErr_FormatV(
    PyObject *exception,
    const char *format,
    va_list vargs);
#endif

#ifndef Py_LIMITED_API
/* Like PyErr_Format(), but saves current exception as __context__ and
   __cause__.
 */
PyAPI_FUNC(PyObject *) _PyErr_FormatFromCause(
    PyObject *exception,
    const char *format,   /* ASCII-encoded string  */
    ...
    );
#endif

#ifdef MS_WINDOWS
PyAPI_FUNC(PyObject *) PyErr_SetFromWindowsErrWithFilename(
    int ierr,
    const char *filename        /* decoded from the filesystem encoding */
    );
#ifndef Py_LIMITED_API
/* XXX redeclare to use WSTRING */
PyAPI_FUNC(PyObject *) PyErr_SetFromWindowsErrWithUnicodeFilename(
    int, const Py_UNICODE *) Py_DEPRECATED(3.3);
#endif
PyAPI_FUNC(PyObject *) PyErr_SetFromWindowsErr(int);
PyAPI_FUNC(PyObject *) PyErr_SetExcFromWindowsErrWithFilenameObject(
    PyObject *,int, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03040000
PyAPI_FUNC(PyObject *) PyErr_SetExcFromWindowsErrWithFilenameObjects(
    PyObject *,int, PyObject *, PyObject *);
#endif
PyAPI_FUNC(PyObject *) PyErr_SetExcFromWindowsErrWithFilename(
    PyObject *exc,
    int ierr,
    const char *filename        /* decoded from the filesystem encoding */
    );
#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) PyErr_SetExcFromWindowsErrWithUnicodeFilename(
    PyObject *,int, const Py_UNICODE *) Py_DEPRECATED(3.3);
#endif
PyAPI_FUNC(PyObject *) PyErr_SetExcFromWindowsErr(PyObject *, int);
#endif /* MS_WINDOWS */

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
PyAPI_FUNC(PyObject *) PyErr_SetImportErrorSubclass(PyObject *, PyObject *,
    PyObject *, PyObject *);
#endif
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyAPI_FUNC(PyObject *) PyErr_SetImportError(PyObject *, PyObject *,
    PyObject *);
#endif

/* Export the old function so that the existing API remains available: */
PyAPI_FUNC(void) PyErr_BadInternalCall(void);
PyAPI_FUNC(void) _PyErr_BadInternalCall(const char *filename, int lineno);
/* Mask the old API with a call to the new API for code compiled under
   Python 2.0: */
#define PyErr_BadInternalCall() _PyErr_BadInternalCall(__FILE__, __LINE__)

/* Function to create a new exception */
PyAPI_FUNC(PyObject *) PyErr_NewException(
    const char *name, PyObject *base, PyObject *dict);
PyAPI_FUNC(PyObject *) PyErr_NewExceptionWithDoc(
    const char *name, const char *doc, PyObject *base, PyObject *dict);
PyAPI_FUNC(void) PyErr_WriteUnraisable(PyObject *);

/* In exceptions.c */
#ifndef Py_LIMITED_API
/* Helper that attempts to replace the current exception with one of the
 * same type but with a prefix added to the exception text. The resulting
 * exception description looks like:
 *
 *     prefix (exc_type: original_exc_str)
 *
 * Only some exceptions can be safely replaced. If the function determines
 * it isn't safe to perform the replacement, it will leave the original
 * unmodified exception in place.
 *
 * Returns a borrowed reference to the new exception (if any), NULL if the
 * existing exception was left in place.
 */
PyAPI_FUNC(PyObject *) _PyErr_TrySetFromCause(
    const char *prefix_format,   /* ASCII-encoded string  */
    ...
    );
#endif


/* In signalmodule.c */
PyAPI_FUNC(int) PyErr_CheckSignals(void);
PyAPI_FUNC(void) PyErr_SetInterrupt(void);

/* In signalmodule.c */
#ifndef Py_LIMITED_API
int PySignal_SetWakeupFd(int fd);
#endif

/* Support for adding program text to SyntaxErrors */
PyAPI_FUNC(void) PyErr_SyntaxLocation(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno);
PyAPI_FUNC(void) PyErr_SyntaxLocationEx(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno,
    int col_offset);
#ifndef Py_LIMITED_API
PyAPI_FUNC(void) PyErr_SyntaxLocationObject(
    PyObject *filename,
    int lineno,
    int col_offset);
#endif
PyAPI_FUNC(PyObject *) PyErr_ProgramText(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno);
#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) PyErr_ProgramTextObject(
    PyObject *filename,
    int lineno);
#endif

/* The following functions are used to create and modify unicode
   exceptions from C */

/* create a UnicodeDecodeError object */
PyAPI_FUNC(PyObject *) PyUnicodeDecodeError_Create(
    const char *encoding,       /* UTF-8 encoded string */
    const char *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );

/* create a UnicodeEncodeError object */
#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) PyUnicodeEncodeError_Create(
    const char *encoding,       /* UTF-8 encoded string */
    const Py_UNICODE *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    ) Py_DEPRECATED(3.3);
#endif

/* create a UnicodeTranslateError object */
#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) PyUnicodeTranslateError_Create(
    const Py_UNICODE *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    ) Py_DEPRECATED(3.3);
PyAPI_FUNC(PyObject *) _PyUnicodeTranslateError_Create(
    PyObject *object,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );
#endif

/* get the encoding attribute */
PyAPI_FUNC(PyObject *) PyUnicodeEncodeError_GetEncoding(PyObject *);
PyAPI_FUNC(PyObject *) PyUnicodeDecodeError_GetEncoding(PyObject *);

/* get the object attribute */
PyAPI_FUNC(PyObject *) PyUnicodeEncodeError_GetObject(PyObject *);
PyAPI_FUNC(PyObject *) PyUnicodeDecodeError_GetObject(PyObject *);
PyAPI_FUNC(PyObject *) PyUnicodeTranslateError_GetObject(PyObject *);

/* get the value of the start attribute (the int * may not be NULL)
   return 0 on success, -1 on failure */
PyAPI_FUNC(int) PyUnicodeEncodeError_GetStart(PyObject *, Py_ssize_t *);
PyAPI_FUNC(int) PyUnicodeDecodeError_GetStart(PyObject *, Py_ssize_t *);
PyAPI_FUNC(int) PyUnicodeTranslateError_GetStart(PyObject *, Py_ssize_t *);

/* assign a new value to the start attribute
   return 0 on success, -1 on failure */
PyAPI_FUNC(int) PyUnicodeEncodeError_SetStart(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) PyUnicodeDecodeError_SetStart(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) PyUnicodeTranslateError_SetStart(PyObject *, Py_ssize_t);

/* get the value of the end attribute (the int *may not be NULL)
 return 0 on success, -1 on failure */
PyAPI_FUNC(int) PyUnicodeEncodeError_GetEnd(PyObject *, Py_ssize_t *);
PyAPI_FUNC(int) PyUnicodeDecodeError_GetEnd(PyObject *, Py_ssize_t *);
PyAPI_FUNC(int) PyUnicodeTranslateError_GetEnd(PyObject *, Py_ssize_t *);

/* assign a new value to the end attribute
   return 0 on success, -1 on failure */
PyAPI_FUNC(int) PyUnicodeEncodeError_SetEnd(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) PyUnicodeDecodeError_SetEnd(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) PyUnicodeTranslateError_SetEnd(PyObject *, Py_ssize_t);

/* get the value of the reason attribute */
PyAPI_FUNC(PyObject *) PyUnicodeEncodeError_GetReason(PyObject *);
PyAPI_FUNC(PyObject *) PyUnicodeDecodeError_GetReason(PyObject *);
PyAPI_FUNC(PyObject *) PyUnicodeTranslateError_GetReason(PyObject *);

/* assign a new value to the reason attribute
   return 0 on success, -1 on failure */
PyAPI_FUNC(int) PyUnicodeEncodeError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );
PyAPI_FUNC(int) PyUnicodeDecodeError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );
PyAPI_FUNC(int) PyUnicodeTranslateError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );

/* These APIs aren't really part of the error implementation, but
   often needed to format error messages; the native C lib APIs are
   not available on all platforms, which is why we provide emulations
   for those platforms in Python/mysnprintf.c,
   WARNING:  The return value of snprintf varies across platforms; do
   not rely on any particular behavior; eventually the C99 defn may
   be reliable.
*/
#if defined(MS_WIN32) && !defined(HAVE_SNPRINTF)
# define HAVE_SNPRINTF
# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

#include <stdarg.h>
PyAPI_FUNC(int) PyOS_snprintf(char *str, size_t size, const char  *format, ...)
                        Py_GCC_ATTRIBUTE((format(printf, 3, 4)));
PyAPI_FUNC(int) PyOS_vsnprintf(char *str, size_t size, const char  *format, va_list va)
                        Py_GCC_ATTRIBUTE((format(printf, 3, 0)));

#ifdef __cplusplus
}
#endif
#endif /* !Py_ERRORS_H */
