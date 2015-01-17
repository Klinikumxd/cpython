// Resource script for Python core DLL.
// Currently only holds version information.
//
#include "winver.h"

#define PYTHON_COMPANY   "Python Software Foundation"
#define PYTHON_COPYRIGHT "Copyright � 2001-2015 Python Software Foundation. Copyright � 2000 BeOpen.com. Copyright � 1995-2001 CNRI. Copyright � 1991-1995 SMC."

#define MS_WINDOWS
#include "modsupport.h"
#include "patchlevel.h"
#ifdef _DEBUG
#   include "pythonnt_rc_d.h"
#   define PYTHON_DEBUG_EXT "_d"
#else
#   include "pythonnt_rc.h"
#   define PYTHON_DEBUG_EXT
#endif

/* e.g., 3.3.0a1
 * PY_VERSION comes from patchlevel.h
 */
#define PYTHON_VERSION PY_VERSION "\0"

/* 64-bit version number as comma-separated list of 4 16-bit ints */
#if PY_MICRO_VERSION > 64
#   error "PY_MICRO_VERSION > 64"
#endif
#if PY_RELEASE_LEVEL > 99
#   error "PY_RELEASE_LEVEL > 99"
#endif
#if PY_RELEASE_SERIAL > 9
#   error "PY_RELEASE_SERIAL > 9"
#endif
#define PYVERSION64 PY_MAJOR_VERSION, PY_MINOR_VERSION, FIELD3, PYTHON_API_VERSION
