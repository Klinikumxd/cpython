/* Static DTrace probes interface */

#ifndef Py_DTRACE_H
#define Py_DTRACE_H

#ifdef WITH_DTRACE

#include "pydtrace_probes.h"

/* pydtrace_probes.h, on systems with DTrace, is auto-generated to include
   `PyDTrace_{PROBE}` and `PyDTrace_{PROBE}_ENABLED()` macros for every probe
   defined in pydtrace_provider.d.

   Calling these functions must be guarded by a `PyDTrace_{PROBE}_ENABLED()`
   check to minimize performance impact when probing is off. For example:

       if (PyDTrace_FUNCTION_ENTRY_ENABLED())
           PyDTrace_FUNCTION_ENTRY(f);
*/

#else

/* Without DTrace, compile to nothing. */

inline void PyDTrace_LINE(const char *arg0, const char *arg1, int arg2) {}
inline void PyDTrace_FUNCTION_ENTRY(const char *arg0, const char *arg1, int arg2)  {}
inline void PyDTrace_FUNCTION_RETURN(const char *arg0, const char *arg1, int arg2) {}
inline void PyDTrace_GC_START(int arg0) {}
inline void PyDTrace_GC_DONE(int arg0) {}
inline void PyDTrace_INSTANCE_NEW_START(int arg0) {}
inline void PyDTrace_INSTANCE_NEW_DONE(int arg0) {}
inline void PyDTrace_INSTANCE_DELETE_START(int arg0) {}
inline void PyDTrace_INSTANCE_DELETE_DONE(int arg0) {}

inline int PyDTrace_LINE_ENABLED(void) { return 0; }
inline int PyDTrace_FUNCTION_ENTRY_ENABLED(void) { return 0; }
inline int PyDTrace_FUNCTION_RETURN_ENABLED(void) { return 0; }
inline int PyDTrace_GC_START_ENABLED(void) { return 0; }
inline int PyDTrace_GC_DONE_ENABLED(void) { return 0; }
inline int PyDTrace_INSTANCE_NEW_START_ENABLED(void) { return 0; }
inline int PyDTrace_INSTANCE_NEW_DONE_ENABLED(void) { return 0; }
inline int PyDTrace_INSTANCE_DELETE_START_ENABLED(void) { return 0; }
inline int PyDTrace_INSTANCE_DELETE_DONE_ENABLED(void) { return 0; }

#endif /* !WITH_DTRACE */

#endif /* !Py_DTRACE_H */
