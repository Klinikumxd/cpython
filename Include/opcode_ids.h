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
#define BINARY_OP_INPLACE_ADD_UNICODE            3
#define BINARY_SLICE                             4
#define BINARY_SUBSCR                            5
#define CHECK_EG_MATCH                           6
#define CHECK_EXC_MATCH                          7
#define CLEANUP_THROW                            8
#define DELETE_SUBSCR                            9
#define END_ASYNC_FOR                           10
#define END_FOR                                 11
#define END_SEND                                12
#define EXIT_INIT_CHECK                         13
#define FORMAT_SIMPLE                           14
#define FORMAT_WITH_SPEC                        15
#define GET_AITER                               16
#define RESERVED                                17
#define GET_ANEXT                               18
#define GET_ITER                                19
#define GET_LEN                                 20
#define GET_YIELD_FROM_ITER                     21
#define INTERPRETER_EXIT                        22
#define LOAD_ASSERTION_ERROR                    23
#define LOAD_BUILD_CLASS                        24
#define LOAD_LOCALS                             25
#define MAKE_FUNCTION                           26
#define MATCH_KEYS                              27
#define MATCH_MAPPING                           28
#define MATCH_SEQUENCE                          29
#define NOP                                     30
#define POP_EXCEPT                              31
#define POP_TOP                                 32
#define PUSH_EXC_INFO                           33
#define PUSH_NULL                               34
#define RETURN_GENERATOR                        35
#define RETURN_VALUE                            36
#define SETUP_ANNOTATIONS                       37
#define STORE_SLICE                             38
#define STORE_SUBSCR                            39
#define TO_BOOL                                 40
#define UNARY_INVERT                            41
#define UNARY_NEGATIVE                          42
#define UNARY_NOT                               43
#define WITH_EXCEPT_START                       44
#define HAVE_ARGUMENT                           45
#define BINARY_OP                               45
#define BUILD_CONST_KEY_MAP                     46
#define BUILD_LIST                              47
#define BUILD_MAP                               48
#define BUILD_SET                               49
#define BUILD_SLICE                             50
#define BUILD_STRING                            51
#define BUILD_TUPLE                             52
#define CALL                                    53
#define CALL_FUNCTION_EX                        54
#define CALL_INTRINSIC_1                        55
#define CALL_INTRINSIC_2                        56
#define COMPARE_OP                              57
#define CONTAINS_OP                             58
#define CONVERT_VALUE                           59
#define COPY                                    60
#define COPY_FREE_VARS                          61
#define DELETE_ATTR                             62
#define DELETE_DEREF                            63
#define DELETE_FAST                             64
#define DELETE_GLOBAL                           65
#define DELETE_NAME                             66
#define DICT_MERGE                              67
#define DICT_UPDATE                             68
#define ENTER_EXECUTOR                          69
#define EXTENDED_ARG                            70
#define FOR_ITER                                71
#define GET_AWAITABLE                           72
#define IMPORT_FROM                             73
#define IMPORT_NAME                             74
#define IS_OP                                   75
#define JUMP_BACKWARD                           76
#define JUMP_BACKWARD_NO_INTERRUPT              77
#define JUMP_FORWARD                            78
#define KW_NAMES                                79
#define LIST_APPEND                             80
#define LIST_EXTEND                             81
#define LOAD_ATTR                               82
#define LOAD_CONST                              83
#define LOAD_DEREF                              84
#define LOAD_FAST                               85
#define LOAD_FAST_AND_CLEAR                     86
#define LOAD_FAST_CHECK                         87
#define LOAD_FAST_LOAD_FAST                     88
#define LOAD_FROM_DICT_OR_DEREF                 89
#define LOAD_FROM_DICT_OR_GLOBALS               90
#define LOAD_GLOBAL                             91
#define LOAD_NAME                               92
#define LOAD_SUPER_ATTR                         93
#define MAKE_CELL                               94
#define MAP_ADD                                 95
#define MATCH_CLASS                             96
#define POP_JUMP_IF_FALSE                       97
#define POP_JUMP_IF_NONE                        98
#define POP_JUMP_IF_NOT_NONE                    99
#define POP_JUMP_IF_TRUE                       100
#define RAISE_VARARGS                          101
#define RERAISE                                102
#define RETURN_CONST                           103
#define SEND                                   104
#define SET_ADD                                105
#define SET_FUNCTION_ATTRIBUTE                 106
#define SET_UPDATE                             107
#define STORE_ATTR                             108
#define STORE_DEREF                            109
#define STORE_FAST                             110
#define STORE_FAST_LOAD_FAST                   111
#define STORE_FAST_STORE_FAST                  112
#define STORE_GLOBAL                           113
#define STORE_NAME                             114
#define SWAP                                   115
#define UNPACK_EX                              116
#define UNPACK_SEQUENCE                        117
#define YIELD_VALUE                            118
#define RESUME                                 149
#define BINARY_OP_ADD_FLOAT                    150
#define BINARY_OP_ADD_INT                      151
#define BINARY_OP_ADD_UNICODE                  152
#define BINARY_OP_MULTIPLY_FLOAT               153
#define BINARY_OP_MULTIPLY_INT                 154
#define BINARY_OP_SUBTRACT_FLOAT               155
#define BINARY_OP_SUBTRACT_INT                 156
#define BINARY_SUBSCR_DICT                     157
#define BINARY_SUBSCR_GETITEM                  158
#define BINARY_SUBSCR_LIST_INT                 159
#define BINARY_SUBSCR_STR_INT                  160
#define BINARY_SUBSCR_TUPLE_INT                161
#define CALL_BOUND_METHOD_EXACT_ARGS           162
#define CALL_BUILTIN_CLASS                     163
#define CALL_BUILTIN_FAST_WITH_KEYWORDS        164
#define CALL_METHOD_DESCRIPTOR_FAST_WITH_KEYWORDS 165
#define CALL_NO_KW_ALLOC_AND_ENTER_INIT        166
#define CALL_NO_KW_BUILTIN_FAST                167
#define CALL_NO_KW_BUILTIN_O                   168
#define CALL_NO_KW_ISINSTANCE                  169
#define CALL_NO_KW_LEN                         170
#define CALL_NO_KW_LIST_APPEND                 171
#define CALL_NO_KW_METHOD_DESCRIPTOR_FAST      172
#define CALL_NO_KW_METHOD_DESCRIPTOR_NOARGS    173
#define CALL_NO_KW_METHOD_DESCRIPTOR_O         174
#define CALL_NO_KW_STR_1                       175
#define CALL_NO_KW_TUPLE_1                     176
#define CALL_NO_KW_TYPE_1                      177
#define CALL_PY_EXACT_ARGS                     178
#define CALL_PY_WITH_DEFAULTS                  179
#define COMPARE_OP_FLOAT                       180
#define COMPARE_OP_INT                         181
#define COMPARE_OP_STR                         182
#define FOR_ITER_GEN                           183
#define FOR_ITER_LIST                          184
#define FOR_ITER_RANGE                         185
#define FOR_ITER_TUPLE                         186
#define LOAD_ATTR_CLASS                        187
#define LOAD_ATTR_GETATTRIBUTE_OVERRIDDEN      188
#define LOAD_ATTR_INSTANCE_VALUE               189
#define LOAD_ATTR_METHOD_LAZY_DICT             190
#define LOAD_ATTR_METHOD_NO_DICT               191
#define LOAD_ATTR_METHOD_WITH_VALUES           192
#define LOAD_ATTR_MODULE                       193
#define LOAD_ATTR_NONDESCRIPTOR_NO_DICT        194
#define LOAD_ATTR_NONDESCRIPTOR_WITH_VALUES    195
#define LOAD_ATTR_PROPERTY                     196
#define LOAD_ATTR_SLOT                         197
#define LOAD_ATTR_WITH_HINT                    198
#define LOAD_GLOBAL_BUILTIN                    199
#define LOAD_GLOBAL_MODULE                     200
#define LOAD_SUPER_ATTR_ATTR                   201
#define LOAD_SUPER_ATTR_METHOD                 202
#define RESUME_CHECK                           203
#define SEND_GEN                               204
#define STORE_ATTR_INSTANCE_VALUE              205
#define STORE_ATTR_SLOT                        206
#define STORE_ATTR_WITH_HINT                   207
#define STORE_SUBSCR_DICT                      208
#define STORE_SUBSCR_LIST_INT                  209
#define TO_BOOL_ALWAYS_TRUE                    210
#define TO_BOOL_BOOL                           211
#define TO_BOOL_INT                            212
#define TO_BOOL_LIST                           213
#define TO_BOOL_NONE                           214
#define TO_BOOL_STR                            215
#define UNPACK_SEQUENCE_LIST                   216
#define UNPACK_SEQUENCE_TUPLE                  217
#define UNPACK_SEQUENCE_TWO_TUPLE              218
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
