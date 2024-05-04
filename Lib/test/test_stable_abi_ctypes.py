# Generated by Tools/build/stable_abi.py

"""Test that all symbols of the Stable ABI are accessible using ctypes
"""

import sys
import unittest
from test.support.import_helper import import_module
try:
    from _testcapi import get_feature_macros
except ImportError:
    raise unittest.SkipTest("requires _testcapi")

feature_macros = get_feature_macros()

# Stable ABI is incompatible with Py_TRACE_REFS builds due to PyObject
# layout differences.
# See https://github.com/python/cpython/issues/88299#issuecomment-1113366226
if feature_macros['Py_TRACE_REFS']:
    raise unittest.SkipTest("incompatible with Py_TRACE_REFS.")

ctypes_test = import_module('ctypes')

class TestStableABIAvailability(unittest.TestCase):
    def test_available_symbols(self):

        for symbol_name in SYMBOL_NAMES:
            with self.subTest(symbol_name):
                ctypes_test.pythonapi[symbol_name]

    def test_feature_macros(self):
        self.assertEqual(
            set(get_feature_macros()), EXPECTED_FEATURE_MACROS)

    # The feature macros for Windows are used in creating the DLL
    # definition, so they must be known on all platforms.
    # If we are on Windows, we check that the hardcoded data matches
    # the reality.
    @unittest.skipIf(sys.platform != "win32", "Windows specific test")
    def test_windows_feature_macros(self):
        for name, value in WINDOWS_FEATURE_MACROS.items():
            if value != 'maybe':
                with self.subTest(name):
                    self.assertEqual(feature_macros[name], value)

SYMBOL_NAMES = (

    "PyAIter_Check",
    "PyArg_Parse",
    "PyArg_ParseTuple",
    "PyArg_ParseTupleAndKeywords",
    "PyArg_UnpackTuple",
    "PyArg_VaParse",
    "PyArg_VaParseTupleAndKeywords",
    "PyArg_ValidateKeywordArguments",
    "PyBaseObject_Type",
    "PyBool_FromLong",
    "PyBool_Type",
    "PyBuffer_FillContiguousStrides",
    "PyBuffer_FillInfo",
    "PyBuffer_FromContiguous",
    "PyBuffer_GetPointer",
    "PyBuffer_IsContiguous",
    "PyBuffer_Release",
    "PyBuffer_SizeFromFormat",
    "PyBuffer_ToContiguous",
    "PyByteArrayIter_Type",
    "PyByteArray_AsString",
    "PyByteArray_Concat",
    "PyByteArray_FromObject",
    "PyByteArray_FromStringAndSize",
    "PyByteArray_Resize",
    "PyByteArray_Size",
    "PyByteArray_Type",
    "PyBytesIter_Type",
    "PyBytes_AsString",
    "PyBytes_AsStringAndSize",
    "PyBytes_Concat",
    "PyBytes_ConcatAndDel",
    "PyBytes_DecodeEscape",
    "PyBytes_FromFormat",
    "PyBytes_FromFormatV",
    "PyBytes_FromObject",
    "PyBytes_FromString",
    "PyBytes_FromStringAndSize",
    "PyBytes_Repr",
    "PyBytes_Size",
    "PyBytes_Type",
    "PyCFunction_Call",
    "PyCFunction_GetFlags",
    "PyCFunction_GetFunction",
    "PyCFunction_GetSelf",
    "PyCFunction_New",
    "PyCFunction_NewEx",
    "PyCFunction_Type",
    "PyCMethod_New",
    "PyCallIter_New",
    "PyCallIter_Type",
    "PyCallable_Check",
    "PyCapsule_GetContext",
    "PyCapsule_GetDestructor",
    "PyCapsule_GetName",
    "PyCapsule_GetPointer",
    "PyCapsule_Import",
    "PyCapsule_IsValid",
    "PyCapsule_New",
    "PyCapsule_SetContext",
    "PyCapsule_SetDestructor",
    "PyCapsule_SetName",
    "PyCapsule_SetPointer",
    "PyCapsule_Type",
    "PyClassMethodDescr_Type",
    "PyCodec_BackslashReplaceErrors",
    "PyCodec_Decode",
    "PyCodec_Decoder",
    "PyCodec_Encode",
    "PyCodec_Encoder",
    "PyCodec_IgnoreErrors",
    "PyCodec_IncrementalDecoder",
    "PyCodec_IncrementalEncoder",
    "PyCodec_KnownEncoding",
    "PyCodec_LookupError",
    "PyCodec_NameReplaceErrors",
    "PyCodec_Register",
    "PyCodec_RegisterError",
    "PyCodec_ReplaceErrors",
    "PyCodec_StreamReader",
    "PyCodec_StreamWriter",
    "PyCodec_StrictErrors",
    "PyCodec_Unregister",
    "PyCodec_XMLCharRefReplaceErrors",
    "PyComplex_FromDoubles",
    "PyComplex_ImagAsDouble",
    "PyComplex_RealAsDouble",
    "PyComplex_Type",
    "PyDescr_NewClassMethod",
    "PyDescr_NewGetSet",
    "PyDescr_NewMember",
    "PyDescr_NewMethod",
    "PyDictItems_Type",
    "PyDictIterItem_Type",
    "PyDictIterKey_Type",
    "PyDictIterValue_Type",
    "PyDictKeys_Type",
    "PyDictProxy_New",
    "PyDictProxy_Type",
    "PyDictRevIterItem_Type",
    "PyDictRevIterKey_Type",
    "PyDictRevIterValue_Type",
    "PyDictValues_Type",
    "PyDict_Clear",
    "PyDict_Contains",
    "PyDict_Copy",
    "PyDict_DelItem",
    "PyDict_DelItemString",
    "PyDict_GetItem",
    "PyDict_GetItemRef",
    "PyDict_GetItemString",
    "PyDict_GetItemStringRef",
    "PyDict_GetItemWithError",
    "PyDict_Items",
    "PyDict_Keys",
    "PyDict_Merge",
    "PyDict_MergeFromSeq2",
    "PyDict_New",
    "PyDict_Next",
    "PyDict_SetItem",
    "PyDict_SetItemString",
    "PyDict_Size",
    "PyDict_Type",
    "PyDict_Update",
    "PyDict_Values",
    "PyEllipsis_Type",
    "PyEnum_Type",
    "PyErr_BadArgument",
    "PyErr_BadInternalCall",
    "PyErr_CheckSignals",
    "PyErr_Clear",
    "PyErr_Display",
    "PyErr_DisplayException",
    "PyErr_ExceptionMatches",
    "PyErr_Fetch",
    "PyErr_Format",
    "PyErr_FormatV",
    "PyErr_GetExcInfo",
    "PyErr_GetHandledException",
    "PyErr_GetRaisedException",
    "PyErr_GivenExceptionMatches",
    "PyErr_NewException",
    "PyErr_NewExceptionWithDoc",
    "PyErr_NoMemory",
    "PyErr_NormalizeException",
    "PyErr_Occurred",
    "PyErr_Print",
    "PyErr_PrintEx",
    "PyErr_ProgramText",
    "PyErr_ResourceWarning",
    "PyErr_Restore",
    "PyErr_SetExcInfo",
    "PyErr_SetFromErrno",
    "PyErr_SetFromErrnoWithFilename",
    "PyErr_SetFromErrnoWithFilenameObject",
    "PyErr_SetFromErrnoWithFilenameObjects",
    "PyErr_SetHandledException",
    "PyErr_SetImportError",
    "PyErr_SetImportErrorSubclass",
    "PyErr_SetInterrupt",
    "PyErr_SetInterruptEx",
    "PyErr_SetNone",
    "PyErr_SetObject",
    "PyErr_SetRaisedException",
    "PyErr_SetString",
    "PyErr_SyntaxLocation",
    "PyErr_SyntaxLocationEx",
    "PyErr_WarnEx",
    "PyErr_WarnExplicit",
    "PyErr_WarnFormat",
    "PyErr_WriteUnraisable",
    "PyEval_AcquireLock",
    "PyEval_AcquireThread",
    "PyEval_CallFunction",
    "PyEval_CallMethod",
    "PyEval_CallObjectWithKeywords",
    "PyEval_EvalCode",
    "PyEval_EvalCodeEx",
    "PyEval_EvalFrame",
    "PyEval_EvalFrameEx",
    "PyEval_GetBuiltins",
    "PyEval_GetFrame",
    "PyEval_GetFrameBuiltins",
    "PyEval_GetFrameGlobals",
    "PyEval_GetFrameLocals",
    "PyEval_GetFuncDesc",
    "PyEval_GetFuncName",
    "PyEval_GetGlobals",
    "PyEval_GetLocals",
    "PyEval_InitThreads",
    "PyEval_ReleaseLock",
    "PyEval_ReleaseThread",
    "PyEval_RestoreThread",
    "PyEval_SaveThread",
    "PyEval_ThreadsInitialized",
    "PyExc_ArithmeticError",
    "PyExc_AssertionError",
    "PyExc_AttributeError",
    "PyExc_BaseException",
    "PyExc_BaseExceptionGroup",
    "PyExc_BlockingIOError",
    "PyExc_BrokenPipeError",
    "PyExc_BufferError",
    "PyExc_BytesWarning",
    "PyExc_ChildProcessError",
    "PyExc_ConnectionAbortedError",
    "PyExc_ConnectionError",
    "PyExc_ConnectionRefusedError",
    "PyExc_ConnectionResetError",
    "PyExc_DeprecationWarning",
    "PyExc_EOFError",
    "PyExc_EncodingWarning",
    "PyExc_EnvironmentError",
    "PyExc_Exception",
    "PyExc_FileExistsError",
    "PyExc_FileNotFoundError",
    "PyExc_FloatingPointError",
    "PyExc_FutureWarning",
    "PyExc_GeneratorExit",
    "PyExc_IOError",
    "PyExc_ImportError",
    "PyExc_ImportWarning",
    "PyExc_IncompleteInputError",
    "PyExc_IndentationError",
    "PyExc_IndexError",
    "PyExc_InterruptedError",
    "PyExc_IsADirectoryError",
    "PyExc_KeyError",
    "PyExc_KeyboardInterrupt",
    "PyExc_LookupError",
    "PyExc_MemoryError",
    "PyExc_ModuleNotFoundError",
    "PyExc_NameError",
    "PyExc_NotADirectoryError",
    "PyExc_NotImplementedError",
    "PyExc_OSError",
    "PyExc_OverflowError",
    "PyExc_PendingDeprecationWarning",
    "PyExc_PermissionError",
    "PyExc_ProcessLookupError",
    "PyExc_RecursionError",
    "PyExc_ReferenceError",
    "PyExc_ResourceWarning",
    "PyExc_RuntimeError",
    "PyExc_RuntimeWarning",
    "PyExc_StopAsyncIteration",
    "PyExc_StopIteration",
    "PyExc_SyntaxError",
    "PyExc_SyntaxWarning",
    "PyExc_SystemError",
    "PyExc_SystemExit",
    "PyExc_TabError",
    "PyExc_TimeoutError",
    "PyExc_TypeError",
    "PyExc_UnboundLocalError",
    "PyExc_UnicodeDecodeError",
    "PyExc_UnicodeEncodeError",
    "PyExc_UnicodeError",
    "PyExc_UnicodeTranslateError",
    "PyExc_UnicodeWarning",
    "PyExc_UserWarning",
    "PyExc_ValueError",
    "PyExc_Warning",
    "PyExc_ZeroDivisionError",
    "PyExceptionClass_Name",
    "PyException_GetArgs",
    "PyException_GetCause",
    "PyException_GetContext",
    "PyException_GetTraceback",
    "PyException_SetArgs",
    "PyException_SetCause",
    "PyException_SetContext",
    "PyException_SetTraceback",
    "PyFile_FromFd",
    "PyFile_GetLine",
    "PyFile_WriteObject",
    "PyFile_WriteString",
    "PyFilter_Type",
    "PyFloat_AsDouble",
    "PyFloat_FromDouble",
    "PyFloat_FromString",
    "PyFloat_GetInfo",
    "PyFloat_GetMax",
    "PyFloat_GetMin",
    "PyFloat_Type",
    "PyFrame_GetCode",
    "PyFrame_GetLineNumber",
    "PyFrozenSet_New",
    "PyFrozenSet_Type",
    "PyGC_Collect",
    "PyGC_Disable",
    "PyGC_Enable",
    "PyGC_IsEnabled",
    "PyGILState_Ensure",
    "PyGILState_GetThisThreadState",
    "PyGILState_Release",
    "PyGetSetDescr_Type",
    "PyImport_AddModule",
    "PyImport_AddModuleObject",
    "PyImport_AddModuleRef",
    "PyImport_AppendInittab",
    "PyImport_ExecCodeModule",
    "PyImport_ExecCodeModuleEx",
    "PyImport_ExecCodeModuleObject",
    "PyImport_ExecCodeModuleWithPathnames",
    "PyImport_GetImporter",
    "PyImport_GetMagicNumber",
    "PyImport_GetMagicTag",
    "PyImport_GetModule",
    "PyImport_GetModuleDict",
    "PyImport_Import",
    "PyImport_ImportFrozenModule",
    "PyImport_ImportFrozenModuleObject",
    "PyImport_ImportModule",
    "PyImport_ImportModuleLevel",
    "PyImport_ImportModuleLevelObject",
    "PyImport_ImportModuleNoBlock",
    "PyImport_ReloadModule",
    "PyIndex_Check",
    "PyInterpreterState_Clear",
    "PyInterpreterState_Delete",
    "PyInterpreterState_Get",
    "PyInterpreterState_GetDict",
    "PyInterpreterState_GetID",
    "PyInterpreterState_New",
    "PyIter_Check",
    "PyIter_Next",
    "PyIter_Send",
    "PyListIter_Type",
    "PyListRevIter_Type",
    "PyList_Append",
    "PyList_AsTuple",
    "PyList_GetItem",
    "PyList_GetItemRef",
    "PyList_GetSlice",
    "PyList_Insert",
    "PyList_New",
    "PyList_Reverse",
    "PyList_SetItem",
    "PyList_SetSlice",
    "PyList_Size",
    "PyList_Sort",
    "PyList_Type",
    "PyLongRangeIter_Type",
    "PyLong_AsDouble",
    "PyLong_AsInt",
    "PyLong_AsLong",
    "PyLong_AsLongAndOverflow",
    "PyLong_AsLongLong",
    "PyLong_AsLongLongAndOverflow",
    "PyLong_AsSize_t",
    "PyLong_AsSsize_t",
    "PyLong_AsUnsignedLong",
    "PyLong_AsUnsignedLongLong",
    "PyLong_AsUnsignedLongLongMask",
    "PyLong_AsUnsignedLongMask",
    "PyLong_AsVoidPtr",
    "PyLong_FromDouble",
    "PyLong_FromLong",
    "PyLong_FromLongLong",
    "PyLong_FromSize_t",
    "PyLong_FromSsize_t",
    "PyLong_FromString",
    "PyLong_FromUnsignedLong",
    "PyLong_FromUnsignedLongLong",
    "PyLong_FromVoidPtr",
    "PyLong_GetInfo",
    "PyLong_Type",
    "PyMap_Type",
    "PyMapping_Check",
    "PyMapping_GetItemString",
    "PyMapping_GetOptionalItem",
    "PyMapping_GetOptionalItemString",
    "PyMapping_HasKey",
    "PyMapping_HasKeyString",
    "PyMapping_HasKeyStringWithError",
    "PyMapping_HasKeyWithError",
    "PyMapping_Items",
    "PyMapping_Keys",
    "PyMapping_Length",
    "PyMapping_SetItemString",
    "PyMapping_Size",
    "PyMapping_Values",
    "PyMarshal_ReadObjectFromString",
    "PyMarshal_WriteObjectToString",
    "PyMem_Calloc",
    "PyMem_Free",
    "PyMem_Malloc",
    "PyMem_RawCalloc",
    "PyMem_RawFree",
    "PyMem_RawMalloc",
    "PyMem_RawRealloc",
    "PyMem_Realloc",
    "PyMemberDescr_Type",
    "PyMember_GetOne",
    "PyMember_SetOne",
    "PyMemoryView_FromBuffer",
    "PyMemoryView_FromMemory",
    "PyMemoryView_FromObject",
    "PyMemoryView_GetContiguous",
    "PyMemoryView_Type",
    "PyMethodDescr_Type",
    "PyModuleDef_Init",
    "PyModuleDef_Type",
    "PyModule_Add",
    "PyModule_AddFunctions",
    "PyModule_AddIntConstant",
    "PyModule_AddObject",
    "PyModule_AddObjectRef",
    "PyModule_AddStringConstant",
    "PyModule_AddType",
    "PyModule_Create2",
    "PyModule_ExecDef",
    "PyModule_FromDefAndSpec2",
    "PyModule_GetDef",
    "PyModule_GetDict",
    "PyModule_GetFilename",
    "PyModule_GetFilenameObject",
    "PyModule_GetName",
    "PyModule_GetNameObject",
    "PyModule_GetState",
    "PyModule_New",
    "PyModule_NewObject",
    "PyModule_SetDocString",
    "PyModule_Type",
    "PyNumber_Absolute",
    "PyNumber_Add",
    "PyNumber_And",
    "PyNumber_AsSsize_t",
    "PyNumber_Check",
    "PyNumber_Divmod",
    "PyNumber_Float",
    "PyNumber_FloorDivide",
    "PyNumber_InPlaceAdd",
    "PyNumber_InPlaceAnd",
    "PyNumber_InPlaceFloorDivide",
    "PyNumber_InPlaceLshift",
    "PyNumber_InPlaceMatrixMultiply",
    "PyNumber_InPlaceMultiply",
    "PyNumber_InPlaceOr",
    "PyNumber_InPlacePower",
    "PyNumber_InPlaceRemainder",
    "PyNumber_InPlaceRshift",
    "PyNumber_InPlaceSubtract",
    "PyNumber_InPlaceTrueDivide",
    "PyNumber_InPlaceXor",
    "PyNumber_Index",
    "PyNumber_Invert",
    "PyNumber_Long",
    "PyNumber_Lshift",
    "PyNumber_MatrixMultiply",
    "PyNumber_Multiply",
    "PyNumber_Negative",
    "PyNumber_Or",
    "PyNumber_Positive",
    "PyNumber_Power",
    "PyNumber_Remainder",
    "PyNumber_Rshift",
    "PyNumber_Subtract",
    "PyNumber_ToBase",
    "PyNumber_TrueDivide",
    "PyNumber_Xor",
    "PyOS_FSPath",
    "PyOS_InputHook",
    "PyOS_InterruptOccurred",
    "PyOS_double_to_string",
    "PyOS_getsig",
    "PyOS_mystricmp",
    "PyOS_mystrnicmp",
    "PyOS_setsig",
    "PyOS_snprintf",
    "PyOS_string_to_double",
    "PyOS_strtol",
    "PyOS_strtoul",
    "PyOS_vsnprintf",
    "PyObject_ASCII",
    "PyObject_AsCharBuffer",
    "PyObject_AsFileDescriptor",
    "PyObject_AsReadBuffer",
    "PyObject_AsWriteBuffer",
    "PyObject_Bytes",
    "PyObject_Call",
    "PyObject_CallFunction",
    "PyObject_CallFunctionObjArgs",
    "PyObject_CallMethod",
    "PyObject_CallMethodObjArgs",
    "PyObject_CallNoArgs",
    "PyObject_CallObject",
    "PyObject_Calloc",
    "PyObject_CheckBuffer",
    "PyObject_CheckReadBuffer",
    "PyObject_ClearWeakRefs",
    "PyObject_CopyData",
    "PyObject_DelAttr",
    "PyObject_DelAttrString",
    "PyObject_DelItem",
    "PyObject_DelItemString",
    "PyObject_Dir",
    "PyObject_Format",
    "PyObject_Free",
    "PyObject_GC_Del",
    "PyObject_GC_IsFinalized",
    "PyObject_GC_IsTracked",
    "PyObject_GC_Track",
    "PyObject_GC_UnTrack",
    "PyObject_GenericGetAttr",
    "PyObject_GenericGetDict",
    "PyObject_GenericSetAttr",
    "PyObject_GenericSetDict",
    "PyObject_GetAIter",
    "PyObject_GetAttr",
    "PyObject_GetAttrString",
    "PyObject_GetBuffer",
    "PyObject_GetItem",
    "PyObject_GetIter",
    "PyObject_GetOptionalAttr",
    "PyObject_GetOptionalAttrString",
    "PyObject_GetTypeData",
    "PyObject_HasAttr",
    "PyObject_HasAttrString",
    "PyObject_HasAttrStringWithError",
    "PyObject_HasAttrWithError",
    "PyObject_Hash",
    "PyObject_HashNotImplemented",
    "PyObject_Init",
    "PyObject_InitVar",
    "PyObject_IsInstance",
    "PyObject_IsSubclass",
    "PyObject_IsTrue",
    "PyObject_Length",
    "PyObject_Malloc",
    "PyObject_Not",
    "PyObject_Realloc",
    "PyObject_Repr",
    "PyObject_RichCompare",
    "PyObject_RichCompareBool",
    "PyObject_SelfIter",
    "PyObject_SetAttr",
    "PyObject_SetAttrString",
    "PyObject_SetItem",
    "PyObject_Size",
    "PyObject_Str",
    "PyObject_Type",
    "PyObject_Vectorcall",
    "PyObject_VectorcallMethod",
    "PyProperty_Type",
    "PyRangeIter_Type",
    "PyRange_Type",
    "PyReversed_Type",
    "PySeqIter_New",
    "PySeqIter_Type",
    "PySequence_Check",
    "PySequence_Concat",
    "PySequence_Contains",
    "PySequence_Count",
    "PySequence_DelItem",
    "PySequence_DelSlice",
    "PySequence_Fast",
    "PySequence_GetItem",
    "PySequence_GetSlice",
    "PySequence_In",
    "PySequence_InPlaceConcat",
    "PySequence_InPlaceRepeat",
    "PySequence_Index",
    "PySequence_Length",
    "PySequence_List",
    "PySequence_Repeat",
    "PySequence_SetItem",
    "PySequence_SetSlice",
    "PySequence_Size",
    "PySequence_Tuple",
    "PySetIter_Type",
    "PySet_Add",
    "PySet_Clear",
    "PySet_Contains",
    "PySet_Discard",
    "PySet_New",
    "PySet_Pop",
    "PySet_Size",
    "PySet_Type",
    "PySlice_AdjustIndices",
    "PySlice_GetIndices",
    "PySlice_GetIndicesEx",
    "PySlice_New",
    "PySlice_Type",
    "PySlice_Unpack",
    "PyState_AddModule",
    "PyState_FindModule",
    "PyState_RemoveModule",
    "PyStructSequence_GetItem",
    "PyStructSequence_New",
    "PyStructSequence_NewType",
    "PyStructSequence_SetItem",
    "PyStructSequence_UnnamedField",
    "PySuper_Type",
    "PySys_AddWarnOption",
    "PySys_AddWarnOptionUnicode",
    "PySys_AddXOption",
    "PySys_Audit",
    "PySys_AuditTuple",
    "PySys_FormatStderr",
    "PySys_FormatStdout",
    "PySys_GetObject",
    "PySys_GetXOptions",
    "PySys_HasWarnOptions",
    "PySys_ResetWarnOptions",
    "PySys_SetArgv",
    "PySys_SetArgvEx",
    "PySys_SetObject",
    "PySys_SetPath",
    "PySys_WriteStderr",
    "PySys_WriteStdout",
    "PyThreadState_Clear",
    "PyThreadState_Delete",
    "PyThreadState_DeleteCurrent",
    "PyThreadState_Get",
    "PyThreadState_GetDict",
    "PyThreadState_GetFrame",
    "PyThreadState_GetID",
    "PyThreadState_GetInterpreter",
    "PyThreadState_New",
    "PyThreadState_SetAsyncExc",
    "PyThreadState_Swap",
    "PyThread_GetInfo",
    "PyThread_ReInitTLS",
    "PyThread_acquire_lock",
    "PyThread_acquire_lock_timed",
    "PyThread_allocate_lock",
    "PyThread_create_key",
    "PyThread_delete_key",
    "PyThread_delete_key_value",
    "PyThread_exit_thread",
    "PyThread_free_lock",
    "PyThread_get_key_value",
    "PyThread_get_stacksize",
    "PyThread_get_thread_ident",
    "PyThread_init_thread",
    "PyThread_release_lock",
    "PyThread_set_key_value",
    "PyThread_set_stacksize",
    "PyThread_start_new_thread",
    "PyThread_tss_alloc",
    "PyThread_tss_create",
    "PyThread_tss_delete",
    "PyThread_tss_free",
    "PyThread_tss_get",
    "PyThread_tss_is_created",
    "PyThread_tss_set",
    "PyTraceBack_Here",
    "PyTraceBack_Print",
    "PyTraceBack_Type",
    "PyTupleIter_Type",
    "PyTuple_GetItem",
    "PyTuple_GetSlice",
    "PyTuple_New",
    "PyTuple_Pack",
    "PyTuple_SetItem",
    "PyTuple_Size",
    "PyTuple_Type",
    "PyType_ClearCache",
    "PyType_FromMetaclass",
    "PyType_FromModuleAndSpec",
    "PyType_FromSpec",
    "PyType_FromSpecWithBases",
    "PyType_GenericAlloc",
    "PyType_GenericNew",
    "PyType_GetFlags",
    "PyType_GetFullyQualifiedName",
    "PyType_GetModule",
    "PyType_GetModuleByDef",
    "PyType_GetModuleName",
    "PyType_GetModuleState",
    "PyType_GetName",
    "PyType_GetQualName",
    "PyType_GetSlot",
    "PyType_GetTypeDataSize",
    "PyType_IsSubtype",
    "PyType_Modified",
    "PyType_Ready",
    "PyType_Type",
    "PyUnicodeDecodeError_Create",
    "PyUnicodeDecodeError_GetEncoding",
    "PyUnicodeDecodeError_GetEnd",
    "PyUnicodeDecodeError_GetObject",
    "PyUnicodeDecodeError_GetReason",
    "PyUnicodeDecodeError_GetStart",
    "PyUnicodeDecodeError_SetEnd",
    "PyUnicodeDecodeError_SetReason",
    "PyUnicodeDecodeError_SetStart",
    "PyUnicodeEncodeError_GetEncoding",
    "PyUnicodeEncodeError_GetEnd",
    "PyUnicodeEncodeError_GetObject",
    "PyUnicodeEncodeError_GetReason",
    "PyUnicodeEncodeError_GetStart",
    "PyUnicodeEncodeError_SetEnd",
    "PyUnicodeEncodeError_SetReason",
    "PyUnicodeEncodeError_SetStart",
    "PyUnicodeIter_Type",
    "PyUnicodeTranslateError_GetEnd",
    "PyUnicodeTranslateError_GetObject",
    "PyUnicodeTranslateError_GetReason",
    "PyUnicodeTranslateError_GetStart",
    "PyUnicodeTranslateError_SetEnd",
    "PyUnicodeTranslateError_SetReason",
    "PyUnicodeTranslateError_SetStart",
    "PyUnicode_Append",
    "PyUnicode_AppendAndDel",
    "PyUnicode_AsASCIIString",
    "PyUnicode_AsCharmapString",
    "PyUnicode_AsDecodedObject",
    "PyUnicode_AsDecodedUnicode",
    "PyUnicode_AsEncodedObject",
    "PyUnicode_AsEncodedString",
    "PyUnicode_AsEncodedUnicode",
    "PyUnicode_AsLatin1String",
    "PyUnicode_AsRawUnicodeEscapeString",
    "PyUnicode_AsUCS4",
    "PyUnicode_AsUCS4Copy",
    "PyUnicode_AsUTF16String",
    "PyUnicode_AsUTF32String",
    "PyUnicode_AsUTF8AndSize",
    "PyUnicode_AsUTF8String",
    "PyUnicode_AsUnicodeEscapeString",
    "PyUnicode_AsWideChar",
    "PyUnicode_AsWideCharString",
    "PyUnicode_BuildEncodingMap",
    "PyUnicode_Compare",
    "PyUnicode_CompareWithASCIIString",
    "PyUnicode_Concat",
    "PyUnicode_Contains",
    "PyUnicode_Count",
    "PyUnicode_Decode",
    "PyUnicode_DecodeASCII",
    "PyUnicode_DecodeCharmap",
    "PyUnicode_DecodeFSDefault",
    "PyUnicode_DecodeFSDefaultAndSize",
    "PyUnicode_DecodeLatin1",
    "PyUnicode_DecodeLocale",
    "PyUnicode_DecodeLocaleAndSize",
    "PyUnicode_DecodeRawUnicodeEscape",
    "PyUnicode_DecodeUTF16",
    "PyUnicode_DecodeUTF16Stateful",
    "PyUnicode_DecodeUTF32",
    "PyUnicode_DecodeUTF32Stateful",
    "PyUnicode_DecodeUTF7",
    "PyUnicode_DecodeUTF7Stateful",
    "PyUnicode_DecodeUTF8",
    "PyUnicode_DecodeUTF8Stateful",
    "PyUnicode_DecodeUnicodeEscape",
    "PyUnicode_EncodeFSDefault",
    "PyUnicode_EncodeLocale",
    "PyUnicode_EqualToUTF8",
    "PyUnicode_EqualToUTF8AndSize",
    "PyUnicode_FSConverter",
    "PyUnicode_FSDecoder",
    "PyUnicode_Find",
    "PyUnicode_FindChar",
    "PyUnicode_Format",
    "PyUnicode_FromEncodedObject",
    "PyUnicode_FromFormat",
    "PyUnicode_FromFormatV",
    "PyUnicode_FromObject",
    "PyUnicode_FromOrdinal",
    "PyUnicode_FromString",
    "PyUnicode_FromStringAndSize",
    "PyUnicode_FromWideChar",
    "PyUnicode_GetDefaultEncoding",
    "PyUnicode_GetLength",
    "PyUnicode_GetSize",
    "PyUnicode_InternFromString",
    "PyUnicode_InternImmortal",
    "PyUnicode_InternInPlace",
    "PyUnicode_IsIdentifier",
    "PyUnicode_Join",
    "PyUnicode_Partition",
    "PyUnicode_RPartition",
    "PyUnicode_RSplit",
    "PyUnicode_ReadChar",
    "PyUnicode_Replace",
    "PyUnicode_Resize",
    "PyUnicode_RichCompare",
    "PyUnicode_Split",
    "PyUnicode_Splitlines",
    "PyUnicode_Substring",
    "PyUnicode_Tailmatch",
    "PyUnicode_Translate",
    "PyUnicode_Type",
    "PyUnicode_WriteChar",
    "PyVectorcall_Call",
    "PyVectorcall_NARGS",
    "PyWeakref_GetObject",
    "PyWeakref_GetRef",
    "PyWeakref_NewProxy",
    "PyWeakref_NewRef",
    "PyWrapperDescr_Type",
    "PyWrapper_New",
    "PyZip_Type",
    "Py_AddPendingCall",
    "Py_AtExit",
    "Py_BuildValue",
    "Py_BytesMain",
    "Py_CompileString",
    "Py_DecRef",
    "Py_DecodeLocale",
    "Py_EncodeLocale",
    "Py_EndInterpreter",
    "Py_EnterRecursiveCall",
    "Py_Exit",
    "Py_FatalError",
    "Py_FileSystemDefaultEncodeErrors",
    "Py_FileSystemDefaultEncoding",
    "Py_Finalize",
    "Py_FinalizeEx",
    "Py_GenericAlias",
    "Py_GenericAliasType",
    "Py_GetArgcArgv",
    "Py_GetBuildInfo",
    "Py_GetCompiler",
    "Py_GetConstant",
    "Py_GetConstantBorrowed",
    "Py_GetCopyright",
    "Py_GetExecPrefix",
    "Py_GetPath",
    "Py_GetPlatform",
    "Py_GetPrefix",
    "Py_GetProgramFullPath",
    "Py_GetProgramName",
    "Py_GetPythonHome",
    "Py_GetRecursionLimit",
    "Py_GetVersion",
    "Py_HasFileSystemDefaultEncoding",
    "Py_IncRef",
    "Py_Initialize",
    "Py_InitializeEx",
    "Py_Is",
    "Py_IsFalse",
    "Py_IsFinalizing",
    "Py_IsInitialized",
    "Py_IsNone",
    "Py_IsTrue",
    "Py_LeaveRecursiveCall",
    "Py_Main",
    "Py_MakePendingCalls",
    "Py_NewInterpreter",
    "Py_NewRef",
    "Py_ReprEnter",
    "Py_ReprLeave",
    "Py_SetPath",
    "Py_SetProgramName",
    "Py_SetPythonHome",
    "Py_SetRecursionLimit",
    "Py_UTF8Mode",
    "Py_VaBuildValue",
    "Py_Version",
    "Py_XNewRef",
    "_PyArg_ParseTupleAndKeywords_SizeT",
    "_PyArg_ParseTuple_SizeT",
    "_PyArg_Parse_SizeT",
    "_PyArg_VaParseTupleAndKeywords_SizeT",
    "_PyArg_VaParse_SizeT",
    "_PyErr_BadInternalCall",
    "_PyObject_CallFunction_SizeT",
    "_PyObject_CallMethod_SizeT",
    "_PyObject_GC_New",
    "_PyObject_GC_NewVar",
    "_PyObject_GC_Resize",
    "_PyObject_New",
    "_PyObject_NewVar",
    "_PyState_AddModule",
    "_PyThreadState_Init",
    "_PyThreadState_Prealloc",
    "_PyWeakref_CallableProxyType",
    "_PyWeakref_ProxyType",
    "_PyWeakref_RefType",
    "_Py_BuildValue_SizeT",
    "_Py_CheckRecursiveCall",
    "_Py_Dealloc",
    "_Py_DecRef",
    "_Py_EllipsisObject",
    "_Py_FalseStruct",
    "_Py_IncRef",
    "_Py_NoneStruct",
    "_Py_NotImplementedStruct",
    "_Py_SetRefcnt",
    "_Py_SwappedOp",
    "_Py_TrueStruct",
    "_Py_VaBuildValue_SizeT",
)
if feature_macros['HAVE_FORK']:
    SYMBOL_NAMES += (
        'PyOS_AfterFork',
        'PyOS_AfterFork_Child',
        'PyOS_AfterFork_Parent',
        'PyOS_BeforeFork',
    )
if feature_macros['MS_WINDOWS']:
    SYMBOL_NAMES += (
        'PyErr_SetExcFromWindowsErr',
        'PyErr_SetExcFromWindowsErrWithFilename',
        'PyErr_SetExcFromWindowsErrWithFilenameObject',
        'PyErr_SetExcFromWindowsErrWithFilenameObjects',
        'PyErr_SetFromWindowsErr',
        'PyErr_SetFromWindowsErrWithFilename',
        'PyExc_WindowsError',
        'PyUnicode_AsMBCSString',
        'PyUnicode_DecodeCodePageStateful',
        'PyUnicode_DecodeMBCS',
        'PyUnicode_DecodeMBCSStateful',
        'PyUnicode_EncodeCodePage',
    )
if feature_macros['PY_HAVE_THREAD_NATIVE_ID']:
    SYMBOL_NAMES += (
        'PyThread_get_thread_native_id',
    )
if feature_macros['Py_REF_DEBUG']:
    SYMBOL_NAMES += (
        '_Py_NegativeRefcount',
        '_Py_RefTotal',
    )
if feature_macros['Py_TRACE_REFS']:
    SYMBOL_NAMES += (
    )
if feature_macros['USE_STACKCHECK']:
    SYMBOL_NAMES += (
        'PyOS_CheckStack',
    )

EXPECTED_FEATURE_MACROS = set(['HAVE_FORK',
 'MS_WINDOWS',
 'PY_HAVE_THREAD_NATIVE_ID',
 'Py_REF_DEBUG',
 'Py_TRACE_REFS',
 'USE_STACKCHECK'])
WINDOWS_FEATURE_MACROS = {'HAVE_FORK': False,
 'MS_WINDOWS': True,
 'PY_HAVE_THREAD_NATIVE_ID': True,
 'Py_REF_DEBUG': 'maybe',
 'Py_TRACE_REFS': 'maybe',
 'USE_STACKCHECK': 'maybe'}
