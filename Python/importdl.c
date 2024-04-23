
/* Support for dynamic loading of extension modules */

#include "Python.h"
#include "pycore_call.h"
#include "pycore_import.h"
#include "pycore_pyerrors.h"      // _PyErr_FormatFromCause()
#include "pycore_pystate.h"
#include "pycore_runtime.h"

/* ./configure sets HAVE_DYNAMIC_LOADING if dynamic loading of modules is
   supported on this platform. configure will then compile and link in one
   of the dynload_*.c files, as appropriate. We will call a function in
   those modules to get a function pointer to the module's init function.
*/
#ifdef HAVE_DYNAMIC_LOADING

#include "pycore_importdl.h"

#ifdef MS_WINDOWS
extern dl_funcptr _PyImport_FindSharedFuncptrWindows(const char *prefix,
                                                     const char *shortname,
                                                     PyObject *pathname,
                                                     FILE *fp);
#else
extern dl_funcptr _PyImport_FindSharedFuncptr(const char *prefix,
                                              const char *shortname,
                                              const char *pathname, FILE *fp);
#endif

static const char * const ascii_only_prefix = "PyInit";
static const char * const nonascii_prefix = "PyInitU";

/* Get the variable part of a module's export symbol name.
 * Returns a bytes instance. For non-ASCII-named modules, the name is
 * encoded as per PEP 489.
 * The hook_prefix pointer is set to either ascii_only_prefix or
 * nonascii_prefix, as appropriate.
 */
static PyObject *
get_encoded_name(PyObject *name, const char **hook_prefix) {
    PyObject *tmp;
    PyObject *encoded = NULL;
    PyObject *modname = NULL;
    Py_ssize_t name_len, lastdot;

    /* Get the short name (substring after last dot) */
    name_len = PyUnicode_GetLength(name);
    if (name_len < 0) {
        return NULL;
    }
    lastdot = PyUnicode_FindChar(name, '.', 0, name_len, -1);
    if (lastdot < -1) {
        return NULL;
    } else if (lastdot >= 0) {
        tmp = PyUnicode_Substring(name, lastdot + 1, name_len);
        if (tmp == NULL)
            return NULL;
        name = tmp;
        /* "name" now holds a new reference to the substring */
    } else {
        Py_INCREF(name);
    }

    /* Encode to ASCII or Punycode, as needed */
    encoded = PyUnicode_AsEncodedString(name, "ascii", NULL);
    if (encoded != NULL) {
        *hook_prefix = ascii_only_prefix;
    } else {
        if (PyErr_ExceptionMatches(PyExc_UnicodeEncodeError)) {
            PyErr_Clear();
            encoded = PyUnicode_AsEncodedString(name, "punycode", NULL);
            if (encoded == NULL) {
                goto error;
            }
            *hook_prefix = nonascii_prefix;
        } else {
            goto error;
        }
    }

    /* Replace '-' by '_' */
    modname = _PyObject_CallMethod(encoded, &_Py_ID(replace), "cc", '-', '_');
    if (modname == NULL)
        goto error;

    Py_DECREF(name);
    Py_DECREF(encoded);
    return modname;
error:
    Py_DECREF(name);
    Py_XDECREF(encoded);
    return NULL;
}

PyObject *
_PyImport_LoadDynamicModuleWithSpec(PyObject *spec, FILE *fp)
{
#ifndef MS_WINDOWS
    PyObject *filename_bytes = NULL;
    const char *filename_buf;
#endif
    PyObject *name_unicode = NULL, *name = NULL, *filename = NULL, *m = NULL;
    const char *name_buf, *hook_prefix;
    const char *oldcontext, *newcontext;
    dl_funcptr exportfunc;
    PyModuleDef *def;
    PyModInitFunction p0;

    name_unicode = PyObject_GetAttrString(spec, "name");
    if (name_unicode == NULL) {
        return NULL;
    }
    if (!PyUnicode_Check(name_unicode)) {
        PyErr_SetString(PyExc_TypeError,
                        "spec.name must be a string");
        goto error;
    }
    newcontext = PyUnicode_AsUTF8(name_unicode);
    if (newcontext == NULL) {
        goto error;
    }

    name = get_encoded_name(name_unicode, &hook_prefix);
    if (name == NULL) {
        goto error;
    }
    name_buf = PyBytes_AS_STRING(name);

    filename = PyObject_GetAttrString(spec, "origin");
    if (filename == NULL) {
        goto error;
    }

#ifdef MS_WINDOWS
    exportfunc = _PyImport_FindSharedFuncptrWindows(
            hook_prefix, name_buf, filename, fp);
#else
    filename_bytes = PyUnicode_EncodeFSDefault(filename);
    if (filename_bytes == NULL) {
        goto error;
    }
    filename_buf = PyBytes_AS_STRING(filename_bytes);
    exportfunc = _PyImport_FindSharedFuncptr(
            hook_prefix, name_buf, filename_buf, fp);
    Py_DECREF(filename_bytes);
#endif

    if (exportfunc == NULL) {
        if (!PyErr_Occurred()) {
            PyObject *msg;
            msg = PyUnicode_FromFormat(
                "dynamic module does not define "
                "module export function (%s_%s)",
                hook_prefix, name_buf);
            if (msg == NULL)
                goto error;
            PyErr_SetImportError(msg, name_unicode, filename);
            Py_DECREF(msg);
        }
        goto error;
    }

    p0 = (PyModInitFunction)exportfunc;

    /* Package context is needed for single-phase init */
    oldcontext = _PyImport_SwapPackageContext(newcontext);
    m = p0();
    _PyImport_SwapPackageContext(oldcontext);

    if (m == NULL) {
        if (!PyErr_Occurred()) {
            PyErr_Format(
                PyExc_SystemError,
                "initialization of %s failed without raising an exception",
                name_buf);
        }
        goto error;
    } else if (PyErr_Occurred()) {
        _PyErr_FormatFromCause(
            PyExc_SystemError,
            "initialization of %s raised unreported exception",
            name_buf);
        m = NULL;
        goto error;
    }
    if (Py_IS_TYPE(m, NULL)) {
        /* This can happen when a PyModuleDef is returned without calling
         * PyModuleDef_Init on it
         */
        PyErr_Format(PyExc_SystemError,
                     "init function of %s returned uninitialized object",
                     name_buf);
        m = NULL; /* prevent segfault in DECREF */
        goto error;
    }
    if (PyObject_TypeCheck(m, &PyModuleDef_Type)) {
        Py_DECREF(name_unicode);
        Py_DECREF(name);
        Py_DECREF(filename);
        return PyModule_FromDefAndSpec((PyModuleDef*)m, spec);
    }

    /* Fall back to single-phase init mechanism */

    if (_PyImport_CheckSubinterpIncompatibleExtensionAllowed(name_buf) < 0) {
        goto error;
    }

    if (hook_prefix == nonascii_prefix) {
        /* don't allow legacy init for non-ASCII module names */
        PyErr_Format(
            PyExc_SystemError,
            "initialization of %s did not return PyModuleDef",
            name_buf);
        goto error;
    }

    /* Remember pointer to module init function. */
    def = PyModule_GetDef(m);
    if (def == NULL) {
        PyErr_Format(PyExc_SystemError,
                     "initialization of %s did not return an extension "
                     "module", name_buf);
        goto error;
    }
    def->m_base.m_init = p0;

    PyObject *modules = PyImport_GetModuleDict();
    if (_PyImport_FixupExtensionObject(m, name_unicode, filename, modules) < 0)
        goto error;

    Py_DECREF(name_unicode);
    Py_DECREF(name);
    Py_DECREF(filename);

    return m;

error:
    Py_DECREF(name_unicode);
    Py_XDECREF(name);
    Py_XDECREF(filename);
    Py_XDECREF(m);
    return NULL;
}

#endif /* HAVE_DYNAMIC_LOADING */
