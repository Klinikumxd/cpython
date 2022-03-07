#ifndef Py_INTERNAL_CODE_H
#define Py_INTERNAL_CODE_H
#ifdef __cplusplus
extern "C" {
#endif

/* PEP 659
 * Specialization and quickening structs and helper functions
 */


// Inline caches. If you change the number of cache entries for an instruction,
// you must *also* update the number of cache entries in Lib/opcode.py and bump
// the magic number in Lib/importlib/_bootstrap_external.py!

#define CACHE_ENTRIES(cache) (sizeof(cache)/sizeof(_Py_CODEUNIT))

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT index;
    _Py_CODEUNIT module_keys_version[2];
    _Py_CODEUNIT builtin_keys_version;
} _PyLoadGlobalCache;

#define INLINE_CACHE_ENTRIES_LOAD_GLOBAL CACHE_ENTRIES(_PyLoadGlobalCache)

typedef struct {
    _Py_CODEUNIT counter;
} _PyBinaryOpCache;

#define INLINE_CACHE_ENTRIES_BINARY_OP CACHE_ENTRIES(_PyBinaryOpCache)

typedef struct {
    _Py_CODEUNIT counter;
} _PyUnpackSequenceCache;

#define INLINE_CACHE_ENTRIES_UNPACK_SEQUENCE \
    CACHE_ENTRIES(_PyUnpackSequenceCache)

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT mask;
} _PyCompareOpCache;

#define INLINE_CACHE_ENTRIES_COMPARE_OP CACHE_ENTRIES(_PyCompareOpCache)

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT type_version[2];
    _Py_CODEUNIT func_version;
} _PyBinarySubscrCache;

#define INLINE_CACHE_ENTRIES_BINARY_SUBSCR CACHE_ENTRIES(_PyBinarySubscrCache)

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT version[2];
    _Py_CODEUNIT index;
} _PyAttrCache;

#define INLINE_CACHE_ENTRIES_LOAD_ATTR CACHE_ENTRIES(_PyAttrCache)

#define INLINE_CACHE_ENTRIES_STORE_ATTR CACHE_ENTRIES(_PyAttrCache)

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT type_version[2];
    _Py_CODEUNIT dict_offset;
    _Py_CODEUNIT keys_version[2];
    _Py_CODEUNIT descr[4];
} _PyLoadMethodCache;

#define INLINE_CACHE_ENTRIES_LOAD_METHOD CACHE_ENTRIES(_PyLoadMethodCache)

typedef struct {
    _Py_CODEUNIT counter;
    _Py_CODEUNIT func_version[2];
    _Py_CODEUNIT min_args;
} _PyCallCache;

#define INLINE_CACHE_ENTRIES_CALL CACHE_ENTRIES(_PyCallCache)

typedef struct {
    _Py_CODEUNIT counter;
} _PyPrecallCache;

#define INLINE_CACHE_ENTRIES_PRECALL CACHE_ENTRIES(_PyPrecallCache)

/* Maximum size of code to quicken, in code units. */
#define MAX_SIZE_TO_QUICKEN 10000

#define QUICKENING_WARMUP_DELAY 8

/* We want to compare to zero for efficiency, so we offset values accordingly */
#define QUICKENING_INITIAL_WARMUP_VALUE (-QUICKENING_WARMUP_DELAY)
#define QUICKENING_WARMUP_COLDEST 1

int _Py_Quicken(PyCodeObject *code);

/* Returns 1 if quickening occurs.
 * -1 if an error occurs
 * 0 otherwise */
static inline int
_Py_IncrementCountAndMaybeQuicken(PyCodeObject *code)
{
    if (code->co_warmup != 0) {
        code->co_warmup++;
        if (code->co_warmup == 0) {
            return _Py_Quicken(code) ? -1 : 1;
        }
    }
    return 0;
}

extern Py_ssize_t _Py_QuickenedCount;

// Borrowed references to common callables:
struct callable_cache {
    PyObject *isinstance;
    PyObject *len;
    PyObject *list_append;
};

/* "Locals plus" for a code object is the set of locals + cell vars +
 * free vars.  This relates to variable names as well as offsets into
 * the "fast locals" storage array of execution frames.  The compiler
 * builds the list of names, their offsets, and the corresponding
 * kind of local.
 *
 * Those kinds represent the source of the initial value and the
 * variable's scope (as related to closures).  A "local" is an
 * argument or other variable defined in the current scope.  A "free"
 * variable is one that is defined in an outer scope and comes from
 * the function's closure.  A "cell" variable is a local that escapes
 * into an inner function as part of a closure, and thus must be
 * wrapped in a cell.  Any "local" can also be a "cell", but the
 * "free" kind is mutually exclusive with both.
 */

// Note that these all fit within a byte, as do combinations.
// Later, we will use the smaller numbers to differentiate the different
// kinds of locals (e.g. pos-only arg, varkwargs, local-only).
#define CO_FAST_LOCAL   0x20
#define CO_FAST_CELL    0x40
#define CO_FAST_FREE    0x80

typedef unsigned char _PyLocals_Kind;

static inline _PyLocals_Kind
_PyLocals_GetKind(PyObject *kinds, int i)
{
    assert(PyBytes_Check(kinds));
    assert(0 <= i && i < PyBytes_GET_SIZE(kinds));
    char *ptr = PyBytes_AS_STRING(kinds);
    return (_PyLocals_Kind)(ptr[i]);
}

static inline void
_PyLocals_SetKind(PyObject *kinds, int i, _PyLocals_Kind kind)
{
    assert(PyBytes_Check(kinds));
    assert(0 <= i && i < PyBytes_GET_SIZE(kinds));
    char *ptr = PyBytes_AS_STRING(kinds);
    ptr[i] = (char) kind;
}


struct _PyCodeConstructor {
    /* metadata */
    PyObject *filename;
    PyObject *name;
    PyObject *qualname;
    int flags;

    /* the code */
    PyObject *code;
    int firstlineno;
    PyObject *linetable;
    PyObject *endlinetable;
    PyObject *columntable;

    /* used by the code */
    PyObject *consts;
    PyObject *names;

    /* mapping frame offsets to information */
    PyObject *localsplusnames;  // Tuple of strings
    PyObject *localspluskinds;  // Bytes object, one byte per variable

    /* args (within varnames) */
    int argcount;
    int posonlyargcount;
    // XXX Replace argcount with posorkwargcount (argcount - posonlyargcount).
    int kwonlyargcount;

    /* needed to create the frame */
    int stacksize;

    /* used by the eval loop */
    PyObject *exceptiontable;
};

// Using an "arguments struct" like this is helpful for maintainability
// in a case such as this with many parameters.  It does bear a risk:
// if the struct changes and callers are not updated properly then the
// compiler will not catch problems (like a missing argument).  This can
// cause hard-to-debug problems.  The risk is mitigated by the use of
// check_code() in codeobject.c.  However, we may decide to switch
// back to a regular function signature.  Regardless, this approach
// wouldn't be appropriate if this weren't a strictly internal API.
// (See the comments in https://github.com/python/cpython/pull/26258.)
PyAPI_FUNC(int) _PyCode_Validate(struct _PyCodeConstructor *);
PyAPI_FUNC(PyCodeObject *) _PyCode_New(struct _PyCodeConstructor *);


/* Private API */

/* Getters for internal PyCodeObject data. */
extern PyObject* _PyCode_GetVarnames(PyCodeObject *);
extern PyObject* _PyCode_GetCellvars(PyCodeObject *);
extern PyObject* _PyCode_GetFreevars(PyCodeObject *);

/* Return the ending source code line number from a bytecode index. */
extern int _PyCode_Addr2EndLine(PyCodeObject *, int);

/* Return the ending source code line number from a bytecode index. */
extern int _PyCode_Addr2EndLine(PyCodeObject *, int);
/* Return the starting source code column offset from a bytecode index. */
extern int _PyCode_Addr2Offset(PyCodeObject *, int);
/* Return the ending source code column offset from a bytecode index. */
extern int _PyCode_Addr2EndOffset(PyCodeObject *, int);

/** API for initializing the line number tables. */
extern int _PyCode_InitAddressRange(PyCodeObject* co, PyCodeAddressRange *bounds);
extern int _PyCode_InitEndAddressRange(PyCodeObject* co, PyCodeAddressRange* bounds);

/** Out of process API for initializing the line number table. */
extern void _PyLineTable_InitAddressRange(
    const char *linetable,
    Py_ssize_t length,
    int firstlineno,
    PyCodeAddressRange *range);

/** API for traversing the line number table. */
extern int _PyLineTable_NextAddressRange(PyCodeAddressRange *range);
extern int _PyLineTable_PreviousAddressRange(PyCodeAddressRange *range);


#define ADAPTIVE_CACHE_BACKOFF 64

/* Specialization functions */

extern int _Py_Specialize_LoadAttr(PyObject *owner, _Py_CODEUNIT *instr,
                                   PyObject *name);
extern int _Py_Specialize_StoreAttr(PyObject *owner, _Py_CODEUNIT *instr,
                                    PyObject *name);
extern int _Py_Specialize_LoadGlobal(PyObject *globals, PyObject *builtins, _Py_CODEUNIT *instr, PyObject *name);
extern int _Py_Specialize_LoadMethod(PyObject *owner, _Py_CODEUNIT *instr,
                                     PyObject *name);
extern int _Py_Specialize_BinarySubscr(PyObject *sub, PyObject *container, _Py_CODEUNIT *instr);
extern int _Py_Specialize_StoreSubscr(PyObject *container, PyObject *sub, _Py_CODEUNIT *instr);
extern int _Py_Specialize_Call(PyObject *callable, _Py_CODEUNIT *instr,
                               int nargs, PyObject *kwnames);
extern int _Py_Specialize_Precall(PyObject *callable, _Py_CODEUNIT *instr,
                                  int nargs, PyObject *kwnames, int oparg);
extern void _Py_Specialize_BinaryOp(PyObject *lhs, PyObject *rhs, _Py_CODEUNIT *instr,
                                    int oparg);
extern void _Py_Specialize_CompareOp(PyObject *lhs, PyObject *rhs,
                                     _Py_CODEUNIT *instr, int oparg);
extern void _Py_Specialize_UnpackSequence(PyObject *seq, _Py_CODEUNIT *instr,
                                          int oparg);

/* Deallocator function for static codeobjects used in deepfreeze.py */
extern void _PyStaticCode_Dealloc(PyCodeObject *co);
/* Function to intern strings of codeobjects */
extern int _PyStaticCode_InternStrings(PyCodeObject *co);

#ifdef Py_STATS

#define SPECIALIZATION_FAILURE_KINDS 30

typedef struct _specialization_stats {
    uint64_t success;
    uint64_t failure;
    uint64_t hit;
    uint64_t deferred;
    uint64_t miss;
    uint64_t deopt;
    uint64_t failure_kinds[SPECIALIZATION_FAILURE_KINDS];
} SpecializationStats;

typedef struct _opcode_stats {
    SpecializationStats specialization;
    uint64_t execution_count;
    uint64_t pair_count[256];
} OpcodeStats;

typedef struct _call_stats {
    uint64_t inlined_py_calls;
    uint64_t pyeval_calls;
    uint64_t frames_pushed;
    uint64_t frame_objects_created;
} CallStats;

typedef struct _object_stats {
    uint64_t allocations;
    uint64_t frees;
    uint64_t new_values;
    uint64_t dict_materialized_on_request;
    uint64_t dict_materialized_new_key;
    uint64_t dict_materialized_too_big;
    uint64_t dict_materialized_str_subclass;
} ObjectStats;

typedef struct _stats {
    OpcodeStats opcode_stats[256];
    CallStats call_stats;
    ObjectStats object_stats;
} PyStats;

extern PyStats _py_stats;

#define STAT_INC(opname, name) _py_stats.opcode_stats[opname].specialization.name++
#define STAT_DEC(opname, name) _py_stats.opcode_stats[opname].specialization.name--
#define OPCODE_EXE_INC(opname) _py_stats.opcode_stats[opname].execution_count++
#define CALL_STAT_INC(name) _py_stats.call_stats.name++
#define OBJECT_STAT_INC(name) _py_stats.object_stats.name++

extern void _Py_PrintSpecializationStats(int to_file);

extern PyObject* _Py_GetSpecializationStats(void);

#else
#define STAT_INC(opname, name) ((void)0)
#define STAT_DEC(opname, name) ((void)0)
#define OPCODE_EXE_INC(opname) ((void)0)
#define CALL_STAT_INC(name) ((void)0)
#define OBJECT_STAT_INC(name) ((void)0)
#endif

// Cache values are only valid in memory, so use native endianness.
#ifdef WORDS_BIGENDIAN

static inline void
write_u32(uint16_t *p, uint32_t val)
{
    p[0] = (uint16_t)(val >> 16);
    p[1] = (uint16_t)(val >>  0);
}

static inline void
write_u64(uint16_t *p, uint64_t val)
{
    p[0] = (uint16_t)(val >> 48);
    p[1] = (uint16_t)(val >> 32);
    p[2] = (uint16_t)(val >> 16);
    p[3] = (uint16_t)(val >>  0);
}

static inline uint32_t
read_u32(uint16_t *p)
{
    uint32_t val = 0;
    val |= (uint32_t)p[0] << 16;
    val |= (uint32_t)p[1] <<  0;
    return val;
}

static inline uint64_t
read_u64(uint16_t *p)
{
    uint64_t val = 0;
    val |= (uint64_t)p[0] << 48;
    val |= (uint64_t)p[1] << 32;
    val |= (uint64_t)p[2] << 16;
    val |= (uint64_t)p[3] <<  0;
    return val;
}

#else

static inline void
write_u32(uint16_t *p, uint32_t val)
{
    p[0] = (uint16_t)(val >>  0);
    p[1] = (uint16_t)(val >> 16);
}

static inline void
write_u64(uint16_t *p, uint64_t val)
{
    p[0] = (uint16_t)(val >>  0);
    p[1] = (uint16_t)(val >> 16);
    p[2] = (uint16_t)(val >> 32);
    p[3] = (uint16_t)(val >> 48);
}

static inline uint32_t
read_u32(uint16_t *p)
{
    uint32_t val = 0;
    val |= (uint32_t)p[0] <<  0;
    val |= (uint32_t)p[1] << 16;
    return val;
}

static inline uint64_t
read_u64(uint16_t *p)
{
    uint64_t val = 0;
    val |= (uint64_t)p[0] <<  0;
    val |= (uint64_t)p[1] << 16;
    val |= (uint64_t)p[2] << 32;
    val |= (uint64_t)p[3] << 48;
    return val;
}

#endif

static inline void
write_obj(uint16_t *p, PyObject *obj)
{
    uintptr_t val = (uintptr_t)obj;
#if SIZEOF_VOID_P == 8
    write_u64(p, val);
#elif SIZEOF_VOID_P == 4
    write_u32(p, val);
#else
    #error "SIZEOF_VOID_P must be 4 or 8"
#endif
}

static inline PyObject *
read_obj(uint16_t *p)
{
    uintptr_t val;
#if SIZEOF_VOID_P == 8
    val = read_u64(p);
#elif SIZEOF_VOID_P == 4
    val = read_u32(p);
#else
    #error "SIZEOF_VOID_P must be 4 or 8"
#endif
    return (PyObject *)val;
}

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_CODE_H */
