import unittest
import os, sys
from ctypes import *
from ctypes.util import find_library
from ctypes.test import is_resource_enabled

if sys.platform == "win32":
    lib_gl = find_library("OpenGL32")
    lib_glu = find_library("Glu32")
    lib_glut = find_library("glut32")
    lib_gle = None
elif sys.platform == "darwin":
    lib_gl = lib_glu = find_library("OpenGL")
    lib_glut = find_library("GLUT")
    lib_gle = None
else:
    lib_gl = find_library("GL")
    lib_glu = find_library("GLU")
    lib_glut = find_library("glut")
    lib_gle = find_library("gle")

## print, for debugging
if is_resource_enabled("printing"):
    if lib_gl or lib_glu or lib_glut or lib_gle:
        print "OpenGL libraries:"
        for item in (("GL", lib_gl),
                     ("GLU", lib_glu),
                     ("glut", lib_glut),
                     ("gle", lib_gle)):
            print "\t", item


# On some systems, loading the OpenGL libraries needs the RTLD_GLOBAL mode.
class Test_OpenGL_libs(unittest.TestCase):
    def setUp(self):
        self.gl = self.glu = self.gle = self.glut = None
        if lib_gl:
            self.gl = CDLL(lib_gl, mode=RTLD_GLOBAL)
        if lib_glu:
            self.glu = CDLL(lib_glu, RTLD_GLOBAL)
        if lib_glut:
            # On some systems, additional libraries seem to be
            # required, loading glut fails with
            # "OSError: /usr/lib/libglut.so.3: undefined symbol: XGetExtensionVersion"
            # I cannot figure out how to repair the test on these
            # systems (red hat), so we ignore it when the glut or gle
            # libraries cannot be loaded.  See also:
            # https://sourceforge.net/tracker/?func=detail&atid=105470&aid=1478253&group_id=5470
            # http://mail.python.org/pipermail/python-dev/2006-May/064789.html
            try:
                self.glut = CDLL(lib_glut)
            except OSError:
                pass
        if lib_gle:
            try:
                self.gle = CDLL(lib_gle)
            except OSError:
                pass

    if lib_gl:
        def test_gl(self):
            if self.gl:
                self.gl.glClearIndex

    if lib_glu:
        def test_glu(self):
            if self.glu:
                self.glu.gluBeginCurve

    if lib_glut:
        def test_glut(self):
            if self.glut:
                self.glut.glutWireTetrahedron

    if lib_gle:
        def test_gle(self):
            if self.gle:
                self.gle.gleGetJoinStyle

##if os.name == "posix" and sys.platform != "darwin":

##    # On platforms where the default shared library suffix is '.so',
##    # at least some libraries can be loaded as attributes of the cdll
##    # object, since ctypes now tries loading the lib again
##    # with '.so' appended of the first try fails.
##    #
##    # Won't work for libc, unfortunately.  OTOH, it isn't
##    # needed for libc since this is already mapped into the current
##    # process (?)
##    #
##    # On MAC OSX, it won't work either, because dlopen() needs a full path,
##    # and the default suffix is either none or '.dylib'.

##    class LoadLibs(unittest.TestCase):
##        def test_libm(self):
##            import math
##            libm = cdll.libm
##            sqrt = libm.sqrt
##            sqrt.argtypes = (c_double,)
##            sqrt.restype = c_double
##            self.failUnlessEqual(sqrt(2), math.sqrt(2))

if __name__ == "__main__":
    unittest.main()
