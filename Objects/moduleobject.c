
/* Module object implementation */

#include "Python.h"
#include "structmember.h"

typedef struct {
	PyObject_HEAD
	PyObject *md_dict;
} PyModuleObject;

static PyMemberDef module_members[] = {
	{"__dict__", T_OBJECT, offsetof(PyModuleObject, md_dict), READONLY},
	{0}
};

PyObject *
PyModule_New(char *name)
{
	PyModuleObject *m;
	PyObject *nameobj;
	m = PyObject_GC_New(PyModuleObject, &PyModule_Type);
	if (m == NULL)
		return NULL;
	nameobj = PyString_FromString(name);
	m->md_dict = PyDict_New();
	if (m->md_dict == NULL || nameobj == NULL)
		goto fail;
	if (PyDict_SetItemString(m->md_dict, "__name__", nameobj) != 0)
		goto fail;
	if (PyDict_SetItemString(m->md_dict, "__doc__", Py_None) != 0)
		goto fail;
	Py_DECREF(nameobj);
	PyObject_GC_Track(m);
	return (PyObject *)m;

 fail:
	Py_XDECREF(nameobj);
	Py_DECREF(m);
	return NULL;
}

PyObject *
PyModule_GetDict(PyObject *m)
{
	PyObject *d;
	if (!PyModule_Check(m)) {
		PyErr_BadInternalCall();
		return NULL;
	}
	d = ((PyModuleObject *)m) -> md_dict;
	if (d == NULL)
		((PyModuleObject *)m) -> md_dict = d = PyDict_New();
	return d;
}

char *
PyModule_GetName(PyObject *m)
{
	PyObject *d;
	PyObject *nameobj;
	if (!PyModule_Check(m)) {
		PyErr_BadArgument();
		return NULL;
	}
	d = ((PyModuleObject *)m)->md_dict;
	if (d == NULL ||
	    (nameobj = PyDict_GetItemString(d, "__name__")) == NULL ||
	    !PyString_Check(nameobj))
	{
		PyErr_SetString(PyExc_SystemError, "nameless module");
		return NULL;
	}
	return PyString_AsString(nameobj);
}

char *
PyModule_GetFilename(PyObject *m)
{
	PyObject *d;
	PyObject *fileobj;
	if (!PyModule_Check(m)) {
		PyErr_BadArgument();
		return NULL;
	}
	d = ((PyModuleObject *)m)->md_dict;
	if (d == NULL ||
	    (fileobj = PyDict_GetItemString(d, "__file__")) == NULL ||
	    !PyString_Check(fileobj))
	{
		PyErr_SetString(PyExc_SystemError, "module filename missing");
		return NULL;
	}
	return PyString_AsString(fileobj);
}

void
_PyModule_Clear(PyObject *m)
{
	/* To make the execution order of destructors for global
	   objects a bit more predictable, we first zap all objects
	   whose name starts with a single underscore, before we clear
	   the entire dictionary.  We zap them by replacing them with
	   None, rather than deleting them from the dictionary, to
	   avoid rehashing the dictionary (to some extent). */

	int pos;
	PyObject *key, *value;
	PyObject *d;

	d = ((PyModuleObject *)m)->md_dict;
	if (d == NULL)
		return;

	/* First, clear only names starting with a single underscore */
	pos = 0;
	while (PyDict_Next(d, &pos, &key, &value)) {
		if (value != Py_None && PyString_Check(key)) {
			char *s = PyString_AsString(key);
			if (s[0] == '_' && s[1] != '_') {
				if (Py_VerboseFlag > 1)
				    PySys_WriteStderr("#   clear[1] %s\n", s);
				PyDict_SetItem(d, key, Py_None);
			}
		}
	}

	/* Next, clear all names except for __builtins__ */
	pos = 0;
	while (PyDict_Next(d, &pos, &key, &value)) {
		if (value != Py_None && PyString_Check(key)) {
			char *s = PyString_AsString(key);
			if (s[0] != '_' || strcmp(s, "__builtins__") != 0) {
				if (Py_VerboseFlag > 1)
				    PySys_WriteStderr("#   clear[2] %s\n", s);
				PyDict_SetItem(d, key, Py_None);
			}
		}
	}

	/* Note: we leave __builtins__ in place, so that destructors
	   of non-global objects defined in this module can still use
	   builtins, in particularly 'None'. */

}

/* Methods */

static int
module_init(PyModuleObject *m, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"name", "doc", NULL};
	PyObject *dict, *name = Py_None, *doc = Py_None;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "S|O", kwlist,
					 &name, &doc))
		return -1;
	dict = m->md_dict;
	if (dict == NULL) {
		dict = PyDict_New();
		if (dict == NULL)
			return -1;
		m->md_dict = dict;
	}
	if (PyDict_SetItemString(dict, "__name__", name) < 0)
		return -1;
	if (PyDict_SetItemString(dict, "__doc__", doc) < 0)
		return -1;
	return 0;
}

static void
module_dealloc(PyModuleObject *m)
{
	PyObject_GC_UnTrack(m);
	if (m->md_dict != NULL) {
		_PyModule_Clear((PyObject *)m);
		Py_DECREF(m->md_dict);
	}
	m->ob_type->tp_free((PyObject *)m);
}

static PyObject *
module_repr(PyModuleObject *m)
{
	char *name;
	char *filename;

	name = PyModule_GetName((PyObject *)m);
	if (name == NULL) {
		PyErr_Clear();
		name = "?";
	}
	filename = PyModule_GetFilename((PyObject *)m);
	if (filename == NULL) {
		PyErr_Clear();
		return PyString_FromFormat("<module '%s' (built-in)>", name);
	}
	return PyString_FromFormat("<module '%s' from '%s'>", name, filename);
}

static PyObject *
find_builtin_names(void)
{
	PyObject *builtins, *names, *key, *value;
	int pos = 0;
	builtins = PyEval_GetBuiltins();
	if (builtins == NULL || !PyDict_Check(builtins)) {
  		PyErr_SetString(PyExc_SystemError, "no builtins dict!");
		return NULL;
	}
	names = PyDict_New();
	if (names == NULL)
		return NULL;
	while (PyDict_Next(builtins, &pos, &key, &value)) {
		if (PyString_Check(key) &&
				PyString_Size(key) > 0 &&
				PyString_AS_STRING(key)[0] != '_') {
			if (PyDict_SetItem(names, key, Py_None) < 0) {
				Py_DECREF(names);
				return NULL;
			}
		}
	}
	return names;
}

/* returns 0 or 1 (and -1 on error) */
static int
shadows_builtin(PyObject *globals, PyObject *name)
{
	static PyObject *builtin_names = NULL;
	if (builtin_names == NULL) {
		builtin_names = find_builtin_names();
		if (builtin_names == NULL)
			return -1;
	}
	if (!PyString_Check(name))
		return 0;
	if (PyDict_GetItem(globals, name) == NULL &&
	    PyDict_GetItem(builtin_names, name) != NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

static int
module_setattr(PyObject *m, PyObject *name, PyObject *value)
{
	PyObject *globals = ((PyModuleObject *)m)->md_dict;
	PyObject *builtins = PyEval_GetBuiltins();
	if (globals != NULL && globals != builtins) {
		int shadows = shadows_builtin(globals, name);
		if (shadows == 1) {
			if (PyErr_Warn(PyExc_DeprecationWarning,
					"assignment shadows builtin") < 0)
				return -1;
		}
		else if (shadows == -1)
			return -1;
	}
	return PyObject_GenericSetAttr(m, name, value);
}

/* We only need a traverse function, no clear function: If the module
   is in a cycle, md_dict will be cleared as well, which will break
   the cycle. */
static int
module_traverse(PyModuleObject *m, visitproc visit, void *arg)
{
	if (m->md_dict != NULL)
		return visit(m->md_dict, arg);
	return 0;
}

PyDoc_STRVAR(module_doc,
"module(name[, doc])\n\
\n\
Create a module object.\n\
The name must be a string; the optional doc argument can have any type.");

PyTypeObject PyModule_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/* ob_size */
	"module",				/* tp_name */
	sizeof(PyModuleObject),			/* tp_size */
	0,					/* tp_itemsize */
	(destructor)module_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	(reprfunc)module_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	module_setattr,				/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
		Py_TPFLAGS_BASETYPE,		/* tp_flags */
	module_doc,				/* tp_doc */
	(traverseproc)module_traverse,		/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	module_members,				/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	offsetof(PyModuleObject, md_dict),	/* tp_dictoffset */
	(initproc)module_init,			/* tp_init */
	PyType_GenericAlloc,			/* tp_alloc */
	PyType_GenericNew,			/* tp_new */
	PyObject_GC_Del,		        /* tp_free */
};
