/*[clinic input]
preserve
[clinic start generated code]*/

PyDoc_STRVAR(pyexpat_xmlparser_Parse__doc__,
"Parse($self, data, isfinal=False, /)\n"
"--\n"
"\n"
"Parse XML data.\n"
"\n"
"`isfinal\' should be true at end of input.");

#define PYEXPAT_XMLPARSER_PARSE_METHODDEF    \
    {"Parse", (PyCFunction)(void(*)(void))pyexpat_xmlparser_Parse, METH_FASTCALL, pyexpat_xmlparser_Parse__doc__},

static PyObject *
pyexpat_xmlparser_Parse_impl(xmlparseobject *self, PyObject *data,
                             int isfinal);

static PyObject *
pyexpat_xmlparser_Parse(xmlparseobject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *return_value = NULL;
    PyObject *data;
    int isfinal = 0;

    if (!_PyArg_CheckPositional("Parse", nargs, 1, 2)) {
        goto exit;
    }
    data = args[0];
    if (nargs < 2) {
        goto skip_optional;
    }
    if (PyFloat_Check(args[1])) {
        PyErr_SetString(PyExc_TypeError,
                        "integer argument expected, got float" );
        goto exit;
    }
    isfinal = _PyLong_AsInt(args[1]);
    if (isfinal == -1 && PyErr_Occurred()) {
        goto exit;
    }
skip_optional:
    return_value = pyexpat_xmlparser_Parse_impl(self, data, isfinal);

exit:
    return return_value;
}

PyDoc_STRVAR(pyexpat_xmlparser_ParseFile__doc__,
"ParseFile($self, file, /)\n"
"--\n"
"\n"
"Parse XML data from file-like object.");

#define PYEXPAT_XMLPARSER_PARSEFILE_METHODDEF    \
    {"ParseFile", (PyCFunction)pyexpat_xmlparser_ParseFile, METH_O, pyexpat_xmlparser_ParseFile__doc__},

PyDoc_STRVAR(pyexpat_xmlparser_SetBase__doc__,
"SetBase($self, base, /)\n"
"--\n"
"\n"
"Set the base URL for the parser.");

#define PYEXPAT_XMLPARSER_SETBASE_METHODDEF    \
    {"SetBase", (PyCFunction)pyexpat_xmlparser_SetBase, METH_O, pyexpat_xmlparser_SetBase__doc__},

static PyObject *
pyexpat_xmlparser_SetBase_impl(xmlparseobject *self, const char *base);

static PyObject *
pyexpat_xmlparser_SetBase(xmlparseobject *self, PyObject *arg)
{
    PyObject *return_value = NULL;
    const char *base;

    if (!PyUnicode_Check(arg)) {
        _PyArg_BadArgument("SetBase", 0, "str", arg);
        goto exit;
    }
    Py_ssize_t base_length;
    base = PyUnicode_AsUTF8AndSize(arg, &base_length);
    if (base == NULL) {
        goto exit;
    }
    if (strlen(base) != (size_t)base_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    return_value = pyexpat_xmlparser_SetBase_impl(self, base);

exit:
    return return_value;
}

PyDoc_STRVAR(pyexpat_xmlparser_GetBase__doc__,
"GetBase($self, /)\n"
"--\n"
"\n"
"Return base URL string for the parser.");

#define PYEXPAT_XMLPARSER_GETBASE_METHODDEF    \
    {"GetBase", (PyCFunction)pyexpat_xmlparser_GetBase, METH_NOARGS, pyexpat_xmlparser_GetBase__doc__},

static PyObject *
pyexpat_xmlparser_GetBase_impl(xmlparseobject *self);

static PyObject *
pyexpat_xmlparser_GetBase(xmlparseobject *self, PyObject *Py_UNUSED(ignored))
{
    return pyexpat_xmlparser_GetBase_impl(self);
}

PyDoc_STRVAR(pyexpat_xmlparser_GetInputContext__doc__,
"GetInputContext($self, /)\n"
"--\n"
"\n"
"Return the untranslated text of the input that caused the current event.\n"
"\n"
"If the event was generated by a large amount of text (such as a start tag\n"
"for an element with many attributes), not all of the text may be available.");

#define PYEXPAT_XMLPARSER_GETINPUTCONTEXT_METHODDEF    \
    {"GetInputContext", (PyCFunction)pyexpat_xmlparser_GetInputContext, METH_NOARGS, pyexpat_xmlparser_GetInputContext__doc__},

static PyObject *
pyexpat_xmlparser_GetInputContext_impl(xmlparseobject *self);

static PyObject *
pyexpat_xmlparser_GetInputContext(xmlparseobject *self, PyObject *Py_UNUSED(ignored))
{
    return pyexpat_xmlparser_GetInputContext_impl(self);
}

PyDoc_STRVAR(pyexpat_xmlparser_ExternalEntityParserCreate__doc__,
"ExternalEntityParserCreate($self, context, encoding=None, /)\n"
"--\n"
"\n"
"Create a parser for parsing an external entity based on the information passed to the ExternalEntityRefHandler.");

#define PYEXPAT_XMLPARSER_EXTERNALENTITYPARSERCREATE_METHODDEF    \
    {"ExternalEntityParserCreate", (PyCFunction)(void(*)(void))pyexpat_xmlparser_ExternalEntityParserCreate, METH_FASTCALL, pyexpat_xmlparser_ExternalEntityParserCreate__doc__},

static PyObject *
pyexpat_xmlparser_ExternalEntityParserCreate_impl(xmlparseobject *self,
                                                  const char *context,
                                                  const char *encoding);

static PyObject *
pyexpat_xmlparser_ExternalEntityParserCreate(xmlparseobject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *return_value = NULL;
    const char *context;
    const char *encoding = NULL;

    if (!_PyArg_CheckPositional("ExternalEntityParserCreate", nargs, 1, 2)) {
        goto exit;
    }
    if (args[0] == Py_None) {
        context = NULL;
    }
    else if (PyUnicode_Check(args[0])) {
        Py_ssize_t context_length;
        context = PyUnicode_AsUTF8AndSize(args[0], &context_length);
        if (context == NULL) {
            goto exit;
        }
        if (strlen(context) != (size_t)context_length) {
            PyErr_SetString(PyExc_ValueError, "embedded null character");
            goto exit;
        }
    }
    else {
        _PyArg_BadArgument("ExternalEntityParserCreate", 1, "str or None", args[0]);
        goto exit;
    }
    if (nargs < 2) {
        goto skip_optional;
    }
    if (!PyUnicode_Check(args[1])) {
        _PyArg_BadArgument("ExternalEntityParserCreate", 2, "str", args[1]);
        goto exit;
    }
    Py_ssize_t encoding_length;
    encoding = PyUnicode_AsUTF8AndSize(args[1], &encoding_length);
    if (encoding == NULL) {
        goto exit;
    }
    if (strlen(encoding) != (size_t)encoding_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
skip_optional:
    return_value = pyexpat_xmlparser_ExternalEntityParserCreate_impl(self, context, encoding);

exit:
    return return_value;
}

PyDoc_STRVAR(pyexpat_xmlparser_SetParamEntityParsing__doc__,
"SetParamEntityParsing($self, flag, /)\n"
"--\n"
"\n"
"Controls parsing of parameter entities (including the external DTD subset).\n"
"\n"
"Possible flag values are XML_PARAM_ENTITY_PARSING_NEVER,\n"
"XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE and\n"
"XML_PARAM_ENTITY_PARSING_ALWAYS. Returns true if setting the flag\n"
"was successful.");

#define PYEXPAT_XMLPARSER_SETPARAMENTITYPARSING_METHODDEF    \
    {"SetParamEntityParsing", (PyCFunction)pyexpat_xmlparser_SetParamEntityParsing, METH_O, pyexpat_xmlparser_SetParamEntityParsing__doc__},

static PyObject *
pyexpat_xmlparser_SetParamEntityParsing_impl(xmlparseobject *self, int flag);

static PyObject *
pyexpat_xmlparser_SetParamEntityParsing(xmlparseobject *self, PyObject *arg)
{
    PyObject *return_value = NULL;
    int flag;

    if (PyFloat_Check(arg)) {
        PyErr_SetString(PyExc_TypeError,
                        "integer argument expected, got float" );
        goto exit;
    }
    flag = _PyLong_AsInt(arg);
    if (flag == -1 && PyErr_Occurred()) {
        goto exit;
    }
    return_value = pyexpat_xmlparser_SetParamEntityParsing_impl(self, flag);

exit:
    return return_value;
}

#if (XML_COMBINED_VERSION >= 19505)

PyDoc_STRVAR(pyexpat_xmlparser_UseForeignDTD__doc__,
"UseForeignDTD($self, flag=True, /)\n"
"--\n"
"\n"
"Allows the application to provide an artificial external subset if one is not specified as part of the document instance.\n"
"\n"
"This readily allows the use of a \'default\' document type controlled by the\n"
"application, while still getting the advantage of providing document type\n"
"information to the parser. \'flag\' defaults to True if not provided.");

#define PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF    \
    {"UseForeignDTD", (PyCFunction)(void(*)(void))pyexpat_xmlparser_UseForeignDTD, METH_FASTCALL, pyexpat_xmlparser_UseForeignDTD__doc__},

static PyObject *
pyexpat_xmlparser_UseForeignDTD_impl(xmlparseobject *self, int flag);

static PyObject *
pyexpat_xmlparser_UseForeignDTD(xmlparseobject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *return_value = NULL;
    int flag = 1;

    if (!_PyArg_CheckPositional("UseForeignDTD", nargs, 0, 1)) {
        goto exit;
    }
    if (nargs < 1) {
        goto skip_optional;
    }
    flag = PyObject_IsTrue(args[0]);
    if (flag < 0) {
        goto exit;
    }
skip_optional:
    return_value = pyexpat_xmlparser_UseForeignDTD_impl(self, flag);

exit:
    return return_value;
}

#endif /* (XML_COMBINED_VERSION >= 19505) */

PyDoc_STRVAR(pyexpat_ParserCreate__doc__,
"ParserCreate($module, /, encoding=None, namespace_separator=None,\n"
"             intern=None)\n"
"--\n"
"\n"
"Return a new XML parser object.");

#define PYEXPAT_PARSERCREATE_METHODDEF    \
    {"ParserCreate", (PyCFunction)(void(*)(void))pyexpat_ParserCreate, METH_FASTCALL|METH_KEYWORDS, pyexpat_ParserCreate__doc__},

static PyObject *
pyexpat_ParserCreate_impl(PyObject *module, const char *encoding,
                          const char *namespace_separator, PyObject *intern);

static PyObject *
pyexpat_ParserCreate(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"encoding", "namespace_separator", "intern", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "ParserCreate", 0};
    PyObject *argsbuf[3];
    Py_ssize_t noptargs = nargs + (kwnames ? PyTuple_GET_SIZE(kwnames) : 0) - 0;
    const char *encoding = NULL;
    const char *namespace_separator = NULL;
    PyObject *intern = NULL;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 0, 3, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    if (!noptargs) {
        goto skip_optional_pos;
    }
    if (args[0]) {
        if (args[0] == Py_None) {
            encoding = NULL;
        }
        else if (PyUnicode_Check(args[0])) {
            Py_ssize_t encoding_length;
            encoding = PyUnicode_AsUTF8AndSize(args[0], &encoding_length);
            if (encoding == NULL) {
                goto exit;
            }
            if (strlen(encoding) != (size_t)encoding_length) {
                PyErr_SetString(PyExc_ValueError, "embedded null character");
                goto exit;
            }
        }
        else {
            _PyArg_BadArgument("ParserCreate", 1, "str or None", args[0]);
            goto exit;
        }
        if (!--noptargs) {
            goto skip_optional_pos;
        }
    }
    if (args[1]) {
        if (args[1] == Py_None) {
            namespace_separator = NULL;
        }
        else if (PyUnicode_Check(args[1])) {
            Py_ssize_t namespace_separator_length;
            namespace_separator = PyUnicode_AsUTF8AndSize(args[1], &namespace_separator_length);
            if (namespace_separator == NULL) {
                goto exit;
            }
            if (strlen(namespace_separator) != (size_t)namespace_separator_length) {
                PyErr_SetString(PyExc_ValueError, "embedded null character");
                goto exit;
            }
        }
        else {
            _PyArg_BadArgument("ParserCreate", 2, "str or None", args[1]);
            goto exit;
        }
        if (!--noptargs) {
            goto skip_optional_pos;
        }
    }
    intern = args[2];
skip_optional_pos:
    return_value = pyexpat_ParserCreate_impl(module, encoding, namespace_separator, intern);

exit:
    return return_value;
}

PyDoc_STRVAR(pyexpat_ErrorString__doc__,
"ErrorString($module, code, /)\n"
"--\n"
"\n"
"Returns string error for given number.");

#define PYEXPAT_ERRORSTRING_METHODDEF    \
    {"ErrorString", (PyCFunction)pyexpat_ErrorString, METH_O, pyexpat_ErrorString__doc__},

static PyObject *
pyexpat_ErrorString_impl(PyObject *module, long code);

static PyObject *
pyexpat_ErrorString(PyObject *module, PyObject *arg)
{
    PyObject *return_value = NULL;
    long code;

    if (PyFloat_Check(arg)) {
        PyErr_SetString(PyExc_TypeError,
                        "integer argument expected, got float" );
        goto exit;
    }
    code = PyLong_AsLong(arg);
    if (code == -1 && PyErr_Occurred()) {
        goto exit;
    }
    return_value = pyexpat_ErrorString_impl(module, code);

exit:
    return return_value;
}

#ifndef PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF
    #define PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF
#endif /* !defined(PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF) */
/*[clinic end generated code: output=e48f37d326956bdd input=a9049054013a1b77]*/
