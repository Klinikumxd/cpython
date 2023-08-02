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
#define INTERPRETER_EXIT                         3
#define END_FOR                                  4
#define END_SEND                                 5
#define TO_BOOL                                  6
#define NOP                                      9
#define UNARY_NEGATIVE                          11
#define UNARY_NOT                               12
#define UNARY_INVERT                            15
#define EXIT_INIT_CHECK                         16
#define RESERVED                                17
#define MAKE_FUNCTION                           24
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
#define FORMAT_SIMPLE                           40
#define FORMAT_WITH_SPEC                        41
#define WITH_EXCEPT_START                       49
#define GET_AITER                               50
#define GET_ANEXT                               51
#define BEFORE_ASYNC_WITH                       52
#define BEFORE_WITH                             53
#define END_ASYNC_FOR                           54
#define CLEANUP_THROW                           55
#define STORE_SUBSCR                            60
#define DELETE_SUBSCR                           61
#define GET_ITER                                68
#define GET_YIELD_FROM_ITER                     69
#define LOAD_BUILD_CLASS                        71
#define LOAD_ASSERTION_ERROR                    74
#define RETURN_GENERATOR                        75
#define RETURN_VALUE                            83
#define SETUP_ANNOTATIONS                       85
#define LOAD_LOCALS                             87
#define POP_EXCEPT                              89
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
#define POP_JUMP_IF_FALSE                      114
#define POP_JUMP_IF_TRUE                       115
#define LOAD_GLOBAL                            116
#define IS_OP                                  117
#define CONTAINS_OP                            118
#define RERAISE                                119
#define COPY                                   120
#define RETURN_CONST                           121
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
#define BUILD_SLICE                            133
#define JUMP_BACKWARD_NO_INTERRUPT             134
#define MAKE_CELL                              135
#define LOAD_DEREF                             137
#define STORE_DEREF                            138
#define DELETE_DEREF                           139
#define JUMP_BACKWARD                          140
#define LOAD_SUPER_ATTR                        141
#define CALL_FUNCTION_EX                       142
#define LOAD_FAST_AND_CLEAR                    143
#define EXTENDED_ARG                           144
#define LIST_APPEND                            145
#define SET_ADD                                146
#define MAP_ADD                                147
#define COPY_FREE_VARS                         149
#define YIELD_VALUE                            150
#define RESUME                                 151
#define MATCH_CLASS                            152
#define BUILD_CONST_KEY_MAP                    156
#define BUILD_STRING                           157
#define CONVERT_VALUE                          158
#define LIST_EXTEND                            162
#define SET_UPDATE                             163
#define DICT_MERGE                             164
#define DICT_UPDATE                            165
#define LOAD_FAST_LOAD_FAST                    168
#define STORE_FAST_LOAD_FAST                   169
#define STORE_FAST_STORE_FAST                  170
#define CALL                                   171
#define KW_NAMES                               172
#define CALL_INTRINSIC_1                       173
#define CALL_INTRINSIC_2                       174
#define LOAD_FROM_DICT_OR_GLOBALS              175
#define LOAD_FROM_DICT_OR_DEREF                176
#define SET_FUNCTION_ATTRIBUTE                 177
#define ENTER_EXECUTOR                         230
#define MIN_INSTRUMENTED_OPCODE                237
#define INSTRUMENTED_LOAD_SUPER_ATTR           237
#define INSTRUMENTED_POP_JUMP_IF_NONE          238
#define INSTRUMENTED_POP_JUMP_IF_NOT_NONE      239
#define INSTRUMENTED_RESUME                    240
#define INSTRUMENTED_CALL                      241
#define INSTRUMENTED_RETURN_VALUE              242
#define INSTRUMENTED_YIELD_VALUE               243
#define INSTRUMENTED_CALL_FUNCTION_EX          244
#define INSTRUMENTED_JUMP_FORWARD              245
#define INSTRUMENTED_JUMP_BACKWARD             246
#define INSTRUMENTED_RETURN_CONST              247
#define INSTRUMENTED_FOR_ITER                  248
#define INSTRUMENTED_POP_JUMP_IF_FALSE         249
#define INSTRUMENTED_POP_JUMP_IF_TRUE          250
#define INSTRUMENTED_END_FOR                   251
#define INSTRUMENTED_END_SEND                  252
#define INSTRUMENTED_INSTRUCTION               253
#define INSTRUMENTED_LINE                      254
#define SETUP_FINALLY                          256
#define SETUP_CLEANUP                          257
#define SETUP_WITH                             258
#define POP_BLOCK                              259
#define JUMP                                   260
#define JUMP_NO_INTERRUPT                      261
#define LOAD_METHOD                            262
#define LOAD_SUPER_METHOD                      263
#define LOAD_ZERO_SUPER_METHOD                 264
#define LOAD_ZERO_SUPER_ATTR                   265
#define STORE_FAST_MAYBE_NULL                  266
#define LOAD_CLOSURE                           267
#define TO_BOOL_ALWAYS_TRUE                      7
#define TO_BOOL_BOOL                             8
#define TO_BOOL_INT                             10
#define TO_BOOL_LIST                            13
#define TO_BOOL_NONE                            14
#define TO_BOOL_STR                             18
#define BINARY_OP_MULTIPLY_INT                  19
#define BINARY_OP_ADD_INT                       20
#define BINARY_OP_SUBTRACT_INT                  21
#define BINARY_OP_MULTIPLY_FLOAT                22
#define BINARY_OP_ADD_FLOAT                     23
#define BINARY_OP_SUBTRACT_FLOAT                28
#define BINARY_OP_ADD_UNICODE                   29
#define BINARY_OP_INPLACE_ADD_UNICODE           34
#define BINARY_SUBSCR_DICT                      38
#define BINARY_SUBSCR_GETITEM                   39
#define BINARY_SUBSCR_LIST_INT                  42
#define BINARY_SUBSCR_TUPLE_INT                 43
#define STORE_SUBSCR_DICT                       44
#define STORE_SUBSCR_LIST_INT                   45
#define SEND_GEN                                46
#define UNPACK_SEQUENCE_TWO_TUPLE               47
#define UNPACK_SEQUENCE_TUPLE                   48
#define UNPACK_SEQUENCE_LIST                    56
#define STORE_ATTR_INSTANCE_VALUE               57
#define STORE_ATTR_SLOT                         58
#define STORE_ATTR_WITH_HINT                    59
#define LOAD_GLOBAL_MODULE                      62
#define LOAD_GLOBAL_BUILTIN                     63
#define LOAD_SUPER_ATTR_ATTR                    64
#define LOAD_SUPER_ATTR_METHOD                  65
#define LOAD_ATTR_INSTANCE_VALUE                66
#define LOAD_ATTR_MODULE                        67
#define LOAD_ATTR_WITH_HINT                     70
#define LOAD_ATTR_SLOT                          72
#define LOAD_ATTR_CLASS                         73
#define LOAD_ATTR_PROPERTY                      76
#define LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN       77
#define LOAD_ATTR_METHOD_WITH_VALUES            78
#define LOAD_ATTR_METHOD_NO_DICT                79
#define LOAD_ATTR_METHOD_LAZY_DICT              80
#define LOAD_ATTR_NONDESCRIPTOR_WITH_VALUES     81
#define LOAD_ATTR_NONDESCRIPTOR_NO_DICT         82
#define COMPARE_OP_FLOAT                        84
#define COMPARE_OP_INT                          86
#define COMPARE_OP_STR                          88
#define FOR_ITER_LIST                          111
#define FOR_ITER_TUPLE                         112
#define FOR_ITER_RANGE                         113
#define FOR_ITER_GEN                           132
#define CALL_BOUND_METHOD_EXACT_ARGS           136
#define CALL_PY_EXACT_ARGS                     148
#define CALL_PY_WITH_DEFAULTS                  153
#define CALL_NO_KW_TYPE_1                      154
#define CALL_NO_KW_STR_1                       155
#define CALL_NO_KW_TUPLE_1                     159
#define CALL_BUILTIN_CLASS                     160
#define CALL_NO_KW_BUILTIN_O                   161
#define CALL_NO_KW_BUILTIN_FAST                166
#define CALL_BUILTIN_FAST_WITH_KEYWORDS        167
#define CALL_NO_KW_LEN                         178
#define CALL_NO_KW_ISINSTANCE                  179
#define CALL_NO_KW_LIST_APPEND                 180
#define CALL_NO_KW_METHOD_DESCRIPTOR_O         181
#define CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS 182
#define CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS    183
#define CALL_NO_KW_METHOD_DESCRIPTOR_FAST      184
#define CALL_NO_KW_ALLOC_AND_ENTER_INIT        185

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


#ifdef __cplusplus
}
#endif
#endif /* !Py_OPCODE_H */
