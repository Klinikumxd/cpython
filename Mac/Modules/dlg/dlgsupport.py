# This script generates the Dialogs interface for Python.
# It uses the "bgen" package to generate C code.
# It execs the file dlggen.py which contain the function definitions
# (dlggen.py was generated by dlgscan.py, scanning the <Dialogs.h> header file).

import addpack
addpack.addpack(':Tools:bgen:bgen')

from macsupport import *

# Create the type objects

DialogPtr = OpaqueByValueType("DialogPtr", "DlgObj")
DialogRef = DialogPtr

ModalFilterProcPtr = InputOnlyType("PyObject*", "O")
ModalFilterProcPtr.passInput = lambda name: "NewModalFilterProc(Dlg_PassFilterProc(%s))" % name
ModalFilterUPP = ModalFilterProcPtr

RgnHandle = FakeType("_self->ob_itself->visRgn") # XXX

DITLMethod = Type("DITLMethod", "h")

includestuff = includestuff + """
#include <Dialogs.h>

#ifndef HAVE_UNIVERSAL_HEADERS
#define NewModalFilterProc(x) (x)
#endif

#define resNotFound -192 /* Can't include <Errors.h> because of Python's "errors.h" */

/* XXX Shouldn't this be a stack? */
static PyObject *Dlg_FilterProc_callback = NULL;

static PyObject *DlgObj_New(DialogPtr); /* Forward */

static pascal Boolean Dlg_UnivFilterProc(DialogPtr dialog,
                                         EventRecord *event,
                                         short *itemHit)
{
	Boolean rv;
	PyObject *args, *res;
	PyObject *callback = Dlg_FilterProc_callback;
	if (callback == NULL)
		return 0; /* Default behavior */
	Dlg_FilterProc_callback = NULL; /* We'll restore it when call successful */
	args = Py_BuildValue("O&O&", WinObj_WhichWindow, dialog, PyMac_BuildEventRecord, event);
	if (args == NULL)
		res = NULL;
	else {
		res = PyEval_CallObject(callback, args);
		Py_DECREF(args);
	}
	if (res == NULL) {
		fprintf(stderr, "Exception in Dialog Filter\\n");
		PyErr_Print();
		*itemHit = -1; /* Fake return item */
		return 1; /* We handled it */
	}
	else {
		Dlg_FilterProc_callback = callback;
		if (PyInt_Check(res)) {
			*itemHit = PyInt_AsLong(res);
			rv = 1;
		}
		else
			rv = PyObject_IsTrue(res);
	}
	Py_DECREF(res);
	return rv;
}

static ModalFilterProcPtr
Dlg_PassFilterProc(PyObject *callback)
{
	PyObject *tmp = Dlg_FilterProc_callback;
	Dlg_FilterProc_callback = NULL;
	if (callback == Py_None) {
		Py_XDECREF(tmp);
		return NULL;
	}
	Py_INCREF(callback);
	Dlg_FilterProc_callback = callback;
	Py_XDECREF(tmp);
	return &Dlg_UnivFilterProc;
}

extern PyMethodChain WinObj_chain;
"""


# Define a class which specializes our object definition
class MyObjectDefinition(GlobalObjectDefinition):
	def __init__(self, name, prefix = None, itselftype = None):
		GlobalObjectDefinition.__init__(self, name, prefix, itselftype)
		self.basechain = "&WinObj_chain"
	def outputInitStructMembers(self):
		GlobalObjectDefinition.outputInitStructMembers(self)
		Output("SetWRefCon(itself, (long)it);")
	def outputCheckNewArg(self):
		Output("if (itself == NULL) return Py_None;")
	def outputCheckConvertArg(self):
		Output("if (v == Py_None) { *p_itself = NULL; return 1; }")
		Output("if (PyInt_Check(v)) { *p_itself = (DialogPtr)PyInt_AsLong(v);")
		Output("                      return 1; }")
	def outputFreeIt(self, itselfname):
		Output("DisposeDialog(%s);", itselfname)

# Create the generator groups and link them
module = MacModule('Dlg', 'Dlg', includestuff, finalstuff, initstuff)
object = MyObjectDefinition('Dialog', 'DlgObj', 'DialogPtr')
module.addobject(object)

# Create the generator classes used to populate the lists
Function = OSErrFunctionGenerator
Method = OSErrMethodGenerator

# Create and populate the lists
functions = []
methods = []
execfile("dlggen.py")

# add the populated lists to the generator groups
for f in functions: module.add(f)
for f in methods: object.add(f)

# generate output
SetOutputFileName('Dlgmodule.c')
module.generate()
