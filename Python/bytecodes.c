// This file contains instruction definitions.
// It is read by Tools/cases_generator/generate_cases.py
// to generate Python/generated_cases.c.h.
// Note that there is some dummy C code at the top and bottom of the file
// to fool text editors like VS Code into believing this is valid C code.
// The actual instruction definitions start at // BEGIN BYTECODES //.
// See Tools/cases_generator/README.md for more information.

#include "Python.h"
#include "pycore_abstract.h"      // _PyIndex_Check()
#include "pycore_ceval.h"         // _PyEval_SignalAsyncExc()
#include "pycore_code.h"
#include "pycore_emscripten_signal.h"  // _Py_CHECK_EMSCRIPTEN_SIGNALS
#include "pycore_function.h"
#include "pycore_instruments.h"
#include "pycore_intrinsics.h"
#include "pycore_long.h"          // _PyLong_GetZero()
#include "pycore_moduleobject.h"  // PyModuleObject
#include "pycore_object.h"        // _PyObject_GC_TRACK()
#include "pycore_opcode.h"        // EXTRA_CASES
#include "pycore_opcode_metadata.h"  // uop names
#include "pycore_opcode_utils.h"  // MAKE_FUNCTION_*
#include "pycore_pyerrors.h"      // _PyErr_GetRaisedException()
#include "pycore_pystate.h"       // _PyInterpreterState_GET()
#include "pycore_range.h"         // _PyRangeIterObject
#include "pycore_setobject.h"     // _PySet_NextEntry()
#include "pycore_sliceobject.h"   // _PyBuildSlice_ConsumeRefs
#include "pycore_sysmodule.h"     // _PySys_Audit()
#include "pycore_tuple.h"         // _PyTuple_ITEMS()
#include "pycore_typeobject.h"    // _PySuper_Lookup()

#include "pycore_dict.h"
#include "dictobject.h"
#include "pycore_frame.h"
#include "opcode.h"
#include "optimizer.h"
#include "pydtrace.h"
#include "setobject.h"


#define USE_COMPUTED_GOTOS 0
#include "ceval_macros.h"

/* Flow control macros */
#define DEOPT_IF(cond, instname) ((void)0)
#define ERROR_IF(cond, labelname) ((void)0)
#define GO_TO_INSTRUCTION(instname) ((void)0)

#define inst(name, ...) case name:
#define op(name, ...) /* NAME is ignored */
#define macro(name) static int MACRO_##name
#define super(name) static int SUPER_##name
#define family(name, ...) static int family_##name
#define pseudo(name) static int pseudo_##name

// Dummy variables for stack effects.
static PyObject *value, *value1, *value2, *left, *right, *res, *sum, *prod, *sub;
static PyObject *container, *start, *stop, *v, *lhs, *rhs, *res2;
static PyObject *list, *tuple, *dict, *owner, *set, *str, *tup, *map, *keys;
static PyObject *exit_func, *lasti, *val, *retval, *obj, *iter, *exhausted;
static PyObject *aiter, *awaitable, *iterable, *w, *exc_value, *bc, *locals;
static PyObject *orig, *excs, *update, *b, *fromlist, *level, *from;
static PyObject **pieces, **values;
static size_t jump;
// Dummy variables for cache effects
static uint16_t invert, counter, index, hint;
#define unused 0  // Used in a macro def, can't be static
static uint32_t type_version;

static PyObject *
dummy_func(
    PyThreadState *tstate,
    _PyInterpreterFrame *frame,
    unsigned char opcode,
    unsigned int oparg,
    _PyCFrame cframe,
    _Py_CODEUNIT *next_instr,
    PyObject **stack_pointer,
    PyObject *kwnames,
    int throwflag,
    PyObject *args[]
)
{
    // Dummy labels.
    pop_1_error:
    // Dummy locals.
    PyObject *dummy;
    PyObject *attr;
    PyObject *attrs;
    PyObject *bottom;
    PyObject *callable;
    PyObject *callargs;
    PyObject *codeobj;
    PyObject *cond;
    PyObject *descr;
    _PyInterpreterFrame  entry_frame;
    PyObject *exc;
    PyObject *exit;
    PyObject *fget;
    PyObject *fmt_spec;
    PyObject *func;
    uint32_t func_version;
    PyObject *getattribute;
    PyObject *kwargs;
    PyObject *kwdefaults;
    PyObject *len_o;
    PyObject *match;
    PyObject *match_type;
    PyObject *method;
    PyObject *mgr;
    Py_ssize_t min_args;
    PyObject *names;
    PyObject *new_exc;
    PyObject *next;
    PyObject *none;
    PyObject *null;
    PyObject *prev_exc;
    PyObject *receiver;
    PyObject *rest;
    int result;
    PyObject *self;
    PyObject *seq;
    PyObject *slice;
    PyObject *step;
    PyObject *subject;
    PyObject *top;
    PyObject *type;
    PyObject *typevars;
    int values_or_none;

    switch (opcode) {

// BEGIN BYTECODES //
        inst(NOP, (--)) {
        }

        inst(RESUME, (--)) {
            assert(tstate->cframe == &cframe);
            assert(frame == cframe.current_frame);
            /* Possibly combine this with eval breaker */
            if (_PyFrame_GetCode(frame)->_co_instrumentation_version != tstate->interp->monitoring_version) {
                int err = _Py_Instrument(_PyFrame_GetCode(frame), tstate->interp);
                ERROR_IF(err, error);
                next_instr--;
            }
            else if (oparg < 2) {
                CHECK_EVAL_BREAKER();
            }
        }

        inst(INSTRUMENTED_RESUME, (--)) {
            /* Possible performance enhancement:
             *   We need to check the eval breaker anyway, can we
             * combine the instrument verison check and the eval breaker test?
             */
            if (_PyFrame_GetCode(frame)->_co_instrumentation_version != tstate->interp->monitoring_version) {
                if (_Py_Instrument(_PyFrame_GetCode(frame), tstate->interp)) {
                    goto error;
                }
                next_instr--;
            }
            else {
                if (oparg < 2) {
                    CHECK_EVAL_BREAKER();
                }
                _PyFrame_SetStackPointer(frame, stack_pointer);
                int err = _Py_call_instrumentation(
                        tstate, oparg > 0, frame, next_instr-1);
                stack_pointer = _PyFrame_GetStackPointer(frame);
                ERROR_IF(err, error);
                if (frame->prev_instr != next_instr-1) {
                    /* Instrumentation has jumped */
                    next_instr = frame->prev_instr;
                    DISPATCH();
                }
            }
        }

        pseudo(LOAD_CLOSURE) = {
            LOAD_FAST,
        };

        inst(LOAD_FAST_CHECK, (-- value)) {
            value = GETLOCAL(oparg);
            ERROR_IF(value == NULL, unbound_local_error);
            Py_INCREF(value);
        }

        inst(LOAD_FAST, (-- value)) {
            value = GETLOCAL(oparg);
            assert(value != NULL);
            Py_INCREF(value);
        }

        inst(LOAD_FAST_AND_CLEAR, (-- value)) {
            value = GETLOCAL(oparg);
            // do not use SETLOCAL here, it decrefs the old value
            GETLOCAL(oparg) = NULL;
        }

        inst(LOAD_FAST_LOAD_FAST, ( -- value1, value2)) {
            uint32_t oparg1 = oparg >> 4;
            uint32_t oparg2 = oparg & 15;
            value1 = GETLOCAL(oparg1);
            value2 = GETLOCAL(oparg2);
            Py_INCREF(value1);
            Py_INCREF(value2);
        }

        inst(LOAD_CONST, (-- value)) {
            value = GETITEM(FRAME_CO_CONSTS, oparg);
            Py_INCREF(value);
        }

        inst(STORE_FAST, (value --)) {
            SETLOCAL(oparg, value);
        }

        pseudo(STORE_FAST_MAYBE_NULL) = {
            STORE_FAST,
        };

        inst(STORE_FAST_LOAD_FAST, (value1 -- value2)) {
            uint32_t oparg1 = oparg >> 4;
            uint32_t oparg2 = oparg & 15;
            SETLOCAL(oparg1, value1);
            value2 = GETLOCAL(oparg2);
            Py_INCREF(value2);
        }

        inst(STORE_FAST_STORE_FAST, (value2, value1 --)) {
            uint32_t oparg1 = oparg >> 4;
            uint32_t oparg2 = oparg & 15;
            SETLOCAL(oparg1, value1);
            SETLOCAL(oparg2, value2);
        }

        inst(POP_TOP, (value --)) {
            DECREF_INPUTS();
        }

        inst(PUSH_NULL, (-- res)) {
            res = NULL;
        }

        macro(END_FOR) = POP_TOP + POP_TOP;

        inst(INSTRUMENTED_END_FOR, (receiver, value --)) {
            /* Need to create a fake StopIteration error here,
             * to conform to PEP 380 */
            if (PyGen_Check(receiver)) {
                PyErr_SetObject(PyExc_StopIteration, value);
                if (monitor_stop_iteration(tstate, frame, next_instr-1)) {
                    goto error;
                }
                PyErr_SetRaisedException(NULL);
            }
            DECREF_INPUTS();
        }

        inst(END_SEND, (receiver, value -- value)) {
            Py_DECREF(receiver);
        }

        inst(INSTRUMENTED_END_SEND, (receiver, value -- value)) {
            if (PyGen_Check(receiver) || PyCoro_CheckExact(receiver)) {
                PyErr_SetObject(PyExc_StopIteration, value);
                if (monitor_stop_iteration(tstate, frame, next_instr-1)) {
                    goto error;
                }
                PyErr_SetRaisedException(NULL);
            }
            Py_DECREF(receiver);
        }

        inst(UNARY_NEGATIVE, (value -- res)) {
            res = PyNumber_Negative(value);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(UNARY_NOT, (value -- res)) {
            assert(PyBool_Check(value));
            res = Py_IsFalse(value) ? Py_True : Py_False;
        }

        family(TO_BOOL, INLINE_CACHE_ENTRIES_TO_BOOL) = {
            TO_BOOL_ALWAYS_TRUE,
            TO_BOOL_BOOL,
            TO_BOOL_INT,
            TO_BOOL_LIST,
            TO_BOOL_NONE,
            TO_BOOL_STR,
        };

        inst(TO_BOOL, (unused/1, unused/2, value -- res)) {
            #if ENABLE_SPECIALIZATION
            _PyToBoolCache *cache = (_PyToBoolCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_ToBool(value, next_instr);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(TO_BOOL, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            int err = PyObject_IsTrue(value);
            DECREF_INPUTS();
            ERROR_IF(err < 0, error);
            res = err ? Py_True : Py_False;
        }

        inst(TO_BOOL_BOOL, (unused/1, unused/2, value -- value)) {
            DEOPT_IF(!PyBool_Check(value), TO_BOOL);
            STAT_INC(TO_BOOL, hit);
        }

        inst(TO_BOOL_INT, (unused/1, unused/2, value -- res)) {
            DEOPT_IF(!PyLong_CheckExact(value), TO_BOOL);
            STAT_INC(TO_BOOL, hit);
            if (_PyLong_IsZero((PyLongObject *)value)) {
                assert(_Py_IsImmortal(value));
                res = Py_False;
            }
            else {
                DECREF_INPUTS();
                res = Py_True;
            }
        }

        inst(TO_BOOL_LIST, (unused/1, unused/2, value -- res)) {
            DEOPT_IF(!PyList_CheckExact(value), TO_BOOL);
            STAT_INC(TO_BOOL, hit);
            res = Py_SIZE(value) ? Py_True : Py_False;
            DECREF_INPUTS();
        }

        inst(TO_BOOL_NONE, (unused/1, unused/2, value -- res)) {
            // This one is a bit weird, because we expect *some* failures:
            DEOPT_IF(!Py_IsNone(value), TO_BOOL);
            STAT_INC(TO_BOOL, hit);
            res = Py_False;
        }

        inst(TO_BOOL_STR, (unused/1, unused/2, value -- res)) {
            DEOPT_IF(!PyUnicode_CheckExact(value), TO_BOOL);
            STAT_INC(TO_BOOL, hit);
            if (value == &_Py_STR(empty)) {
                assert(_Py_IsImmortal(value));
                res = Py_False;
            }
            else {
                assert(Py_SIZE(value));
                DECREF_INPUTS();
                res = Py_True;
            }
        }

        inst(TO_BOOL_ALWAYS_TRUE, (unused/1, version/2, value -- res)) {
            // This one is a bit weird, because we expect *some* failures:
            assert(version);
            DEOPT_IF(Py_TYPE(value)->tp_version_tag != version, TO_BOOL);
            STAT_INC(TO_BOOL, hit);
            DECREF_INPUTS();
            res = Py_True;
        }

        inst(UNARY_INVERT, (value -- res)) {
            res = PyNumber_Invert(value);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        family(BINARY_OP, INLINE_CACHE_ENTRIES_BINARY_OP) = {
            BINARY_OP_MULTIPLY_INT,
            BINARY_OP_ADD_INT,
            BINARY_OP_SUBTRACT_INT,
            BINARY_OP_MULTIPLY_FLOAT,
            BINARY_OP_ADD_FLOAT,
            BINARY_OP_SUBTRACT_FLOAT,
            BINARY_OP_ADD_UNICODE,
            // BINARY_OP_INPLACE_ADD_UNICODE,  // See comments at that opcode.
        };

        op(_GUARD_BOTH_INT, (left, right -- left, right)) {
            DEOPT_IF(!PyLong_CheckExact(left), BINARY_OP);
            DEOPT_IF(!PyLong_CheckExact(right), BINARY_OP);
        }

        op(_BINARY_OP_MULTIPLY_INT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            res = _PyLong_Multiply((PyLongObject *)left, (PyLongObject *)right);
            _Py_DECREF_SPECIALIZED(right, (destructor)PyObject_Free);
            _Py_DECREF_SPECIALIZED(left, (destructor)PyObject_Free);
            ERROR_IF(res == NULL, error);
        }

        op(_BINARY_OP_ADD_INT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            res = _PyLong_Add((PyLongObject *)left, (PyLongObject *)right);
            _Py_DECREF_SPECIALIZED(right, (destructor)PyObject_Free);
            _Py_DECREF_SPECIALIZED(left, (destructor)PyObject_Free);
            ERROR_IF(res == NULL, error);
        }

        op(_BINARY_OP_SUBTRACT_INT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            res = _PyLong_Subtract((PyLongObject *)left, (PyLongObject *)right);
            _Py_DECREF_SPECIALIZED(right, (destructor)PyObject_Free);
            _Py_DECREF_SPECIALIZED(left, (destructor)PyObject_Free);
            ERROR_IF(res == NULL, error);
        }

        macro(BINARY_OP_MULTIPLY_INT) =
            _GUARD_BOTH_INT + _BINARY_OP_MULTIPLY_INT;
        macro(BINARY_OP_ADD_INT) =
            _GUARD_BOTH_INT + _BINARY_OP_ADD_INT;
        macro(BINARY_OP_SUBTRACT_INT) =
            _GUARD_BOTH_INT + _BINARY_OP_SUBTRACT_INT;

        op(_GUARD_BOTH_FLOAT, (left, right -- left, right)) {
            DEOPT_IF(!PyFloat_CheckExact(left), BINARY_OP);
            DEOPT_IF(!PyFloat_CheckExact(right), BINARY_OP);
        }

        op(_BINARY_OP_MULTIPLY_FLOAT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            double dres =
                ((PyFloatObject *)left)->ob_fval *
                ((PyFloatObject *)right)->ob_fval;
            DECREF_INPUTS_AND_REUSE_FLOAT(left, right, dres, res);
        }

        op(_BINARY_OP_ADD_FLOAT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            double dres =
                ((PyFloatObject *)left)->ob_fval +
                ((PyFloatObject *)right)->ob_fval;
            DECREF_INPUTS_AND_REUSE_FLOAT(left, right, dres, res);
        }

        op(_BINARY_OP_SUBTRACT_FLOAT, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            double dres =
                ((PyFloatObject *)left)->ob_fval -
                ((PyFloatObject *)right)->ob_fval;
            DECREF_INPUTS_AND_REUSE_FLOAT(left, right, dres, res);
        }

        macro(BINARY_OP_MULTIPLY_FLOAT) =
            _GUARD_BOTH_FLOAT + _BINARY_OP_MULTIPLY_FLOAT;
        macro(BINARY_OP_ADD_FLOAT) =
            _GUARD_BOTH_FLOAT + _BINARY_OP_ADD_FLOAT;
        macro(BINARY_OP_SUBTRACT_FLOAT) =
            _GUARD_BOTH_FLOAT + _BINARY_OP_SUBTRACT_FLOAT;

        op(_GUARD_BOTH_UNICODE, (left, right -- left, right)) {
            DEOPT_IF(!PyUnicode_CheckExact(left), BINARY_OP);
            DEOPT_IF(!PyUnicode_CheckExact(right), BINARY_OP);
        }

        op(_BINARY_OP_ADD_UNICODE, (unused/1, left, right -- res)) {
            STAT_INC(BINARY_OP, hit);
            res = PyUnicode_Concat(left, right);
            _Py_DECREF_SPECIALIZED(left, _PyUnicode_ExactDealloc);
            _Py_DECREF_SPECIALIZED(right, _PyUnicode_ExactDealloc);
            ERROR_IF(res == NULL, error);
        }

        macro(BINARY_OP_ADD_UNICODE) =
            _GUARD_BOTH_UNICODE + _BINARY_OP_ADD_UNICODE;

        // This is a subtle one. It's a super-instruction for
        // BINARY_OP_ADD_UNICODE followed by STORE_FAST
        // where the store goes into the left argument.
        // So the inputs are the same as for all BINARY_OP
        // specializations, but there is no output.
        // At the end we just skip over the STORE_FAST.
        op(_BINARY_OP_INPLACE_ADD_UNICODE, (left, right --)) {
            _Py_CODEUNIT true_next = next_instr[INLINE_CACHE_ENTRIES_BINARY_OP];
            assert(true_next.op.code == STORE_FAST);
            PyObject **target_local = &GETLOCAL(true_next.op.arg);
            DEOPT_IF(*target_local != left, BINARY_OP);
            STAT_INC(BINARY_OP, hit);
            /* Handle `left = left + right` or `left += right` for str.
             *
             * When possible, extend `left` in place rather than
             * allocating a new PyUnicodeObject. This attempts to avoid
             * quadratic behavior when one neglects to use str.join().
             *
             * If `left` has only two references remaining (one from
             * the stack, one in the locals), DECREFing `left` leaves
             * only the locals reference, so PyUnicode_Append knows
             * that the string is safe to mutate.
             */
            assert(Py_REFCNT(left) >= 2);
            _Py_DECREF_NO_DEALLOC(left);
            PyUnicode_Append(target_local, right);
            _Py_DECREF_SPECIALIZED(right, _PyUnicode_ExactDealloc);
            ERROR_IF(*target_local == NULL, error);
            // The STORE_FAST is already done.
            SKIP_OVER(INLINE_CACHE_ENTRIES_BINARY_OP + 1);
        }

        macro(BINARY_OP_INPLACE_ADD_UNICODE) =
            _GUARD_BOTH_UNICODE + _BINARY_OP_INPLACE_ADD_UNICODE;

        family(BINARY_SUBSCR, INLINE_CACHE_ENTRIES_BINARY_SUBSCR) = {
            BINARY_SUBSCR_DICT,
            BINARY_SUBSCR_GETITEM,
            BINARY_SUBSCR_LIST_INT,
            BINARY_SUBSCR_STR_INT,
            BINARY_SUBSCR_TUPLE_INT,
        };

        inst(BINARY_SUBSCR, (unused/1, container, sub -- res)) {
            #if ENABLE_SPECIALIZATION
            _PyBinarySubscrCache *cache = (_PyBinarySubscrCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_BinarySubscr(container, sub, next_instr);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(BINARY_SUBSCR, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            res = PyObject_GetItem(container, sub);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(BINARY_SLICE, (container, start, stop -- res)) {
            PyObject *slice = _PyBuildSlice_ConsumeRefs(start, stop);
            // Can't use ERROR_IF() here, because we haven't
            // DECREF'ed container yet, and we still own slice.
            if (slice == NULL) {
                res = NULL;
            }
            else {
                res = PyObject_GetItem(container, slice);
                Py_DECREF(slice);
            }
            Py_DECREF(container);
            ERROR_IF(res == NULL, error);
        }

        inst(STORE_SLICE, (v, container, start, stop -- )) {
            PyObject *slice = _PyBuildSlice_ConsumeRefs(start, stop);
            int err;
            if (slice == NULL) {
                err = 1;
            }
            else {
                err = PyObject_SetItem(container, slice, v);
                Py_DECREF(slice);
            }
            Py_DECREF(v);
            Py_DECREF(container);
            ERROR_IF(err, error);
        }

        inst(BINARY_SUBSCR_LIST_INT, (unused/1, list, sub -- res)) {
            DEOPT_IF(!PyLong_CheckExact(sub), BINARY_SUBSCR);
            DEOPT_IF(!PyList_CheckExact(list), BINARY_SUBSCR);

            // Deopt unless 0 <= sub < PyList_Size(list)
            DEOPT_IF(!_PyLong_IsNonNegativeCompact((PyLongObject *)sub), BINARY_SUBSCR);
            Py_ssize_t index = ((PyLongObject*)sub)->long_value.ob_digit[0];
            DEOPT_IF(index >= PyList_GET_SIZE(list), BINARY_SUBSCR);
            STAT_INC(BINARY_SUBSCR, hit);
            res = PyList_GET_ITEM(list, index);
            assert(res != NULL);
            Py_INCREF(res);
            _Py_DECREF_SPECIALIZED(sub, (destructor)PyObject_Free);
            Py_DECREF(list);
        }

        inst(BINARY_SUBSCR_STR_INT, (unused/1, str, sub -- res)) {
            DEOPT_IF(!PyLong_CheckExact(sub), BINARY_SUBSCR);
            DEOPT_IF(!PyUnicode_CheckExact(str), BINARY_SUBSCR);
            DEOPT_IF(!_PyLong_IsNonNegativeCompact((PyLongObject *)sub), BINARY_SUBSCR);
            Py_ssize_t index = ((PyLongObject*)sub)->long_value.ob_digit[0];
            DEOPT_IF(PyUnicode_GET_LENGTH(str) <= index, BINARY_SUBSCR);
            // Specialize for reading an ASCII character from any string:
            Py_UCS4 c = PyUnicode_READ_CHAR(str, index);
            DEOPT_IF(Py_ARRAY_LENGTH(_Py_SINGLETON(strings).ascii) <= c, BINARY_SUBSCR);
            STAT_INC(BINARY_SUBSCR, hit);
            res = (PyObject*)&_Py_SINGLETON(strings).ascii[c];
            _Py_DECREF_SPECIALIZED(sub, (destructor)PyObject_Free);
            Py_DECREF(str);
        }

        inst(BINARY_SUBSCR_TUPLE_INT, (unused/1, tuple, sub -- res)) {
            DEOPT_IF(!PyLong_CheckExact(sub), BINARY_SUBSCR);
            DEOPT_IF(!PyTuple_CheckExact(tuple), BINARY_SUBSCR);

            // Deopt unless 0 <= sub < PyTuple_Size(list)
            DEOPT_IF(!_PyLong_IsNonNegativeCompact((PyLongObject *)sub), BINARY_SUBSCR);
            Py_ssize_t index = ((PyLongObject*)sub)->long_value.ob_digit[0];
            DEOPT_IF(index >= PyTuple_GET_SIZE(tuple), BINARY_SUBSCR);
            STAT_INC(BINARY_SUBSCR, hit);
            res = PyTuple_GET_ITEM(tuple, index);
            assert(res != NULL);
            Py_INCREF(res);
            _Py_DECREF_SPECIALIZED(sub, (destructor)PyObject_Free);
            Py_DECREF(tuple);
        }

        inst(BINARY_SUBSCR_DICT, (unused/1, dict, sub -- res)) {
            DEOPT_IF(!PyDict_CheckExact(dict), BINARY_SUBSCR);
            STAT_INC(BINARY_SUBSCR, hit);
            res = PyDict_GetItemWithError(dict, sub);
            if (res == NULL) {
                if (!_PyErr_Occurred(tstate)) {
                    _PyErr_SetKeyError(sub);
                }
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }
            Py_INCREF(res);  // Do this before DECREF'ing dict, sub
            DECREF_INPUTS();
        }

        inst(BINARY_SUBSCR_GETITEM, (unused/1, container, sub -- unused)) {
            DEOPT_IF(tstate->interp->eval_frame, BINARY_SUBSCR);
            PyTypeObject *tp = Py_TYPE(container);
            DEOPT_IF(!PyType_HasFeature(tp, Py_TPFLAGS_HEAPTYPE), BINARY_SUBSCR);
            PyHeapTypeObject *ht = (PyHeapTypeObject *)tp;
            PyObject *cached = ht->_spec_cache.getitem;
            DEOPT_IF(cached == NULL, BINARY_SUBSCR);
            assert(PyFunction_Check(cached));
            PyFunctionObject *getitem = (PyFunctionObject *)cached;
            uint32_t cached_version = ht->_spec_cache.getitem_version;
            DEOPT_IF(getitem->func_version != cached_version, BINARY_SUBSCR);
            PyCodeObject *code = (PyCodeObject *)getitem->func_code;
            assert(code->co_argcount == 2);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize), BINARY_SUBSCR);
            STAT_INC(BINARY_SUBSCR, hit);
            Py_INCREF(getitem);
            _PyInterpreterFrame *new_frame = _PyFrame_PushUnchecked(tstate, getitem, 2);
            STACK_SHRINK(2);
            new_frame->localsplus[0] = container;
            new_frame->localsplus[1] = sub;
            SKIP_OVER(INLINE_CACHE_ENTRIES_BINARY_SUBSCR);
            frame->return_offset = 0;
            DISPATCH_INLINED(new_frame);
        }

        inst(LIST_APPEND, (list, unused[oparg-1], v -- list, unused[oparg-1])) {
            ERROR_IF(_PyList_AppendTakeRef((PyListObject *)list, v) < 0, error);
        }

        inst(SET_ADD, (set, unused[oparg-1], v -- set, unused[oparg-1])) {
            int err = PySet_Add(set, v);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        family(STORE_SUBSCR, INLINE_CACHE_ENTRIES_STORE_SUBSCR) = {
            STORE_SUBSCR_DICT,
            STORE_SUBSCR_LIST_INT,
        };

        inst(STORE_SUBSCR, (unused/1, v, container, sub -- )) {
            #if ENABLE_SPECIALIZATION
            _PyStoreSubscrCache *cache = (_PyStoreSubscrCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_StoreSubscr(container, sub, next_instr);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(STORE_SUBSCR, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            /* container[sub] = v */
            int err = PyObject_SetItem(container, sub, v);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(STORE_SUBSCR_LIST_INT, (unused/1, value, list, sub -- )) {
            DEOPT_IF(!PyLong_CheckExact(sub), STORE_SUBSCR);
            DEOPT_IF(!PyList_CheckExact(list), STORE_SUBSCR);

            // Ensure nonnegative, zero-or-one-digit ints.
            DEOPT_IF(!_PyLong_IsNonNegativeCompact((PyLongObject *)sub), STORE_SUBSCR);
            Py_ssize_t index = ((PyLongObject*)sub)->long_value.ob_digit[0];
            // Ensure index < len(list)
            DEOPT_IF(index >= PyList_GET_SIZE(list), STORE_SUBSCR);
            STAT_INC(STORE_SUBSCR, hit);

            PyObject *old_value = PyList_GET_ITEM(list, index);
            PyList_SET_ITEM(list, index, value);
            assert(old_value != NULL);
            Py_DECREF(old_value);
            _Py_DECREF_SPECIALIZED(sub, (destructor)PyObject_Free);
            Py_DECREF(list);
        }

        inst(STORE_SUBSCR_DICT, (unused/1, value, dict, sub -- )) {
            DEOPT_IF(!PyDict_CheckExact(dict), STORE_SUBSCR);
            STAT_INC(STORE_SUBSCR, hit);
            int err = _PyDict_SetItem_Take2((PyDictObject *)dict, sub, value);
            Py_DECREF(dict);
            ERROR_IF(err, error);
        }

        inst(DELETE_SUBSCR, (container, sub --)) {
            /* del container[sub] */
            int err = PyObject_DelItem(container, sub);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(CALL_INTRINSIC_1, (value -- res)) {
            assert(oparg <= MAX_INTRINSIC_1);
            res = _PyIntrinsics_UnaryFunctions[oparg].func(tstate, value);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(CALL_INTRINSIC_2, (value2, value1 -- res)) {
            assert(oparg <= MAX_INTRINSIC_2);
            res = _PyIntrinsics_BinaryFunctions[oparg].func(tstate, value2, value1);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(RAISE_VARARGS, (args[oparg] -- )) {
            PyObject *cause = NULL, *exc = NULL;
            switch (oparg) {
            case 2:
                cause = args[1];
                /* fall through */
            case 1:
                exc = args[0];
                /* fall through */
            case 0:
                if (do_raise(tstate, exc, cause)) {
                    assert(oparg == 0);
                    monitor_reraise(tstate, frame, next_instr-1);
                    goto exception_unwind;
                }
                break;
            default:
                _PyErr_SetString(tstate, PyExc_SystemError,
                                 "bad RAISE_VARARGS oparg");
                break;
            }
            ERROR_IF(true, error);
        }

        inst(INTERPRETER_EXIT, (retval --)) {
            assert(frame == &entry_frame);
            assert(_PyFrame_IsIncomplete(frame));
            /* Restore previous cframe and return. */
            tstate->cframe = cframe.previous;
            assert(tstate->cframe->current_frame == frame->previous);
            assert(!_PyErr_Occurred(tstate));
            tstate->c_recursion_remaining += PY_EVAL_C_STACK_UNITS;
            return retval;
        }

        inst(RETURN_VALUE, (retval --)) {
            STACK_SHRINK(1);
            assert(EMPTY());
            _PyFrame_SetStackPointer(frame, stack_pointer);
            _Py_LeaveRecursiveCallPy(tstate);
            assert(frame != &entry_frame);
            // GH-99729: We need to unlink the frame *before* clearing it:
            _PyInterpreterFrame *dying = frame;
            frame = cframe.current_frame = dying->previous;
            _PyEvalFrameClearAndPop(tstate, dying);
            frame->prev_instr += frame->return_offset;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(INSTRUMENTED_RETURN_VALUE, (retval --)) {
            int err = _Py_call_instrumentation_arg(
                    tstate, PY_MONITORING_EVENT_PY_RETURN,
                    frame, next_instr-1, retval);
            if (err) goto error;
            STACK_SHRINK(1);
            assert(EMPTY());
            _PyFrame_SetStackPointer(frame, stack_pointer);
            _Py_LeaveRecursiveCallPy(tstate);
            assert(frame != &entry_frame);
            // GH-99729: We need to unlink the frame *before* clearing it:
            _PyInterpreterFrame *dying = frame;
            frame = cframe.current_frame = dying->previous;
            _PyEvalFrameClearAndPop(tstate, dying);
            frame->prev_instr += frame->return_offset;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(RETURN_CONST, (--)) {
            PyObject *retval = GETITEM(FRAME_CO_CONSTS, oparg);
            Py_INCREF(retval);
            assert(EMPTY());
            _PyFrame_SetStackPointer(frame, stack_pointer);
            _Py_LeaveRecursiveCallPy(tstate);
            assert(frame != &entry_frame);
            // GH-99729: We need to unlink the frame *before* clearing it:
            _PyInterpreterFrame *dying = frame;
            frame = cframe.current_frame = dying->previous;
            _PyEvalFrameClearAndPop(tstate, dying);
            frame->prev_instr += frame->return_offset;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(INSTRUMENTED_RETURN_CONST, (--)) {
            PyObject *retval = GETITEM(FRAME_CO_CONSTS, oparg);
            int err = _Py_call_instrumentation_arg(
                    tstate, PY_MONITORING_EVENT_PY_RETURN,
                    frame, next_instr-1, retval);
            if (err) goto error;
            Py_INCREF(retval);
            assert(EMPTY());
            _PyFrame_SetStackPointer(frame, stack_pointer);
            _Py_LeaveRecursiveCallPy(tstate);
            assert(frame != &entry_frame);
            // GH-99729: We need to unlink the frame *before* clearing it:
            _PyInterpreterFrame *dying = frame;
            frame = cframe.current_frame = dying->previous;
            _PyEvalFrameClearAndPop(tstate, dying);
            frame->prev_instr += frame->return_offset;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(GET_AITER, (obj -- iter)) {
            unaryfunc getter = NULL;
            PyTypeObject *type = Py_TYPE(obj);

            if (type->tp_as_async != NULL) {
                getter = type->tp_as_async->am_aiter;
            }

            if (getter == NULL) {
                _PyErr_Format(tstate, PyExc_TypeError,
                              "'async for' requires an object with "
                              "__aiter__ method, got %.100s",
                              type->tp_name);
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }

            iter = (*getter)(obj);
            DECREF_INPUTS();
            ERROR_IF(iter == NULL, error);

            if (Py_TYPE(iter)->tp_as_async == NULL ||
                    Py_TYPE(iter)->tp_as_async->am_anext == NULL) {

                _PyErr_Format(tstate, PyExc_TypeError,
                              "'async for' received an object from __aiter__ "
                              "that does not implement __anext__: %.100s",
                              Py_TYPE(iter)->tp_name);
                Py_DECREF(iter);
                ERROR_IF(true, error);
            }
        }

        inst(GET_ANEXT, (aiter -- aiter, awaitable)) {
            unaryfunc getter = NULL;
            PyObject *next_iter = NULL;
            PyTypeObject *type = Py_TYPE(aiter);

            if (PyAsyncGen_CheckExact(aiter)) {
                awaitable = type->tp_as_async->am_anext(aiter);
                if (awaitable == NULL) {
                    goto error;
                }
            } else {
                if (type->tp_as_async != NULL){
                    getter = type->tp_as_async->am_anext;
                }

                if (getter != NULL) {
                    next_iter = (*getter)(aiter);
                    if (next_iter == NULL) {
                        goto error;
                    }
                }
                else {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                  "'async for' requires an iterator with "
                                  "__anext__ method, got %.100s",
                                  type->tp_name);
                    goto error;
                }

                awaitable = _PyCoro_GetAwaitableIter(next_iter);
                if (awaitable == NULL) {
                    _PyErr_FormatFromCause(
                        PyExc_TypeError,
                        "'async for' received an invalid object "
                        "from __anext__: %.100s",
                        Py_TYPE(next_iter)->tp_name);

                    Py_DECREF(next_iter);
                    goto error;
                } else {
                    Py_DECREF(next_iter);
                }
            }
        }

        inst(GET_AWAITABLE, (iterable -- iter)) {
            iter = _PyCoro_GetAwaitableIter(iterable);

            if (iter == NULL) {
                _PyEval_FormatAwaitableError(tstate, Py_TYPE(iterable), oparg);
            }

            DECREF_INPUTS();

            if (iter != NULL && PyCoro_CheckExact(iter)) {
                PyObject *yf = _PyGen_yf((PyGenObject*)iter);
                if (yf != NULL) {
                    /* `iter` is a coroutine object that is being
                       awaited, `yf` is a pointer to the current awaitable
                       being awaited on. */
                    Py_DECREF(yf);
                    Py_CLEAR(iter);
                    _PyErr_SetString(tstate, PyExc_RuntimeError,
                                     "coroutine is being awaited already");
                    /* The code below jumps to `error` if `iter` is NULL. */
                }
            }

            ERROR_IF(iter == NULL, error);
        }

        family(SEND, INLINE_CACHE_ENTRIES_SEND) = {
            SEND_GEN,
        };

        inst(SEND, (unused/1, receiver, v -- receiver, retval)) {
            #if ENABLE_SPECIALIZATION
            _PySendCache *cache = (_PySendCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_Send(receiver, next_instr);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(SEND, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            assert(frame != &entry_frame);
            if ((tstate->interp->eval_frame == NULL) &&
                (Py_TYPE(receiver) == &PyGen_Type || Py_TYPE(receiver) == &PyCoro_Type) &&
                ((PyGenObject *)receiver)->gi_frame_state < FRAME_EXECUTING)
            {
                PyGenObject *gen = (PyGenObject *)receiver;
                _PyInterpreterFrame *gen_frame = (_PyInterpreterFrame *)gen->gi_iframe;
                frame->return_offset = oparg;
                STACK_SHRINK(1);
                _PyFrame_StackPush(gen_frame, v);
                gen->gi_frame_state = FRAME_EXECUTING;
                gen->gi_exc_state.previous_item = tstate->exc_info;
                tstate->exc_info = &gen->gi_exc_state;
                SKIP_OVER(INLINE_CACHE_ENTRIES_SEND);
                DISPATCH_INLINED(gen_frame);
            }
            if (Py_IsNone(v) && PyIter_Check(receiver)) {
                retval = Py_TYPE(receiver)->tp_iternext(receiver);
            }
            else {
                retval = PyObject_CallMethodOneArg(receiver, &_Py_ID(send), v);
            }
            if (retval == NULL) {
                if (_PyErr_ExceptionMatches(tstate, PyExc_StopIteration)
                ) {
                    monitor_raise(tstate, frame, next_instr-1);
                }
                if (_PyGen_FetchStopIterationValue(&retval) == 0) {
                    assert(retval != NULL);
                    JUMPBY(oparg);
                }
                else {
                    goto error;
                }
            }
            Py_DECREF(v);
        }

        inst(SEND_GEN, (unused/1, receiver, v -- receiver, unused)) {
            DEOPT_IF(tstate->interp->eval_frame, SEND);
            PyGenObject *gen = (PyGenObject *)receiver;
            DEOPT_IF(Py_TYPE(gen) != &PyGen_Type &&
                     Py_TYPE(gen) != &PyCoro_Type, SEND);
            DEOPT_IF(gen->gi_frame_state >= FRAME_EXECUTING, SEND);
            STAT_INC(SEND, hit);
            _PyInterpreterFrame *gen_frame = (_PyInterpreterFrame *)gen->gi_iframe;
            frame->return_offset = oparg;
            STACK_SHRINK(1);
            _PyFrame_StackPush(gen_frame, v);
            gen->gi_frame_state = FRAME_EXECUTING;
            gen->gi_exc_state.previous_item = tstate->exc_info;
            tstate->exc_info = &gen->gi_exc_state;
            SKIP_OVER(INLINE_CACHE_ENTRIES_SEND);
            DISPATCH_INLINED(gen_frame);
        }

        inst(INSTRUMENTED_YIELD_VALUE, (retval -- unused)) {
            assert(frame != &entry_frame);
            assert(oparg >= 0); /* make the generator identify this as HAS_ARG */
            PyGenObject *gen = _PyFrame_GetGenerator(frame);
            gen->gi_frame_state = FRAME_SUSPENDED;
            _PyFrame_SetStackPointer(frame, stack_pointer - 1);
            int err = _Py_call_instrumentation_arg(
                    tstate, PY_MONITORING_EVENT_PY_YIELD,
                    frame, next_instr-1, retval);
            if (err) goto error;
            tstate->exc_info = gen->gi_exc_state.previous_item;
            gen->gi_exc_state.previous_item = NULL;
            _Py_LeaveRecursiveCallPy(tstate);
            _PyInterpreterFrame *gen_frame = frame;
            frame = cframe.current_frame = frame->previous;
            gen_frame->previous = NULL;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(YIELD_VALUE, (retval -- unused)) {
            // NOTE: It's important that YIELD_VALUE never raises an exception!
            // The compiler treats any exception raised here as a failed close()
            // or throw() call.
            assert(oparg >= 0); /* make the generator identify this as HAS_ARG */
            assert(frame != &entry_frame);
            PyGenObject *gen = _PyFrame_GetGenerator(frame);
            gen->gi_frame_state = FRAME_SUSPENDED;
            _PyFrame_SetStackPointer(frame, stack_pointer - 1);
            tstate->exc_info = gen->gi_exc_state.previous_item;
            gen->gi_exc_state.previous_item = NULL;
            _Py_LeaveRecursiveCallPy(tstate);
            _PyInterpreterFrame *gen_frame = frame;
            frame = cframe.current_frame = frame->previous;
            gen_frame->previous = NULL;
            _PyFrame_StackPush(frame, retval);
            goto resume_frame;
        }

        inst(POP_EXCEPT, (exc_value -- )) {
            _PyErr_StackItem *exc_info = tstate->exc_info;
            Py_XSETREF(exc_info->exc_value, exc_value);
        }

        inst(RERAISE, (values[oparg], exc -- values[oparg])) {
            assert(oparg >= 0 && oparg <= 2);
            if (oparg) {
                PyObject *lasti = values[0];
                if (PyLong_Check(lasti)) {
                    frame->prev_instr = _PyCode_CODE(_PyFrame_GetCode(frame)) + PyLong_AsLong(lasti);
                    assert(!_PyErr_Occurred(tstate));
                }
                else {
                    assert(PyLong_Check(lasti));
                    _PyErr_SetString(tstate, PyExc_SystemError, "lasti is not an int");
                    goto error;
                }
            }
            assert(exc && PyExceptionInstance_Check(exc));
            Py_INCREF(exc);
            _PyErr_SetRaisedException(tstate, exc);
            monitor_reraise(tstate, frame, next_instr-1);
            goto exception_unwind;
        }

        inst(END_ASYNC_FOR, (awaitable, exc -- )) {
            assert(exc && PyExceptionInstance_Check(exc));
            if (PyErr_GivenExceptionMatches(exc, PyExc_StopAsyncIteration)) {
                DECREF_INPUTS();
            }
            else {
                Py_INCREF(exc);
                _PyErr_SetRaisedException(tstate, exc);
                monitor_reraise(tstate, frame, next_instr-1);
                goto exception_unwind;
            }
        }

        inst(CLEANUP_THROW, (sub_iter, last_sent_val, exc_value -- none, value)) {
            assert(throwflag);
            assert(exc_value && PyExceptionInstance_Check(exc_value));
            if (PyErr_GivenExceptionMatches(exc_value, PyExc_StopIteration)) {
                value = Py_NewRef(((PyStopIterationObject *)exc_value)->value);
                DECREF_INPUTS();
                none = Py_None;
            }
            else {
                _PyErr_SetRaisedException(tstate, Py_NewRef(exc_value));
                monitor_reraise(tstate, frame, next_instr-1);
                goto exception_unwind;
            }
        }

        inst(LOAD_ASSERTION_ERROR, ( -- value)) {
            value = Py_NewRef(PyExc_AssertionError);
        }

        inst(LOAD_BUILD_CLASS, ( -- bc)) {
            ERROR_IF(PyMapping_GetOptionalItem(BUILTINS(), &_Py_ID(__build_class__), &bc) < 0, error);
            if (bc == NULL) {
                _PyErr_SetString(tstate, PyExc_NameError,
                                 "__build_class__ not found");
                ERROR_IF(true, error);
            }
        }


        inst(STORE_NAME, (v -- )) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            PyObject *ns = LOCALS();
            int err;
            if (ns == NULL) {
                _PyErr_Format(tstate, PyExc_SystemError,
                              "no locals found when storing %R", name);
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }
            if (PyDict_CheckExact(ns))
                err = PyDict_SetItem(ns, name, v);
            else
                err = PyObject_SetItem(ns, name, v);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(DELETE_NAME, (--)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            PyObject *ns = LOCALS();
            int err;
            if (ns == NULL) {
                _PyErr_Format(tstate, PyExc_SystemError,
                              "no locals when deleting %R", name);
                goto error;
            }
            err = PyObject_DelItem(ns, name);
            // Can't use ERROR_IF here.
            if (err != 0) {
                _PyEval_FormatExcCheckArg(tstate, PyExc_NameError,
                                          NAME_ERROR_MSG,
                                          name);
                goto error;
            }
        }

        family(UNPACK_SEQUENCE, INLINE_CACHE_ENTRIES_UNPACK_SEQUENCE) = {
            UNPACK_SEQUENCE_TWO_TUPLE,
            UNPACK_SEQUENCE_TUPLE,
            UNPACK_SEQUENCE_LIST,
        };

        inst(UNPACK_SEQUENCE, (unused/1, seq -- unused[oparg])) {
            #if ENABLE_SPECIALIZATION
            _PyUnpackSequenceCache *cache = (_PyUnpackSequenceCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_UnpackSequence(seq, next_instr, oparg);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(UNPACK_SEQUENCE, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            PyObject **top = stack_pointer + oparg - 1;
            int res = _PyEval_UnpackIterable(tstate, seq, oparg, -1, top);
            DECREF_INPUTS();
            ERROR_IF(res == 0, error);
        }

        inst(UNPACK_SEQUENCE_TWO_TUPLE, (unused/1, seq -- values[oparg])) {
            DEOPT_IF(!PyTuple_CheckExact(seq), UNPACK_SEQUENCE);
            DEOPT_IF(PyTuple_GET_SIZE(seq) != 2, UNPACK_SEQUENCE);
            assert(oparg == 2);
            STAT_INC(UNPACK_SEQUENCE, hit);
            values[0] = Py_NewRef(PyTuple_GET_ITEM(seq, 1));
            values[1] = Py_NewRef(PyTuple_GET_ITEM(seq, 0));
            DECREF_INPUTS();
        }

        inst(UNPACK_SEQUENCE_TUPLE, (unused/1, seq -- values[oparg])) {
            DEOPT_IF(!PyTuple_CheckExact(seq), UNPACK_SEQUENCE);
            DEOPT_IF(PyTuple_GET_SIZE(seq) != oparg, UNPACK_SEQUENCE);
            STAT_INC(UNPACK_SEQUENCE, hit);
            PyObject **items = _PyTuple_ITEMS(seq);
            for (int i = oparg; --i >= 0; ) {
                *values++ = Py_NewRef(items[i]);
            }
            DECREF_INPUTS();
        }

        inst(UNPACK_SEQUENCE_LIST, (unused/1, seq -- values[oparg])) {
            DEOPT_IF(!PyList_CheckExact(seq), UNPACK_SEQUENCE);
            DEOPT_IF(PyList_GET_SIZE(seq) != oparg, UNPACK_SEQUENCE);
            STAT_INC(UNPACK_SEQUENCE, hit);
            PyObject **items = _PyList_ITEMS(seq);
            for (int i = oparg; --i >= 0; ) {
                *values++ = Py_NewRef(items[i]);
            }
            DECREF_INPUTS();
        }

        inst(UNPACK_EX, (seq -- unused[oparg & 0xFF], unused, unused[oparg >> 8])) {
            int totalargs = 1 + (oparg & 0xFF) + (oparg >> 8);
            PyObject **top = stack_pointer + totalargs - 1;
            int res = _PyEval_UnpackIterable(tstate, seq, oparg & 0xFF, oparg >> 8, top);
            DECREF_INPUTS();
            ERROR_IF(res == 0, error);
        }

        family(STORE_ATTR, INLINE_CACHE_ENTRIES_STORE_ATTR) = {
            STORE_ATTR_INSTANCE_VALUE,
            STORE_ATTR_SLOT,
            STORE_ATTR_WITH_HINT,
        };

        inst(STORE_ATTR, (unused/1, unused/3, v, owner --)) {
            #if ENABLE_SPECIALIZATION
            _PyAttrCache *cache = (_PyAttrCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
                next_instr--;
                _Py_Specialize_StoreAttr(owner, next_instr, name);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(STORE_ATTR, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            int err = PyObject_SetAttr(owner, name, v);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(DELETE_ATTR, (owner --)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            int err = PyObject_DelAttr(owner, name);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(STORE_GLOBAL, (v --)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            int err = PyDict_SetItem(GLOBALS(), name, v);
            DECREF_INPUTS();
            ERROR_IF(err, error);
        }

        inst(DELETE_GLOBAL, (--)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            int err;
            err = PyDict_DelItem(GLOBALS(), name);
            // Can't use ERROR_IF here.
            if (err != 0) {
                if (_PyErr_ExceptionMatches(tstate, PyExc_KeyError)) {
                    _PyEval_FormatExcCheckArg(tstate, PyExc_NameError,
                                              NAME_ERROR_MSG, name);
                }
                goto error;
            }
        }

        op(_LOAD_LOCALS, ( -- locals)) {
            locals = LOCALS();
            if (locals == NULL) {
                _PyErr_SetString(tstate, PyExc_SystemError,
                                 "no locals found");
                ERROR_IF(true, error);
            }
            Py_INCREF(locals);
        }

        macro(LOAD_LOCALS) = _LOAD_LOCALS;

        op(_LOAD_FROM_DICT_OR_GLOBALS, (mod_or_class_dict -- v)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            if (PyMapping_GetOptionalItem(mod_or_class_dict, name, &v) < 0) {
                Py_DECREF(mod_or_class_dict);
                goto error;
            }
            Py_DECREF(mod_or_class_dict);
            if (v == NULL) {
                v = PyDict_GetItemWithError(GLOBALS(), name);
                if (v != NULL) {
                    Py_INCREF(v);
                }
                else if (_PyErr_Occurred(tstate)) {
                    goto error;
                }
                else {
                    if (PyMapping_GetOptionalItem(BUILTINS(), name, &v) < 0) {
                        goto error;
                    }
                    if (v == NULL) {
                        _PyEval_FormatExcCheckArg(
                                    tstate, PyExc_NameError,
                                    NAME_ERROR_MSG, name);
                        goto error;
                    }
                }
            }
        }

        macro(LOAD_NAME) = _LOAD_LOCALS + _LOAD_FROM_DICT_OR_GLOBALS;

        macro(LOAD_FROM_DICT_OR_GLOBALS) = _LOAD_FROM_DICT_OR_GLOBALS;

        family(LOAD_GLOBAL, INLINE_CACHE_ENTRIES_LOAD_GLOBAL) = {
            LOAD_GLOBAL_MODULE,
            LOAD_GLOBAL_BUILTIN,
        };

        inst(LOAD_GLOBAL, (unused/1, unused/1, unused/1, unused/1 -- res, null if (oparg & 1))) {
            #if ENABLE_SPECIALIZATION
            _PyLoadGlobalCache *cache = (_PyLoadGlobalCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                PyObject *name = GETITEM(FRAME_CO_NAMES, oparg>>1);
                next_instr--;
                _Py_Specialize_LoadGlobal(GLOBALS(), BUILTINS(), next_instr, name);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(LOAD_GLOBAL, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg>>1);
            if (PyDict_CheckExact(GLOBALS())
                && PyDict_CheckExact(BUILTINS()))
            {
                res = _PyDict_LoadGlobal((PyDictObject *)GLOBALS(),
                                         (PyDictObject *)BUILTINS(),
                                         name);
                if (res == NULL) {
                    if (!_PyErr_Occurred(tstate)) {
                        /* _PyDict_LoadGlobal() returns NULL without raising
                         * an exception if the key doesn't exist */
                        _PyEval_FormatExcCheckArg(tstate, PyExc_NameError,
                                                  NAME_ERROR_MSG, name);
                    }
                    ERROR_IF(true, error);
                }
                Py_INCREF(res);
            }
            else {
                /* Slow-path if globals or builtins is not a dict */

                /* namespace 1: globals */
                ERROR_IF(PyMapping_GetOptionalItem(GLOBALS(), name, &res) < 0, error);
                if (res == NULL) {
                    /* namespace 2: builtins */
                    ERROR_IF(PyMapping_GetOptionalItem(BUILTINS(), name, &res) < 0, error);
                    if (res == NULL) {
                        _PyEval_FormatExcCheckArg(
                                    tstate, PyExc_NameError,
                                    NAME_ERROR_MSG, name);
                        ERROR_IF(true, error);
                    }
                }
            }
            null = NULL;
        }

        op(_GUARD_GLOBALS_VERSION, (version/1 --)) {
            PyDictObject *dict = (PyDictObject *)GLOBALS();
            DEOPT_IF(!PyDict_CheckExact(dict), LOAD_GLOBAL);
            DEOPT_IF(dict->ma_keys->dk_version != version, LOAD_GLOBAL);
            assert(DK_IS_UNICODE(dict->ma_keys));
        }

        op(_GUARD_BUILTINS_VERSION, (version/1 --)) {
            PyDictObject *dict = (PyDictObject *)BUILTINS();
            DEOPT_IF(!PyDict_CheckExact(dict), LOAD_GLOBAL);
            DEOPT_IF(dict->ma_keys->dk_version != version, LOAD_GLOBAL);
            assert(DK_IS_UNICODE(dict->ma_keys));
        }

        op(_LOAD_GLOBAL_MODULE, (index/1 -- res, null if (oparg & 1))) {
            PyDictObject *dict = (PyDictObject *)GLOBALS();
            PyDictUnicodeEntry *entries = DK_UNICODE_ENTRIES(dict->ma_keys);
            res = entries[index].me_value;
            DEOPT_IF(res == NULL, LOAD_GLOBAL);
            Py_INCREF(res);
            STAT_INC(LOAD_GLOBAL, hit);
            null = NULL;
        }

        op(_LOAD_GLOBAL_BUILTINS, (index/1 -- res, null if (oparg & 1))) {
            PyDictObject *bdict = (PyDictObject *)BUILTINS();
            PyDictUnicodeEntry *entries = DK_UNICODE_ENTRIES(bdict->ma_keys);
            res = entries[index].me_value;
            DEOPT_IF(res == NULL, LOAD_GLOBAL);
            Py_INCREF(res);
            STAT_INC(LOAD_GLOBAL, hit);
            null = NULL;
        }

        macro(LOAD_GLOBAL_MODULE) =
            unused/1 + // Skip over the counter
            _GUARD_GLOBALS_VERSION +
            unused/1 + // Skip over the builtins version
            _LOAD_GLOBAL_MODULE;

        macro(LOAD_GLOBAL_BUILTIN) =
            unused/1 + // Skip over the counter
            _GUARD_GLOBALS_VERSION +
            _GUARD_BUILTINS_VERSION +
            _LOAD_GLOBAL_BUILTINS;

        inst(DELETE_FAST, (--)) {
            PyObject *v = GETLOCAL(oparg);
            ERROR_IF(v == NULL, unbound_local_error);
            SETLOCAL(oparg, NULL);
        }

        inst(MAKE_CELL, (--)) {
            // "initial" is probably NULL but not if it's an arg (or set
            // via PyFrame_LocalsToFast() before MAKE_CELL has run).
            PyObject *initial = GETLOCAL(oparg);
            PyObject *cell = PyCell_New(initial);
            if (cell == NULL) {
                goto resume_with_error;
            }
            SETLOCAL(oparg, cell);
        }

        inst(DELETE_DEREF, (--)) {
            PyObject *cell = GETLOCAL(oparg);
            PyObject *oldobj = PyCell_GET(cell);
            // Can't use ERROR_IF here.
            // Fortunately we don't need its superpower.
            if (oldobj == NULL) {
                _PyEval_FormatExcUnbound(tstate, _PyFrame_GetCode(frame), oparg);
                goto error;
            }
            PyCell_SET(cell, NULL);
            Py_DECREF(oldobj);
        }

        inst(LOAD_FROM_DICT_OR_DEREF, (class_dict -- value)) {
            PyObject *name;
            assert(class_dict);
            assert(oparg >= 0 && oparg < _PyFrame_GetCode(frame)->co_nlocalsplus);
            name = PyTuple_GET_ITEM(_PyFrame_GetCode(frame)->co_localsplusnames, oparg);
            if (PyMapping_GetOptionalItem(class_dict, name, &value) < 0) {
                Py_DECREF(class_dict);
                goto error;
            }
            Py_DECREF(class_dict);
            if (!value) {
                PyObject *cell = GETLOCAL(oparg);
                value = PyCell_GET(cell);
                if (value == NULL) {
                    _PyEval_FormatExcUnbound(tstate, _PyFrame_GetCode(frame), oparg);
                    goto error;
                }
                Py_INCREF(value);
            }
        }

        inst(LOAD_DEREF, ( -- value)) {
            PyObject *cell = GETLOCAL(oparg);
            value = PyCell_GET(cell);
            if (value == NULL) {
                _PyEval_FormatExcUnbound(tstate, _PyFrame_GetCode(frame), oparg);
                ERROR_IF(true, error);
            }
            Py_INCREF(value);
        }

        inst(STORE_DEREF, (v --)) {
            PyObject *cell = GETLOCAL(oparg);
            PyObject *oldobj = PyCell_GET(cell);
            PyCell_SET(cell, v);
            Py_XDECREF(oldobj);
        }

        inst(COPY_FREE_VARS, (--)) {
            /* Copy closure variables to free variables */
            PyCodeObject *co = _PyFrame_GetCode(frame);
            assert(PyFunction_Check(frame->f_funcobj));
            PyObject *closure = ((PyFunctionObject *)frame->f_funcobj)->func_closure;
            assert(oparg == co->co_nfreevars);
            int offset = co->co_nlocalsplus - oparg;
            for (int i = 0; i < oparg; ++i) {
                PyObject *o = PyTuple_GET_ITEM(closure, i);
                frame->localsplus[offset + i] = Py_NewRef(o);
            }
        }

        inst(BUILD_STRING, (pieces[oparg] -- str)) {
            str = _PyUnicode_JoinArray(&_Py_STR(empty), pieces, oparg);
            DECREF_INPUTS();
            ERROR_IF(str == NULL, error);
        }

        inst(BUILD_TUPLE, (values[oparg] -- tup)) {
            tup = _PyTuple_FromArraySteal(values, oparg);
            ERROR_IF(tup == NULL, error);
        }

        inst(BUILD_LIST, (values[oparg] -- list)) {
            list = _PyList_FromArraySteal(values, oparg);
            ERROR_IF(list == NULL, error);
        }

        inst(LIST_EXTEND, (list, unused[oparg-1], iterable -- list, unused[oparg-1])) {
            PyObject *none_val = _PyList_Extend((PyListObject *)list, iterable);
            if (none_val == NULL) {
                if (_PyErr_ExceptionMatches(tstate, PyExc_TypeError) &&
                   (Py_TYPE(iterable)->tp_iter == NULL && !PySequence_Check(iterable)))
                {
                    _PyErr_Clear(tstate);
                    _PyErr_Format(tstate, PyExc_TypeError,
                          "Value after * must be an iterable, not %.200s",
                          Py_TYPE(iterable)->tp_name);
                }
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }
            assert(Py_IsNone(none_val));
            DECREF_INPUTS();
        }

        inst(SET_UPDATE, (set, unused[oparg-1], iterable -- set, unused[oparg-1])) {
            int err = _PySet_Update(set, iterable);
            DECREF_INPUTS();
            ERROR_IF(err < 0, error);
        }

        inst(BUILD_SET, (values[oparg] -- set)) {
            set = PySet_New(NULL);
            if (set == NULL)
                goto error;
            int err = 0;
            for (int i = 0; i < oparg; i++) {
                PyObject *item = values[i];
                if (err == 0)
                    err = PySet_Add(set, item);
                Py_DECREF(item);
            }
            if (err != 0) {
                Py_DECREF(set);
                ERROR_IF(true, error);
            }
        }

        inst(BUILD_MAP, (values[oparg*2] -- map)) {
            map = _PyDict_FromItems(
                    values, 2,
                    values+1, 2,
                    oparg);
            if (map == NULL)
                goto error;

            DECREF_INPUTS();
            ERROR_IF(map == NULL, error);
        }

        inst(SETUP_ANNOTATIONS, (--)) {
            int err;
            PyObject *ann_dict;
            if (LOCALS() == NULL) {
                _PyErr_Format(tstate, PyExc_SystemError,
                              "no locals found when setting up annotations");
                ERROR_IF(true, error);
            }
            /* check if __annotations__ in locals()... */
            if (PyDict_CheckExact(LOCALS())) {
                ann_dict = _PyDict_GetItemWithError(LOCALS(),
                                                    &_Py_ID(__annotations__));
                if (ann_dict == NULL) {
                    ERROR_IF(_PyErr_Occurred(tstate), error);
                    /* ...if not, create a new one */
                    ann_dict = PyDict_New();
                    ERROR_IF(ann_dict == NULL, error);
                    err = PyDict_SetItem(LOCALS(), &_Py_ID(__annotations__),
                                         ann_dict);
                    Py_DECREF(ann_dict);
                    ERROR_IF(err, error);
                }
            }
            else {
                /* do the same if locals() is not a dict */
                ERROR_IF(PyMapping_GetOptionalItem(LOCALS(), &_Py_ID(__annotations__), &ann_dict) < 0, error);
                if (ann_dict == NULL) {
                    ann_dict = PyDict_New();
                    ERROR_IF(ann_dict == NULL, error);
                    err = PyObject_SetItem(LOCALS(), &_Py_ID(__annotations__),
                                           ann_dict);
                    Py_DECREF(ann_dict);
                    ERROR_IF(err, error);
                }
                else {
                    Py_DECREF(ann_dict);
                }
            }
        }

        inst(BUILD_CONST_KEY_MAP, (values[oparg], keys -- map)) {
            if (!PyTuple_CheckExact(keys) ||
                PyTuple_GET_SIZE(keys) != (Py_ssize_t)oparg) {
                _PyErr_SetString(tstate, PyExc_SystemError,
                                 "bad BUILD_CONST_KEY_MAP keys argument");
                goto error;  // Pop the keys and values.
            }
            map = _PyDict_FromItems(
                    &PyTuple_GET_ITEM(keys, 0), 1,
                    values, 1, oparg);
            DECREF_INPUTS();
            ERROR_IF(map == NULL, error);
        }

        inst(DICT_UPDATE, (dict, unused[oparg - 1], update -- dict, unused[oparg - 1])) {
            if (PyDict_Update(dict, update) < 0) {
                if (_PyErr_ExceptionMatches(tstate, PyExc_AttributeError)) {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                    "'%.200s' object is not a mapping",
                                    Py_TYPE(update)->tp_name);
                }
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }
            DECREF_INPUTS();
        }

        inst(DICT_MERGE, (callable, unused, unused, dict, unused[oparg - 1], update -- callable, unused, unused, dict, unused[oparg - 1])) {
            if (_PyDict_MergeEx(dict, update, 2) < 0) {
                _PyEval_FormatKwargsError(tstate, callable, update);
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }
            DECREF_INPUTS();
        }

        inst(MAP_ADD, (dict, unused[oparg - 1], key, value -- dict, unused[oparg - 1])) {
            assert(PyDict_CheckExact(dict));
            /* dict[key] = value */
            // Do not DECREF INPUTS because the function steals the references
            ERROR_IF(_PyDict_SetItem_Take2((PyDictObject *)dict, key, value) != 0, error);
        }

        inst(INSTRUMENTED_LOAD_SUPER_ATTR, (unused/9, unused, unused, unused -- unused, unused if (oparg & 1))) {
            _PySuperAttrCache *cache = (_PySuperAttrCache *)next_instr;
            // cancel out the decrement that will happen in LOAD_SUPER_ATTR; we
            // don't want to specialize instrumented instructions
            INCREMENT_ADAPTIVE_COUNTER(cache->counter);
            GO_TO_INSTRUCTION(LOAD_SUPER_ATTR);
        }

        family(LOAD_SUPER_ATTR, INLINE_CACHE_ENTRIES_LOAD_SUPER_ATTR) = {
            LOAD_SUPER_ATTR_ATTR,
            LOAD_SUPER_ATTR_METHOD,
        };

        inst(LOAD_SUPER_ATTR, (unused/1, global_super, class, self -- attr, null if (oparg & 1))) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg >> 2);
            int load_method = oparg & 1;
            #if ENABLE_SPECIALIZATION
            _PySuperAttrCache *cache = (_PySuperAttrCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_LoadSuperAttr(global_super, class, next_instr, load_method);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(LOAD_SUPER_ATTR, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */

            if (opcode == INSTRUMENTED_LOAD_SUPER_ATTR) {
                PyObject *arg = oparg & 2 ? class : &_PyInstrumentation_MISSING;
                int err = _Py_call_instrumentation_2args(
                        tstate, PY_MONITORING_EVENT_CALL,
                        frame, next_instr-1, global_super, arg);
                ERROR_IF(err, error);
            }

            // we make no attempt to optimize here; specializations should
            // handle any case whose performance we care about
            PyObject *stack[] = {class, self};
            PyObject *super = PyObject_Vectorcall(global_super, stack, oparg & 2, NULL);
            if (opcode == INSTRUMENTED_LOAD_SUPER_ATTR) {
                PyObject *arg = oparg & 2 ? class : &_PyInstrumentation_MISSING;
                if (super == NULL) {
                    _Py_call_instrumentation_exc2(
                        tstate, PY_MONITORING_EVENT_C_RAISE,
                        frame, next_instr-1, global_super, arg);
                }
                else {
                    int err = _Py_call_instrumentation_2args(
                        tstate, PY_MONITORING_EVENT_C_RETURN,
                        frame, next_instr-1, global_super, arg);
                    if (err < 0) {
                        Py_CLEAR(super);
                    }
                }
            }
            DECREF_INPUTS();
            ERROR_IF(super == NULL, error);
            attr = PyObject_GetAttr(super, name);
            Py_DECREF(super);
            ERROR_IF(attr == NULL, error);
            null = NULL;
        }

        pseudo(LOAD_SUPER_METHOD) = {
            LOAD_SUPER_ATTR,
        };

        pseudo(LOAD_ZERO_SUPER_METHOD) = {
            LOAD_SUPER_ATTR,
        };

        pseudo(LOAD_ZERO_SUPER_ATTR) = {
            LOAD_SUPER_ATTR,
        };

        inst(LOAD_SUPER_ATTR_ATTR, (unused/1, global_super, class, self -- attr, unused if (0))) {
            assert(!(oparg & 1));
            DEOPT_IF(global_super != (PyObject *)&PySuper_Type, LOAD_SUPER_ATTR);
            DEOPT_IF(!PyType_Check(class), LOAD_SUPER_ATTR);
            STAT_INC(LOAD_SUPER_ATTR, hit);
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg >> 2);
            attr = _PySuper_Lookup((PyTypeObject *)class, self, name, NULL);
            DECREF_INPUTS();
            ERROR_IF(attr == NULL, error);
        }

        inst(LOAD_SUPER_ATTR_METHOD, (unused/1, global_super, class, self -- attr, self_or_null)) {
            assert(oparg & 1);
            DEOPT_IF(global_super != (PyObject *)&PySuper_Type, LOAD_SUPER_ATTR);
            DEOPT_IF(!PyType_Check(class), LOAD_SUPER_ATTR);
            STAT_INC(LOAD_SUPER_ATTR, hit);
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg >> 2);
            PyTypeObject *cls = (PyTypeObject *)class;
            int method_found = 0;
            attr = _PySuper_Lookup(cls, self, name,
                                   Py_TYPE(self)->tp_getattro == PyObject_GenericGetAttr ? &method_found : NULL);
            Py_DECREF(global_super);
            Py_DECREF(class);
            if (attr == NULL) {
                Py_DECREF(self);
                ERROR_IF(true, error);
            }
            if (method_found) {
                self_or_null = self; // transfer ownership
            } else {
                Py_DECREF(self);
                self_or_null = NULL;
            }
        }

        family(LOAD_ATTR, INLINE_CACHE_ENTRIES_LOAD_ATTR) = {
            LOAD_ATTR_INSTANCE_VALUE,
            LOAD_ATTR_MODULE,
            LOAD_ATTR_WITH_HINT,
            LOAD_ATTR_SLOT,
            LOAD_ATTR_CLASS,
            LOAD_ATTR_PROPERTY,
            LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN,
            LOAD_ATTR_METHOD_WITH_VALUES,
            LOAD_ATTR_METHOD_NO_DICT,
            LOAD_ATTR_METHOD_LAZY_DICT,
            LOAD_ATTR_NONDESCRIPTOR_WITH_VALUES,
            LOAD_ATTR_NONDESCRIPTOR_NO_DICT,
        };

        inst(LOAD_ATTR, (unused/9, owner -- attr, self_or_null if (oparg & 1))) {
            #if ENABLE_SPECIALIZATION
            _PyAttrCache *cache = (_PyAttrCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                PyObject *name = GETITEM(FRAME_CO_NAMES, oparg>>1);
                next_instr--;
                _Py_Specialize_LoadAttr(owner, next_instr, name);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(LOAD_ATTR, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg >> 1);
            if (oparg & 1) {
                /* Designed to work in tandem with CALL, pushes two values. */
                attr = NULL;
                if (_PyObject_GetMethod(owner, name, &attr)) {
                    /* We can bypass temporary bound method object.
                       meth is unbound method and obj is self.

                       meth | self | arg1 | ... | argN
                     */
                    assert(attr != NULL);  // No errors on this branch
                    self_or_null = owner;  // Transfer ownership
                }
                else {
                    /* meth is not an unbound method (but a regular attr, or
                       something was returned by a descriptor protocol).  Set
                       the second element of the stack to NULL, to signal
                       CALL that it's not a method call.

                       NULL | meth | arg1 | ... | argN
                    */
                    DECREF_INPUTS();
                    ERROR_IF(attr == NULL, error);
                    self_or_null = NULL;
                }
            }
            else {
                /* Classic, pushes one value. */
                attr = PyObject_GetAttr(owner, name);
                DECREF_INPUTS();
                ERROR_IF(attr == NULL, error);
            }
        }

        pseudo(LOAD_METHOD) = {
            LOAD_ATTR,
        };

        op(_GUARD_TYPE_VERSION, (type_version/2, owner -- owner)) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, LOAD_ATTR);
        }

        op(_CHECK_MANAGED_OBJECT_HAS_VALUES, (owner -- owner)) {
            assert(Py_TYPE(owner)->tp_dictoffset < 0);
            assert(Py_TYPE(owner)->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues *dorv = _PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(!_PyDictOrValues_IsValues(*dorv) &&
                     !_PyObject_MakeInstanceAttributesFromDict(owner, dorv),
                     LOAD_ATTR);
        }

        op(_LOAD_ATTR_INSTANCE_VALUE, (index/1, owner -- attr, null if (oparg & 1))) {
            PyDictOrValues dorv = *_PyObject_DictOrValuesPointer(owner);
            attr = _PyDictOrValues_GetValues(dorv)->values[index];
            DEOPT_IF(attr == NULL, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            Py_INCREF(attr);
            null = NULL;
            DECREF_INPUTS();
        }

        macro(LOAD_ATTR_INSTANCE_VALUE) =
            unused/1 + // Skip over the counter
            _GUARD_TYPE_VERSION +
            _CHECK_MANAGED_OBJECT_HAS_VALUES +
            _LOAD_ATTR_INSTANCE_VALUE +
            unused/5;  // Skip over rest of cache

        inst(LOAD_ATTR_MODULE, (unused/1, type_version/2, index/1, unused/5, owner -- attr, null if (oparg & 1))) {
            DEOPT_IF(!PyModule_CheckExact(owner), LOAD_ATTR);
            PyDictObject *dict = (PyDictObject *)((PyModuleObject *)owner)->md_dict;
            assert(dict != NULL);
            DEOPT_IF(dict->ma_keys->dk_version != type_version, LOAD_ATTR);
            assert(dict->ma_keys->dk_kind == DICT_KEYS_UNICODE);
            assert(index < dict->ma_keys->dk_nentries);
            PyDictUnicodeEntry *ep = DK_UNICODE_ENTRIES(dict->ma_keys) + index;
            attr = ep->me_value;
            DEOPT_IF(attr == NULL, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            Py_INCREF(attr);
            null = NULL;
            DECREF_INPUTS();
        }

        inst(LOAD_ATTR_WITH_HINT, (unused/1, type_version/2, index/1, unused/5, owner -- attr, null if (oparg & 1))) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, LOAD_ATTR);
            assert(tp->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues dorv = *_PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(_PyDictOrValues_IsValues(dorv), LOAD_ATTR);
            PyDictObject *dict = (PyDictObject *)_PyDictOrValues_GetDict(dorv);
            DEOPT_IF(dict == NULL, LOAD_ATTR);
            assert(PyDict_CheckExact((PyObject *)dict));
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg>>1);
            uint16_t hint = index;
            DEOPT_IF(hint >= (size_t)dict->ma_keys->dk_nentries, LOAD_ATTR);
            if (DK_IS_UNICODE(dict->ma_keys)) {
                PyDictUnicodeEntry *ep = DK_UNICODE_ENTRIES(dict->ma_keys) + hint;
                DEOPT_IF(ep->me_key != name, LOAD_ATTR);
                attr = ep->me_value;
            }
            else {
                PyDictKeyEntry *ep = DK_ENTRIES(dict->ma_keys) + hint;
                DEOPT_IF(ep->me_key != name, LOAD_ATTR);
                attr = ep->me_value;
            }
            DEOPT_IF(attr == NULL, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            Py_INCREF(attr);
            null = NULL;
            DECREF_INPUTS();
        }

        inst(LOAD_ATTR_SLOT, (unused/1, type_version/2, index/1, unused/5, owner -- attr, null if (oparg & 1))) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, LOAD_ATTR);
            char *addr = (char *)owner + index;
            attr = *(PyObject **)addr;
            DEOPT_IF(attr == NULL, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            Py_INCREF(attr);
            null = NULL;
            DECREF_INPUTS();
        }

        inst(LOAD_ATTR_CLASS, (unused/1, type_version/2, unused/2, descr/4, owner -- attr, null if (oparg & 1))) {

            DEOPT_IF(!PyType_Check(owner), LOAD_ATTR);
            DEOPT_IF(((PyTypeObject *)owner)->tp_version_tag != type_version,
                LOAD_ATTR);
            assert(type_version != 0);

            STAT_INC(LOAD_ATTR, hit);
            null = NULL;
            attr = descr;
            assert(attr != NULL);
            Py_INCREF(attr);
            DECREF_INPUTS();
        }

        inst(LOAD_ATTR_PROPERTY, (unused/1, type_version/2, func_version/2, fget/4, owner -- unused, unused if (0))) {
            assert((oparg & 1) == 0);
            DEOPT_IF(tstate->interp->eval_frame, LOAD_ATTR);

            PyTypeObject *cls = Py_TYPE(owner);
            DEOPT_IF(cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(type_version != 0);
            assert(Py_IS_TYPE(fget, &PyFunction_Type));
            PyFunctionObject *f = (PyFunctionObject *)fget;
            assert(func_version != 0);
            DEOPT_IF(f->func_version != func_version, LOAD_ATTR);
            PyCodeObject *code = (PyCodeObject *)f->func_code;
            assert(code->co_argcount == 1);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize), LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            Py_INCREF(fget);
            _PyInterpreterFrame *new_frame = _PyFrame_PushUnchecked(tstate, f, 1);
            // Manipulate stack directly because we exit with DISPATCH_INLINED().
            STACK_SHRINK(1);
            new_frame->localsplus[0] = owner;
            SKIP_OVER(INLINE_CACHE_ENTRIES_LOAD_ATTR);
            frame->return_offset = 0;
            DISPATCH_INLINED(new_frame);
        }

        inst(LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN, (unused/1, type_version/2, func_version/2, getattribute/4, owner -- unused, unused if (0))) {
            assert((oparg & 1) == 0);
            DEOPT_IF(tstate->interp->eval_frame, LOAD_ATTR);
            PyTypeObject *cls = Py_TYPE(owner);
            DEOPT_IF(cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(type_version != 0);
            assert(Py_IS_TYPE(getattribute, &PyFunction_Type));
            PyFunctionObject *f = (PyFunctionObject *)getattribute;
            assert(func_version != 0);
            DEOPT_IF(f->func_version != func_version, LOAD_ATTR);
            PyCodeObject *code = (PyCodeObject *)f->func_code;
            assert(code->co_argcount == 2);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize), LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);

            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg >> 1);
            Py_INCREF(f);
            _PyInterpreterFrame *new_frame = _PyFrame_PushUnchecked(tstate, f, 2);
            // Manipulate stack directly because we exit with DISPATCH_INLINED().
            STACK_SHRINK(1);
            new_frame->localsplus[0] = owner;
            new_frame->localsplus[1] = Py_NewRef(name);
            SKIP_OVER(INLINE_CACHE_ENTRIES_LOAD_ATTR);
            frame->return_offset = 0;
            DISPATCH_INLINED(new_frame);
        }

        inst(STORE_ATTR_INSTANCE_VALUE, (unused/1, type_version/2, index/1, value, owner --)) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, STORE_ATTR);
            assert(tp->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues dorv = *_PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(!_PyDictOrValues_IsValues(dorv), STORE_ATTR);
            STAT_INC(STORE_ATTR, hit);
            PyDictValues *values = _PyDictOrValues_GetValues(dorv);
            PyObject *old_value = values->values[index];
            values->values[index] = value;
            if (old_value == NULL) {
                _PyDictValues_AddToInsertionOrder(values, index);
            }
            else {
                Py_DECREF(old_value);
            }
            Py_DECREF(owner);
        }

        inst(STORE_ATTR_WITH_HINT, (unused/1, type_version/2, hint/1, value, owner --)) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, STORE_ATTR);
            assert(tp->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues dorv = *_PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(_PyDictOrValues_IsValues(dorv), STORE_ATTR);
            PyDictObject *dict = (PyDictObject *)_PyDictOrValues_GetDict(dorv);
            DEOPT_IF(dict == NULL, STORE_ATTR);
            assert(PyDict_CheckExact((PyObject *)dict));
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            DEOPT_IF(hint >= (size_t)dict->ma_keys->dk_nentries, STORE_ATTR);
            PyObject *old_value;
            uint64_t new_version;
            if (DK_IS_UNICODE(dict->ma_keys)) {
                PyDictUnicodeEntry *ep = DK_UNICODE_ENTRIES(dict->ma_keys) + hint;
                DEOPT_IF(ep->me_key != name, STORE_ATTR);
                old_value = ep->me_value;
                DEOPT_IF(old_value == NULL, STORE_ATTR);
                new_version = _PyDict_NotifyEvent(tstate->interp, PyDict_EVENT_MODIFIED, dict, name, value);
                ep->me_value = value;
            }
            else {
                PyDictKeyEntry *ep = DK_ENTRIES(dict->ma_keys) + hint;
                DEOPT_IF(ep->me_key != name, STORE_ATTR);
                old_value = ep->me_value;
                DEOPT_IF(old_value == NULL, STORE_ATTR);
                new_version = _PyDict_NotifyEvent(tstate->interp, PyDict_EVENT_MODIFIED, dict, name, value);
                ep->me_value = value;
            }
            Py_DECREF(old_value);
            STAT_INC(STORE_ATTR, hit);
            /* Ensure dict is GC tracked if it needs to be */
            if (!_PyObject_GC_IS_TRACKED(dict) && _PyObject_GC_MAY_BE_TRACKED(value)) {
                _PyObject_GC_TRACK(dict);
            }
            /* PEP 509 */
            dict->ma_version_tag = new_version;
            Py_DECREF(owner);
        }

        inst(STORE_ATTR_SLOT, (unused/1, type_version/2, index/1, value, owner --)) {
            PyTypeObject *tp = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(tp->tp_version_tag != type_version, STORE_ATTR);
            char *addr = (char *)owner + index;
            STAT_INC(STORE_ATTR, hit);
            PyObject *old_value = *(PyObject **)addr;
            *(PyObject **)addr = value;
            Py_XDECREF(old_value);
            Py_DECREF(owner);
        }

        family(COMPARE_OP, INLINE_CACHE_ENTRIES_COMPARE_OP) = {
            COMPARE_OP_FLOAT,
            COMPARE_OP_INT,
            COMPARE_OP_STR,
        };

        inst(COMPARE_OP, (unused/1, left, right -- res)) {
            #if ENABLE_SPECIALIZATION
            _PyCompareOpCache *cache = (_PyCompareOpCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_CompareOp(left, right, next_instr, oparg);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(COMPARE_OP, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            assert((oparg >> 5) <= Py_GE);
            res = PyObject_RichCompare(left, right, oparg >> 5);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
            if (oparg & 16) {
                int res_bool = PyObject_IsTrue(res);
                Py_DECREF(res);
                ERROR_IF(res_bool < 0, error);
                res = res_bool ? Py_True : Py_False;
            }
        }

        inst(COMPARE_OP_FLOAT, (unused/1, left, right -- res)) {
            DEOPT_IF(!PyFloat_CheckExact(left), COMPARE_OP);
            DEOPT_IF(!PyFloat_CheckExact(right), COMPARE_OP);
            STAT_INC(COMPARE_OP, hit);
            double dleft = PyFloat_AS_DOUBLE(left);
            double dright = PyFloat_AS_DOUBLE(right);
            // 1 if NaN, 2 if <, 4 if >, 8 if ==; this matches low four bits of the oparg
            int sign_ish = COMPARISON_BIT(dleft, dright);
            _Py_DECREF_SPECIALIZED(left, _PyFloat_ExactDealloc);
            _Py_DECREF_SPECIALIZED(right, _PyFloat_ExactDealloc);
            res = (sign_ish & oparg) ? Py_True : Py_False;
            // It's always a bool, so we don't care about oparg & 16.
        }

        // Similar to COMPARE_OP_FLOAT
        inst(COMPARE_OP_INT, (unused/1, left, right -- res)) {
            DEOPT_IF(!PyLong_CheckExact(left), COMPARE_OP);
            DEOPT_IF(!PyLong_CheckExact(right), COMPARE_OP);
            DEOPT_IF(!_PyLong_IsCompact((PyLongObject *)left), COMPARE_OP);
            DEOPT_IF(!_PyLong_IsCompact((PyLongObject *)right), COMPARE_OP);
            STAT_INC(COMPARE_OP, hit);
            assert(_PyLong_DigitCount((PyLongObject *)left) <= 1 &&
                   _PyLong_DigitCount((PyLongObject *)right) <= 1);
            Py_ssize_t ileft = _PyLong_CompactValue((PyLongObject *)left);
            Py_ssize_t iright = _PyLong_CompactValue((PyLongObject *)right);
            // 2 if <, 4 if >, 8 if ==; this matches the low 4 bits of the oparg
            int sign_ish = COMPARISON_BIT(ileft, iright);
            _Py_DECREF_SPECIALIZED(left, (destructor)PyObject_Free);
            _Py_DECREF_SPECIALIZED(right, (destructor)PyObject_Free);
            res = (sign_ish & oparg) ? Py_True : Py_False;
            // It's always a bool, so we don't care about oparg & 16.
        }

        // Similar to COMPARE_OP_FLOAT, but for ==, != only
        inst(COMPARE_OP_STR, (unused/1, left, right -- res)) {
            DEOPT_IF(!PyUnicode_CheckExact(left), COMPARE_OP);
            DEOPT_IF(!PyUnicode_CheckExact(right), COMPARE_OP);
            STAT_INC(COMPARE_OP, hit);
            int eq = _PyUnicode_Equal(left, right);
            assert((oparg >> 5) == Py_EQ || (oparg >> 5) == Py_NE);
            _Py_DECREF_SPECIALIZED(left, _PyUnicode_ExactDealloc);
            _Py_DECREF_SPECIALIZED(right, _PyUnicode_ExactDealloc);
            assert(eq == 0 || eq == 1);
            assert((oparg & 0xf) == COMPARISON_NOT_EQUALS || (oparg & 0xf) == COMPARISON_EQUALS);
            assert(COMPARISON_NOT_EQUALS + 1 == COMPARISON_EQUALS);
            res = ((COMPARISON_NOT_EQUALS + eq) & oparg) ? Py_True : Py_False;
            // It's always a bool, so we don't care about oparg & 16.
        }

        inst(IS_OP, (left, right -- b)) {
            int res = Py_Is(left, right) ^ oparg;
            DECREF_INPUTS();
            b = res ? Py_True : Py_False;
        }

        inst(CONTAINS_OP, (left, right -- b)) {
            int res = PySequence_Contains(right, left);
            DECREF_INPUTS();
            ERROR_IF(res < 0, error);
            b = (res ^ oparg) ? Py_True : Py_False;
        }

        inst(CHECK_EG_MATCH, (exc_value, match_type -- rest, match)) {
            if (_PyEval_CheckExceptStarTypeValid(tstate, match_type) < 0) {
                DECREF_INPUTS();
                ERROR_IF(true, error);
            }

            match = NULL;
            rest = NULL;
            int res = _PyEval_ExceptionGroupMatch(exc_value, match_type,
                                                  &match, &rest);
            DECREF_INPUTS();
            ERROR_IF(res < 0, error);

            assert((match == NULL) == (rest == NULL));
            ERROR_IF(match == NULL, error);

            if (!Py_IsNone(match)) {
                PyErr_SetHandledException(match);
            }
        }

        inst(CHECK_EXC_MATCH, (left, right -- left, b)) {
            assert(PyExceptionInstance_Check(left));
            if (_PyEval_CheckExceptTypeValid(tstate, right) < 0) {
                 DECREF_INPUTS();
                 ERROR_IF(true, error);
            }

            int res = PyErr_GivenExceptionMatches(left, right);
            DECREF_INPUTS();
            b = res ? Py_True : Py_False;
        }

         inst(IMPORT_NAME, (level, fromlist -- res)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            res = import_name(tstate, frame, name, fromlist, level);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(IMPORT_FROM, (from -- from, res)) {
            PyObject *name = GETITEM(FRAME_CO_NAMES, oparg);
            res = import_from(tstate, from, name);
            ERROR_IF(res == NULL, error);
        }

        inst(JUMP_FORWARD, (--)) {
            JUMPBY(oparg);
        }

        inst(JUMP_BACKWARD, (--)) {
            CHECK_EVAL_BREAKER();
            _Py_CODEUNIT *here = next_instr - 1;
            assert(oparg <= INSTR_OFFSET());
            JUMPBY(1-oparg);
            #if ENABLE_SPECIALIZATION
            here[1].cache += (1 << OPTIMIZER_BITS_IN_COUNTER);
            if (here[1].cache > tstate->interp->optimizer_backedge_threshold &&
                // Double-check that the opcode isn't instrumented or something:
                here->op.code == JUMP_BACKWARD &&
                // _PyOptimizer_BackEdge is going to change frame->prev_instr,
                // which breaks line event calculations:
                next_instr->op.code != INSTRUMENTED_LINE
                )
            {
                OBJECT_STAT_INC(optimization_attempts);
                frame = _PyOptimizer_BackEdge(frame, here, next_instr, stack_pointer);
                if (frame == NULL) {
                    frame = cframe.current_frame;
                    goto resume_with_error;
                }
                assert(frame == cframe.current_frame);
                here[1].cache &= ((1 << OPTIMIZER_BITS_IN_COUNTER) -1);
                goto resume_frame;
            }
            #endif  /* ENABLE_SPECIALIZATION */
        }

        pseudo(JUMP) = {
            JUMP_FORWARD,
            JUMP_BACKWARD,
        };

        pseudo(JUMP_NO_INTERRUPT) = {
            JUMP_FORWARD,
            JUMP_BACKWARD_NO_INTERRUPT,
        };

        inst(ENTER_EXECUTOR, (--)) {
            CHECK_EVAL_BREAKER();

            PyCodeObject *code = _PyFrame_GetCode(frame);
            _PyExecutorObject *executor = (_PyExecutorObject *)code->co_executors->executors[oparg&255];
            int original_oparg = executor->vm_data.oparg | (oparg & 0xfffff00);
            JUMPBY(1-original_oparg);
            frame->prev_instr = next_instr - 1;
            Py_INCREF(executor);
            frame = executor->execute(executor, frame, stack_pointer);
            if (frame == NULL) {
                frame = cframe.current_frame;
                goto resume_with_error;
            }
            goto resume_frame;
        }

        inst(POP_JUMP_IF_FALSE, (cond -- )) {
            assert(PyBool_Check(cond));
            JUMPBY(oparg * Py_IsFalse(cond));
        }

        inst(POP_JUMP_IF_TRUE, (cond -- )) {
            assert(PyBool_Check(cond));
            JUMPBY(oparg * Py_IsTrue(cond));
        }

        op(IS_NONE, (value -- b)) {
            if (Py_IsNone(value)) {
                b = Py_True;
            }
            else {
                b = Py_False;
                DECREF_INPUTS();
            }
        }

        macro(POP_JUMP_IF_NONE) = IS_NONE + POP_JUMP_IF_TRUE;

        macro(POP_JUMP_IF_NOT_NONE) = IS_NONE + POP_JUMP_IF_FALSE;

        inst(JUMP_BACKWARD_NO_INTERRUPT, (--)) {
            /* This bytecode is used in the `yield from` or `await` loop.
             * If there is an interrupt, we want it handled in the innermost
             * generator or coroutine, so we deliberately do not check it here.
             * (see bpo-30039).
             */
            JUMPBY(-oparg);
        }

        inst(GET_LEN, (obj -- obj, len_o)) {
            // PUSH(len(TOS))
            Py_ssize_t len_i = PyObject_Length(obj);
            ERROR_IF(len_i < 0, error);
            len_o = PyLong_FromSsize_t(len_i);
            ERROR_IF(len_o == NULL, error);
        }

        inst(MATCH_CLASS, (subject, type, names -- attrs)) {
            // Pop TOS and TOS1. Set TOS to a tuple of attributes on success, or
            // None on failure.
            assert(PyTuple_CheckExact(names));
            attrs = _PyEval_MatchClass(tstate, subject, type, oparg, names);
            DECREF_INPUTS();
            if (attrs) {
                assert(PyTuple_CheckExact(attrs));  // Success!
            }
            else {
                ERROR_IF(_PyErr_Occurred(tstate), error);  // Error!
                attrs = Py_None;  // Failure!
            }
        }

        inst(MATCH_MAPPING, (subject -- subject, res)) {
            int match = Py_TYPE(subject)->tp_flags & Py_TPFLAGS_MAPPING;
            res = match ? Py_True : Py_False;
        }

        inst(MATCH_SEQUENCE, (subject -- subject, res)) {
            int match = Py_TYPE(subject)->tp_flags & Py_TPFLAGS_SEQUENCE;
            res = match ? Py_True : Py_False;
        }

        inst(MATCH_KEYS, (subject, keys -- subject, keys, values_or_none)) {
            // On successful match, PUSH(values). Otherwise, PUSH(None).
            values_or_none = _PyEval_MatchKeys(tstate, subject, keys);
            ERROR_IF(values_or_none == NULL, error);
        }

        inst(GET_ITER, (iterable -- iter)) {
            /* before: [obj]; after [getiter(obj)] */
            iter = PyObject_GetIter(iterable);
            DECREF_INPUTS();
            ERROR_IF(iter == NULL, error);
        }

        inst(GET_YIELD_FROM_ITER, (iterable -- iter)) {
            /* before: [obj]; after [getiter(obj)] */
            if (PyCoro_CheckExact(iterable)) {
                /* `iterable` is a coroutine */
                if (!(_PyFrame_GetCode(frame)->co_flags & (CO_COROUTINE | CO_ITERABLE_COROUTINE))) {
                    /* and it is used in a 'yield from' expression of a
                       regular generator. */
                    _PyErr_SetString(tstate, PyExc_TypeError,
                                     "cannot 'yield from' a coroutine object "
                                     "in a non-coroutine generator");
                    goto error;
                }
                iter = iterable;
            }
            else if (PyGen_CheckExact(iterable)) {
                iter = iterable;
            }
            else {
                /* `iterable` is not a generator. */
                iter = PyObject_GetIter(iterable);
                if (iter == NULL) {
                    goto error;
                }
                DECREF_INPUTS();
            }
        }

        // Most members of this family are "secretly" super-instructions.
        // When the loop is exhausted, they jump, and the jump target is
        // always END_FOR, which pops two values off the stack.
        // This is optimized by skipping that instruction and combining
        // its effect (popping 'iter' instead of pushing 'next'.)

        family(FOR_ITER, INLINE_CACHE_ENTRIES_FOR_ITER) = {
            FOR_ITER_LIST,
            FOR_ITER_TUPLE,
            FOR_ITER_RANGE,
            FOR_ITER_GEN,
        };

        inst(FOR_ITER, (unused/1, iter -- iter, next)) {
            #if ENABLE_SPECIALIZATION
            _PyForIterCache *cache = (_PyForIterCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_ForIter(iter, next_instr, oparg);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(FOR_ITER, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            /* before: [iter]; after: [iter, iter()] *or* [] (and jump over END_FOR.) */
            next = (*Py_TYPE(iter)->tp_iternext)(iter);
            if (next == NULL) {
                if (_PyErr_Occurred(tstate)) {
                    if (!_PyErr_ExceptionMatches(tstate, PyExc_StopIteration)) {
                        goto error;
                    }
                    monitor_raise(tstate, frame, next_instr-1);
                    _PyErr_Clear(tstate);
                }
                /* iterator ended normally */
                assert(next_instr[INLINE_CACHE_ENTRIES_FOR_ITER + oparg].op.code == END_FOR ||
                       next_instr[INLINE_CACHE_ENTRIES_FOR_ITER + oparg].op.code == INSTRUMENTED_END_FOR);
                Py_DECREF(iter);
                STACK_SHRINK(1);
                SKIP_OVER(INLINE_CACHE_ENTRIES_FOR_ITER);
                /* Jump forward oparg, then skip following END_FOR instruction */
                JUMPBY(oparg + 1);
                DISPATCH();
            }
            // Common case: no jump, leave it to the code generator
        }

        inst(INSTRUMENTED_FOR_ITER, ( -- )) {
            _Py_CODEUNIT *here = next_instr-1;
            _Py_CODEUNIT *target;
            PyObject *iter = TOP();
            PyObject *next = (*Py_TYPE(iter)->tp_iternext)(iter);
            if (next != NULL) {
                PUSH(next);
                target = next_instr + INLINE_CACHE_ENTRIES_FOR_ITER;
            }
            else {
                if (_PyErr_Occurred(tstate)) {
                    if (!_PyErr_ExceptionMatches(tstate, PyExc_StopIteration)) {
                        goto error;
                    }
                    monitor_raise(tstate, frame, here);
                    _PyErr_Clear(tstate);
                }
                /* iterator ended normally */
                assert(next_instr[INLINE_CACHE_ENTRIES_FOR_ITER + oparg].op.code == END_FOR ||
                       next_instr[INLINE_CACHE_ENTRIES_FOR_ITER + oparg].op.code == INSTRUMENTED_END_FOR);
                STACK_SHRINK(1);
                Py_DECREF(iter);
                /* Skip END_FOR */
                target = next_instr + INLINE_CACHE_ENTRIES_FOR_ITER + oparg + 1;
            }
            INSTRUMENTED_JUMP(here, target, PY_MONITORING_EVENT_BRANCH);
        }

        op(_ITER_CHECK_LIST, (iter -- iter)) {
            DEOPT_IF(Py_TYPE(iter) != &PyListIter_Type, FOR_ITER);
        }

        op(_ITER_JUMP_LIST, (iter -- iter)) {
            _PyListIterObject *it = (_PyListIterObject *)iter;
            assert(Py_TYPE(iter) == &PyListIter_Type);
            STAT_INC(FOR_ITER, hit);
            PyListObject *seq = it->it_seq;
            if (seq == NULL || it->it_index >= PyList_GET_SIZE(seq)) {
                if (seq != NULL) {
                    it->it_seq = NULL;
                    Py_DECREF(seq);
                }
                Py_DECREF(iter);
                STACK_SHRINK(1);
                SKIP_OVER(INLINE_CACHE_ENTRIES_FOR_ITER);
                /* Jump forward oparg, then skip following END_FOR instruction */
                JUMPBY(oparg + 1);
                DISPATCH();
            }
        }

        // Only used by Tier 2
        op(_IS_ITER_EXHAUSTED_LIST, (iter -- iter, exhausted)) {
            _PyListIterObject *it = (_PyListIterObject *)iter;
            assert(Py_TYPE(iter) == &PyListIter_Type);
            PyListObject *seq = it->it_seq;
            if (seq == NULL) {
                exhausted = Py_True;
            }
            else if (it->it_index >= PyList_GET_SIZE(seq)) {
                Py_DECREF(seq);
                it->it_seq = NULL;
                exhausted = Py_True;
            }
            else {
                exhausted = Py_False;
            }
        }

        op(_ITER_NEXT_LIST, (iter -- iter, next)) {
            _PyListIterObject *it = (_PyListIterObject *)iter;
            assert(Py_TYPE(iter) == &PyListIter_Type);
            PyListObject *seq = it->it_seq;
            assert(seq);
            assert(it->it_index < PyList_GET_SIZE(seq));
            next = Py_NewRef(PyList_GET_ITEM(seq, it->it_index++));
        }

        macro(FOR_ITER_LIST) =
            unused/1 +  // Skip over the counter
            _ITER_CHECK_LIST +
            _ITER_JUMP_LIST +
            _ITER_NEXT_LIST;

        op(_ITER_CHECK_TUPLE, (iter -- iter)) {
            DEOPT_IF(Py_TYPE(iter) != &PyTupleIter_Type, FOR_ITER);
        }

        op(_ITER_JUMP_TUPLE, (iter -- iter)) {
            _PyTupleIterObject *it = (_PyTupleIterObject *)iter;
            assert(Py_TYPE(iter) == &PyTupleIter_Type);
            STAT_INC(FOR_ITER, hit);
            PyTupleObject *seq = it->it_seq;
            if (seq == NULL || it->it_index >= PyTuple_GET_SIZE(seq)) {
                if (seq != NULL) {
                    it->it_seq = NULL;
                    Py_DECREF(seq);
                }
                Py_DECREF(iter);
                STACK_SHRINK(1);
                SKIP_OVER(INLINE_CACHE_ENTRIES_FOR_ITER);
                /* Jump forward oparg, then skip following END_FOR instruction */
                JUMPBY(oparg + 1);
                DISPATCH();
            }
        }

        // Only used by Tier 2
        op(_IS_ITER_EXHAUSTED_TUPLE, (iter -- iter, exhausted)) {
            _PyTupleIterObject *it = (_PyTupleIterObject *)iter;
            assert(Py_TYPE(iter) == &PyTupleIter_Type);
            PyTupleObject *seq = it->it_seq;
            if (seq == NULL) {
                exhausted = Py_True;
            }
            else if (it->it_index >= PyTuple_GET_SIZE(seq)) {
                Py_DECREF(seq);
                it->it_seq = NULL;
                exhausted = Py_True;
            }
            else {
                exhausted = Py_False;
            }
        }

        op(_ITER_NEXT_TUPLE, (iter -- iter, next)) {
            _PyTupleIterObject *it = (_PyTupleIterObject *)iter;
            assert(Py_TYPE(iter) == &PyTupleIter_Type);
            PyTupleObject *seq = it->it_seq;
            assert(seq);
            assert(it->it_index < PyTuple_GET_SIZE(seq));
            next = Py_NewRef(PyTuple_GET_ITEM(seq, it->it_index++));
        }

        macro(FOR_ITER_TUPLE) =
            unused/1 +  // Skip over the counter
            _ITER_CHECK_TUPLE +
            _ITER_JUMP_TUPLE +
            _ITER_NEXT_TUPLE;

        op(_ITER_CHECK_RANGE, (iter -- iter)) {
            _PyRangeIterObject *r = (_PyRangeIterObject *)iter;
            DEOPT_IF(Py_TYPE(r) != &PyRangeIter_Type, FOR_ITER);
        }

        op(_ITER_JUMP_RANGE, (iter -- iter)) {
            _PyRangeIterObject *r = (_PyRangeIterObject *)iter;
            assert(Py_TYPE(r) == &PyRangeIter_Type);
            STAT_INC(FOR_ITER, hit);
            if (r->len <= 0) {
                STACK_SHRINK(1);
                Py_DECREF(r);
                SKIP_OVER(INLINE_CACHE_ENTRIES_FOR_ITER);
                // Jump over END_FOR instruction.
                JUMPBY(oparg + 1);
                DISPATCH();
            }
        }

        // Only used by Tier 2
        op(_IS_ITER_EXHAUSTED_RANGE, (iter -- iter, exhausted)) {
            _PyRangeIterObject *r = (_PyRangeIterObject *)iter;
            assert(Py_TYPE(r) == &PyRangeIter_Type);
            exhausted = r->len <= 0 ? Py_True : Py_False;
        }

        op(_ITER_NEXT_RANGE, (iter -- iter, next)) {
            _PyRangeIterObject *r = (_PyRangeIterObject *)iter;
            assert(Py_TYPE(r) == &PyRangeIter_Type);
            assert(r->len > 0);
            long value = r->start;
            r->start = value + r->step;
            r->len--;
            next = PyLong_FromLong(value);
            ERROR_IF(next == NULL, error);
        }

        macro(FOR_ITER_RANGE) =
            unused/1 +  // Skip over the counter
            _ITER_CHECK_RANGE +
            _ITER_JUMP_RANGE +
            _ITER_NEXT_RANGE;

        inst(FOR_ITER_GEN, (unused/1, iter -- iter, unused)) {
            DEOPT_IF(tstate->interp->eval_frame, FOR_ITER);
            PyGenObject *gen = (PyGenObject *)iter;
            DEOPT_IF(Py_TYPE(gen) != &PyGen_Type, FOR_ITER);
            DEOPT_IF(gen->gi_frame_state >= FRAME_EXECUTING, FOR_ITER);
            STAT_INC(FOR_ITER, hit);
            _PyInterpreterFrame *gen_frame = (_PyInterpreterFrame *)gen->gi_iframe;
            frame->return_offset = oparg;
            _PyFrame_StackPush(gen_frame, Py_None);
            gen->gi_frame_state = FRAME_EXECUTING;
            gen->gi_exc_state.previous_item = tstate->exc_info;
            tstate->exc_info = &gen->gi_exc_state;
            SKIP_OVER(INLINE_CACHE_ENTRIES_FOR_ITER);
            assert(next_instr[oparg].op.code == END_FOR ||
                   next_instr[oparg].op.code == INSTRUMENTED_END_FOR);
            DISPATCH_INLINED(gen_frame);
        }

        inst(BEFORE_ASYNC_WITH, (mgr -- exit, res)) {
            PyObject *enter = _PyObject_LookupSpecial(mgr, &_Py_ID(__aenter__));
            if (enter == NULL) {
                if (!_PyErr_Occurred(tstate)) {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                  "'%.200s' object does not support the "
                                  "asynchronous context manager protocol",
                                  Py_TYPE(mgr)->tp_name);
                }
                goto error;
            }
            exit = _PyObject_LookupSpecial(mgr, &_Py_ID(__aexit__));
            if (exit == NULL) {
                if (!_PyErr_Occurred(tstate)) {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                  "'%.200s' object does not support the "
                                  "asynchronous context manager protocol "
                                  "(missed __aexit__ method)",
                                  Py_TYPE(mgr)->tp_name);
                }
                Py_DECREF(enter);
                goto error;
            }
            DECREF_INPUTS();
            res = _PyObject_CallNoArgs(enter);
            Py_DECREF(enter);
            if (res == NULL) {
                Py_DECREF(exit);
                ERROR_IF(true, error);
            }
        }

        inst(BEFORE_WITH, (mgr -- exit, res)) {
            /* pop the context manager, push its __exit__ and the
             * value returned from calling its __enter__
             */
            PyObject *enter = _PyObject_LookupSpecial(mgr, &_Py_ID(__enter__));
            if (enter == NULL) {
                if (!_PyErr_Occurred(tstate)) {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                  "'%.200s' object does not support the "
                                  "context manager protocol",
                                  Py_TYPE(mgr)->tp_name);
                }
                goto error;
            }
            exit = _PyObject_LookupSpecial(mgr, &_Py_ID(__exit__));
            if (exit == NULL) {
                if (!_PyErr_Occurred(tstate)) {
                    _PyErr_Format(tstate, PyExc_TypeError,
                                  "'%.200s' object does not support the "
                                  "context manager protocol "
                                  "(missed __exit__ method)",
                                  Py_TYPE(mgr)->tp_name);
                }
                Py_DECREF(enter);
                goto error;
            }
            DECREF_INPUTS();
            res = _PyObject_CallNoArgs(enter);
            Py_DECREF(enter);
            if (res == NULL) {
                Py_DECREF(exit);
                ERROR_IF(true, error);
            }
        }

        inst(WITH_EXCEPT_START, (exit_func, lasti, unused, val -- exit_func, lasti, unused, val, res)) {
            /* At the top of the stack are 4 values:
               - val: TOP = exc_info()
               - unused: SECOND = previous exception
               - lasti: THIRD = lasti of exception in exc_info()
               - exit_func: FOURTH = the context.__exit__ bound method
               We call FOURTH(type(TOP), TOP, GetTraceback(TOP)).
               Then we push the __exit__ return value.
            */
            PyObject *exc, *tb;

            assert(val && PyExceptionInstance_Check(val));
            exc = PyExceptionInstance_Class(val);
            tb = PyException_GetTraceback(val);
            if (tb == NULL) {
                tb = Py_None;
            }
            else {
                Py_DECREF(tb);
            }
            assert(PyLong_Check(lasti));
            (void)lasti; // Shut up compiler warning if asserts are off
            PyObject *stack[4] = {NULL, exc, val, tb};
            res = PyObject_Vectorcall(exit_func, stack + 1,
                    3 | PY_VECTORCALL_ARGUMENTS_OFFSET, NULL);
            ERROR_IF(res == NULL, error);
        }

        pseudo(SETUP_FINALLY) = {
            NOP,
        };

        pseudo(SETUP_CLEANUP) = {
            NOP,
        };

        pseudo(SETUP_WITH) = {
            NOP,
        };

        pseudo(POP_BLOCK) = {
            NOP,
        };

        inst(PUSH_EXC_INFO, (new_exc -- prev_exc, new_exc)) {
            _PyErr_StackItem *exc_info = tstate->exc_info;
            if (exc_info->exc_value != NULL) {
                prev_exc = exc_info->exc_value;
            }
            else {
                prev_exc = Py_None;
            }
            assert(PyExceptionInstance_Check(new_exc));
            exc_info->exc_value = Py_NewRef(new_exc);
        }

        inst(LOAD_ATTR_METHOD_WITH_VALUES, (unused/1, type_version/2, keys_version/2, descr/4, owner -- attr, self if (1))) {
            assert(oparg & 1);
            /* Cached method object */
            PyTypeObject *owner_cls = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(owner_cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(owner_cls->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues *dorv = _PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(!_PyDictOrValues_IsValues(*dorv) &&
                     !_PyObject_MakeInstanceAttributesFromDict(owner, dorv),
                     LOAD_ATTR);
            PyHeapTypeObject *owner_heap_type = (PyHeapTypeObject *)owner_cls;
            DEOPT_IF(owner_heap_type->ht_cached_keys->dk_version !=
                     keys_version, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            assert(descr != NULL);
            attr = Py_NewRef(descr);
            assert(_PyType_HasFeature(Py_TYPE(attr), Py_TPFLAGS_METHOD_DESCRIPTOR));
            self = owner;
        }

        inst(LOAD_ATTR_METHOD_NO_DICT, (unused/1, type_version/2, unused/2, descr/4, owner -- attr, self if (1))) {
            assert(oparg & 1);
            PyTypeObject *owner_cls = Py_TYPE(owner);
            DEOPT_IF(owner_cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(owner_cls->tp_dictoffset == 0);
            STAT_INC(LOAD_ATTR, hit);
            assert(descr != NULL);
            assert(_PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_METHOD_DESCRIPTOR));
            attr = Py_NewRef(descr);
            self = owner;
        }

        inst(LOAD_ATTR_NONDESCRIPTOR_WITH_VALUES, (unused/1, type_version/2, keys_version/2, descr/4, owner -- attr, unused if (0))) {
            assert((oparg & 1) == 0);
            PyTypeObject *owner_cls = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(owner_cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(owner_cls->tp_flags & Py_TPFLAGS_MANAGED_DICT);
            PyDictOrValues *dorv = _PyObject_DictOrValuesPointer(owner);
            DEOPT_IF(!_PyDictOrValues_IsValues(*dorv) &&
                     !_PyObject_MakeInstanceAttributesFromDict(owner, dorv),
                     LOAD_ATTR);
            PyHeapTypeObject *owner_heap_type = (PyHeapTypeObject *)owner_cls;
            DEOPT_IF(owner_heap_type->ht_cached_keys->dk_version !=
                     keys_version, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            assert(descr != NULL);
            DECREF_INPUTS();
            attr = Py_NewRef(descr);
        }

        inst(LOAD_ATTR_NONDESCRIPTOR_NO_DICT, (unused/1, type_version/2, unused/2, descr/4, owner -- attr, unused if (0))) {
            assert((oparg & 1) == 0);
            PyTypeObject *owner_cls = Py_TYPE(owner);
            assert(type_version != 0);
            DEOPT_IF(owner_cls->tp_version_tag != type_version, LOAD_ATTR);
            assert(owner_cls->tp_dictoffset == 0);
            STAT_INC(LOAD_ATTR, hit);
            assert(descr != NULL);
            DECREF_INPUTS();
            attr = Py_NewRef(descr);
        }

        inst(LOAD_ATTR_METHOD_LAZY_DICT, (unused/1, type_version/2, unused/2, descr/4, owner -- attr, self if (1))) {
            assert(oparg & 1);
            PyTypeObject *owner_cls = Py_TYPE(owner);
            DEOPT_IF(owner_cls->tp_version_tag != type_version, LOAD_ATTR);
            Py_ssize_t dictoffset = owner_cls->tp_dictoffset;
            assert(dictoffset > 0);
            PyObject *dict = *(PyObject **)((char *)owner + dictoffset);
            /* This object has a __dict__, just not yet created */
            DEOPT_IF(dict != NULL, LOAD_ATTR);
            STAT_INC(LOAD_ATTR, hit);
            assert(descr != NULL);
            assert(_PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_METHOD_DESCRIPTOR));
            attr = Py_NewRef(descr);
            self = owner;
        }

        inst(KW_NAMES, (--)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg < PyTuple_GET_SIZE(FRAME_CO_CONSTS));
            kwnames = GETITEM(FRAME_CO_CONSTS, oparg);
        }

        inst(INSTRUMENTED_CALL, ( -- )) {
            int is_meth = PEEK(oparg + 1) != NULL;
            int total_args = oparg + is_meth;
            PyObject *function = PEEK(oparg + 2);
            PyObject *arg = total_args == 0 ?
                &_PyInstrumentation_MISSING : PEEK(total_args);
            int err = _Py_call_instrumentation_2args(
                    tstate, PY_MONITORING_EVENT_CALL,
                    frame, next_instr-1, function, arg);
            ERROR_IF(err, error);
            _PyCallCache *cache = (_PyCallCache *)next_instr;
            INCREMENT_ADAPTIVE_COUNTER(cache->counter);
            GO_TO_INSTRUCTION(CALL);
        }

        // Cache layout: counter/1, func_version/2
        // Neither CALL_INTRINSIC_1/2 nor CALL_FUNCTION_EX are members!
        family(CALL, INLINE_CACHE_ENTRIES_CALL) = {
            CALL_BOUND_METHOD_EXACT_ARGS,
            CALL_PY_EXACT_ARGS,
            CALL_PY_WITH_DEFAULTS,
            CALL_NO_KW_TYPE_1,
            CALL_NO_KW_STR_1,
            CALL_NO_KW_TUPLE_1,
            CALL_BUILTIN_CLASS,
            CALL_NO_KW_BUILTIN_O,
            CALL_NO_KW_BUILTIN_FAST,
            CALL_BUILTIN_FAST_WITH_KEYWORDS,
            CALL_NO_KW_LEN,
            CALL_NO_KW_ISINSTANCE,
            CALL_NO_KW_LIST_APPEND,
            CALL_NO_KW_METHOD_DESCRIPTOR_O,
            CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS,
            CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS,
            CALL_NO_KW_METHOD_DESCRIPTOR_FAST,
            CALL_NO_KW_ALLOC_AND_ENTER_INIT,
        };

        // On entry, the stack is either
        //   [NULL, callable, arg1, arg2, ...]
        // or
        //   [method, self, arg1, arg2, ...]
        // (Some args may be keywords, see KW_NAMES, which sets 'kwnames'.)
        // On exit, the stack is [result].
        // When calling Python, inline the call using DISPATCH_INLINED().
        inst(CALL, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            #if ENABLE_SPECIALIZATION
            _PyCallCache *cache = (_PyCallCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_Call(callable, next_instr, total_args, kwnames);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(CALL, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            if (self_or_null == NULL && Py_TYPE(callable) == &PyMethod_Type) {
                args--;
                total_args++;
                PyObject *self = ((PyMethodObject *)callable)->im_self;
                args[0] = Py_NewRef(self);
                PyObject *method = ((PyMethodObject *)callable)->im_func;
                args[-1] = Py_NewRef(method);
                Py_DECREF(callable);
                callable = method;
            }
            int positional_args = total_args - KWNAMES_LEN();
            // Check if the call can be inlined or not
            if (Py_TYPE(callable) == &PyFunction_Type &&
                tstate->interp->eval_frame == NULL &&
                ((PyFunctionObject *)callable)->vectorcall == _PyFunction_Vectorcall)
            {
                int code_flags = ((PyCodeObject*)PyFunction_GET_CODE(callable))->co_flags;
                PyObject *locals = code_flags & CO_OPTIMIZED ? NULL : Py_NewRef(PyFunction_GET_GLOBALS(callable));
                _PyInterpreterFrame *new_frame = _PyEvalFramePushAndInit(
                    tstate, (PyFunctionObject *)callable, locals,
                    args, positional_args, kwnames
                );
                kwnames = NULL;
                // Manipulate stack directly since we leave using DISPATCH_INLINED().
                STACK_SHRINK(oparg + 2);
                // The frame has stolen all the arguments from the stack,
                // so there is no need to clean them up.
                if (new_frame == NULL) {
                    goto error;
                }
                SKIP_OVER(INLINE_CACHE_ENTRIES_CALL);
                frame->return_offset = 0;
                DISPATCH_INLINED(new_frame);
            }
            /* Callable is not a normal Python function */
            res = PyObject_Vectorcall(
                callable, args,
                positional_args | PY_VECTORCALL_ARGUMENTS_OFFSET,
                kwnames);
            if (opcode == INSTRUMENTED_CALL) {
                PyObject *arg = total_args == 0 ?
                    &_PyInstrumentation_MISSING : args[0];
                if (res == NULL) {
                    _Py_call_instrumentation_exc2(
                        tstate, PY_MONITORING_EVENT_C_RAISE,
                        frame, next_instr-1, callable, arg);
                }
                else {
                    int err = _Py_call_instrumentation_2args(
                        tstate, PY_MONITORING_EVENT_C_RETURN,
                        frame, next_instr-1, callable, arg);
                    if (err < 0) {
                        Py_CLEAR(res);
                    }
                }
            }
            kwnames = NULL;
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            Py_DECREF(callable);
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        // Start out with [NULL, bound_method, arg1, arg2, ...]
        // Transform to [callable, self, arg1, arg2, ...]
        // Then fall through to CALL_PY_EXACT_ARGS
        inst(CALL_BOUND_METHOD_EXACT_ARGS, (unused/1, unused/2, callable, null, unused[oparg] -- unused)) {
            DEOPT_IF(null != NULL, CALL);
            DEOPT_IF(Py_TYPE(callable) != &PyMethod_Type, CALL);
            STAT_INC(CALL, hit);
            PyObject *self = ((PyMethodObject *)callable)->im_self;
            PEEK(oparg + 1) = Py_NewRef(self);  // self_or_null
            PyObject *meth = ((PyMethodObject *)callable)->im_func;
            PEEK(oparg + 2) = Py_NewRef(meth);  // callable
            Py_DECREF(callable);
            GO_TO_INSTRUCTION(CALL_PY_EXACT_ARGS);
        }

        inst(CALL_PY_EXACT_ARGS, (unused/1, func_version/2, callable, self_or_null, args[oparg] -- unused)) {
            ASSERT_KWNAMES_IS_NULL();
            DEOPT_IF(tstate->interp->eval_frame, CALL);
            int argcount = oparg;
            if (self_or_null != NULL) {
                args--;
                argcount++;
            }
            DEOPT_IF(!PyFunction_Check(callable), CALL);
            PyFunctionObject *func = (PyFunctionObject *)callable;
            DEOPT_IF(func->func_version != func_version, CALL);
            PyCodeObject *code = (PyCodeObject *)func->func_code;
            DEOPT_IF(code->co_argcount != argcount, CALL);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize), CALL);
            STAT_INC(CALL, hit);
            _PyInterpreterFrame *new_frame = _PyFrame_PushUnchecked(tstate, func, argcount);
            for (int i = 0; i < argcount; i++) {
                new_frame->localsplus[i] = args[i];
            }
            // Manipulate stack directly since we leave using DISPATCH_INLINED().
            STACK_SHRINK(oparg + 2);
            SKIP_OVER(INLINE_CACHE_ENTRIES_CALL);
            frame->return_offset = 0;
            DISPATCH_INLINED(new_frame);
        }

        inst(CALL_PY_WITH_DEFAULTS, (unused/1, func_version/2, callable, self_or_null, args[oparg] -- unused)) {
            ASSERT_KWNAMES_IS_NULL();
            DEOPT_IF(tstate->interp->eval_frame, CALL);
            int argcount = oparg;
            if (self_or_null != NULL) {
                args--;
                argcount++;
            }
            DEOPT_IF(!PyFunction_Check(callable), CALL);
            PyFunctionObject *func = (PyFunctionObject *)callable;
            DEOPT_IF(func->func_version != func_version, CALL);
            PyCodeObject *code = (PyCodeObject *)func->func_code;
            assert(func->func_defaults);
            assert(PyTuple_CheckExact(func->func_defaults));
            int defcount = (int)PyTuple_GET_SIZE(func->func_defaults);
            assert(defcount <= code->co_argcount);
            int min_args = code->co_argcount - defcount;
            DEOPT_IF(argcount > code->co_argcount, CALL);
            DEOPT_IF(argcount < min_args, CALL);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize), CALL);
            STAT_INC(CALL, hit);
            _PyInterpreterFrame *new_frame = _PyFrame_PushUnchecked(tstate, func, code->co_argcount);
            for (int i = 0; i < argcount; i++) {
                new_frame->localsplus[i] = args[i];
            }
            for (int i = argcount; i < code->co_argcount; i++) {
                PyObject *def = PyTuple_GET_ITEM(func->func_defaults, i - min_args);
                new_frame->localsplus[i] = Py_NewRef(def);
            }
            // Manipulate stack and cache directly since we leave using DISPATCH_INLINED().
            STACK_SHRINK(oparg + 2);
            SKIP_OVER(INLINE_CACHE_ENTRIES_CALL);
            frame->return_offset = 0;
            DISPATCH_INLINED(new_frame);
        }

        inst(CALL_NO_KW_TYPE_1, (unused/1, unused/2, callable, null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg == 1);
            DEOPT_IF(null != NULL, CALL);
            PyObject *obj = args[0];
            DEOPT_IF(callable != (PyObject *)&PyType_Type, CALL);
            STAT_INC(CALL, hit);
            res = Py_NewRef(Py_TYPE(obj));
            Py_DECREF(obj);
            Py_DECREF(&PyType_Type);  // I.e., callable
        }

        inst(CALL_NO_KW_STR_1, (unused/1, unused/2, callable, null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg == 1);
            DEOPT_IF(null != NULL, CALL);
            DEOPT_IF(callable != (PyObject *)&PyUnicode_Type, CALL);
            STAT_INC(CALL, hit);
            PyObject *arg = args[0];
            res = PyObject_Str(arg);
            Py_DECREF(arg);
            Py_DECREF(&PyUnicode_Type);  // I.e., callable
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_TUPLE_1, (unused/1, unused/2, callable, null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg == 1);
            DEOPT_IF(null != NULL, CALL);
            DEOPT_IF(callable != (PyObject *)&PyTuple_Type, CALL);
            STAT_INC(CALL, hit);
            PyObject *arg = args[0];
            res = PySequence_Tuple(arg);
            Py_DECREF(arg);
            Py_DECREF(&PyTuple_Type);  // I.e., tuple
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_ALLOC_AND_ENTER_INIT, (unused/1, unused/2, callable, null, args[oparg] -- unused)) {
            /* This instruction does the following:
             * 1. Creates the object (by calling ``object.__new__``)
             * 2. Pushes a shim frame to the frame stack (to cleanup after ``__init__``)
             * 3. Pushes the frame for ``__init__`` to the frame stack
             * */
            ASSERT_KWNAMES_IS_NULL();
            _PyCallCache *cache = (_PyCallCache *)next_instr;
            DEOPT_IF(null != NULL, CALL);
            DEOPT_IF(!PyType_Check(callable), CALL);
            PyTypeObject *tp = (PyTypeObject *)callable;
            DEOPT_IF(tp->tp_version_tag != read_u32(cache->func_version), CALL);
            PyHeapTypeObject *cls = (PyHeapTypeObject *)callable;
            PyFunctionObject *init = (PyFunctionObject *)cls->_spec_cache.init;
            PyCodeObject *code = (PyCodeObject *)init->func_code;
            DEOPT_IF(code->co_argcount != oparg+1, CALL);
            DEOPT_IF(!_PyThreadState_HasStackSpace(tstate, code->co_framesize + _Py_InitCleanup.co_framesize), CALL);
            STAT_INC(CALL, hit);
            PyObject *self = _PyType_NewManagedObject(tp);
            if (self == NULL) {
                goto error;
            }
            Py_DECREF(tp);
            _PyInterpreterFrame *shim = _PyFrame_PushTrampolineUnchecked(
                tstate, (PyCodeObject *)&_Py_InitCleanup, 1, 0);
            assert(_PyCode_CODE((PyCodeObject *)shim->f_executable)[1].op.code == EXIT_INIT_CHECK);
            /* Push self onto stack of shim */
            Py_INCREF(self);
            shim->localsplus[0] = self;
            Py_INCREF(init);
            _PyInterpreterFrame *init_frame = _PyFrame_PushUnchecked(tstate, init, oparg+1);
            /* Copy self followed by args to __init__ frame */
            init_frame->localsplus[0] = self;
            for (int i = 0; i < oparg; i++) {
                init_frame->localsplus[i+1] = args[i];
            }
            SKIP_OVER(INLINE_CACHE_ENTRIES_CALL);
            frame->prev_instr = next_instr - 1;
            frame->return_offset = 0;
            STACK_SHRINK(oparg+2);
            _PyFrame_SetStackPointer(frame, stack_pointer);
            /* Link frames */
            init_frame->previous = shim;
            shim->previous = frame;
            frame = cframe.current_frame = init_frame;
            CALL_STAT_INC(inlined_py_calls);
            /* Account for pushing the extra frame.
             * We don't check recursion depth here,
             * as it will be checked after start_frame */
            tstate->py_recursion_remaining--;
            goto start_frame;
        }

        inst(EXIT_INIT_CHECK, (should_be_none -- )) {
            assert(STACK_LEVEL() == 2);
            if (should_be_none != Py_None) {
                PyErr_Format(PyExc_TypeError,
                    "__init__() should return None, not '%.200s'",
                    Py_TYPE(should_be_none)->tp_name);
                goto error;
            }
        }

        inst(CALL_BUILTIN_CLASS, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            int kwnames_len = KWNAMES_LEN();
            DEOPT_IF(!PyType_Check(callable), CALL);
            PyTypeObject *tp = (PyTypeObject *)callable;
            DEOPT_IF(tp->tp_vectorcall == NULL, CALL);
            STAT_INC(CALL, hit);
            res = tp->tp_vectorcall((PyObject *)tp, args,
                                    total_args - kwnames_len, kwnames);
            kwnames = NULL;
            /* Free the arguments. */
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            Py_DECREF(tp);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_BUILTIN_O, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            /* Builtin METH_O functions */
            ASSERT_KWNAMES_IS_NULL();
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(total_args != 1, CALL);
            DEOPT_IF(!PyCFunction_CheckExact(callable), CALL);
            DEOPT_IF(PyCFunction_GET_FLAGS(callable) != METH_O, CALL);
            STAT_INC(CALL, hit);
            PyCFunction cfunc = PyCFunction_GET_FUNCTION(callable);
            // This is slower but CPython promises to check all non-vectorcall
            // function calls.
            if (_Py_EnterRecursiveCallTstate(tstate, " while calling a Python object")) {
                goto error;
            }
            PyObject *arg = args[0];
            res = _PyCFunction_TrampolineCall(cfunc, PyCFunction_GET_SELF(callable), arg);
            _Py_LeaveRecursiveCallTstate(tstate);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));

            Py_DECREF(arg);
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_BUILTIN_FAST, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            /* Builtin METH_FASTCALL functions, without keywords */
            ASSERT_KWNAMES_IS_NULL();
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(!PyCFunction_CheckExact(callable), CALL);
            DEOPT_IF(PyCFunction_GET_FLAGS(callable) != METH_FASTCALL, CALL);
            STAT_INC(CALL, hit);
            PyCFunction cfunc = PyCFunction_GET_FUNCTION(callable);
            /* res = func(self, args, nargs) */
            res = ((_PyCFunctionFast)(void(*)(void))cfunc)(
                PyCFunction_GET_SELF(callable),
                args,
                total_args);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));

            /* Free the arguments. */
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
                /* Not deopting because this doesn't mean our optimization was
                   wrong. `res` can be NULL for valid reasons. Eg. getattr(x,
                   'invalid'). In those cases an exception is set, so we must
                   handle it.
                */
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_BUILTIN_FAST_WITH_KEYWORDS, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            /* Builtin METH_FASTCALL | METH_KEYWORDS functions */
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(!PyCFunction_CheckExact(callable), CALL);
            DEOPT_IF(PyCFunction_GET_FLAGS(callable) !=
                (METH_FASTCALL | METH_KEYWORDS), CALL);
            STAT_INC(CALL, hit);
            /* res = func(self, args, nargs, kwnames) */
            _PyCFunctionFastWithKeywords cfunc =
                (_PyCFunctionFastWithKeywords)(void(*)(void))
                PyCFunction_GET_FUNCTION(callable);
            res = cfunc(
                PyCFunction_GET_SELF(callable),
                args,
                total_args - KWNAMES_LEN(),
                kwnames
            );
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            kwnames = NULL;

            /* Free the arguments. */
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_LEN, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            /* len(o) */
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(total_args != 1, CALL);
            PyInterpreterState *interp = tstate->interp;
            DEOPT_IF(callable != interp->callable_cache.len, CALL);
            STAT_INC(CALL, hit);
            PyObject *arg = args[0];
            Py_ssize_t len_i = PyObject_Length(arg);
            if (len_i < 0) {
                goto error;
            }
            res = PyLong_FromSsize_t(len_i);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));

            Py_DECREF(callable);
            Py_DECREF(arg);
            ERROR_IF(res == NULL, error);
        }

        inst(CALL_NO_KW_ISINSTANCE, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            /* isinstance(o, o2) */
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(total_args != 2, CALL);
            PyInterpreterState *interp = tstate->interp;
            DEOPT_IF(callable != interp->callable_cache.isinstance, CALL);
            STAT_INC(CALL, hit);
            PyObject *cls = args[1];
            PyObject *inst = args[0];
            int retval = PyObject_IsInstance(inst, cls);
            if (retval < 0) {
                goto error;
            }
            res = PyBool_FromLong(retval);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));

            Py_DECREF(inst);
            Py_DECREF(cls);
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
        }

        // This is secretly a super-instruction
        inst(CALL_NO_KW_LIST_APPEND, (unused/1, unused/2, callable, self, args[oparg] -- unused)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg == 1);
            assert(self != NULL);
            PyInterpreterState *interp = tstate->interp;
            DEOPT_IF(callable != interp->callable_cache.list_append, CALL);
            DEOPT_IF(!PyList_Check(self), CALL);
            STAT_INC(CALL, hit);
            if (_PyList_AppendTakeRef((PyListObject *)self, args[0]) < 0) {
                goto pop_1_error;  // Since arg is DECREF'ed already
            }
            Py_DECREF(self);
            Py_DECREF(callable);
            STACK_SHRINK(3);
            // CALL + POP_TOP
            SKIP_OVER(INLINE_CACHE_ENTRIES_CALL + 1);
            assert(next_instr[-1].op.code == POP_TOP);
            DISPATCH();
        }

        inst(CALL_NO_KW_METHOD_DESCRIPTOR_O, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            PyMethodDescrObject *method = (PyMethodDescrObject *)callable;
            DEOPT_IF(total_args != 2, CALL);
            DEOPT_IF(!Py_IS_TYPE(method, &PyMethodDescr_Type), CALL);
            PyMethodDef *meth = method->d_method;
            DEOPT_IF(meth->ml_flags != METH_O, CALL);
            PyObject *arg = args[1];
            PyObject *self = args[0];
            DEOPT_IF(!Py_IS_TYPE(self, method->d_common.d_type), CALL);
            STAT_INC(CALL, hit);
            PyCFunction cfunc = meth->ml_meth;
            // This is slower but CPython promises to check all non-vectorcall
            // function calls.
            if (_Py_EnterRecursiveCallTstate(tstate, " while calling a Python object")) {
                goto error;
            }
            res = _PyCFunction_TrampolineCall(cfunc, self, arg);
            _Py_LeaveRecursiveCallTstate(tstate);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            Py_DECREF(self);
            Py_DECREF(arg);
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            PyMethodDescrObject *method = (PyMethodDescrObject *)callable;
            DEOPT_IF(!Py_IS_TYPE(method, &PyMethodDescr_Type), CALL);
            PyMethodDef *meth = method->d_method;
            DEOPT_IF(meth->ml_flags != (METH_FASTCALL|METH_KEYWORDS), CALL);
            PyTypeObject *d_type = method->d_common.d_type;
            PyObject *self = args[0];
            DEOPT_IF(!Py_IS_TYPE(self, d_type), CALL);
            STAT_INC(CALL, hit);
            int nargs = total_args - 1;
            _PyCFunctionFastWithKeywords cfunc =
                (_PyCFunctionFastWithKeywords)(void(*)(void))meth->ml_meth;
            res = cfunc(self, args + 1, nargs - KWNAMES_LEN(), kwnames);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            kwnames = NULL;

            /* Free the arguments. */
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            assert(oparg == 0 || oparg == 1);
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            DEOPT_IF(total_args != 1, CALL);
            PyMethodDescrObject *method = (PyMethodDescrObject *)callable;
            DEOPT_IF(!Py_IS_TYPE(method, &PyMethodDescr_Type), CALL);
            PyMethodDef *meth = method->d_method;
            PyObject *self = args[0];
            DEOPT_IF(!Py_IS_TYPE(self, method->d_common.d_type), CALL);
            DEOPT_IF(meth->ml_flags != METH_NOARGS, CALL);
            STAT_INC(CALL, hit);
            PyCFunction cfunc = meth->ml_meth;
            // This is slower but CPython promises to check all non-vectorcall
            // function calls.
            if (_Py_EnterRecursiveCallTstate(tstate, " while calling a Python object")) {
                goto error;
            }
            res = _PyCFunction_TrampolineCall(cfunc, self, NULL);
            _Py_LeaveRecursiveCallTstate(tstate);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            Py_DECREF(self);
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(CALL_NO_KW_METHOD_DESCRIPTOR_FAST, (unused/1, unused/2, callable, self_or_null, args[oparg] -- res)) {
            ASSERT_KWNAMES_IS_NULL();
            int total_args = oparg;
            if (self_or_null != NULL) {
                args--;
                total_args++;
            }
            PyMethodDescrObject *method = (PyMethodDescrObject *)callable;
            /* Builtin METH_FASTCALL methods, without keywords */
            DEOPT_IF(!Py_IS_TYPE(method, &PyMethodDescr_Type), CALL);
            PyMethodDef *meth = method->d_method;
            DEOPT_IF(meth->ml_flags != METH_FASTCALL, CALL);
            PyObject *self = args[0];
            DEOPT_IF(!Py_IS_TYPE(self, method->d_common.d_type), CALL);
            STAT_INC(CALL, hit);
            _PyCFunctionFast cfunc =
                (_PyCFunctionFast)(void(*)(void))meth->ml_meth;
            int nargs = total_args - 1;
            res = cfunc(self, args + 1, nargs);
            assert((res != NULL) ^ (_PyErr_Occurred(tstate) != NULL));
            /* Clear the stack of the arguments. */
            for (int i = 0; i < total_args; i++) {
                Py_DECREF(args[i]);
            }
            Py_DECREF(callable);
            ERROR_IF(res == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(INSTRUMENTED_CALL_FUNCTION_EX, ( -- )) {
            GO_TO_INSTRUCTION(CALL_FUNCTION_EX);
        }

        inst(CALL_FUNCTION_EX, (func, unused, callargs, kwargs if (oparg & 1) -- result)) {
            // DICT_MERGE is called before this opcode if there are kwargs.
            // It converts all dict subtypes in kwargs into regular dicts.
            assert(kwargs == NULL || PyDict_CheckExact(kwargs));
            if (!PyTuple_CheckExact(callargs)) {
                if (check_args_iterable(tstate, func, callargs) < 0) {
                    goto error;
                }
                PyObject *tuple = PySequence_Tuple(callargs);
                if (tuple == NULL) {
                    goto error;
                }
                Py_SETREF(callargs, tuple);
            }
            assert(PyTuple_CheckExact(callargs));
            EVAL_CALL_STAT_INC_IF_FUNCTION(EVAL_CALL_FUNCTION_EX, func);
            if (opcode == INSTRUMENTED_CALL_FUNCTION_EX &&
                !PyFunction_Check(func) && !PyMethod_Check(func)
            ) {
                PyObject *arg = PyTuple_GET_SIZE(callargs) > 0 ?
                    PyTuple_GET_ITEM(callargs, 0) : Py_None;
                int err = _Py_call_instrumentation_2args(
                    tstate, PY_MONITORING_EVENT_CALL,
                    frame, next_instr-1, func, arg);
                if (err) goto error;
                result = PyObject_Call(func, callargs, kwargs);
                if (result == NULL) {
                    _Py_call_instrumentation_exc2(
                        tstate, PY_MONITORING_EVENT_C_RAISE,
                        frame, next_instr-1, func, arg);
                }
                else {
                    int err = _Py_call_instrumentation_2args(
                        tstate, PY_MONITORING_EVENT_C_RETURN,
                        frame, next_instr-1, func, arg);
                    if (err < 0) {
                        Py_CLEAR(result);
                    }
                }
            }
            else {
                if (Py_TYPE(func) == &PyFunction_Type &&
                    tstate->interp->eval_frame == NULL &&
                    ((PyFunctionObject *)func)->vectorcall == _PyFunction_Vectorcall) {
                    assert(PyTuple_CheckExact(callargs));
                    Py_ssize_t nargs = PyTuple_GET_SIZE(callargs);
                    int code_flags = ((PyCodeObject *)PyFunction_GET_CODE(func))->co_flags;
                    PyObject *locals = code_flags & CO_OPTIMIZED ? NULL : Py_NewRef(PyFunction_GET_GLOBALS(func));

                    _PyInterpreterFrame *new_frame = _PyEvalFramePushAndInit_Ex(tstate,
                                                                                (PyFunctionObject *)func, locals,
                                                                                nargs, callargs, kwargs);
                    // Need to manually shrink the stack since we exit with DISPATCH_INLINED.
                    STACK_SHRINK(oparg + 3);
                    if (new_frame == NULL) {
                        goto error;
                    }
                    frame->return_offset = 0;
                    DISPATCH_INLINED(new_frame);
                }
                result = PyObject_Call(func, callargs, kwargs);
            }
            DECREF_INPUTS();
            assert(PEEK(2 + (oparg & 1)) == NULL);
            ERROR_IF(result == NULL, error);
            CHECK_EVAL_BREAKER();
        }

        inst(MAKE_FUNCTION, (codeobj -- func)) {

            PyFunctionObject *func_obj = (PyFunctionObject *)
                PyFunction_New(codeobj, GLOBALS());

            Py_DECREF(codeobj);
            if (func_obj == NULL) {
                goto error;
            }

            func_obj->func_version = ((PyCodeObject *)codeobj)->co_version;
            func = (PyObject *)func_obj;
        }

        inst(SET_FUNCTION_ATTRIBUTE, (attr, func -- func)) {
            assert(PyFunction_Check(func));
            PyFunctionObject *func_obj = (PyFunctionObject *)func;
            switch(oparg) {
                case MAKE_FUNCTION_CLOSURE:
                    assert(func_obj->func_closure == NULL);
                    func_obj->func_closure = attr;
                    break;
                case MAKE_FUNCTION_ANNOTATIONS:
                    assert(func_obj->func_annotations == NULL);
                    func_obj->func_annotations = attr;
                    break;
                case MAKE_FUNCTION_KWDEFAULTS:
                    assert(PyDict_CheckExact(attr));
                    assert(func_obj->func_kwdefaults == NULL);
                    func_obj->func_kwdefaults = attr;
                    break;
                case MAKE_FUNCTION_DEFAULTS:
                    assert(PyTuple_CheckExact(attr));
                    assert(func_obj->func_defaults == NULL);
                    func_obj->func_defaults = attr;
                    break;
                default:
                    Py_UNREACHABLE();
            }
        }

        inst(RETURN_GENERATOR, (--)) {
            assert(PyFunction_Check(frame->f_funcobj));
            PyFunctionObject *func = (PyFunctionObject *)frame->f_funcobj;
            PyGenObject *gen = (PyGenObject *)_Py_MakeCoro(func);
            if (gen == NULL) {
                goto error;
            }
            assert(EMPTY());
            _PyFrame_SetStackPointer(frame, stack_pointer);
            _PyInterpreterFrame *gen_frame = (_PyInterpreterFrame *)gen->gi_iframe;
            _PyFrame_Copy(frame, gen_frame);
            assert(frame->frame_obj == NULL);
            gen->gi_frame_state = FRAME_CREATED;
            gen_frame->owner = FRAME_OWNED_BY_GENERATOR;
            _Py_LeaveRecursiveCallPy(tstate);
            assert(frame != &entry_frame);
            _PyInterpreterFrame *prev = frame->previous;
            _PyThreadState_PopFrame(tstate, frame);
            frame = cframe.current_frame = prev;
            _PyFrame_StackPush(frame, (PyObject *)gen);
            goto resume_frame;
        }

        inst(BUILD_SLICE, (start, stop, step if (oparg == 3) -- slice)) {
            slice = PySlice_New(start, stop, step);
            DECREF_INPUTS();
            ERROR_IF(slice == NULL, error);
        }

        inst(CONVERT_VALUE, (value -- result)) {
            convertion_func_ptr  conv_fn;
            assert(oparg >= FVC_STR && oparg <= FVC_ASCII);
            conv_fn = CONVERSION_FUNCTIONS[oparg];
            result = conv_fn(value);
            Py_DECREF(value);
            ERROR_IF(result == NULL, error);
        }

        inst(FORMAT_SIMPLE, (value -- res)) {
            /* If value is a unicode object, then we know the result
             * of format(value) is value itself. */
            if (!PyUnicode_CheckExact(value)) {
                res = PyObject_Format(value, NULL);
                Py_DECREF(value);
                ERROR_IF(res == NULL, error);
            }
            else {
                res = value;
            }
        }

        inst(FORMAT_WITH_SPEC, (value, fmt_spec -- res)) {
            res = PyObject_Format(value, fmt_spec);
            Py_DECREF(value);
            Py_DECREF(fmt_spec);
            ERROR_IF(res == NULL, error);
        }

        inst(COPY, (bottom, unused[oparg-1] -- bottom, unused[oparg-1], top)) {
            assert(oparg > 0);
            top = Py_NewRef(bottom);
        }

        inst(BINARY_OP, (unused/1, lhs, rhs -- res)) {
            #if ENABLE_SPECIALIZATION
            _PyBinaryOpCache *cache = (_PyBinaryOpCache *)next_instr;
            if (ADAPTIVE_COUNTER_IS_ZERO(cache->counter)) {
                next_instr--;
                _Py_Specialize_BinaryOp(lhs, rhs, next_instr, oparg, LOCALS_ARRAY);
                DISPATCH_SAME_OPARG();
            }
            STAT_INC(BINARY_OP, deferred);
            DECREMENT_ADAPTIVE_COUNTER(cache->counter);
            #endif  /* ENABLE_SPECIALIZATION */
            assert(NB_ADD <= oparg);
            assert(oparg <= NB_INPLACE_XOR);
            assert(_PyEval_BinaryOps[oparg]);
            res = _PyEval_BinaryOps[oparg](lhs, rhs);
            DECREF_INPUTS();
            ERROR_IF(res == NULL, error);
        }

        inst(SWAP, (bottom, unused[oparg-2], top --
                    top, unused[oparg-2], bottom)) {
            assert(oparg >= 2);
        }

        inst(INSTRUMENTED_INSTRUCTION, ( -- )) {
            int next_opcode = _Py_call_instrumentation_instruction(
                tstate, frame, next_instr-1);
            ERROR_IF(next_opcode < 0, error);
            next_instr--;
            if (_PyOpcode_Caches[next_opcode]) {
                _PyBinaryOpCache *cache = (_PyBinaryOpCache *)(next_instr+1);
                INCREMENT_ADAPTIVE_COUNTER(cache->counter);
            }
            assert(next_opcode > 0 && next_opcode < 256);
            opcode = next_opcode;
            DISPATCH_GOTO();
        }

        inst(INSTRUMENTED_JUMP_FORWARD, ( -- )) {
            INSTRUMENTED_JUMP(next_instr-1, next_instr+oparg, PY_MONITORING_EVENT_JUMP);
        }

        inst(INSTRUMENTED_JUMP_BACKWARD, ( -- )) {
            CHECK_EVAL_BREAKER();
            INSTRUMENTED_JUMP(next_instr-1, next_instr+1-oparg, PY_MONITORING_EVENT_JUMP);
        }

        inst(INSTRUMENTED_POP_JUMP_IF_TRUE, ( -- )) {
            PyObject *cond = POP();
            assert(PyBool_Check(cond));
            _Py_CODEUNIT *here = next_instr - 1;
            int offset = Py_IsTrue(cond) * oparg;
            INSTRUMENTED_JUMP(here, next_instr + offset, PY_MONITORING_EVENT_BRANCH);
        }

        inst(INSTRUMENTED_POP_JUMP_IF_FALSE, ( -- )) {
            PyObject *cond = POP();
            assert(PyBool_Check(cond));
            _Py_CODEUNIT *here = next_instr - 1;
            int offset = Py_IsFalse(cond) * oparg;
            INSTRUMENTED_JUMP(here, next_instr + offset, PY_MONITORING_EVENT_BRANCH);
        }

        inst(INSTRUMENTED_POP_JUMP_IF_NONE, ( -- )) {
            PyObject *value = POP();
            _Py_CODEUNIT *here = next_instr-1;
            int offset;
            if (Py_IsNone(value)) {
                offset = oparg;
            }
            else {
                Py_DECREF(value);
                offset = 0;
            }
            INSTRUMENTED_JUMP(here, next_instr + offset, PY_MONITORING_EVENT_BRANCH);
        }

        inst(INSTRUMENTED_POP_JUMP_IF_NOT_NONE, ( -- )) {
            PyObject *value = POP();
            _Py_CODEUNIT *here = next_instr-1;
            int offset;
            if (Py_IsNone(value)) {
                offset = 0;
            }
            else {
                Py_DECREF(value);
                offset = oparg;
            }
            INSTRUMENTED_JUMP(here, next_instr + offset, PY_MONITORING_EVENT_BRANCH);
        }

        inst(EXTENDED_ARG, ( -- )) {
            assert(oparg);
            opcode = next_instr->op.code;
            oparg = oparg << 8 | next_instr->op.arg;
            PRE_DISPATCH_GOTO();
            DISPATCH_GOTO();
        }

        inst(CACHE, (--)) {
            assert(0 && "Executing a cache.");
            Py_UNREACHABLE();
        }

        inst(RESERVED, (--)) {
            assert(0 && "Executing RESERVED instruction.");
            Py_UNREACHABLE();
        }

        ///////// Tier-2 only opcodes /////////

        op(_POP_JUMP_IF_FALSE, (flag -- )) {
            if (Py_IsFalse(flag)) {
                pc = oparg;
            }
        }

        op(_POP_JUMP_IF_TRUE, (flag -- )) {
            if (Py_IsTrue(flag)) {
                pc = oparg;
            }
        }

        op(JUMP_TO_TOP, (--)) {
            pc = 0;
            CHECK_EVAL_BREAKER();
        }

        op(SAVE_IP, (--)) {
            frame->prev_instr = ip_offset + oparg;
        }

        op(EXIT_TRACE, (--)) {
            frame->prev_instr--;  // Back up to just before destination
            _PyFrame_SetStackPointer(frame, stack_pointer);
            Py_DECREF(self);
            return frame;
        }

        op(INSERT, (unused[oparg], top -- top, unused[oparg])) {
            // Inserts TOS at position specified by oparg;
            memmove(&stack_pointer[-1 - oparg], &stack_pointer[-oparg], oparg * sizeof(stack_pointer[0]));
        }


// END BYTECODES //

    }
 dispatch_opcode:
 error:
 exception_unwind:
 exit_unwind:
 handle_eval_breaker:
 resume_frame:
 resume_with_error:
 start_frame:
 unbound_local_error:
    ;
}

// Future families go below this point //
