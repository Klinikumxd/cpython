/* This is built as a stand-alone executable by the Makefile, and helps turn
   modules into frozen modules.

   This is used directly by Tools/build/freeze_modules.py, and indirectly by "make regen-frozen".

   See Python/frozen.c for more info.

   Keep this file in sync with Programs/_freeze_module.py.
*/


#include <Python.h>
#include <marshal.h>
#include "pycore_fileutils.h"     // _Py_stat_struct
#include <pycore_import.h>

#include <stdio.h>
#include <stdlib.h>               // malloc()
#include <sys/types.h>
#include <sys/stat.h>
#ifndef MS_WINDOWS
#  include <unistd.h>
#endif

/* Empty initializer for deepfrozen modules */
int _Py_Deepfreeze_Init(void)
{
    return 0;
}
/* Empty finalizer for deepfrozen modules */
void
_Py_Deepfreeze_Fini(void)
{
}

/* To avoid a circular dependency on frozen.o, we create our own structure
   of frozen modules instead, left deliberately blank so as to avoid
   unintentional import of a stale version of _frozen_importlib. */

static const struct _frozen no_modules[] = {
    {0, 0, 0} /* sentinel */
};
static const struct _module_alias aliases[] = {
    {0, 0} /* sentinel */
};

const struct _frozen *_PyImport_FrozenBootstrap;
const struct _frozen *_PyImport_FrozenStdlib;
const struct _frozen *_PyImport_FrozenTest;
const struct _frozen *PyImport_FrozenModules;
const struct _module_alias *_PyImport_FrozenAliases;

static const char header[] =
    "/* Auto-generated by Programs/_freeze_module.c */";

static void
runtime_init(void)
{
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);

    config.site_import = 0;

    PyStatus status;
    status = PyConfig_SetString(&config, &config.program_name,
                                L"./_freeze_module");
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);
        Py_ExitStatusException(status);
    }

    /* Don't install importlib, since it could execute outdated bytecode. */
    config._install_importlib = 0;
    config._init_main = 0;

    status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);
    if (PyStatus_Exception(status)) {
        Py_ExitStatusException(status);
    }
}

static const char *
read_text(const char *inpath)
{
    FILE *infile = fopen(inpath, "rb");
    if (infile == NULL) {
        fprintf(stderr, "cannot open '%s' for reading\n", inpath);
        return NULL;
    }

    struct _Py_stat_struct stat;
    if (_Py_fstat_noraise(fileno(infile), &stat)) {
        fprintf(stderr, "cannot fstat '%s'\n", inpath);
        fclose(infile);
        return NULL;
    }
    size_t text_size = (size_t)stat.st_size;

    char *text = (char *) malloc(text_size + 1);
    if (text == NULL) {
        fprintf(stderr, "could not allocate %ld bytes\n", (long) text_size);
        fclose(infile);
        return NULL;
    }
    size_t n = fread(text, 1, text_size, infile);
    fclose(infile);

    if (n < text_size) {
        fprintf(stderr, "read too short: got %ld instead of %ld bytes\n",
                (long) n, (long) text_size);
        free(text);
        return NULL;
    }

    text[text_size] = '\0';
    return (const char *)text;
}

static PyObject *
compile_and_marshal(const char *name, const char *text)
{
    char *filename = (char *) malloc(strlen(name) + 10);
    sprintf(filename, "<frozen %s>", name);
    PyObject *code = Py_CompileStringExFlags(text, filename,
                                             Py_file_input, NULL, 0);
    free(filename);
    if (code == NULL) {
        return NULL;
    }

    PyObject *marshalled = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION);
    Py_CLEAR(code);
    if (marshalled == NULL) {
        return NULL;
    }
    assert(PyBytes_CheckExact(marshalled));

    return marshalled;
}

static char *
get_varname(const char *name, const char *prefix)
{
    size_t n = strlen(prefix);
    char *varname = (char *) malloc(strlen(name) + n + 1);
    (void)strcpy(varname, prefix);
    for (size_t i = 0; name[i] != '\0'; i++) {
        if (name[i] == '.') {
            varname[n++] = '_';
        }
        else {
            varname[n++] = name[i];
        }
    }
    varname[n] = '\0';
    return varname;
}

static void
write_code(FILE *outfile, PyObject *marshalled, const char *varname)
{
    unsigned char *data = (unsigned char *) PyBytes_AS_STRING(marshalled);
    size_t data_size = PyBytes_GET_SIZE(marshalled);

    fprintf(outfile, "const unsigned char %s[] = {\n", varname);
    for (size_t n = 0; n < data_size; n += 16) {
        size_t i, end = Py_MIN(n + 16, data_size);
        fprintf(outfile, "    ");
        for (i = n; i < end; i++) {
            fprintf(outfile, "%u,", (unsigned int) data[i]);
        }
        fprintf(outfile, "\n");
    }
    fprintf(outfile, "};\n");
}

static int
write_frozen(const char *outpath, const char *inpath, const char *name,
             PyObject *marshalled)
{
    /* Open the file in text mode. The hg checkout should be using the eol extension,
       which in turn should cause the EOL style match the C library's text mode */
    FILE *outfile = fopen(outpath, "w");
    if (outfile == NULL) {
        fprintf(stderr, "cannot open '%s' for writing\n", outpath);
        return -1;
    }

    fprintf(outfile, "%s\n", header);
    char *arrayname = get_varname(name, "_Py_M__");
    write_code(outfile, marshalled, arrayname);
    free(arrayname);

    if (ferror(outfile)) {
        fprintf(stderr, "error when writing to '%s'\n", outpath);
        fclose(outfile);
        return -1;
    }
    fclose(outfile);
    return 0;
}

int
main(int argc, char *argv[])
{
    const char *name, *inpath, *outpath;

    _PyImport_FrozenBootstrap = no_modules;
    _PyImport_FrozenStdlib = no_modules;
    _PyImport_FrozenTest = no_modules;
    PyImport_FrozenModules = NULL;
    _PyImport_FrozenAliases = aliases;

    if (argc != 4) {
        fprintf(stderr, "need to specify the name, input and output paths\n");
        return 2;
    }
    name = argv[1];
    inpath = argv[2];
    outpath = argv[3];

    runtime_init();

    const char *text = read_text(inpath);
    if (text == NULL) {
        goto error;
    }

    PyObject *marshalled = compile_and_marshal(name, text);
    free((char *)text);
    if (marshalled == NULL) {
        goto error;
    }

    int res = write_frozen(outpath, inpath, name, marshalled);
    Py_DECREF(marshalled);
    if (res != 0) {
        goto error;
    }

    Py_Finalize();
    return 0;

error:
    PyErr_Print();
    Py_Finalize();
    return 1;
}

