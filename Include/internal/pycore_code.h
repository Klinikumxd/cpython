#ifndef Py_INTERNAL_CODE_H
#define Py_INTERNAL_CODE_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

#define CODE_MAX_WATCHERS 8

/* PEP 659
 * Specialization and quickening structs and helper functions
 */


// Inline caches. If you change the number of cache entries for an instruction,
// you must *also* update the number of cache entries in Lib/opcode.py and bump
// the magic number in Lib/importlib/_bootstrap_external.py!

#define CACHE_ENTRIES(cache) (sizeof(cache)/sizeof(_Py_CODEUNIT))

typedef struct {
    uint16_t counter;
    uint16_t module_keys_version;
    uint16_t builtin_keys_version;
    uint16_t index;
} _PyLoadGlobalCache;

#define INLINE_CACHE_ENTRIES_LOAD_GLOBAL CACHE_ENTRIES(_PyLoadGlobalCache)

typedef struct {
    uint16_t counter;
} _PyBinaryOpCache;

#define INLINE_CACHE_ENTRIES_BINARY_OP CACHE_ENTRIES(_PyBinaryOpCache)

typedef struct {
    uint16_t counter;
} _PyUnpackSequenceCache;

#define INLINE_CACHE_ENTRIES_UNPACK_SEQUENCE \
    CACHE_ENTRIES(_PyUnpackSequenceCache)

typedef struct {
    uint16_t counter;
} _PyCompareOpCache;

#define INLINE_CACHE_ENTRIES_COMPARE_OP CACHE_ENTRIES(_PyCompareOpCache)

typedef struct {
    uint16_t counter;
} _PyBinarySubscrCache;

#define INLINE_CACHE_ENTRIES_BINARY_SUBSCR CACHE_ENTRIES(_PyBinarySubscrCache)

typedef struct {
    uint16_t counter;
} _PySuperAttrCache;

#define INLINE_CACHE_ENTRIES_LOAD_SUPER_ATTR CACHE_ENTRIES(_PySuperAttrCache)

typedef struct {
    uint16_t counter;
    uint16_t version[2];
    uint16_t index;
} _PyAttrCache;

typedef struct {
    uint16_t counter;
    uint16_t type_version[2];
    uint16_t keys_version[2];
    uint16_t descr[4];
} _PyLoadMethodCache;


// MUST be the max(_PyAttrCache, _PyLoadMethodCache)
#define INLINE_CACHE_ENTRIES_LOAD_ATTR CACHE_ENTRIES(_PyLoadMethodCache)

#define INLINE_CACHE_ENTRIES_STORE_ATTR CACHE_ENTRIES(_PyAttrCache)

typedef struct {
    uint16_t counter;
    uint16_t func_version[2];
} _PyCallCache;

#define INLINE_CACHE_ENTRIES_CALL CACHE_ENTRIES(_PyCallCache)

typedef struct {
    uint16_t counter;
} _PyStoreSubscrCache;

#define INLINE_CACHE_ENTRIES_STORE_SUBSCR CACHE_ENTRIES(_PyStoreSubscrCache)

typedef struct {
    uint16_t counter;
} _PyForIterCache;

#define INLINE_CACHE_ENTRIES_FOR_ITER CACHE_ENTRIES(_PyForIterCache)

typedef struct {
    uint16_t counter;
} _PySendCache;

#define INLINE_CACHE_ENTRIES_SEND CACHE_ENTRIES(_PySendCache)

typedef struct {
    uint16_t counter;
    uint16_t version[2];
} _PyToBoolCache;

#define INLINE_CACHE_ENTRIES_TO_BOOL CACHE_ENTRIES(_PyToBoolCache)

// Borrowed references to common callables:
struct callable_cache {
    PyObject *isinstance;
    PyObject *len;
    PyObject *list_append;
    PyObject *object__getattribute__;
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
#define CO_FAST_HIDDEN  0x10
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
extern int _PyCode_Validate(struct _PyCodeConstructor *);
extern PyCodeObject* _PyCode_New(struct _PyCodeConstructor *);


/* Private API */

/* Getters for internal PyCodeObject data. */
extern PyObject* _PyCode_GetVarnames(PyCodeObject *);
extern PyObject* _PyCode_GetCellvars(PyCodeObject *);
extern PyObject* _PyCode_GetFreevars(PyCodeObject *);
extern PyObject* _PyCode_GetCode(PyCodeObject *);

/** API for initializing the line number tables. */
extern int _PyCode_InitAddressRange(PyCodeObject* co, PyCodeAddressRange *bounds);

/** Out of process API for initializing the location table. */
extern void _PyLineTable_InitAddressRange(
    const char *linetable,
    Py_ssize_t length,
    int firstlineno,
    PyCodeAddressRange *range);

/** API for traversing the line number table. */
extern int _PyLineTable_NextAddressRange(PyCodeAddressRange *range);
extern int _PyLineTable_PreviousAddressRange(PyCodeAddressRange *range);

/** API for executors */
extern void _PyCode_Clear_Executors(PyCodeObject *code);

#define ENABLE_SPECIALIZATION 1

/* Specialization functions */

extern void _Py_Specialize_LoadSuperAttr(PyObject *global_super, PyObject *cls,
                                         _Py_CODEUNIT *instr, int load_method);
extern void _Py_Specialize_LoadAttr(PyObject *owner, _Py_CODEUNIT *instr,
                                    PyObject *name);
extern void _Py_Specialize_StoreAttr(PyObject *owner, _Py_CODEUNIT *instr,
                                     PyObject *name);
extern void _Py_Specialize_LoadGlobal(PyObject *globals, PyObject *builtins,
                                      _Py_CODEUNIT *instr, PyObject *name);
extern void _Py_Specialize_BinarySubscr(PyObject *sub, PyObject *container,
                                        _Py_CODEUNIT *instr);
extern void _Py_Specialize_StoreSubscr(PyObject *container, PyObject *sub,
                                       _Py_CODEUNIT *instr);
extern void _Py_Specialize_Call(PyObject *callable, _Py_CODEUNIT *instr,
                                int nargs);
extern void _Py_Specialize_BinaryOp(PyObject *lhs, PyObject *rhs, _Py_CODEUNIT *instr,
                                    int oparg, PyObject **locals);
extern void _Py_Specialize_CompareOp(PyObject *lhs, PyObject *rhs,
                                     _Py_CODEUNIT *instr, int oparg);
extern void _Py_Specialize_UnpackSequence(PyObject *seq, _Py_CODEUNIT *instr,
                                          int oparg);
extern void _Py_Specialize_ForIter(PyObject *iter, _Py_CODEUNIT *instr, int oparg);
extern void _Py_Specialize_Send(PyObject *receiver, _Py_CODEUNIT *instr);
extern void _Py_Specialize_ToBool(PyObject *value, _Py_CODEUNIT *instr);

/* Finalizer function for static codeobjects used in deepfreeze.py */
extern void _PyStaticCode_Fini(PyCodeObject *co);
/* Function to intern strings of codeobjects and quicken the bytecode */
extern int _PyStaticCode_Init(PyCodeObject *co);

#ifdef Py_STATS

#define STAT_INC(opname, name) do { if (_Py_stats) _Py_stats->opcode_stats[opname].specialization.name++; } while (0)
#define STAT_DEC(opname, name) do { if (_Py_stats) _Py_stats->opcode_stats[opname].specialization.name--; } while (0)
#define OPCODE_EXE_INC(opname) do { if (_Py_stats) _Py_stats->opcode_stats[opname].execution_count++; } while (0)
#define CALL_STAT_INC(name) do { if (_Py_stats) _Py_stats->call_stats.name++; } while (0)
#define OBJECT_STAT_INC(name) do { if (_Py_stats) _Py_stats->object_stats.name++; } while (0)
#define OBJECT_STAT_INC_COND(name, cond) \
    do { if (_Py_stats && cond) _Py_stats->object_stats.name++; } while (0)
#define EVAL_CALL_STAT_INC(name) do { if (_Py_stats) _Py_stats->call_stats.eval_calls[name]++; } while (0)
#define EVAL_CALL_STAT_INC_IF_FUNCTION(name, callable) \
    do { if (_Py_stats && PyFunction_Check(callable)) _Py_stats->call_stats.eval_calls[name]++; } while (0)
#define GC_STAT_ADD(gen, name, n) do { if (_Py_stats) _Py_stats->gc_stats[(gen)].name += (n); } while (0)
#define OPT_STAT_INC(name) do { if (_Py_stats) _Py_stats->optimization_stats.name++; } while (0)
#define UOP_STAT_INC(opname, name) do { if (_Py_stats) { assert(opname < 512); _Py_stats->optimization_stats.opcode[opname].name++; } } while (0)
#define OPT_UNSUPPORTED_OPCODE(opname) do { if (_Py_stats) _Py_stats->optimization_stats.unsupported_opcode[opname]++; } while (0)
#define OPT_HIST(length, name) \
    do { \
        if (_Py_stats) { \
            int bucket = _Py_bit_length(length >= 1 ? length - 1 : 0); \
            bucket = (bucket >= _Py_UOP_HIST_SIZE) ? _Py_UOP_HIST_SIZE - 1 : bucket; \
            _Py_stats->optimization_stats.name[bucket]++; \
        } \
    } while (0)

// Export for '_opcode' shared extension
PyAPI_FUNC(PyObject*) _Py_GetSpecializationStats(void);

#else
#define STAT_INC(opname, name) ((void)0)
#define STAT_DEC(opname, name) ((void)0)
#define OPCODE_EXE_INC(opname) ((void)0)
#define CALL_STAT_INC(name) ((void)0)
#define OBJECT_STAT_INC(name) ((void)0)
#define OBJECT_STAT_INC_COND(name, cond) ((void)0)
#define EVAL_CALL_STAT_INC(name) ((void)0)
#define EVAL_CALL_STAT_INC_IF_FUNCTION(name, callable) ((void)0)
#define GC_STAT_ADD(gen, name, n) ((void)0)
#define OPT_STAT_INC(name) ((void)0)
#define UOP_STAT_INC(opname, name) ((void)0)
#define OPT_UNSUPPORTED_OPCODE(opname) ((void)0)
#define OPT_HIST(length, name) ((void)0)
#endif  // !Py_STATS

// Utility functions for reading/writing 32/64-bit values in the inline caches.
// Great care should be taken to ensure that these functions remain correct and
// performant! They should compile to just "move" instructions on all supported
// compilers and platforms.

// We use memcpy to let the C compiler handle unaligned accesses and endianness
// issues for us. It also seems to produce better code than manual copying for
// most compilers (see https://blog.regehr.org/archives/959 for more info).

static inline void
write_u32(uint16_t *p, uint32_t val)
{
    memcpy(p, &val, sizeof(val));
}

static inline void
write_u64(uint16_t *p, uint64_t val)
{
    memcpy(p, &val, sizeof(val));
}

static inline void
write_obj(uint16_t *p, PyObject *val)
{
    memcpy(p, &val, sizeof(val));
}

static inline uint16_t
read_u16(uint16_t *p)
{
    return *p;
}

static inline uint32_t
read_u32(uint16_t *p)
{
    uint32_t val;
    memcpy(&val, p, sizeof(val));
    return val;
}

static inline uint64_t
read_u64(uint16_t *p)
{
    uint64_t val;
    memcpy(&val, p, sizeof(val));
    return val;
}

static inline PyObject *
read_obj(uint16_t *p)
{
    PyObject *val;
    memcpy(&val, p, sizeof(val));
    return val;
}

/* See Objects/exception_handling_notes.txt for details.
 */
static inline unsigned char *
parse_varint(unsigned char *p, int *result) {
    int val = p[0] & 63;
    while (p[0] & 64) {
        p++;
        val = (val << 6) | (p[0] & 63);
    }
    *result = val;
    return p+1;
}

static inline int
write_varint(uint8_t *ptr, unsigned int val)
{
    int written = 1;
    while (val >= 64) {
        *ptr++ = 64 | (val & 63);
        val >>= 6;
        written++;
    }
    *ptr = val;
    return written;
}

static inline int
write_signed_varint(uint8_t *ptr, int val)
{
    if (val < 0) {
        val = ((-val)<<1) | 1;
    }
    else {
        val = val << 1;
    }
    return write_varint(ptr, val);
}

static inline int
write_location_entry_start(uint8_t *ptr, int code, int length)
{
    assert((code & 15) == code);
    *ptr = 128 | (code << 3) | (length - 1);
    return 1;
}


/** Counters
 * The first 16-bit value in each inline cache is a counter.
 * When counting misses, the counter is treated as a simple unsigned value.
 *
 * When counting executions until the next specialization attempt,
 * exponential backoff is used to reduce the number of specialization failures.
 * The high 12 bits store the counter, the low 4 bits store the backoff exponent.
 * On a specialization failure, the backoff exponent is incremented and the
 * counter set to (2**backoff - 1).
 * Backoff == 6 -> starting counter == 63, backoff == 10 -> starting counter == 1023.
 */

/* With a 16-bit counter, we have 12 bits for the counter value, and 4 bits for the backoff */
#define ADAPTIVE_BACKOFF_BITS 4

// A value of 1 means that we attempt to specialize the *second* time each
// instruction is executed. Executing twice is a much better indicator of
// "hotness" than executing once, but additional warmup delays only prevent
// specialization. Most types stabilize by the second execution, too:
#define ADAPTIVE_WARMUP_VALUE 1
#define ADAPTIVE_WARMUP_BACKOFF 1

// A value of 52 means that we attempt to re-specialize after 53 misses (a prime
// number, useful for avoiding artifacts if every nth value is a different type
// or something). Setting the backoff to 0 means that the counter is reset to
// the same state as a warming-up instruction (value == 1, backoff == 1) after
// deoptimization. This isn't strictly necessary, but it is bit easier to reason
// about when thinking about the opcode transitions as a state machine:
#define ADAPTIVE_COOLDOWN_VALUE 52
#define ADAPTIVE_COOLDOWN_BACKOFF 0

#define MAX_BACKOFF_VALUE (16 - ADAPTIVE_BACKOFF_BITS)


static inline uint16_t
adaptive_counter_bits(int value, int backoff) {
    return (value << ADAPTIVE_BACKOFF_BITS) |
        (backoff & ((1<<ADAPTIVE_BACKOFF_BITS)-1));
}

static inline uint16_t
adaptive_counter_warmup(void) {
    return adaptive_counter_bits(ADAPTIVE_WARMUP_VALUE,
                                 ADAPTIVE_WARMUP_BACKOFF);
}

static inline uint16_t
adaptive_counter_cooldown(void) {
    return adaptive_counter_bits(ADAPTIVE_COOLDOWN_VALUE,
                                 ADAPTIVE_COOLDOWN_BACKOFF);
}

static inline uint16_t
adaptive_counter_backoff(uint16_t counter) {
    unsigned int backoff = counter & ((1<<ADAPTIVE_BACKOFF_BITS)-1);
    backoff++;
    if (backoff > MAX_BACKOFF_VALUE) {
        backoff = MAX_BACKOFF_VALUE;
    }
    unsigned int value = (1 << backoff) - 1;
    return adaptive_counter_bits(value, backoff);
}


/* Comparison bit masks. */

/* Note this evaluates its arguments twice each */
#define COMPARISON_BIT(x, y) (1 << (2 * ((x) >= (y)) + ((x) <= (y))))

/*
 * The following bits are chosen so that the value of
 * COMPARSION_BIT(left, right)
 * masked by the values below will be non-zero if the
 * comparison is true, and zero if it is false */

/* This is for values that are unordered, ie. NaN, not types that are unordered, e.g. sets */
#define COMPARISON_UNORDERED 1

#define COMPARISON_LESS_THAN 2
#define COMPARISON_GREATER_THAN 4
#define COMPARISON_EQUALS 8

#define COMPARISON_NOT_EQUALS (COMPARISON_UNORDERED | COMPARISON_LESS_THAN | COMPARISON_GREATER_THAN)

extern int _Py_Instrument(PyCodeObject *co, PyInterpreterState *interp);

extern int _Py_GetBaseOpcode(PyCodeObject *code, int offset);

extern int _PyInstruction_GetLength(PyCodeObject *code, int offset);

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_CODE_H */
