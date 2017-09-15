/* File automatically generated by Parser/asdl_c.py. */

#include "asdl.h"

typedef struct _mod *mod_ty;

typedef struct _stmt *stmt_ty;

typedef struct _expr *expr_ty;

typedef enum _expr_context { Load=1, Store=2, Del=3, AugLoad=4, AugStore=5,
                             Param=6 } expr_context_ty;

typedef struct _slice *slice_ty;

typedef enum _boolop { And=1, Or=2 } boolop_ty;

typedef enum _operator { Add=1, Sub=2, Mult=3, MatMult=4, Div=5, Mod=6, Pow=7,
                         LShift=8, RShift=9, BitOr=10, BitXor=11, BitAnd=12,
                         FloorDiv=13 } operator_ty;

typedef enum _unaryop { Invert=1, Not=2, UAdd=3, USub=4 } unaryop_ty;

typedef enum _cmpop { Eq=1, NotEq=2, Lt=3, LtE=4, Gt=5, GtE=6, Is=7, IsNot=8,
                      In=9, NotIn=10 } cmpop_ty;

typedef struct _comprehension *comprehension_ty;

typedef struct _excepthandler *excepthandler_ty;

typedef struct _arguments *arguments_ty;

typedef struct _arg *arg_ty;

typedef struct _keyword *keyword_ty;

typedef struct _alias *alias_ty;

typedef struct _withitem *withitem_ty;


enum _mod_kind {Module_kind=1, Interactive_kind=2, Expression_kind=3,
                 Suite_kind=4};
struct _mod {
    enum _mod_kind kind;
    union {
        struct {
            asdl_seq *body;
            string docstring;
        } Module;

        struct {
            asdl_seq *body;
        } Interactive;

        struct {
            expr_ty body;
        } Expression;

        struct {
            asdl_seq *body;
        } Suite;

    } v;
};

enum _stmt_kind {FunctionDef_kind=1, AsyncFunctionDef_kind=2, ClassDef_kind=3,
                  Return_kind=4, Delete_kind=5, Assign_kind=6,
                  AugAssign_kind=7, AnnAssign_kind=8, For_kind=9,
                  AsyncFor_kind=10, While_kind=11, If_kind=12, With_kind=13,
                  AsyncWith_kind=14, Raise_kind=15, Try_kind=16,
                  Assert_kind=17, Import_kind=18, ImportFrom_kind=19,
                  Global_kind=20, Nonlocal_kind=21, Expr_kind=22, Pass_kind=23,
                  Break_kind=24, Continue_kind=25};
struct _stmt {
    enum _stmt_kind kind;
    union {
        struct {
            identifier name;
            arguments_ty args;
            asdl_seq *body;
            asdl_seq *decorator_list;
            expr_ty returns;
            string docstring;
        } FunctionDef;

        struct {
            identifier name;
            arguments_ty args;
            asdl_seq *body;
            asdl_seq *decorator_list;
            expr_ty returns;
            string docstring;
        } AsyncFunctionDef;

        struct {
            identifier name;
            asdl_seq *bases;
            asdl_seq *keywords;
            asdl_seq *body;
            asdl_seq *decorator_list;
            string docstring;
        } ClassDef;

        struct {
            expr_ty value;
        } Return;

        struct {
            asdl_seq *targets;
        } Delete;

        struct {
            asdl_seq *targets;
            expr_ty value;
        } Assign;

        struct {
            expr_ty target;
            operator_ty op;
            expr_ty value;
        } AugAssign;

        struct {
            expr_ty target;
            expr_ty annotation;
            expr_ty value;
            int simple;
        } AnnAssign;

        struct {
            expr_ty target;
            expr_ty iter;
            asdl_seq *body;
            asdl_seq *orelse;
        } For;

        struct {
            expr_ty target;
            expr_ty iter;
            asdl_seq *body;
            asdl_seq *orelse;
        } AsyncFor;

        struct {
            expr_ty test;
            asdl_seq *body;
            asdl_seq *orelse;
        } While;

        struct {
            expr_ty test;
            asdl_seq *body;
            asdl_seq *orelse;
        } If;

        struct {
            asdl_seq *items;
            asdl_seq *body;
        } With;

        struct {
            asdl_seq *items;
            asdl_seq *body;
        } AsyncWith;

        struct {
            expr_ty exc;
            expr_ty cause;
        } Raise;

        struct {
            asdl_seq *body;
            asdl_seq *handlers;
            asdl_seq *orelse;
            asdl_seq *finalbody;
        } Try;

        struct {
            expr_ty test;
            expr_ty msg;
        } Assert;

        struct {
            asdl_seq *names;
        } Import;

        struct {
            identifier module;
            asdl_seq *names;
            int level;
        } ImportFrom;

        struct {
            asdl_seq *names;
        } Global;

        struct {
            asdl_seq *names;
        } Nonlocal;

        struct {
            expr_ty value;
        } Expr;

    } v;
    int lineno;
    int col_offset;
};

enum _expr_kind {BoolOp_kind=1, BinOp_kind=2, UnaryOp_kind=3, Lambda_kind=4,
                  IfExp_kind=5, Dict_kind=6, Set_kind=7, ListComp_kind=8,
                  SetComp_kind=9, DictComp_kind=10, GeneratorExp_kind=11,
                  Await_kind=12, Yield_kind=13, YieldFrom_kind=14,
                  Compare_kind=15, Call_kind=16, Num_kind=17, Str_kind=18,
                  FormattedValue_kind=19, JoinedStr_kind=20, Bytes_kind=21,
                  NameConstant_kind=22, Ellipsis_kind=23, Constant_kind=24,
                  Attribute_kind=25, Subscript_kind=26, Starred_kind=27,
                  Name_kind=28, List_kind=29, Tuple_kind=30};
struct _expr {
    enum _expr_kind kind;
    union {
        struct {
            boolop_ty op;
            asdl_seq *values;
        } BoolOp;

        struct {
            expr_ty left;
            operator_ty op;
            expr_ty right;
        } BinOp;

        struct {
            unaryop_ty op;
            expr_ty operand;
        } UnaryOp;

        struct {
            arguments_ty args;
            expr_ty body;
        } Lambda;

        struct {
            expr_ty test;
            expr_ty body;
            expr_ty orelse;
        } IfExp;

        struct {
            asdl_seq *keys;
            asdl_seq *values;
        } Dict;

        struct {
            asdl_seq *elts;
        } Set;

        struct {
            expr_ty elt;
            asdl_seq *generators;
        } ListComp;

        struct {
            expr_ty elt;
            asdl_seq *generators;
        } SetComp;

        struct {
            expr_ty key;
            expr_ty value;
            asdl_seq *generators;
        } DictComp;

        struct {
            expr_ty elt;
            asdl_seq *generators;
        } GeneratorExp;

        struct {
            expr_ty value;
        } Await;

        struct {
            expr_ty value;
        } Yield;

        struct {
            expr_ty value;
        } YieldFrom;

        struct {
            expr_ty left;
            asdl_int_seq *ops;
            asdl_seq *comparators;
        } Compare;

        struct {
            expr_ty func;
            asdl_seq *args;
            asdl_seq *keywords;
        } Call;

        struct {
            object n;
        } Num;

        struct {
            string s;
        } Str;

        struct {
            expr_ty value;
            int conversion;
            expr_ty format_spec;
        } FormattedValue;

        struct {
            asdl_seq *values;
        } JoinedStr;

        struct {
            bytes s;
        } Bytes;

        struct {
            singleton value;
        } NameConstant;

        struct {
            constant value;
        } Constant;

        struct {
            expr_ty value;
            identifier attr;
            expr_context_ty ctx;
        } Attribute;

        struct {
            expr_ty value;
            slice_ty slice;
            expr_context_ty ctx;
        } Subscript;

        struct {
            expr_ty value;
            expr_context_ty ctx;
        } Starred;

        struct {
            identifier id;
            expr_context_ty ctx;
        } Name;

        struct {
            asdl_seq *elts;
            expr_context_ty ctx;
        } List;

        struct {
            asdl_seq *elts;
            expr_context_ty ctx;
        } Tuple;

    } v;
    int lineno;
    int col_offset;
};

enum _slice_kind {Slice_kind=1, ExtSlice_kind=2, Index_kind=3};
struct _slice {
    enum _slice_kind kind;
    union {
        struct {
            expr_ty lower;
            expr_ty upper;
            expr_ty step;
        } Slice;

        struct {
            asdl_seq *dims;
        } ExtSlice;

        struct {
            expr_ty value;
        } Index;

    } v;
};

struct _comprehension {
    expr_ty target;
    expr_ty iter;
    asdl_seq *ifs;
    int is_async;
};

enum _excepthandler_kind {ExceptHandler_kind=1};
struct _excepthandler {
    enum _excepthandler_kind kind;
    union {
        struct {
            expr_ty type;
            identifier name;
            asdl_seq *body;
        } ExceptHandler;

    } v;
    int lineno;
    int col_offset;
};

struct _arguments {
    asdl_seq *args;
    arg_ty vararg;
    asdl_seq *kwonlyargs;
    asdl_seq *kw_defaults;
    arg_ty kwarg;
    asdl_seq *defaults;
};

struct _arg {
    identifier arg;
    expr_ty annotation;
    int lineno;
    int col_offset;
};

struct _keyword {
    identifier arg;
    expr_ty value;
};

struct _alias {
    identifier name;
    identifier asname;
};

struct _withitem {
    expr_ty context_expr;
    expr_ty optional_vars;
};


#define Module(a0, a1, a2) _Py_Module(a0, a1, a2)
mod_ty _Py_Module(asdl_seq * body, string docstring, PyArena *arena);
#define Interactive(a0, a1) _Py_Interactive(a0, a1)
mod_ty _Py_Interactive(asdl_seq * body, PyArena *arena);
#define Expression(a0, a1) _Py_Expression(a0, a1)
mod_ty _Py_Expression(expr_ty body, PyArena *arena);
#define Suite(a0, a1) _Py_Suite(a0, a1)
mod_ty _Py_Suite(asdl_seq * body, PyArena *arena);
#define FunctionDef(a0, a1, a2, a3, a4, a5, a6, a7, a8) _Py_FunctionDef(a0, a1, a2, a3, a4, a5, a6, a7, a8)
stmt_ty _Py_FunctionDef(identifier name, arguments_ty args, asdl_seq * body,
                        asdl_seq * decorator_list, expr_ty returns, string
                        docstring, int lineno, int col_offset, PyArena *arena);
#define AsyncFunctionDef(a0, a1, a2, a3, a4, a5, a6, a7, a8) _Py_AsyncFunctionDef(a0, a1, a2, a3, a4, a5, a6, a7, a8)
stmt_ty _Py_AsyncFunctionDef(identifier name, arguments_ty args, asdl_seq *
                             body, asdl_seq * decorator_list, expr_ty returns,
                             string docstring, int lineno, int col_offset,
                             PyArena *arena);
#define ClassDef(a0, a1, a2, a3, a4, a5, a6, a7, a8) _Py_ClassDef(a0, a1, a2, a3, a4, a5, a6, a7, a8)
stmt_ty _Py_ClassDef(identifier name, asdl_seq * bases, asdl_seq * keywords,
                     asdl_seq * body, asdl_seq * decorator_list, string
                     docstring, int lineno, int col_offset, PyArena *arena);
#define Return(a0, a1, a2, a3) _Py_Return(a0, a1, a2, a3)
stmt_ty _Py_Return(expr_ty value, int lineno, int col_offset, PyArena *arena);
#define Delete(a0, a1, a2, a3) _Py_Delete(a0, a1, a2, a3)
stmt_ty _Py_Delete(asdl_seq * targets, int lineno, int col_offset, PyArena
                   *arena);
#define Assign(a0, a1, a2, a3, a4) _Py_Assign(a0, a1, a2, a3, a4)
stmt_ty _Py_Assign(asdl_seq * targets, expr_ty value, int lineno, int
                   col_offset, PyArena *arena);
#define AugAssign(a0, a1, a2, a3, a4, a5) _Py_AugAssign(a0, a1, a2, a3, a4, a5)
stmt_ty _Py_AugAssign(expr_ty target, operator_ty op, expr_ty value, int
                      lineno, int col_offset, PyArena *arena);
#define AnnAssign(a0, a1, a2, a3, a4, a5, a6) _Py_AnnAssign(a0, a1, a2, a3, a4, a5, a6)
stmt_ty _Py_AnnAssign(expr_ty target, expr_ty annotation, expr_ty value, int
                      simple, int lineno, int col_offset, PyArena *arena);
#define For(a0, a1, a2, a3, a4, a5, a6) _Py_For(a0, a1, a2, a3, a4, a5, a6)
stmt_ty _Py_For(expr_ty target, expr_ty iter, asdl_seq * body, asdl_seq *
                orelse, int lineno, int col_offset, PyArena *arena);
#define AsyncFor(a0, a1, a2, a3, a4, a5, a6) _Py_AsyncFor(a0, a1, a2, a3, a4, a5, a6)
stmt_ty _Py_AsyncFor(expr_ty target, expr_ty iter, asdl_seq * body, asdl_seq *
                     orelse, int lineno, int col_offset, PyArena *arena);
#define While(a0, a1, a2, a3, a4, a5) _Py_While(a0, a1, a2, a3, a4, a5)
stmt_ty _Py_While(expr_ty test, asdl_seq * body, asdl_seq * orelse, int lineno,
                  int col_offset, PyArena *arena);
#define If(a0, a1, a2, a3, a4, a5) _Py_If(a0, a1, a2, a3, a4, a5)
stmt_ty _Py_If(expr_ty test, asdl_seq * body, asdl_seq * orelse, int lineno,
               int col_offset, PyArena *arena);
#define With(a0, a1, a2, a3, a4) _Py_With(a0, a1, a2, a3, a4)
stmt_ty _Py_With(asdl_seq * items, asdl_seq * body, int lineno, int col_offset,
                 PyArena *arena);
#define AsyncWith(a0, a1, a2, a3, a4) _Py_AsyncWith(a0, a1, a2, a3, a4)
stmt_ty _Py_AsyncWith(asdl_seq * items, asdl_seq * body, int lineno, int
                      col_offset, PyArena *arena);
#define Raise(a0, a1, a2, a3, a4) _Py_Raise(a0, a1, a2, a3, a4)
stmt_ty _Py_Raise(expr_ty exc, expr_ty cause, int lineno, int col_offset,
                  PyArena *arena);
#define Try(a0, a1, a2, a3, a4, a5, a6) _Py_Try(a0, a1, a2, a3, a4, a5, a6)
stmt_ty _Py_Try(asdl_seq * body, asdl_seq * handlers, asdl_seq * orelse,
                asdl_seq * finalbody, int lineno, int col_offset, PyArena
                *arena);
#define Assert(a0, a1, a2, a3, a4) _Py_Assert(a0, a1, a2, a3, a4)
stmt_ty _Py_Assert(expr_ty test, expr_ty msg, int lineno, int col_offset,
                   PyArena *arena);
#define Import(a0, a1, a2, a3) _Py_Import(a0, a1, a2, a3)
stmt_ty _Py_Import(asdl_seq * names, int lineno, int col_offset, PyArena
                   *arena);
#define ImportFrom(a0, a1, a2, a3, a4, a5) _Py_ImportFrom(a0, a1, a2, a3, a4, a5)
stmt_ty _Py_ImportFrom(identifier module, asdl_seq * names, int level, int
                       lineno, int col_offset, PyArena *arena);
#define Global(a0, a1, a2, a3) _Py_Global(a0, a1, a2, a3)
stmt_ty _Py_Global(asdl_seq * names, int lineno, int col_offset, PyArena
                   *arena);
#define Nonlocal(a0, a1, a2, a3) _Py_Nonlocal(a0, a1, a2, a3)
stmt_ty _Py_Nonlocal(asdl_seq * names, int lineno, int col_offset, PyArena
                     *arena);
#define Expr(a0, a1, a2, a3) _Py_Expr(a0, a1, a2, a3)
stmt_ty _Py_Expr(expr_ty value, int lineno, int col_offset, PyArena *arena);
#define Pass(a0, a1, a2) _Py_Pass(a0, a1, a2)
stmt_ty _Py_Pass(int lineno, int col_offset, PyArena *arena);
#define Break(a0, a1, a2) _Py_Break(a0, a1, a2)
stmt_ty _Py_Break(int lineno, int col_offset, PyArena *arena);
#define Continue(a0, a1, a2) _Py_Continue(a0, a1, a2)
stmt_ty _Py_Continue(int lineno, int col_offset, PyArena *arena);
#define BoolOp(a0, a1, a2, a3, a4) _Py_BoolOp(a0, a1, a2, a3, a4)
expr_ty _Py_BoolOp(boolop_ty op, asdl_seq * values, int lineno, int col_offset,
                   PyArena *arena);
#define BinOp(a0, a1, a2, a3, a4, a5) _Py_BinOp(a0, a1, a2, a3, a4, a5)
expr_ty _Py_BinOp(expr_ty left, operator_ty op, expr_ty right, int lineno, int
                  col_offset, PyArena *arena);
#define UnaryOp(a0, a1, a2, a3, a4) _Py_UnaryOp(a0, a1, a2, a3, a4)
expr_ty _Py_UnaryOp(unaryop_ty op, expr_ty operand, int lineno, int col_offset,
                    PyArena *arena);
#define Lambda(a0, a1, a2, a3, a4) _Py_Lambda(a0, a1, a2, a3, a4)
expr_ty _Py_Lambda(arguments_ty args, expr_ty body, int lineno, int col_offset,
                   PyArena *arena);
#define IfExp(a0, a1, a2, a3, a4, a5) _Py_IfExp(a0, a1, a2, a3, a4, a5)
expr_ty _Py_IfExp(expr_ty test, expr_ty body, expr_ty orelse, int lineno, int
                  col_offset, PyArena *arena);
#define Dict(a0, a1, a2, a3, a4) _Py_Dict(a0, a1, a2, a3, a4)
expr_ty _Py_Dict(asdl_seq * keys, asdl_seq * values, int lineno, int
                 col_offset, PyArena *arena);
#define Set(a0, a1, a2, a3) _Py_Set(a0, a1, a2, a3)
expr_ty _Py_Set(asdl_seq * elts, int lineno, int col_offset, PyArena *arena);
#define ListComp(a0, a1, a2, a3, a4) _Py_ListComp(a0, a1, a2, a3, a4)
expr_ty _Py_ListComp(expr_ty elt, asdl_seq * generators, int lineno, int
                     col_offset, PyArena *arena);
#define SetComp(a0, a1, a2, a3, a4) _Py_SetComp(a0, a1, a2, a3, a4)
expr_ty _Py_SetComp(expr_ty elt, asdl_seq * generators, int lineno, int
                    col_offset, PyArena *arena);
#define DictComp(a0, a1, a2, a3, a4, a5) _Py_DictComp(a0, a1, a2, a3, a4, a5)
expr_ty _Py_DictComp(expr_ty key, expr_ty value, asdl_seq * generators, int
                     lineno, int col_offset, PyArena *arena);
#define GeneratorExp(a0, a1, a2, a3, a4) _Py_GeneratorExp(a0, a1, a2, a3, a4)
expr_ty _Py_GeneratorExp(expr_ty elt, asdl_seq * generators, int lineno, int
                         col_offset, PyArena *arena);
#define Await(a0, a1, a2, a3) _Py_Await(a0, a1, a2, a3)
expr_ty _Py_Await(expr_ty value, int lineno, int col_offset, PyArena *arena);
#define Yield(a0, a1, a2, a3) _Py_Yield(a0, a1, a2, a3)
expr_ty _Py_Yield(expr_ty value, int lineno, int col_offset, PyArena *arena);
#define YieldFrom(a0, a1, a2, a3) _Py_YieldFrom(a0, a1, a2, a3)
expr_ty _Py_YieldFrom(expr_ty value, int lineno, int col_offset, PyArena
                      *arena);
#define Compare(a0, a1, a2, a3, a4, a5) _Py_Compare(a0, a1, a2, a3, a4, a5)
expr_ty _Py_Compare(expr_ty left, asdl_int_seq * ops, asdl_seq * comparators,
                    int lineno, int col_offset, PyArena *arena);
#define Call(a0, a1, a2, a3, a4, a5) _Py_Call(a0, a1, a2, a3, a4, a5)
expr_ty _Py_Call(expr_ty func, asdl_seq * args, asdl_seq * keywords, int
                 lineno, int col_offset, PyArena *arena);
#define Num(a0, a1, a2, a3) _Py_Num(a0, a1, a2, a3)
expr_ty _Py_Num(object n, int lineno, int col_offset, PyArena *arena);
#define Str(a0, a1, a2, a3) _Py_Str(a0, a1, a2, a3)
expr_ty _Py_Str(string s, int lineno, int col_offset, PyArena *arena);
#define FormattedValue(a0, a1, a2, a3, a4, a5) _Py_FormattedValue(a0, a1, a2, a3, a4, a5)
expr_ty _Py_FormattedValue(expr_ty value, int conversion, expr_ty format_spec,
                           int lineno, int col_offset, PyArena *arena);
#define JoinedStr(a0, a1, a2, a3) _Py_JoinedStr(a0, a1, a2, a3)
expr_ty _Py_JoinedStr(asdl_seq * values, int lineno, int col_offset, PyArena
                      *arena);
#define Bytes(a0, a1, a2, a3) _Py_Bytes(a0, a1, a2, a3)
expr_ty _Py_Bytes(bytes s, int lineno, int col_offset, PyArena *arena);
#define NameConstant(a0, a1, a2, a3) _Py_NameConstant(a0, a1, a2, a3)
expr_ty _Py_NameConstant(singleton value, int lineno, int col_offset, PyArena
                         *arena);
#define Ellipsis(a0, a1, a2) _Py_Ellipsis(a0, a1, a2)
expr_ty _Py_Ellipsis(int lineno, int col_offset, PyArena *arena);
#define Constant(a0, a1, a2, a3) _Py_Constant(a0, a1, a2, a3)
expr_ty _Py_Constant(constant value, int lineno, int col_offset, PyArena
                     *arena);
#define Attribute(a0, a1, a2, a3, a4, a5) _Py_Attribute(a0, a1, a2, a3, a4, a5)
expr_ty _Py_Attribute(expr_ty value, identifier attr, expr_context_ty ctx, int
                      lineno, int col_offset, PyArena *arena);
#define Subscript(a0, a1, a2, a3, a4, a5) _Py_Subscript(a0, a1, a2, a3, a4, a5)
expr_ty _Py_Subscript(expr_ty value, slice_ty slice, expr_context_ty ctx, int
                      lineno, int col_offset, PyArena *arena);
#define Starred(a0, a1, a2, a3, a4) _Py_Starred(a0, a1, a2, a3, a4)
expr_ty _Py_Starred(expr_ty value, expr_context_ty ctx, int lineno, int
                    col_offset, PyArena *arena);
#define Name(a0, a1, a2, a3, a4) _Py_Name(a0, a1, a2, a3, a4)
expr_ty _Py_Name(identifier id, expr_context_ty ctx, int lineno, int
                 col_offset, PyArena *arena);
#define List(a0, a1, a2, a3, a4) _Py_List(a0, a1, a2, a3, a4)
expr_ty _Py_List(asdl_seq * elts, expr_context_ty ctx, int lineno, int
                 col_offset, PyArena *arena);
#define Tuple(a0, a1, a2, a3, a4) _Py_Tuple(a0, a1, a2, a3, a4)
expr_ty _Py_Tuple(asdl_seq * elts, expr_context_ty ctx, int lineno, int
                  col_offset, PyArena *arena);
#define Slice(a0, a1, a2, a3) _Py_Slice(a0, a1, a2, a3)
slice_ty _Py_Slice(expr_ty lower, expr_ty upper, expr_ty step, PyArena *arena);
#define ExtSlice(a0, a1) _Py_ExtSlice(a0, a1)
slice_ty _Py_ExtSlice(asdl_seq * dims, PyArena *arena);
#define Index(a0, a1) _Py_Index(a0, a1)
slice_ty _Py_Index(expr_ty value, PyArena *arena);
#define comprehension(a0, a1, a2, a3, a4) _Py_comprehension(a0, a1, a2, a3, a4)
comprehension_ty _Py_comprehension(expr_ty target, expr_ty iter, asdl_seq *
                                   ifs, int is_async, PyArena *arena);
#define ExceptHandler(a0, a1, a2, a3, a4, a5) _Py_ExceptHandler(a0, a1, a2, a3, a4, a5)
excepthandler_ty _Py_ExceptHandler(expr_ty type, identifier name, asdl_seq *
                                   body, int lineno, int col_offset, PyArena
                                   *arena);
#define arguments(a0, a1, a2, a3, a4, a5, a6) _Py_arguments(a0, a1, a2, a3, a4, a5, a6)
arguments_ty _Py_arguments(asdl_seq * args, arg_ty vararg, asdl_seq *
                           kwonlyargs, asdl_seq * kw_defaults, arg_ty kwarg,
                           asdl_seq * defaults, PyArena *arena);
#define arg(a0, a1, a2, a3, a4) _Py_arg(a0, a1, a2, a3, a4)
arg_ty _Py_arg(identifier arg, expr_ty annotation, int lineno, int col_offset,
               PyArena *arena);
#define keyword(a0, a1, a2) _Py_keyword(a0, a1, a2)
keyword_ty _Py_keyword(identifier arg, expr_ty value, PyArena *arena);
#define alias(a0, a1, a2) _Py_alias(a0, a1, a2)
alias_ty _Py_alias(identifier name, identifier asname, PyArena *arena);
#define withitem(a0, a1, a2) _Py_withitem(a0, a1, a2)
withitem_ty _Py_withitem(expr_ty context_expr, expr_ty optional_vars, PyArena
                         *arena);

PyObject* PyAST_mod2obj(mod_ty t);
mod_ty PyAST_obj2mod(PyObject* ast, PyArena* arena, int mode);
int PyAST_Check(PyObject* obj);
