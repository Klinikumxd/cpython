# This script generates a Python interface for an Apple Macintosh Manager.
# It uses the "bgen" package to generate C code.
# The function specifications are generated by scanning the mamager's header file,
# using the "scantools" package (customized for this particular manager).

import string

# Declarations that change for each manager
MACHEADERFILE = 'Files.h'		# The Apple header file
MODNAME = '_File'				# The name of the module

# The following is *usually* unchanged but may still require tuning
MODPREFIX = 'File'			# The prefix for module-wide routines
INPUTFILE = string.lower(MODPREFIX) + 'gen.py' # The file generated by the scanner
OUTPUTFILE = MODNAME + "module.c"	# The file generated by this program

from macsupport import *

# Create the type objects
#ConstStrFileNameParam = ConstStr255Param
#StrFileName = Str255
#FolderClass = OSTypeType("FolderClass")
# FolderDesc
#FolderDescFlags = Type("FolderDescFlags", "l")
#FolderLocation = OSTypeType("FolderLocation")
# FolderRouting
#FolderType = OSTypeType("FolderType")
#RoutingFlags = Type("RoutingFlags", "l")

class UniCharCountBuffer(InputOnlyType):
	pass

#CatPositionRec
ConstStr63Param = OpaqueArrayType("Str63", "PyMac_BuildStr255", "PyMac_GetStr255")
FInfo = OpaqueByValueStructType("FInfo", "PyMac_BuildFInfo", "PyMac_GetFInfo")
FInfo_ptr = OpaqueType("FInfo", "PyMac_BuildFInfo", "PyMac_GetFInfo")
FNMessage = Type("FNMessage", "l")
FSAllocationFlags = Type("FSAllocationFlags", "H")
#FSCatalogInfo
FSCatalogInfoBitmap = Type("FSCatalogInfoBitmap", "l")
#FSForkInfo
#FSIterator
FSIteratorFlags = Type("FSIteratorFlags", "l")
#FSVolumeInfo
FSVolumeRefNum = Type("FSVolumeRefNum", "h")
HFSUniStr255 = OpaqueType("HFSUniStr255", "PyMac_BuildHFSUniStr255", "PyMac_GetHFSUniStr255")
SInt64 = Type("SInt64", "L")
UInt64 = Type("UInt64", "L")
#UInt8_ptr
#UniCharCount
#char_ptr
#void_ptr


includestuff = includestuff + """
#ifdef WITHOUT_FRAMEWORKS
#include <Files.h>
#else
#include <Carbon/Carbon.h>
#endif

/*
** Parse/generate objsect
*/
static PyObject *
PyMac_BuildHFSUniStr255(HFSUniStr255 *itself)
{

	return Py_BuildValue("u#", itself->unicode, itself->length);
}

#if 0
static int
PyMac_GetHFSUniStr255(PyObject *v, HFSUniStr255 *itself)
{
	return PyArg_ParseTuple(v, "O&O&O&O&O&",
		PyMac_GetFixed, &itself->ascent,
		PyMac_GetFixed, &itself->descent,
		PyMac_GetFixed, &itself->leading,
		PyMac_GetFixed, &itself->widMax,
		ResObj_Convert, &itself->wTabHandle);
}
#endif

/*
** Parse/generate objsect
*/
static PyObject *
PyMac_BuildFInfo(FInfo *itself)
{

	return Py_BuildValue("O&O&HO&h",
		PyMac_BuildOSType, itself->fdType,
		PyMac_BuildOSType, itself->fdCreator,
		itself->fdFlags,
		PyMac_BuildPoint, &itself->fdLocation,
		itself->fdFldr);
}

static int
PyMac_GetFInfo(PyObject *v, FInfo *itself)
{
	return PyArg_ParseTuple(v, "O&O&HO&h",
		PyMac_GetOSType, &itself->fdType,
		PyMac_GetOSType, &itself->fdCreator,
		&itself->fdFlags,
		PyMac_GetPoint, &itself->fdLocation,
		&itself->fdFldr);
}

"""

execfile(string.lower(MODPREFIX) + 'typetest.py')

# From here on it's basically all boiler plate...

# Create the generator groups and link them
module = MacModule(MODNAME, MODPREFIX, includestuff, finalstuff, initstuff)

# Create the generator classes used to populate the lists
Function = OSErrFunctionGenerator

# Create and populate the lists
functions = []
execfile(INPUTFILE)

# Manual generators:
FSRefMakePath_body = """
OSStatus _err;
FSRef ref;
#define MAXPATHNAME 1024
UInt8 path[MAXPATHNAME];
UInt32 maxPathSize = MAXPATHNAME;

if (!PyArg_ParseTuple(_args, "O&",
					  PyMac_GetFSRef, &ref))
	return NULL;
_err = FSRefMakePath(&ref,
					 path,
					 maxPathSize);
if (_err != noErr) return PyMac_Error(_err);
_res = Py_BuildValue("s", path);
return _res;
"""
f = ManualGenerator("FSRefMakePath", FSRefMakePath_body)
f.docstring = lambda: "(FSRef) -> string"
functions.append(f)

# add the populated lists to the generator groups
# (in a different wordl the scan program would generate this)
for f in functions: module.add(f)

# generate output (open the output file as late as possible)
SetOutputFileName(OUTPUTFILE)
module.generate()

