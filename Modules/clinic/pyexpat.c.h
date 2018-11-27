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

    if (!_PyArg_ParseStack(args, nargs, "O|i:Parse",
        &data, &isfinal)) {
        goto exit;
    }
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

    if (!PyArg_Parse(arg, "s:SetBase", &base)) {
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

    if (!_PyArg_ParseStack(args, nargs, "z|s:ExternalEntityParserCreate",
        &context, &encoding)) {
        goto exit;
    }
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

    if (!PyArg_Parse(arg, "i:SetParamEntityParsing", &flag)) {
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

    if (!_PyArg_ParseStack(args, nargs, "|p:UseForeignDTD",
        &flag)) {
        goto exit;
    }
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
    static _PyArg_Parser _parser = {"|zzO:ParserCreate", _keywords, 0};
    const char *encoding = NULL;
    const char *namespace_separator = NULL;
    PyObject *intern = NULL;

    if (!_PyArg_ParseStackAndKeywords(args, nargs, kwnames, &_parser,
        &encoding, &namespace_separator, &intern)) {
        goto exit;
    }
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

    if (!PyArg_Parse(arg, "l:ErrorString", &code)) {
        goto exit;
    }
    return_value = pyexpat_ErrorString_impl(module, code);

exit:
    return return_value;
}

#ifndef PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF
    #define PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF
#endif /* !defined(PYEXPAT_XMLPARSER_USEFOREIGNDTD_METHODDEF) */
/*[clinic end generated code: output=c390207761c679d3 input=a9049054013a1b77]*/
