# This script generates a Python interface for an Apple Macintosh Manager.
# It uses the "bgen" package to generate C code.
# The function specifications are generated by scanning the mamager's header file,
# using the "scantools" package (customized for this particular manager).

import string

# Declarations that change for each manager
MACHEADERFILE = 'TextEdit.h'		# The Apple header file
MODNAME = 'TE'				# The name of the module
OBJECTNAME = 'TE'			# The basic name of the objects used here
KIND = 'Handle'				# Usually 'Ptr' or 'Handle'

# The following is *usually* unchanged but may still require tuning
MODPREFIX = MODNAME			# The prefix for module-wide routines
OBJECTTYPE = "TEHandle"		# The C type used to represent them
OBJECTPREFIX = MODPREFIX + 'Obj'	# The prefix for object methods
INPUTFILE = string.lower(MODPREFIX) + 'gen.py' # The file generated by the scanner
OUTPUTFILE = MODNAME + "module.c"	# The file generated by this program

from macsupport import *

# Create the type objects
TEHandle = OpaqueByValueType("TEHandle", "TEObj")
CharsHandle = OpaqueByValueType("CharsHandle", "ResObj")
Handle = OpaqueByValueType("Handle", "ResObj")
StScrpHandle = OpaqueByValueType("StScrpHandle", "ResObj")
TEStyleHandle = OpaqueByValueType("TEStyleHandle", "ResObj")
RgnHandle = OpaqueByValueType("RgnHandle", "ResObj")

TextStyle = OpaqueType("TextStyle", "TextStyle")
TextStyle_ptr = TextStyle

includestuff = includestuff + """
#include <%s>""" % MACHEADERFILE + """

#define as_TE(h) ((TEHandle)h)
#define as_Resource(teh) ((Handle)teh)

/*
** Parse/generate TextStyle records
*/
static PyObject *
TextStyle_New(itself)
	TextStylePtr itself;
{

	return Py_BuildValue("lllO&", (long)itself->tsFont, (long)itself->tsFace, (long)itself->tsSize, QdRGB_New,
				&itself->tsColor);
}

static int
TextStyle_Convert(v, p_itself)
	PyObject *v;
	TextStylePtr p_itself;
{
	long font, face, size;
	
	if( !PyArg_ParseTuple(v, "lllO&", &font, &face, &size, QdRGB_Convert, &p_itself->tsColor) )
		return 0;
	p_itself->tsFont = (short)font;
	p_itself->tsFace = (Style)face;
	p_itself->tsSize = (short)size;
	return 1;
}
"""

class TEMethodGenerator(OSErrMethodGenerator):
	"""Similar to MethodGenerator, but has self as last argument"""

	def parseArgumentList(self, args):
		args, a0 = args[:-1], args[-1]
		t0, n0, m0 = a0
		if m0 != InMode:
			raise ValueError, "method's 'self' must be 'InMode'"
		self.itself = Variable(t0, "_self->ob_itself", SelfMode)
		FunctionGenerator.parseArgumentList(self, args)
		self.argumentList.append(self.itself)



class MyObjectDefinition(GlobalObjectDefinition):
	def outputCheckNewArg(self):
		Output("""if (itself == NULL) {
					PyErr_SetString(TE_Error,"Cannot create null TE");
					return NULL;
				}""")
	def outputFreeIt(self, itselfname):
		Output("TEDispose(%s);", itselfname)
		
	def outputGetattrHook(self):
		Output("""
			if( strcmp(name, "destRect") == 0 )
				return Py_BuildValue("O&", PyMac_BuildRect,
						&(*self->ob_itself)->destRect);
			if( strcmp(name, "viewRect") == 0 )
				return Py_BuildValue("O&", PyMac_BuildRect,
						&(*self->ob_itself)->viewRect);
			if( strcmp(name, "selRect") == 0 )
				return Py_BuildValue("O&", PyMac_BuildRect,
						&(*self->ob_itself)->selRect);
			if( strcmp(name, "lineHeight") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->lineHeight);
			if( strcmp(name, "fontAscent") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->fontAscent);
			if( strcmp(name, "selPoint") == 0 )
				return Py_BuildValue("O&", PyMac_BuildPoint,
						(*self->ob_itself)->selPoint);
			if( strcmp(name, "selStart") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->selStart);
			if( strcmp(name, "selEnd") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->selEnd);
			if( strcmp(name, "active") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->active);
			if( strcmp(name, "just") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->just);
			if( strcmp(name, "teLength") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->teLength);
			if( strcmp(name, "txFont") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->txFont);
			if( strcmp(name, "txFace") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->txFace);
			if( strcmp(name, "txMode") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->txMode);
			if( strcmp(name, "txSize") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->txSize);
			if( strcmp(name, "nLines") == 0 )
				return Py_BuildValue("h", (*self->ob_itself)->nLines);
		""")

# From here on it's basically all boiler plate...

# Create the generator groups and link them
module = MacModule(MODNAME, MODPREFIX, includestuff, finalstuff, initstuff)
object = MyObjectDefinition(OBJECTNAME, OBJECTPREFIX, OBJECTTYPE)
module.addobject(object)

# Create the generator classes used to populate the lists
Function = OSErrFunctionGenerator
Method = TEMethodGenerator

# Create and populate the lists
functions = []
methods = []
execfile(INPUTFILE)

# Converter from/to handle
f = Function(TEHandle, 'as_TE', (Handle, 'h', InMode))
functions.append(f)
f = Method(Handle, 'as_Resource', (TEHandle, 'teh', InMode))
methods.append(f)

# add the populated lists to the generator groups
# (in a different wordl the scan program would generate this)
for f in functions: module.add(f)
for f in methods: object.add(f)

# generate output (open the output file as late as possible)
SetOutputFileName(OUTPUTFILE)
module.generate()

