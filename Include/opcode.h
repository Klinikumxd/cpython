// Auto-generated by Tools/build/generate_opcode_h.py from Lib/opcode.py

#ifndef Py_OPCODE_H
#define Py_OPCODE_H
#ifdef __cplusplus
extern "C" {
#endif


/* Instruction opcodes for compiled code */
#define CACHE                                    0
#define POP_TOP                                  1
#define PUSH_NULL                                2
#define END_FOR                                  4
#define NOP                                      9
#define UNARY_POSITIVE                          10
#define UNARY_NEGATIVE                          11
#define UNARY_NOT                               12
#define UNARY_INVERT                            15
#define BINARY_SUBSCR                           25
#define BINARY_SLICE                            26
#define STORE_SLICE                             27
#define GET_LEN                                 30
#define MATCH_MAPPING                           31
#define MATCH_SEQUENCE                          32
#define MATCH_KEYS                              33
#define PUSH_EXC_INFO                           35
#define CHECK_EXC_MATCH                         36
#define CHECK_EG_MATCH                          37
#define WITH_EXCEPT_START                       49
#define GET_AITER                               50
#define GET_ANEXT                               51
#define BEFORE_ASYNC_WITH                       52
#define BEFORE_WITH                             53
#define END_ASYNC_FOR                           54
#define CLEANUP_THROW                           55
#define STORE_SUBSCR                            60
#define DELETE_SUBSCR                           61
#define STOPITERATION_ERROR                     63
#define GET_ITER                                68
#define GET_YIELD_FROM_ITER                     69
#define PRINT_EXPR                              70
#define LOAD_BUILD_CLASS                        71
#define LOAD_ASSERTION_ERROR                    74
#define RETURN_GENERATOR                        75
#define LIST_TO_TUPLE                           82
#define RETURN_VALUE                            83
#define IMPORT_STAR                             84
#define SETUP_ANNOTATIONS                       85
#define ASYNC_GEN_WRAP                          87
#define PREP_RERAISE_STAR                       88
#define POP_EXCEPT                              89
#define HAVE_ARGUMENT                           90
#define STORE_NAME                              90
#define DELETE_NAME                             91
#define UNPACK_SEQUENCE                         92
#define FOR_ITER                                93
#define UNPACK_EX                               94
#define STORE_ATTR                              95
#define DELETE_ATTR                             96
#define STORE_GLOBAL                            97
#define DELETE_GLOBAL                           98
#define SWAP                                    99
#define LOAD_CONST                             100
#define LOAD_NAME                              101
#define BUILD_TUPLE                            102
#define BUILD_LIST                             103
#define BUILD_SET                              104
#define BUILD_MAP                              105
#define LOAD_ATTR                              106
#define COMPARE_OP                             107
#define IMPORT_NAME                            108
#define IMPORT_FROM                            109
#define JUMP_FORWARD                           110
#define JUMP_IF_FALSE_OR_POP                   111
#define JUMP_IF_TRUE_OR_POP                    112
#define POP_JUMP_IF_FALSE                      114
#define POP_JUMP_IF_TRUE                       115
#define LOAD_GLOBAL                            116
#define IS_OP                                  117
#define CONTAINS_OP                            118
#define RERAISE                                119
#define COPY                                   120
#define BINARY_OP                              122
#define SEND                                   123
#define LOAD_FAST                              124
#define STORE_FAST                             125
#define DELETE_FAST                            126
#define LOAD_FAST_CHECK                        127
#define POP_JUMP_IF_NOT_NONE                   128
#define POP_JUMP_IF_NONE                       129
#define RAISE_VARARGS                          130
#define GET_AWAITABLE                          131
#define MAKE_FUNCTION                          132
#define BUILD_SLICE                            133
#define JUMP_BACKWARD_NO_INTERRUPT             134
#define MAKE_CELL                              135
#define LOAD_CLOSURE                           136
#define LOAD_DEREF                             137
#define STORE_DEREF                            138
#define DELETE_DEREF                           139
#define JUMP_BACKWARD                          140
#define CALL_FUNCTION_EX                       142
#define EXTENDED_ARG                           144
#define LIST_APPEND                            145
#define SET_ADD                                146
#define MAP_ADD                                147
#define LOAD_CLASSDEREF                        148
#define COPY_FREE_VARS                         149
#define YIELD_VALUE                            150
#define RESUME                                 151
#define MATCH_CLASS                            152
#define FORMAT_VALUE                           155
#define BUILD_CONST_KEY_MAP                    156
#define BUILD_STRING                           157
#define LIST_EXTEND                            162
#define SET_UPDATE                             163
#define DICT_MERGE                             164
#define DICT_UPDATE                            165
#define CALL                                   171
#define KW_NAMES                               172
#define MIN_PSEUDO_OPCODE                      256
#define SETUP_FINALLY                          256
#define SETUP_CLEANUP                          257
#define SETUP_WITH                             258
#define POP_BLOCK                              259
#define JUMP                                   260
#define JUMP_NO_INTERRUPT                      261
#define LOAD_METHOD                            262
#define MAX_PSEUDO_OPCODE                      262
#define BINARY_OP_ADAPTIVE                       3
#define BINARY_OP_ADD_FLOAT                      5
#define BINARY_OP_ADD_INT                        6
#define BINARY_OP_ADD_UNICODE                    7
#define BINARY_OP_INPLACE_ADD_UNICODE            8
#define BINARY_OP_MULTIPLY_FLOAT                13
#define BINARY_OP_MULTIPLY_INT                  14
#define BINARY_OP_SUBTRACT_FLOAT                16
#define BINARY_OP_SUBTRACT_INT                  17
#define BINARY_SUBSCR_ADAPTIVE                  18
#define BINARY_SUBSCR_DICT                      19
#define BINARY_SUBSCR_GETITEM                   20
#define BINARY_SUBSCR_LIST_INT                  21
#define BINARY_SUBSCR_TUPLE_INT                 22
#define CALL_ADAPTIVE                           23
#define CALL_PY_EXACT_ARGS                      24
#define CALL_PY_WITH_DEFAULTS                   28
#define CALL_BOUND_METHOD_EXACT_ARGS            29
#define CALL_BUILTIN_CLASS                      34
#define CALL_BUILTIN_FAST_WITH_KEYWORDS         38
#define CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS  39
#define CALL_NO_KW_BUILTIN_FAST                 40
#define CALL_NO_KW_BUILTIN_O                    41
#define CALL_NO_KW_ISINSTANCE                   42
#define CALL_NO_KW_LEN                          43
#define CALL_NO_KW_LIST_APPEND                  44
#define CALL_NO_KW_METHOD_DESCRIPTOR_FAST       45
#define CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS     46
#define CALL_NO_KW_METHOD_DESCRIPTOR_O          47
#define CALL_NO_KW_STR_1                        48
#define CALL_NO_KW_TUPLE_1                      56
#define CALL_NO_KW_TYPE_1                       57
#define COMPARE_OP_ADAPTIVE                     58
#define COMPARE_OP_FLOAT_JUMP                   59
#define COMPARE_OP_INT_JUMP                     62
#define COMPARE_OP_STR_JUMP                     64
#define EXTENDED_ARG_QUICK                      65
#define FOR_ITER_ADAPTIVE                       66
#define FOR_ITER_LIST                           67
#define FOR_ITER_RANGE                          72
#define LOAD_ATTR_ADAPTIVE                      73
#define LOAD_ATTR_CLASS                         76
#define LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN       77
#define LOAD_ATTR_INSTANCE_VALUE                78
#define LOAD_ATTR_MODULE                        79
#define LOAD_ATTR_PROPERTY                      80
#define LOAD_ATTR_SLOT                          81
#define LOAD_ATTR_WITH_HINT                     86
#define LOAD_ATTR_METHOD_LAZY_DICT             113
#define LOAD_ATTR_METHOD_NO_DICT               121
#define LOAD_ATTR_METHOD_WITH_DICT             141
#define LOAD_ATTR_METHOD_WITH_VALUES           143
#define LOAD_CONST__LOAD_FAST                  153
#define LOAD_FAST__LOAD_CONST                  154
#define LOAD_FAST__LOAD_FAST                   158
#define LOAD_GLOBAL_ADAPTIVE                   159
#define LOAD_GLOBAL_BUILTIN                    160
#define LOAD_GLOBAL_MODULE                     161
#define STORE_ATTR_ADAPTIVE                    166
#define STORE_ATTR_INSTANCE_VALUE              167
#define STORE_ATTR_SLOT                        168
#define STORE_ATTR_WITH_HINT                   169
#define STORE_FAST__LOAD_FAST                  170
#define STORE_FAST__STORE_FAST                 173
#define STORE_SUBSCR_ADAPTIVE                  174
#define STORE_SUBSCR_DICT                      175
#define STORE_SUBSCR_LIST_INT                  176
#define UNPACK_SEQUENCE_ADAPTIVE               177
#define UNPACK_SEQUENCE_LIST                   178
#define UNPACK_SEQUENCE_TUPLE                  179
#define UNPACK_SEQUENCE_TWO_TUPLE              180
#define DO_TRACING                             255

#define HAS_ARG(op) ((((op) >= HAVE_ARGUMENT) && (!IS_PSEUDO_OPCODE(op)))\
    || ((op) == JUMP) \
    || ((op) == JUMP_NO_INTERRUPT) \
    || ((op) == LOAD_METHOD) \
    )

#define HAS_CONST(op) (false\
    || ((op) == LOAD_CONST) \
    || ((op) == KW_NAMES) \
    )

#define NB_ADD                                   0
#define NB_AND                                   1
#define NB_FLOOR_DIVIDE                          2
#define NB_LSHIFT                                3
#define NB_MATRIX_MULTIPLY                       4
#define NB_MULTIPLY                              5
#define NB_REMAINDER                             6
#define NB_OR                                    7
#define NB_POWER                                 8
#define NB_RSHIFT                                9
#define NB_SUBTRACT                             10
#define NB_TRUE_DIVIDE                          11
#define NB_XOR                                  12
#define NB_INPLACE_ADD                          13
#define NB_INPLACE_AND                          14
#define NB_INPLACE_FLOOR_DIVIDE                 15
#define NB_INPLACE_LSHIFT                       16
#define NB_INPLACE_MATRIX_MULTIPLY              17
#define NB_INPLACE_MULTIPLY                     18
#define NB_INPLACE_REMAINDER                    19
#define NB_INPLACE_OR                           20
#define NB_INPLACE_POWER                        21
#define NB_INPLACE_RSHIFT                       22
#define NB_INPLACE_SUBTRACT                     23
#define NB_INPLACE_TRUE_DIVIDE                  24
#define NB_INPLACE_XOR                          25


#define IS_PSEUDO_OPCODE(op) (((op) >= MIN_PSEUDO_OPCODE) && ((op) <= MAX_PSEUDO_OPCODE))

#ifdef __cplusplus
}
#endif
#endif /* !Py_OPCODE_H */
