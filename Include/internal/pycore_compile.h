#ifndef Py_INTERNAL_COMPILE_H
#define Py_INTERNAL_COMPILE_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

struct _mod;   // Type defined in pycore_ast.h

// Export the symbol for test_peg_generator (built as a library)
PyAPI_FUNC(PyCodeObject*) _PyAST_Compile(
    struct _mod *mod,
    PyObject *filename,
    PyCompilerFlags *flags,
    int optimize,
    PyArena *arena);
extern PyFutureFeatures* _PyFuture_FromAST(
    struct _mod * mod,
    PyObject *filename
    );

extern PyObject* _Py_Mangle(PyObject *p, PyObject *name);

typedef struct {
    int optimize;
    int ff_features;
} _PyASTOptimizeState;

extern int _PyAST_Optimize(
    struct _mod *,
    PyArena *arena,
    _PyASTOptimizeState *state);

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_COMPILE_H */
