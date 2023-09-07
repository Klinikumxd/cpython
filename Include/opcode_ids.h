// This file is generated by Tools/cases_generator/generate_cases.py
// from:
//   Python/bytecodes.c
// Do not edit!

#ifndef Py_OPCODE_IDS_H
#define Py_OPCODE_IDS_H
#ifdef __cplusplus
extern "C" {
#endif

/* Instruction opcodes for compiled code */
#define CACHE                                    0
#define BEFORE_ASYNC_WITH                        1
#define BEFORE_WITH                              2
#define BINARY_OP_ADD_FLOAT                      3
#define BINARY_OP_ADD_INT                        4
#define BINARY_OP_ADD_UNICODE                    5
#define BINARY_OP_INPLACE_ADD_UNICODE            6
#define BINARY_OP_MULTIPLY_FLOAT                 7
#define BINARY_OP_MULTIPLY_INT                   8
#define BINARY_OP_SUBTRACT_FLOAT                 9
#define BINARY_OP_SUBTRACT_INT                  10
#define BINARY_SLICE                            11
#define BINARY_SUBSCR                           12
#define BINARY_SUBSCR_DICT                      13
#define BINARY_SUBSCR_GETITEM                   14
#define BINARY_SUBSCR_LIST_INT                  15
#define BINARY_SUBSCR_STR_INT                   16
#define RESERVED                                17
#define BINARY_SUBSCR_TUPLE_INT                 18
#define CHECK_EG_MATCH                          19
#define CHECK_EXC_MATCH                         20
#define CLEANUP_THROW                           21
#define DELETE_SUBSCR                           22
#define END_ASYNC_FOR                           23
#define END_FOR                                 24
#define END_SEND                                25
#define EXIT_INIT_CHECK                         26
#define FORMAT_SIMPLE                           27
#define FORMAT_WITH_SPEC                        28
#define GET_AITER                               29
#define GET_ANEXT                               30
#define GET_ITER                                31
#define GET_LEN                                 32
#define GET_YIELD_FROM_ITER                     33
#define INTERPRETER_EXIT                        34
#define LOAD_ASSERTION_ERROR                    35
#define LOAD_BUILD_CLASS                        36
#define LOAD_LOCALS                             37
#define MAKE_FUNCTION                           38
#define MATCH_KEYS                              39
#define MATCH_MAPPING                           40
#define MATCH_SEQUENCE                          41
#define NOP                                     42
#define POP_EXCEPT                              43
#define POP_TOP                                 44
#define PUSH_EXC_INFO                           45
#define PUSH_NULL                               46
#define RESUME_CHECK                            47
#define RETURN_GENERATOR                        48
#define RETURN_VALUE                            49
#define SETUP_ANNOTATIONS                       50
#define STORE_ATTR_INSTANCE_VALUE               51
#define STORE_ATTR_SLOT                         52
#define STORE_SLICE                             53
#define STORE_SUBSCR                            54
#define STORE_SUBSCR_DICT                       55
#define STORE_SUBSCR_LIST_INT                   56
#define TO_BOOL                                 57
#define TO_BOOL_ALWAYS_TRUE                     58
#define TO_BOOL_BOOL                            59
#define TO_BOOL_INT                             60
#define TO_BOOL_LIST                            61
#define TO_BOOL_NONE                            62
#define TO_BOOL_STR                             63
#define UNARY_INVERT                            64
#define UNARY_NEGATIVE                          65
#define UNARY_NOT                               66
#define WITH_EXCEPT_START                       67
#define HAVE_ARGUMENT                           68
#define BINARY_OP                               68
#define BUILD_CONST_KEY_MAP                     69
#define BUILD_LIST                              70
#define BUILD_MAP                               71
#define BUILD_SET                               72
#define BUILD_SLICE                             73
#define BUILD_STRING                            74
#define BUILD_TUPLE                             75
#define CALL                                    76
#define CALL_BOUND_METHOD_EXACT_ARGS            77
#define CALL_BUILTIN_CLASS                      78
#define CALL_BUILTIN_FAST_WITH_KEYWORDS         79
#define CALL_FUNCTION_EX                        80
#define CALL_INTRINSIC_1                        81
#define CALL_INTRINSIC_2                        82
#define CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS  83
#define CALL_NO_KW_ALLOC_AND_ENTER_INIT         84
#define CALL_NO_KW_BUILTIN_FAST                 85
#define CALL_NO_KW_BUILTIN_O                    86
#define CALL_NO_KW_ISINSTANCE                   87
#define CALL_NO_KW_LEN                          88
#define CALL_NO_KW_LIST_APPEND                  89
#define CALL_NO_KW_METHOD_DESCRIPTOR_FAST       90
#define CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS     91
#define CALL_NO_KW_METHOD_DESCRIPTOR_O          92
#define CALL_NO_KW_STR_1                        93
#define CALL_NO_KW_TUPLE_1                      94
#define CALL_NO_KW_TYPE_1                       95
#define CALL_PY_EXACT_ARGS                      96
#define CALL_PY_WITH_DEFAULTS                   97
#define COMPARE_OP                              98
#define COMPARE_OP_FLOAT                        99
#define COMPARE_OP_INT                         100
#define COMPARE_OP_STR                         101
#define CONTAINS_OP                            102
#define CONVERT_VALUE                          103
#define COPY                                   104
#define COPY_FREE_VARS                         105
#define DELETE_ATTR                            106
#define DELETE_DEREF                           107
#define DELETE_FAST                            108
#define DELETE_GLOBAL                          109
#define DELETE_NAME                            110
#define DICT_MERGE                             111
#define DICT_UPDATE                            112
#define ENTER_EXECUTOR                         113
#define EXTENDED_ARG                           114
#define FOR_ITER                               115
#define FOR_ITER_GEN                           116
#define FOR_ITER_LIST                          117
#define FOR_ITER_RANGE                         118
#define FOR_ITER_TUPLE                         119
#define GET_AWAITABLE                          120
#define IMPORT_FROM                            121
#define IMPORT_NAME                            122
#define IS_OP                                  123
#define JUMP_BACKWARD                          124
#define JUMP_BACKWARD_NO_INTERRUPT             125
#define JUMP_FORWARD                           126
#define KW_NAMES                               127
#define LIST_APPEND                            128
#define LIST_EXTEND                            129
#define LOAD_ATTR                              130
#define LOAD_ATTR_CLASS                        131
#define LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN      132
#define LOAD_ATTR_INSTANCE_VALUE               133
#define LOAD_ATTR_METHOD_LAZY_DICT             134
#define LOAD_ATTR_METHOD_NO_DICT               135
#define LOAD_ATTR_METHOD_WITH_VALUES           136
#define LOAD_ATTR_MODULE                       137
#define LOAD_ATTR_NONDESCRIPTOR_NO_DICT        138
#define LOAD_ATTR_NONDESCRIPTOR_WITH_VALUES    139
#define LOAD_ATTR_PROPERTY                     140
#define LOAD_ATTR_SLOT                         141
#define LOAD_ATTR_WITH_HINT                    142
#define LOAD_CONST                             143
#define LOAD_DEREF                             144
#define LOAD_FAST                              145
#define LOAD_FAST_AND_CLEAR                    146
#define LOAD_FAST_CHECK                        147
#define LOAD_FAST_LOAD_FAST                    148
#define LOAD_FROM_DICT_OR_DEREF                149
#define LOAD_FROM_DICT_OR_GLOBALS              150
#define LOAD_GLOBAL                            151
#define LOAD_GLOBAL_BUILTIN                    152
#define LOAD_GLOBAL_MODULE                     153
#define LOAD_NAME                              154
#define LOAD_SUPER_ATTR                        155
#define LOAD_SUPER_ATTR_ATTR                   156
#define LOAD_SUPER_ATTR_METHOD                 157
#define MAKE_CELL                              158
#define MAP_ADD                                159
#define MATCH_CLASS                            160
#define POP_JUMP_IF_FALSE                      161
#define POP_JUMP_IF_NONE                       162
#define POP_JUMP_IF_NOT_NONE                   163
#define POP_JUMP_IF_TRUE                       164
#define RAISE_VARARGS                          165
#define RESUME                                 166
#define RERAISE                                167
#define RETURN_CONST                           168
#define SEND                                   169
#define SEND_GEN                               170
#define SET_ADD                                171
#define SET_FUNCTION_ATTRIBUTE                 172
#define SET_UPDATE                             173
#define STORE_ATTR                             174
#define STORE_ATTR_WITH_HINT                   175
#define STORE_DEREF                            176
#define STORE_FAST                             177
#define STORE_FAST_LOAD_FAST                   178
#define STORE_FAST_STORE_FAST                  179
#define STORE_GLOBAL                           180
#define STORE_NAME                             181
#define SWAP                                   182
#define UNPACK_EX                              183
#define UNPACK_SEQUENCE                        184
#define UNPACK_SEQUENCE_LIST                   185
#define UNPACK_SEQUENCE_TUPLE                  186
#define UNPACK_SEQUENCE_TWO_TUPLE              187
#define YIELD_VALUE                            188
#define MIN_INSTRUMENTED_OPCODE                237
#define INSTRUMENTED_RESUME                    237
#define INSTRUMENTED_END_FOR                   238
#define INSTRUMENTED_END_SEND                  239
#define INSTRUMENTED_RETURN_VALUE              240
#define INSTRUMENTED_RETURN_CONST              241
#define INSTRUMENTED_YIELD_VALUE               242
#define INSTRUMENTED_LOAD_SUPER_ATTR           243
#define INSTRUMENTED_FOR_ITER                  244
#define INSTRUMENTED_CALL                      245
#define INSTRUMENTED_CALL_FUNCTION_EX          246
#define INSTRUMENTED_INSTRUCTION               247
#define INSTRUMENTED_JUMP_FORWARD              248
#define INSTRUMENTED_JUMP_BACKWARD             249
#define INSTRUMENTED_POP_JUMP_IF_TRUE          250
#define INSTRUMENTED_POP_JUMP_IF_FALSE         251
#define INSTRUMENTED_POP_JUMP_IF_NONE          252
#define INSTRUMENTED_POP_JUMP_IF_NOT_NONE      253
#define INSTRUMENTED_LINE                      254
#define JUMP                                   256
#define JUMP_NO_INTERRUPT                      257
#define LOAD_CLOSURE                           258
#define LOAD_METHOD                            259
#define LOAD_SUPER_METHOD                      260
#define LOAD_ZERO_SUPER_ATTR                   261
#define LOAD_ZERO_SUPER_METHOD                 262
#define POP_BLOCK                              263
#define SETUP_CLEANUP                          264
#define SETUP_FINALLY                          265
#define SETUP_WITH                             266
#define STORE_FAST_MAYBE_NULL                  267

#ifdef __cplusplus
}
#endif
#endif /* !Py_OPCODE_IDS_H */
