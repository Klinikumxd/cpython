import sys, os, string

WINMAINTEMPLATE = """
#include <windows.h>

int WINAPI WinMain(
    HINSTANCE hInstance,      // handle to current instance
    HINSTANCE hPrevInstance,  // handle to previous instance
    LPSTR lpCmdLine,          // pointer to command line
    int nCmdShow              // show state of window
    )
{
    return main(__argc, __argv);
}
"""

def makemakefile(outfp, vars, files, target):
    save = sys.stdout
    try:
        sys.stdout = outfp
        realwork(vars, files, target)
    finally:
        sys.stdout = save

def realwork(vars, files, target):
    print "# Makefile for Windows (NT or 95) generated by freeze.py script"
    print
    print "target =", target
    print "pythonhome =", vars['prefix']
    # XXX The following line is fishy and may need manual fixing
    print "pythonlib =", vars['exec_prefix'] + "/pcbuild/release/python15.lib"
    print "subsystem =", vars['subsystem']
    print
    print "all: $(target).exe"
    print

    objects = []
    for file in files:
        base = os.path.basename(file)
        base, ext = os.path.splitext(base)
        objects.append(base + ".obj")
        print "%s.obj: %s" % (base, file)
        print "\t$(CC) -c $(cdl)",
        print "-I$(pythonhome)/Include  -I$(pythonhome)/PC \\"
        print "\t\t$(cflags) $(cdebug) $(cinclude) \\"
        print "\t\t", file
	print

    print "$(target).exe:",
    for obj in objects: print obj,
    print
    print "\tlink -out:$(target).exe",
    for obj in objects: print obj,
    print "\\"
    print "\t\t$(pythonlib) $(lcustom) shell32.lib comdlg32.lib wsock32.lib \\"
    print "\t\t-subsystem:$(subsystem) $(resources)"

# Local Variables:
# indent-tabs-mode: nil
# End:
