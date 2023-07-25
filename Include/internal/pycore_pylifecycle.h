#ifndef Py_INTERNAL_LIFECYCLE_H
#define Py_INTERNAL_LIFECYCLE_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

#include "pycore_runtime.h"       // _PyRuntimeState

/* Forward declarations */
struct _PyArgv;
struct pyruntimestate;

extern int _Py_SetFileSystemEncoding(
    const char *encoding,
    const char *errors);
extern void _Py_ClearFileSystemEncoding(void);
extern PyStatus _PyUnicode_InitEncodings(PyThreadState *tstate);
#ifdef MS_WINDOWS
extern int _PyUnicode_EnableLegacyWindowsFSEncoding(void);
#endif

extern int _Py_IsLocaleCoercionTarget(const char *ctype_loc);

/* Various one-time initializers */

extern void _Py_InitVersion(void);
extern PyStatus _PyFaulthandler_Init(int enable);
extern PyObject * _PyBuiltin_Init(PyInterpreterState *interp);
extern PyStatus _PySys_Create(
    PyThreadState *tstate,
    PyObject **sysmod_p);
extern PyStatus _PySys_ReadPreinitWarnOptions(PyWideStringList *options);
extern PyStatus _PySys_ReadPreinitXOptions(PyConfig *config);
extern int _PySys_UpdateConfig(PyThreadState *tstate);
extern void _PySys_FiniTypes(PyInterpreterState *interp);
extern int _PyBuiltins_AddExceptions(PyObject * bltinmod);
extern PyStatus _Py_HashRandomization_Init(const PyConfig *);

extern PyStatus _PyTime_Init(void);
extern PyStatus _PyGC_Init(PyInterpreterState *interp);
extern PyStatus _PyAtExit_Init(PyInterpreterState *interp);
extern int _Py_Deepfreeze_Init(void);

/* Various internal finalizers */

extern int _PySignal_Init(int install_signal_handlers);
extern void _PySignal_Fini(void);

extern void _PyGC_Fini(PyInterpreterState *interp);
extern void _Py_HashRandomization_Fini(void);
extern void _PyFaulthandler_Fini(void);
extern void _PyHash_Fini(void);
extern void _PyTraceMalloc_Fini(void);
extern void _PyWarnings_Fini(PyInterpreterState *interp);
extern void _PyAST_Fini(PyInterpreterState *interp);
extern void _PyAtExit_Fini(PyInterpreterState *interp);
extern void _PyThread_FiniType(PyInterpreterState *interp);
extern void _Py_Deepfreeze_Fini(void);
extern void _PyArg_Fini(void);
extern void _Py_FinalizeAllocatedBlocks(_PyRuntimeState *);

extern PyStatus _PyGILState_Init(PyInterpreterState *interp);
extern PyStatus _PyGILState_SetTstate(PyThreadState *tstate);
extern void _PyGILState_Fini(PyInterpreterState *interp);

extern void _PyGC_DumpShutdownStats(PyInterpreterState *interp);

extern PyStatus _Py_PreInitializeFromPyArgv(
    const PyPreConfig *src_config,
    const struct _PyArgv *args);
extern PyStatus _Py_PreInitializeFromConfig(
    const PyConfig *config,
    const struct _PyArgv *args);

extern wchar_t * _Py_GetStdlibDir(void);

extern int _Py_HandleSystemExit(int *exitcode_p);

extern PyObject* _PyErr_WriteUnraisableDefaultHook(PyObject *unraisable);

extern void _PyErr_Print(PyThreadState *tstate);
extern void _PyErr_Display(PyObject *file, PyObject *exception,
                                PyObject *value, PyObject *tb);
extern void _PyErr_DisplayException(PyObject *file, PyObject *exc);

extern void _PyThreadState_DeleteCurrent(PyThreadState *tstate);

extern void _PyAtExit_Call(PyInterpreterState *interp);

extern int _Py_IsCoreInitialized(void);

extern int _Py_FdIsInteractive(FILE *fp, PyObject *filename);

extern const char* _Py_gitidentifier(void);
extern const char* _Py_gitversion(void);

extern int _Py_IsFinalizing(void);
PyAPI_FUNC(int) _Py_IsInterpreterFinalizing(PyInterpreterState *interp);

/* Random */
extern int _PyOS_URandom(void *buffer, Py_ssize_t size);
// Export for '_random' shared extension
PyAPI_FUNC(int) _PyOS_URandomNonblock(void *buffer, Py_ssize_t size);

/* Legacy locale support */
extern int _Py_CoerceLegacyLocale(int warn);
extern int _Py_LegacyLocaleDetected(int warn);
PyAPI_FUNC(char*) _Py_SetLocaleFromEnv(int category);

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_LIFECYCLE_H */
