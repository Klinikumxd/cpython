import mkcwproject
import sys
import os
import string

PYTHONDIR = sys.prefix
PROJECTDIR = os.path.join(PYTHONDIR, ":Mac:Build")
MODULEDIRS = [	# Relative to projectdirs
	"::Modules:%s",
	"::Modules",
	":::Modules",
]

# Global variable to control forced rebuild (otherwise the project is only rebuilt
# when it is changed)
FORCEREBUILD=0

def relpath(base, path):
	"""Turn abs path into path relative to another. Only works for 2 abs paths
	both pointing to folders"""
	if not os.path.isabs(base) or not os.path.isabs(path):
		raise 'Absolute paths only'
	if base[-1] == ':':
		base = base[:-1]
	basefields = string.split(base, os.sep)
	pathfields = string.split(path, os.sep)
	commonfields = len(os.path.commonprefix((basefields, pathfields)))
	basefields = basefields[commonfields:]
	pathfields = pathfields[commonfields:]
	pathfields = ['']*(len(basefields)+1) + pathfields
	rv = string.join(pathfields, os.sep)
	return rv

def genpluginproject(architecture, module,
		project=None, projectdir=None,
		sources=[], sourcedirs=[],
		libraries=[], extradirs=[],
		extraexportsymbols=[], outputdir=":::Lib:lib-dynload"):
	if architecture == "all":
		# For the time being we generate two project files. Not as nice as
		# a single multitarget project, but easier to implement for now.
		genpluginproject("ppc", module, project, projectdir, sources, sourcedirs,
				libraries, extradirs, extraexportsymbols, outputdir)
		genpluginproject("carbon", module, project, projectdir, sources, sourcedirs,
				libraries, extradirs, extraexportsymbols, outputdir)
		return
	templatename = "template-%s" % architecture
	targetname = "%s.%s" % (module, architecture)
	dllname = "%s.%s.slb" % (module, architecture)
	if not project:
		if architecture != "ppc":
			project = "%s.%s.mcp"%(module, architecture)
		else:
			project = "%s.mcp"%module
	if not projectdir:
		projectdir = PROJECTDIR
	if not sources:
		sources = [module + 'module.c']
	if not sourcedirs:
		for moduledir in MODULEDIRS:
			if '%' in moduledir:
				moduledir = moduledir % module
			fn = os.path.join(projectdir, os.path.join(moduledir, sources[0]))
			if os.path.exists(fn):
				moduledir, sourcefile = os.path.split(fn)
				sourcedirs = [relpath(projectdir, moduledir)]
				sources[0] = sourcefile
				break
		else:
			print "Warning: %s: sourcefile not found: %s"%(module, sources[0])
			sourcedirs = []
	if architecture == "carbon":
		prefixname = "mwerks_carbonplugin_config.h"
	else:
		prefixname = "mwerks_plugin_config.h"
	dict = {
		"sysprefix" : relpath(projectdir, sys.prefix),
		"sources" : sources,
		"extrasearchdirs" : sourcedirs + extradirs,
		"libraries": libraries,
		"mac_outputdir" : outputdir,
		"extraexportsymbols" : extraexportsymbols,
		"mac_targetname" : targetname,
		"mac_dllname" : dllname,
		"prefixname" : prefixname,
	}
	mkcwproject.mkproject(os.path.join(projectdir, project), module, dict, 
			force=FORCEREBUILD, templatename=templatename)

def	genallprojects(force=0):
	global FORCEREBUILD
	FORCEREBUILD = force
	# Standard Python modules
	genpluginproject("all", "pyexpat", 
		sources=["pyexpat.c"], 
		libraries=["libexpat.ppc.lib"], 
		extradirs=["::::expat:*"])
	genpluginproject("all", "zlib", 
		libraries=["zlib.ppc.Lib"], 
		extradirs=["::::imglibs:zlib:mac", "::::imglibs:zlib"])
	genpluginproject("all", "gdbm", 
		libraries=["gdbm.ppc.gusi.lib"], 
		extradirs=["::::gdbm:mac", "::::gdbm"])
	genpluginproject("all", "_weakref", sources=["_weakref.c"])
	genpluginproject("all", "_symtable", sources=["symtablemodule.c"])
	genpluginproject("all", "_testcapi")
	
	# bgen-generated Toolbox modules
	genpluginproject("carbon", "AE", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "AE", libraries=["ObjectSupportLib"], outputdir="::Lib:Carbon")
	genpluginproject("ppc", "App", libraries=["AppearanceLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "App", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Cm", libraries=["QuickTimeLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Cm", outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Ctl", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Ctl", libraries=["ControlsLib", "AppearanceLib"], 
			outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Dlg", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Dlg", libraries=["DialogsLib", "AppearanceLib"],
			outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Drag", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Drag", libraries=["DragLib"], outputdir="::Lib:Carbon")
	genpluginproject("all", "Evt", outputdir="::Lib:Carbon")
	genpluginproject("all", "Fm", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Help", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Icn", libraries=["IconServicesLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Icn", outputdir="::Lib:Carbon")
	genpluginproject("all", "List", outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Menu", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Menu", libraries=["MenusLib", "ContextualMenu", "AppearanceLib"],
			outputdir="::Lib:Carbon")
	genpluginproject("all", "Qd", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Qt", libraries=["QuickTimeLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Qt", outputdir="::Lib:Carbon")
	genpluginproject("all", "Qdoffs", outputdir="::Lib:Carbon")
	genpluginproject("all", "Res", outputdir="::Lib:Carbon")
	genpluginproject("all", "Scrap", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Snd", libraries=["SoundLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Snd", outputdir="::Lib:Carbon")
	genpluginproject("all", "Sndihooks", sources=[":snd:Sndihooks.c"], outputdir="::Lib:Carbon")
	genpluginproject("ppc", "TE", libraries=["DragLib"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "TE", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Mlte", libraries=["Textension"], outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Mlte", outputdir="::Lib:Carbon")
	genpluginproject("carbon", "Win", outputdir="::Lib:Carbon")
	genpluginproject("ppc", "Win", libraries=["WindowsLib", "AppearanceLib"],
			outputdir="::Lib:Carbon")
	# Carbon Only?
	genpluginproject("carbon", "CF", outputdir="::Lib:Carbon")
	
	# Other Mac modules
	genpluginproject("all", "calldll", sources=["calldll.c"])
	genpluginproject("all", "ColorPicker")
	genpluginproject("ppc", "Printing")
	genpluginproject("ppc", "waste",
		sources=[
			"wastemodule.c",
			'WEAccessors.c', 'WEBirthDeath.c', 'WEDebug.c',
			'WEDrawing.c', 'WEFontTables.c', 'WEHighLevelEditing.c',
			'WEICGlue.c', 'WEInlineInput.c', 'WELineLayout.c', 'WELongCoords.c',
			'WELowLevelEditing.c', 'WEMouse.c', 'WEObjects.c', 'WEScraps.c',
			'WESelecting.c', 'WESelectors.c', 'WEUserSelectors.c', 'WEUtilities.c',
			'WEObjectHandlers.c',
			'WETabs.c',
			'WETabHooks.c'],
		libraries=['DragLib'],
		extradirs=[
			'::::Waste 1.3 Distribution:*',
			'::::ICProgKit1.4:APIs']
		)
	# This is a hack, combining parts of Waste 2.0 with parts of 1.3
	genpluginproject("carbon", "waste",
		sources=[
			"wastemodule.c",
			"WEObjectHandlers.c",
			"WETabs.c", "WETabHooks.c"],
		libraries=["WASTE.Carbon.lib"],
		extradirs=[
			'{Compiler}:MacOS Support:(Third Party Support):Waste 2.0 Distribution:C_C++ Headers',
			'{Compiler}:MacOS Support:(Third Party Support):Waste 2.0 Distribution:Static Libraries',
			'::::Waste 1.3 Distribution:Extras:Sample Object Handlers',
			'::::Waste 1.3 Distribution:Extras:Waste Tabs 1.3.2']
		)
	genpluginproject("ppc", "ctb")
	genpluginproject("ppc", "icglue", sources=["icgluemodule.c"], 
		libraries=["ICGlueCFM-PPC.lib"], 
		extradirs=["::::ICProgKit1.4:APIs"])
	genpluginproject("carbon", "icglue", sources=["icgluemodule.c"], 
		extradirs=["::::ICProgKit1.4:APIs"])
	genpluginproject("ppc", "macspeech", libraries=["SpeechLib"])

if __name__ == '__main__':
	genallprojects()
	
	
