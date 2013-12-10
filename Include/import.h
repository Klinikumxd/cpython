
/* Module definition and import interface */

#ifndef Py_IMPORT_H
#define Py_IMPORT_H
#ifdef __cplusplus
extern "C" {
#endif

PyAPI_FUNC(void) _PyImportZip_Init(void);

PyMODINIT_FUNC PyInit_imp(void);
PyAPI_FUNC(long) PyImport_GetMagicNumber(void);
PyAPI_FUNC(const char *) PyImport_GetMagicTag(void);
PyAPI_FUNC(PyObject *) PyImport_ExecCodeModule(
    char *name,                 /* UTF-8 encoded string */
    PyObject *co
    );
PyAPI_FUNC(PyObject *) PyImport_ExecCodeModuleEx(
    char *name,                 /* UTF-8 encoded string */
    PyObject *co,
    char *pathname              /* decoded from the filesystem encoding */
    );
PyAPI_FUNC(PyObject *) PyImport_ExecCodeModuleWithPathnames(
    char *name,                 /* UTF-8 encoded string */
    PyObject *co,
    char *pathname,             /* decoded from the filesystem encoding */
    char *cpathname             /* decoded from the filesystem encoding */
    );
PyAPI_FUNC(PyObject *) PyImport_ExecCodeModuleObject(
    PyObject *name,
    PyObject *co,
    PyObject *pathname,
    PyObject *cpathname
    );
PyAPI_FUNC(PyObject *) PyImport_GetModuleDict(void);
PyAPI_FUNC(PyObject *) PyImport_AddModuleObject(
    PyObject *name
    );
PyAPI_FUNC(PyObject *) PyImport_AddModule(
    const char *name            /* UTF-8 encoded string */
    );
PyAPI_FUNC(PyObject *) PyImport_ImportModule(
    const char *name            /* UTF-8 encoded string */
    );
PyAPI_FUNC(PyObject *) PyImport_ImportModuleNoBlock(
    const char *name            /* UTF-8 encoded string */
    );
PyAPI_FUNC(PyObject *) PyImport_ImportModuleLevel(
    const char *name,           /* UTF-8 encoded string */
    PyObject *globals,
    PyObject *locals,
    PyObject *fromlist,
    int level
    );
PyAPI_FUNC(PyObject *) PyImport_ImportModuleLevelObject(
    PyObject *name,
    PyObject *globals,
    PyObject *locals,
    PyObject *fromlist,
    int level
    );

#define PyImport_ImportModuleEx(n, g, l, f) \
    PyImport_ImportModuleLevel(n, g, l, f, 0)

PyAPI_FUNC(PyObject *) PyImport_GetImporter(PyObject *path);
PyAPI_FUNC(PyObject *) PyImport_Import(PyObject *name);
PyAPI_FUNC(PyObject *) PyImport_ReloadModule(PyObject *m);
PyAPI_FUNC(void) PyImport_Cleanup(void);
PyAPI_FUNC(int) PyImport_ImportFrozenModuleObject(
    PyObject *name
    );
PyAPI_FUNC(int) PyImport_ImportFrozenModule(
    char *name                  /* UTF-8 encoded string */
    );

#ifndef Py_LIMITED_API
#ifdef WITH_THREAD
PyAPI_FUNC(void) _PyImport_AcquireLock(void);
PyAPI_FUNC(int) _PyImport_ReleaseLock(void);
#else
#define _PyImport_AcquireLock()
#define _PyImport_ReleaseLock() 1
#endif

PyAPI_FUNC(void) _PyImport_ReInitLock(void);

PyAPI_FUNC(PyObject *) _PyImport_FindBuiltin(
    const char *name            /* UTF-8 encoded string */
    );
PyAPI_FUNC(PyObject *) _PyImport_FindExtensionObject(PyObject *, PyObject *);
PyAPI_FUNC(int) _PyImport_FixupBuiltin(
    PyObject *mod,
    char *name                  /* UTF-8 encoded string */
    );
PyAPI_FUNC(int) _PyImport_FixupExtensionObject(PyObject*, PyObject *, PyObject *);

struct _inittab {
    char *name;                 /* ASCII encoded string */
    PyObject* (*initfunc)(void);
};
PyAPI_DATA(struct _inittab *) PyImport_Inittab;
PyAPI_FUNC(int) PyImport_ExtendInittab(struct _inittab *newtab);
#endif /* Py_LIMITED_API */

PyAPI_DATA(PyTypeObject) PyNullImporter_Type;

PyAPI_FUNC(int) PyImport_AppendInittab(
    const char *name,           /* ASCII encoded string */
    PyObject* (*initfunc)(void)
    );

#ifndef Py_LIMITED_API
struct _frozen {
    char *name;                 /* ASCII encoded string */
    unsigned char *code;
    int size;
};

/* Embedding apps may change this pointer to point to their favorite
   collection of frozen modules: */

PyAPI_DATA(struct _frozen *) PyImport_FrozenModules;
#endif

#ifdef __cplusplus
}
#endif
#endif /* !Py_IMPORT_H */
