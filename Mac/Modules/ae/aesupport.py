# This script will generate the AppleEvents interface for Python.
# It uses the "bgen" package to generate C code.
# It execs the file aegen.py which contain the function definitions
# (aegen.py was generated by aescan.py, scanning the <AppleEvents.h> header file).

import addpack
addpack.addpack(':Tools:bgen:bgen')

from macsupport import *


AEArrayType = Type("AEArrayType", "c")
AESendMode = Type("AESendMode", "l")
AESendPriority = Type("AESendPriority", "h")
AEInteractAllowed = Type("AEInteractAllowed", "b")


AEEventClass = OSTypeType('AEEventClass')
AEEventID = OSTypeType('AEEventID')
AEKeyword = OSTypeType('AEKeyword')
DescType = OSTypeType('DescType')


AEDesc = OpaqueType('AEDesc')
AEDesc_ptr = OpaqueType('AEDesc')

AEAddressDesc = OpaqueType('AEAddressDesc', 'AEDesc')
AEAddressDesc_ptr = OpaqueType('AEAddressDesc', 'AEDesc')

AEDescList = OpaqueType('AEDescList', 'AEDesc')
AEDescList_ptr = OpaqueType('AEDescList', 'AEDesc')

AERecord = OpaqueType('AERecord', 'AEDesc')
AERecord_ptr = OpaqueType('AERecord', 'AEDesc')

AppleEvent = OpaqueType('AppleEvent', 'AEDesc')
AppleEvent_ptr = OpaqueType('AppleEvent', 'AEDesc')


class EHType(Type):
	def __init__(self, name = 'EventHandler', format = ''):
		Type.__init__(self, name, format)
	def declare(self, name):
		Output("AEEventHandlerUPP %s__proc__ = upp_GenericEventHandler;", name)
		Output("PyObject *%s;", name)
	def getargsFormat(self):
		return "O"
	def getargsArgs(self, name):
		return "&%s" % name
	def passInput(self, name):
		return "%s__proc__, (long)%s" % (name, name)
	def passOutput(self, name):
		return "&%s__proc__, (long *)&%s" % (name, name)
	def mkvalueFormat(self):
		return "O"
	def mkvalueArgs(self, name):
		return name
	def cleanup(self, name):
		Output("Py_INCREF(%s); /* XXX leak, but needed */", name)

class EHNoRefConType(EHType):
	def passInput(self, name):
		return "upp_GenericEventHandler"

EventHandler = EHType()
EventHandlerNoRefCon = EHNoRefConType()


IdleProcPtr = FakeType("upp_AEIdleProc")
AEIdleUPP = IdleProcPtr
EventFilterProcPtr = FakeType("(AEFilterUPP)0")
AEFilterUPP = EventFilterProcPtr
NMRecPtr = FakeType("(NMRecPtr)0")
EventHandlerProcPtr = FakeType("upp_GenericEventHandler")
AEEventHandlerUPP = EventHandlerProcPtr
AlwaysFalse = FakeType("0")


AEFunction = OSErrFunctionGenerator
AEMethod = OSErrMethodGenerator


includestuff = includestuff + """
#include <AppleEvents.h>

#ifndef HAVE_UNIVERSAL_HEADERS
#define AEIdleProcPtr IdleProcPtr
#define AEFilterProcPtr EventFilterProcPtr
#define AEEventHandlerProcPtr EventHandlerProcPtr
#endif

#ifndef HAVE_UNIVERSAL_HEADERS
/* I'm trying to setup the code here so that is easily automated,
** as follows:
** - Use the UPP in the source
** - for pre-universal headers, #define each UPP as the corresponding ProcPtr
** - for each routine we pass we declare a upp_xxx that
**   we initialize to the correct value in the init routine.
*/
#define AEIdleUPP AEIdleProcPtr
#define AEFilterUPP AEFilterProcPtr
#define AEEventHandlerUPP AEEventHandlerProcPtr
#define NewAEIdleProc(x) (x)
#define NewAEFilterProc(x) (x)
#define NewAEEventHandlerProc(x) (x)
#endif

static pascal OSErr GenericEventHandler(); /* Forward */

AEEventHandlerUPP upp_GenericEventHandler;

static pascal Boolean AEIdleProc(EventRecord *theEvent, long *sleepTime, RgnHandle *mouseRgn)
{
	if ( PyOS_InterruptOccurred() )
		return 1;
	if ( PyMac_HandleEvent(theEvent) < 0 ) {
		fprintf(stderr, "Exception in user event handler during AE processing\\n");
		PyErr_Clear();
	}
	return 0;
}

AEIdleUPP upp_AEIdleProc;
"""

finalstuff = finalstuff + """
static pascal OSErr
GenericEventHandler(AppleEvent *request, AppleEvent *reply, long refcon)
{
	PyObject *handler = (PyObject *)refcon;
	AEDescObject *requestObject, *replyObject;
	PyObject *args, *res;
	if ((requestObject = (AEDescObject *)AEDesc_New(request)) == NULL) {
		return -1;
	}
	if ((replyObject = (AEDescObject *)AEDesc_New(reply)) == NULL) {
		Py_DECREF(requestObject);
		return -1;
	}
	if ((args = Py_BuildValue("OO", requestObject, replyObject)) == NULL) {
		Py_DECREF(requestObject);
		Py_DECREF(replyObject);
		return -1;
	}
	res = PyEval_CallObject(handler, args);
	requestObject->ob_itself.descriptorType = 'null';
	requestObject->ob_itself.dataHandle = NULL;
	replyObject->ob_itself.descriptorType = 'null';
	replyObject->ob_itself.dataHandle = NULL;
	Py_DECREF(args);
	if (res == NULL)
		return -1;
	Py_DECREF(res);
	return noErr;
}
"""

initstuff = initstuff + """
	upp_AEIdleProc = NewAEIdleProc(AEIdleProc);
	upp_GenericEventHandler = NewAEEventHandlerProc(GenericEventHandler);
"""

module = MacModule('AE', 'AE', includestuff, finalstuff, initstuff)

class AEDescDefinition(GlobalObjectDefinition):

	def __init__(self, name, prefix = None, itselftype = None):
		GlobalObjectDefinition.__init__(self, name, prefix or name, itselftype or name)
		self.argref = "*"

	def outputFreeIt(self, name):
		Output("AEDisposeDesc(&%s);", name)

	def outputGetattrHook(self):
		Output("""
if (strcmp(name, "type") == 0)
	return PyMac_BuildOSType(self->ob_itself.descriptorType);
if (strcmp(name, "data") == 0) {
	PyObject *res;
	char state;
	state = HGetState(self->ob_itself.dataHandle);
	HLock(self->ob_itself.dataHandle);
	res = PyString_FromStringAndSize(
		*self->ob_itself.dataHandle,
		GetHandleSize(self->ob_itself.dataHandle));
	HUnlock(self->ob_itself.dataHandle);
	HSetState(self->ob_itself.dataHandle, state);
	return res;
}
if (strcmp(name, "__members__") == 0)
	return Py_BuildValue("[ss]", "data", "type");
""")


aedescobject = AEDescDefinition('AEDesc')
module.addobject(aedescobject)

functions = []
aedescmethods = []

execfile('aegen.py')

for f in functions: module.add(f)
for f in aedescmethods: aedescobject.add(f)

SetOutputFileName('AEmodule.c')
module.generate()
