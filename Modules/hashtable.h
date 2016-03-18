#ifndef Py_HASHTABLE_H
#define Py_HASHTABLE_H

/* The whole API is private */
#ifndef Py_LIMITED_API

typedef struct _Py_slist_item_s {
    struct _Py_slist_item_s *next;
} _Py_slist_item_t;

typedef struct {
    _Py_slist_item_t *head;
} _Py_slist_t;

#define _Py_SLIST_ITEM_NEXT(ITEM) (((_Py_slist_item_t *)ITEM)->next)

#define _Py_SLIST_HEAD(SLIST) (((_Py_slist_t *)SLIST)->head)

typedef struct {
    /* used by _Py_hashtable_t.buckets to link entries */
    _Py_slist_item_t _Py_slist_item;

    Py_uhash_t key_hash;

    /* data follows */
} _Py_hashtable_entry_t;

#define _Py_HASHTABLE_ENTRY_KEY(ENTRY) \
        ((void *)((char *)(ENTRY) + sizeof(_Py_hashtable_entry_t)))

#define _Py_HASHTABLE_ENTRY_DATA(TABLE, ENTRY) \
        ((char *)(ENTRY) + sizeof(_Py_hashtable_entry_t) + (TABLE)->key_size)

#define _Py_HASHTABLE_ENTRY_DATA_AS_VOID_P(TABLE, ENTRY) \
        (*(void **)_Py_HASHTABLE_ENTRY_DATA(TABLE, ENTRY))

#define _Py_HASHTABLE_ENTRY_READ_DATA(TABLE, DATA, DATA_SIZE, ENTRY) \
    do { \
        assert((DATA_SIZE) == (TABLE)->data_size); \
        memcpy(DATA, _Py_HASHTABLE_ENTRY_DATA(TABLE, ENTRY), DATA_SIZE); \
    } while (0)

typedef Py_uhash_t (*_Py_hashtable_hash_func) (size_t key_size,
                                               const void *pkey);
typedef int (*_Py_hashtable_compare_func) (size_t key_size,
                                           const void *pkey,
                                           const _Py_hashtable_entry_t *he);
typedef void* (*_Py_hashtable_copy_data_func)(void *data);
typedef void (*_Py_hashtable_free_data_func)(void *data);
typedef size_t (*_Py_hashtable_get_data_size_func)(void *data);

typedef struct {
    /* allocate a memory block */
    void* (*malloc) (size_t size);

    /* release a memory block */
    void (*free) (void *ptr);
} _Py_hashtable_allocator_t;

typedef struct {
    size_t num_buckets;
    size_t entries; /* Total number of entries in the table. */
    _Py_slist_t *buckets;
    size_t key_size;
    size_t data_size;

    _Py_hashtable_hash_func hash_func;
    _Py_hashtable_compare_func compare_func;
    _Py_hashtable_copy_data_func copy_data_func;
    _Py_hashtable_free_data_func free_data_func;
    _Py_hashtable_get_data_size_func get_data_size_func;
    _Py_hashtable_allocator_t alloc;
} _Py_hashtable_t;

/* hash and compare functions for integers and pointers */
PyAPI_FUNC(Py_uhash_t) _Py_hashtable_hash_ptr(
    size_t key_size,
    const void *pkey);
PyAPI_FUNC(int) _Py_hashtable_compare_direct(
    size_t key_size,
    const void *pkey,
    const _Py_hashtable_entry_t *entry);

PyAPI_FUNC(_Py_hashtable_t *) _Py_hashtable_new(
    size_t key_size,
    size_t data_size,
    _Py_hashtable_hash_func hash_func,
    _Py_hashtable_compare_func compare_func);
PyAPI_FUNC(_Py_hashtable_t *) _Py_hashtable_new_full(
    size_t key_size,
    size_t data_size,
    size_t init_size,
    _Py_hashtable_hash_func hash_func,
    _Py_hashtable_compare_func compare_func,
    _Py_hashtable_copy_data_func copy_data_func,
    _Py_hashtable_free_data_func free_data_func,
    _Py_hashtable_get_data_size_func get_data_size_func,
    _Py_hashtable_allocator_t *allocator);
PyAPI_FUNC(_Py_hashtable_t *) _Py_hashtable_copy(_Py_hashtable_t *src);
PyAPI_FUNC(void) _Py_hashtable_clear(_Py_hashtable_t *ht);
PyAPI_FUNC(void) _Py_hashtable_destroy(_Py_hashtable_t *ht);

typedef int (*_Py_hashtable_foreach_func) (_Py_hashtable_t *ht,
                                           _Py_hashtable_entry_t *entry,
                                           void *arg);

PyAPI_FUNC(int) _Py_hashtable_foreach(
    _Py_hashtable_t *ht,
    _Py_hashtable_foreach_func func,
    void *arg);
PyAPI_FUNC(size_t) _Py_hashtable_size(_Py_hashtable_t *ht);

PyAPI_FUNC(_Py_hashtable_entry_t*) _Py_hashtable_get_entry(
    _Py_hashtable_t *ht,
    size_t key_size,
    const void *pkey);

/* Don't call directly this function,
   but use _Py_HASHTABLE_SET() and _Py_HASHTABLE_SET_NODATA() macros */
PyAPI_FUNC(int) _Py_hashtable_set(
    _Py_hashtable_t *ht,
    size_t key_size,
    const void *pkey,
    size_t data_size,
    void *data);

/* Don't call directly this function, but use _Py_HASHTABLE_GET() macro */
PyAPI_FUNC(int) _Py_hashtable_get(
    _Py_hashtable_t *ht,
    size_t key_size,
    const void *pkey,
    size_t data_size,
    void *data);

/* Don't call directly this function, but use _Py_HASHTABLE_POP() macro */
PyAPI_FUNC(int) _Py_hashtable_pop(
    _Py_hashtable_t *ht,
    size_t key_size,
    const void *pkey,
    size_t data_size,
    void *data);

PyAPI_FUNC(void) _Py_hashtable_delete(
    _Py_hashtable_t *ht,
    size_t key_size,
    const void *pkey);

#define _Py_HASHTABLE_SET(TABLE, KEY, DATA) \
    _Py_hashtable_set(TABLE, sizeof(KEY), &KEY, sizeof(DATA), &(DATA))

#define _Py_HASHTABLE_SET_NODATA(TABLE, KEY) \
    _Py_hashtable_set(TABLE, sizeof(KEY), &KEY, 0, NULL)

#define _Py_HASHTABLE_GET_ENTRY(TABLE, KEY) \
    _Py_hashtable_get_entry(TABLE, sizeof(KEY), &(KEY))

#define _Py_HASHTABLE_GET(TABLE, KEY, DATA) \
    _Py_hashtable_get(TABLE, sizeof(KEY), &(KEY), sizeof(DATA), &(DATA))

#define _Py_HASHTABLE_POP(TABLE, KEY, DATA) \
    _Py_hashtable_pop(TABLE, sizeof(KEY), &(KEY), sizeof(DATA), &(DATA))

#endif   /* Py_LIMITED_API */

#endif
