// File automatically generated by Parser/asdl_c.py.

#ifndef Py_INTERNAL_AST_STATE_H
#define Py_INTERNAL_AST_STATE_H

#include "pycore_lock.h"    // _PyOnceFlag

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

struct ast_state {
    _PyOnceFlag once;
    int finalized;
    PyObject *AST_type;
    PyObject *Add_singleton;
    PyObject *Add_type;
    PyObject *And_singleton;
    PyObject *And_type;
    PyObject *AnnAssign_type;
    PyObject *Assert_type;
    PyObject *Assign_type;
    PyObject *AsyncFor_type;
    PyObject *AsyncFunctionDef_type;
    PyObject *AsyncWith_type;
    PyObject *Attribute_type;
    PyObject *AugAssign_type;
    PyObject *Await_type;
    PyObject *BinOp_type;
    PyObject *BitAnd_singleton;
    PyObject *BitAnd_type;
    PyObject *BitOr_singleton;
    PyObject *BitOr_type;
    PyObject *BitXor_singleton;
    PyObject *BitXor_type;
    PyObject *BoolOp_type;
    PyObject *Break_type;
    PyObject *Call_type;
    PyObject *ClassDef_type;
    PyObject *Compare_type;
    PyObject *Constant_type;
    PyObject *Continue_type;
    PyObject *Del_singleton;
    PyObject *Del_type;
    PyObject *Delete_type;
    PyObject *DictComp_type;
    PyObject *Dict_type;
    PyObject *Div_singleton;
    PyObject *Div_type;
    PyObject *Eq_singleton;
    PyObject *Eq_type;
    PyObject *ExceptHandler_type;
    PyObject *Expr_type;
    PyObject *Expression_type;
    PyObject *FloorDiv_singleton;
    PyObject *FloorDiv_type;
    PyObject *For_type;
    PyObject *FormattedValue_type;
    PyObject *FunctionDef_type;
    PyObject *FunctionType_type;
    PyObject *GeneratorExp_type;
    PyObject *Global_type;
    PyObject *GtE_singleton;
    PyObject *GtE_type;
    PyObject *Gt_singleton;
    PyObject *Gt_type;
    PyObject *IfExp_type;
    PyObject *If_type;
    PyObject *ImportFrom_type;
    PyObject *Import_type;
    PyObject *In_singleton;
    PyObject *In_type;
    PyObject *Interactive_type;
    PyObject *Invert_singleton;
    PyObject *Invert_type;
    PyObject *IsNot_singleton;
    PyObject *IsNot_type;
    PyObject *Is_singleton;
    PyObject *Is_type;
    PyObject *JoinedStr_type;
    PyObject *LShift_singleton;
    PyObject *LShift_type;
    PyObject *Lambda_type;
    PyObject *ListComp_type;
    PyObject *List_type;
    PyObject *Load_singleton;
    PyObject *Load_type;
    PyObject *LtE_singleton;
    PyObject *LtE_type;
    PyObject *Lt_singleton;
    PyObject *Lt_type;
    PyObject *MatMult_singleton;
    PyObject *MatMult_type;
    PyObject *MatchAs_type;
    PyObject *MatchClass_type;
    PyObject *MatchMapping_type;
    PyObject *MatchOr_type;
    PyObject *MatchSequence_type;
    PyObject *MatchSingleton_type;
    PyObject *MatchStar_type;
    PyObject *MatchValue_type;
    PyObject *Match_type;
    PyObject *Mod_singleton;
    PyObject *Mod_type;
    PyObject *Module_type;
    PyObject *Mult_singleton;
    PyObject *Mult_type;
    PyObject *Name_type;
    PyObject *NamedExpr_type;
    PyObject *Nonlocal_type;
    PyObject *NotEq_singleton;
    PyObject *NotEq_type;
    PyObject *NotIn_singleton;
    PyObject *NotIn_type;
    PyObject *Not_singleton;
    PyObject *Not_type;
    PyObject *Or_singleton;
    PyObject *Or_type;
    PyObject *ParamSpec_type;
    PyObject *Pass_type;
    PyObject *Pow_singleton;
    PyObject *Pow_type;
    PyObject *RShift_singleton;
    PyObject *RShift_type;
    PyObject *Raise_type;
    PyObject *Return_type;
    PyObject *SetComp_type;
    PyObject *Set_type;
    PyObject *Slice_type;
    PyObject *Starred_type;
    PyObject *Store_singleton;
    PyObject *Store_type;
    PyObject *Sub_singleton;
    PyObject *Sub_type;
    PyObject *Subscript_type;
    PyObject *TryStar_type;
    PyObject *Try_type;
    PyObject *Tuple_type;
    PyObject *TypeAlias_type;
    PyObject *TypeIgnore_type;
    PyObject *TypeVarTuple_type;
    PyObject *TypeVar_type;
    PyObject *UAdd_singleton;
    PyObject *UAdd_type;
    PyObject *USub_singleton;
    PyObject *USub_type;
    PyObject *UnaryOp_type;
    PyObject *While_type;
    PyObject *With_type;
    PyObject *YieldFrom_type;
    PyObject *Yield_type;
    PyObject *__dict__;
    PyObject *__doc__;
    PyObject *__match_args__;
    PyObject *__module__;
    PyObject *_attributes;
    PyObject *_fields;
    PyObject *alias_type;
    PyObject *annotation;
    PyObject *arg;
    PyObject *arg_type;
    PyObject *args;
    PyObject *argtypes;
    PyObject *arguments_type;
    PyObject *asname;
    PyObject *ast;
    PyObject *attr;
    PyObject *bases;
    PyObject *body;
    PyObject *boolop_type;
    PyObject *bound;
    PyObject *cases;
    PyObject *cause;
    PyObject *cls;
    PyObject *cmpop_type;
    PyObject *col_offset;
    PyObject *comparators;
    PyObject *comprehension_type;
    PyObject *context_expr;
    PyObject *conversion;
    PyObject *ctx;
    PyObject *decorator_list;
    PyObject *default_value;
    PyObject *defaults;
    PyObject *elt;
    PyObject *elts;
    PyObject *end_col_offset;
    PyObject *end_lineno;
    PyObject *exc;
    PyObject *excepthandler_type;
    PyObject *expr_context_type;
    PyObject *expr_type;
    PyObject *finalbody;
    PyObject *format_spec;
    PyObject *func;
    PyObject *generators;
    PyObject *guard;
    PyObject *handlers;
    PyObject *id;
    PyObject *ifs;
    PyObject *is_async;
    PyObject *items;
    PyObject *iter;
    PyObject *key;
    PyObject *keys;
    PyObject *keyword_type;
    PyObject *keywords;
    PyObject *kind;
    PyObject *kw_defaults;
    PyObject *kwarg;
    PyObject *kwd_attrs;
    PyObject *kwd_patterns;
    PyObject *kwonlyargs;
    PyObject *left;
    PyObject *level;
    PyObject *lineno;
    PyObject *lower;
    PyObject *match_case_type;
    PyObject *mod_type;
    PyObject *module;
    PyObject *msg;
    PyObject *name;
    PyObject *names;
    PyObject *op;
    PyObject *operand;
    PyObject *operator_type;
    PyObject *ops;
    PyObject *optional_vars;
    PyObject *orelse;
    PyObject *pattern;
    PyObject *pattern_type;
    PyObject *patterns;
    PyObject *posonlyargs;
    PyObject *rest;
    PyObject *returns;
    PyObject *right;
    PyObject *simple;
    PyObject *slice;
    PyObject *step;
    PyObject *stmt_type;
    PyObject *subject;
    PyObject *tag;
    PyObject *target;
    PyObject *targets;
    PyObject *test;
    PyObject *type;
    PyObject *type_comment;
    PyObject *type_ignore_type;
    PyObject *type_ignores;
    PyObject *type_param_type;
    PyObject *type_params;
    PyObject *unaryop_type;
    PyObject *upper;
    PyObject *value;
    PyObject *values;
    PyObject *vararg;
    PyObject *withitem_type;
};

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_AST_STATE_H */

