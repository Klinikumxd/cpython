/* The PyMem_ family:  low-level memory allocation interfaces.
   See objimpl.h for the PyObject_ memory family.
*/

#ifndef Py_PYMEM_H
#define Py_PYMEM_H

#include "pyport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* user context passed as the first argument to the 3 functions */
    void *ctx;

    /* allocate memory */
    void* (*malloc) (void *ctx, size_t size);

    /* allocate memory or resize a memory buffer */
    void* (*realloc) (void *ctx, void *ptr, size_t new_size);

    /* release memory */
    void (*free) (void *ctx, void *ptr);
} PyMemAllocators;

/* Raw memory allocators, system functions: malloc(), realloc(), free().

   These functions are thread-safe, the GIL does not need to be held. */

/* Get internal functions of PyMem_RawMalloc(), PyMem_RawRealloc() and
   PyMem_RawFree(). *ctx_p is an arbitrary user value. */
PyAPI_FUNC(void) PyMem_GetRawAllocators(PyMemAllocators *allocators);

/* Set internal functions of PyMem_RawMalloc(), PyMem_RawRealloc() and
   PyMem_RawFree(). ctx is an arbitrary user value.

   PyMem_SetupDebugHooks() should be called to reinstall debug hooks if new
   functions do no call original functions anymore. */
PyAPI_FUNC(void) PyMem_SetRawAllocators(PyMemAllocators *allocators);

PyAPI_FUNC(void *) PyMem_RawMalloc(size_t size);
PyAPI_FUNC(void *) PyMem_RawRealloc(void *ptr, size_t new_size);
PyAPI_FUNC(void) PyMem_RawFree(void *ptr);


/* BEWARE:

   Each interface exports both functions and macros.  Extension modules should
   use the functions, to ensure binary compatibility across Python versions.
   Because the Python implementation is free to change internal details, and
   the macros may (or may not) expose details for speed, if you do use the
   macros you must recompile your extensions with each Python release.

   Never mix calls to PyMem_ with calls to the platform malloc/realloc/
   calloc/free.  For example, on Windows different DLLs may end up using
   different heaps, and if you use PyMem_Malloc you'll get the memory from the
   heap used by the Python DLL; it could be a disaster if you free()'ed that
   directly in your own extension.  Using PyMem_Free instead ensures Python
   can return the memory to the proper heap.  As another example, in
   PYMALLOC_DEBUG mode, Python wraps all calls to all PyMem_ and PyObject_
   memory functions in special debugging wrappers that add additional
   debugging info to dynamic memory blocks.  The system routines have no idea
   what to do with that stuff, and the Python wrappers have no idea what to do
   with raw blocks obtained directly by the system routines then.

   The GIL must be held when using these APIs.
*/

/*
 * Raw memory interface
 * ====================
 */

/* Functions

   Functions supplying platform-independent semantics for malloc/realloc/
   free.  These functions make sure that allocating 0 bytes returns a distinct
   non-NULL pointer (whenever possible -- if we're flat out of memory, NULL
   may be returned), even if the platform malloc and realloc don't.
   Returned pointers must be checked for NULL explicitly.  No action is
   performed on failure (no exception is set, no warning is printed, etc).
*/

PyAPI_FUNC(void *) PyMem_Malloc(size_t size);
PyAPI_FUNC(void *) PyMem_Realloc(void *ptr, size_t new_size);
PyAPI_FUNC(void) PyMem_Free(void *ptr);

/* Macros. */

/* PyMem_MALLOC(0) means malloc(1). Some systems would return NULL
   for malloc(0), which would be treated as an error. Some platforms
   would return a pointer with no memory behind it, which would break
   pymalloc. To solve these problems, allocate an extra byte. */
/* Returns NULL to indicate error if a negative size or size larger than
   Py_ssize_t can represent is supplied.  Helps prevents security holes. */
#define PyMem_MALLOC(n)         PyMem_Malloc(n)
#define PyMem_REALLOC(p, n)     PyMem_Realloc(p, n)
#define PyMem_FREE(p)           PyMem_Free(p)

/*
 * Type-oriented memory interface
 * ==============================
 *
 * Allocate memory for n objects of the given type.  Returns a new pointer
 * or NULL if the request was too large or memory allocation failed.  Use
 * these macros rather than doing the multiplication yourself so that proper
 * overflow checking is always done.
 */

#define PyMem_New(type, n) \
  ( ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	( (type *) PyMem_Malloc((n) * sizeof(type)) ) )
#define PyMem_NEW(type, n) \
  ( ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	( (type *) PyMem_MALLOC((n) * sizeof(type)) ) )

/*
 * The value of (p) is always clobbered by this macro regardless of success.
 * The caller MUST check if (p) is NULL afterwards and deal with the memory
 * error if so.  This means the original value of (p) MUST be saved for the
 * caller's memory error handler to not lose track of it.
 */
#define PyMem_Resize(p, type, n) \
  ( (p) = ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	(type *) PyMem_Realloc((p), (n) * sizeof(type)) )
#define PyMem_RESIZE(p, type, n) \
  ( (p) = ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	(type *) PyMem_REALLOC((p), (n) * sizeof(type)) )

/* PyMem{Del,DEL} are left over from ancient days, and shouldn't be used
 * anymore.  They're just confusing aliases for PyMem_{Free,FREE} now.
 */
#define PyMem_Del		PyMem_Free
#define PyMem_DEL		PyMem_FREE

/* Get internal functions of PyMem_Malloc(), PyMem_Realloc()
   and PyMem_Free() */
PyAPI_FUNC(void) PyMem_GetAllocators(PyMemAllocators *allocators);

/* Set internal functions of PyMem_Malloc(), PyMem_Realloc() and PyMem_Free().

   malloc(ctx, 0) and realloc(ctx, ptr, 0) must not return NULL: it would be
   treated as an error.

   PyMem_SetupDebugHooks() should be called to reinstall debug hooks if new
   functions do no call original functions anymore. */
PyAPI_FUNC(void) PyMem_SetAllocators(PyMemAllocators *allocators);

/* Setup hooks to detect bugs in the following Python memory allocator
   functions:

   - PyMem_RawMalloc(), PyMem_RawRealloc(), PyMem_RawFree()
   - PyMem_Malloc(), PyMem_Realloc(), PyMem_Free()
   - PyObject_Malloc(), PyObject_Realloc() and PyObject_Free()

   Newly allocated memory is filled with the byte 0xCB, freed memory is filled
   with the byte 0xDB. Additionnal checks:

   - detect API violations, ex: PyObject_Free() called on a buffer allocated
     by PyMem_Malloc()
   - detect write before the start of the buffer (buffer underflow)
   - detect write after the end of the buffer (buffer overflow)

   The function does nothing if Python is not compiled is debug mode. */
PyAPI_FUNC(void) PyMem_SetupDebugHooks(void);

#ifdef __cplusplus
}
#endif

#endif /* !Py_PYMEM_H */
