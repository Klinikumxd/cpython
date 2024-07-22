#ifndef Py_INTERNAL_FREELIST_STATE_H
#define Py_INTERNAL_FREELIST_STATE_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

#ifdef WITH_FREELISTS
// with freelists
#  define PyTuple_MAXSAVESIZE 20     // Largest tuple to save on freelist
#  define Py_tuple_MAXFREELIST 2000  // Maximum number of tuples of each size to save
#  define Py_lists_MAXFREELIST 80
#  define Py_dicts_MAXFREELIST 80
#  define Py_dictkeys_MAXFREELIST 80
#  define Py_floats_MAXFREELIST 100
#  define Py_slices_MAXFREELIST 1
#  define Py_contexts_MAXFREELIST 255
#  define Py_async_gens_MAXFREELIST 80
#  define Py_async_gen_asends_MAXFREELIST 80
#  define Py_object_stack_chunks_MAXFREELIST 4
#else
#  define PyTuple_MAXSAVESIZE 0
#endif

// A generic freelist of either PyObjects or other data structures.
struct _Py_freelist {
    // Entries are linked together using the first word of the the object.
    // For PyObjects, this overlaps with the `ob_refcnt` field or the `ob_tid`
    // field.
    void *freelist;

    // The number of items in the free list or -1 if the free list is disabled
    Py_ssize_t size;
};

struct _Py_freelists {
#ifdef WITH_FREELISTS
    struct _Py_freelist floats;
    struct _Py_freelist tuples[PyTuple_MAXSAVESIZE];
    struct _Py_freelist lists;
    struct _Py_freelist dicts;
    struct _Py_freelist dictkeys;
    struct _Py_freelist slices;
    struct _Py_freelist contexts;
    struct _Py_freelist async_gens;
    struct _Py_freelist async_gen_asends;
    struct _Py_freelist object_stack_chunks;
#else
    char _unused;  // Empty structs are not allowed.
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_FREELIST_STATE_H */
