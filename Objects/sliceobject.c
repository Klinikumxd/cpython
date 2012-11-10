/*
Written by Jim Hugunin and Chris Chase.

This includes both the singular ellipsis object and slice objects.

Guido, feel free to do whatever you want in the way of copyrights
for this file.
*/

/*
Py_Ellipsis encodes the '...' rubber index token. It is similar to
the Py_NoneStruct in that there is no way to create other objects of
this type and there is exactly one in existence.
*/

#include "Python.h"
#include "structmember.h"

static PyObject *
ellipsis_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    if (PyTuple_GET_SIZE(args) || (kwargs && PyDict_Size(kwargs))) {
        PyErr_SetString(PyExc_TypeError, "EllipsisType takes no arguments");
        return NULL;
    }
    Py_INCREF(Py_Ellipsis);
    return Py_Ellipsis;
}

static PyObject *
ellipsis_repr(PyObject *op)
{
    return PyUnicode_FromString("Ellipsis");
}

PyTypeObject PyEllipsis_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "ellipsis",                         /* tp_name */
    0,                                  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    0, /*never called*/                 /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    ellipsis_repr,                      /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                 /* tp_flags */
    0,                                  /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    ellipsis_new,                       /* tp_new */
};

PyObject _Py_EllipsisObject = {
    _PyObject_EXTRA_INIT
    1, &PyEllipsis_Type
};


/* Slice object implementation */

/* Using a cache is very effective since typically only a single slice is
 * created and then deleted again
 */
static PySliceObject *slice_cache = NULL;
void PySlice_Fini(void)
{
    PySliceObject *obj = slice_cache;
    if (obj != NULL) {
        slice_cache = NULL;
        PyObject_Del(obj);
    }
}

/* start, stop, and step are python objects with None indicating no
   index is present.
*/

PyObject *
PySlice_New(PyObject *start, PyObject *stop, PyObject *step)
{
    PySliceObject *obj;
    if (slice_cache != NULL) {
        obj = slice_cache;
        slice_cache = NULL;
        _Py_NewReference((PyObject *)obj);
    } else {
        obj = PyObject_New(PySliceObject, &PySlice_Type);
        if (obj == NULL)
            return NULL;
    }

    if (step == NULL) step = Py_None;
    Py_INCREF(step);
    if (start == NULL) start = Py_None;
    Py_INCREF(start);
    if (stop == NULL) stop = Py_None;
    Py_INCREF(stop);

    obj->step = step;
    obj->start = start;
    obj->stop = stop;

    return (PyObject *) obj;
}

PyObject *
_PySlice_FromIndices(Py_ssize_t istart, Py_ssize_t istop)
{
    PyObject *start, *end, *slice;
    start = PyLong_FromSsize_t(istart);
    if (!start)
        return NULL;
    end = PyLong_FromSsize_t(istop);
    if (!end) {
        Py_DECREF(start);
        return NULL;
    }

    slice = PySlice_New(start, end, NULL);
    Py_DECREF(start);
    Py_DECREF(end);
    return slice;
}

int
PySlice_GetIndices(PyObject *_r, Py_ssize_t length,
                   Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step)
{
    PySliceObject *r = (PySliceObject*)_r;
    /* XXX support long ints */
    if (r->step == Py_None) {
        *step = 1;
    } else {
        if (!PyLong_Check(r->step)) return -1;
        *step = PyLong_AsSsize_t(r->step);
    }
    if (r->start == Py_None) {
        *start = *step < 0 ? length-1 : 0;
    } else {
        if (!PyLong_Check(r->start)) return -1;
        *start = PyLong_AsSsize_t(r->start);
        if (*start < 0) *start += length;
    }
    if (r->stop == Py_None) {
        *stop = *step < 0 ? -1 : length;
    } else {
        if (!PyLong_Check(r->stop)) return -1;
        *stop = PyLong_AsSsize_t(r->stop);
        if (*stop < 0) *stop += length;
    }
    if (*stop > length) return -1;
    if (*start >= length) return -1;
    if (*step == 0) return -1;
    return 0;
}

int
PySlice_GetIndicesEx(PyObject *_r, Py_ssize_t length,
                     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
                     Py_ssize_t *slicelength)
{
    PySliceObject *r = (PySliceObject*)_r;
    /* this is harder to get right than you might think */

    Py_ssize_t defstart, defstop;

    if (r->step == Py_None) {
        *step = 1;
    }
    else {
        if (!_PyEval_SliceIndex(r->step, step)) return -1;
        if (*step == 0) {
            PyErr_SetString(PyExc_ValueError,
                            "slice step cannot be zero");
            return -1;
        }
        /* Here *step might be -PY_SSIZE_T_MAX-1; in this case we replace it
         * with -PY_SSIZE_T_MAX.  This doesn't affect the semantics, and it
         * guards against later undefined behaviour resulting from code that
         * does "step = -step" as part of a slice reversal.
         */
        if (*step < -PY_SSIZE_T_MAX)
            *step = -PY_SSIZE_T_MAX;
    }

    defstart = *step < 0 ? length-1 : 0;
    defstop = *step < 0 ? -1 : length;

    if (r->start == Py_None) {
        *start = defstart;
    }
    else {
        if (!_PyEval_SliceIndex(r->start, start)) return -1;
        if (*start < 0) *start += length;
        if (*start < 0) *start = (*step < 0) ? -1 : 0;
        if (*start >= length)
            *start = (*step < 0) ? length - 1 : length;
    }

    if (r->stop == Py_None) {
        *stop = defstop;
    }
    else {
        if (!_PyEval_SliceIndex(r->stop, stop)) return -1;
        if (*stop < 0) *stop += length;
        if (*stop < 0) *stop = (*step < 0) ? -1 : 0;
        if (*stop >= length)
            *stop = (*step < 0) ? length - 1 : length;
    }

    if ((*step < 0 && *stop >= *start)
        || (*step > 0 && *start >= *stop)) {
        *slicelength = 0;
    }
    else if (*step < 0) {
        *slicelength = (*stop-*start+1)/(*step)+1;
    }
    else {
        *slicelength = (*stop-*start-1)/(*step)+1;
    }

    return 0;
}

static PyObject *
slice_new(PyTypeObject *type, PyObject *args, PyObject *kw)
{
    PyObject *start, *stop, *step;

    start = stop = step = NULL;

    if (!_PyArg_NoKeywords("slice()", kw))
        return NULL;

    if (!PyArg_UnpackTuple(args, "slice", 1, 3, &start, &stop, &step))
        return NULL;

    /* This swapping of stop and start is to maintain similarity with
       range(). */
    if (stop == NULL) {
        stop = start;
        start = NULL;
    }
    return PySlice_New(start, stop, step);
}

PyDoc_STRVAR(slice_doc,
"slice(stop)\n\
slice(start, stop[, step])\n\
\n\
Create a slice object.  This is used for extended slicing (e.g. a[0:10:2]).");

static void
slice_dealloc(PySliceObject *r)
{
    Py_DECREF(r->step);
    Py_DECREF(r->start);
    Py_DECREF(r->stop);
    if (slice_cache == NULL)
        slice_cache = r;
    else
        PyObject_Del(r);
}

static PyObject *
slice_repr(PySliceObject *r)
{
    return PyUnicode_FromFormat("slice(%R, %R, %R)", r->start, r->stop, r->step);
}

static PyMemberDef slice_members[] = {
    {"start", T_OBJECT, offsetof(PySliceObject, start), READONLY},
    {"stop", T_OBJECT, offsetof(PySliceObject, stop), READONLY},
    {"step", T_OBJECT, offsetof(PySliceObject, step), READONLY},
    {0}
};

/* Helper function to convert a slice argument to a PyLong, and raise TypeError
   with a suitable message on failure. */

static PyObject*
evaluate_slice_index(PyObject *v)
{
    if (PyIndex_Check(v)) {
        return PyNumber_Index(v);
    }
    else {
        PyErr_SetString(PyExc_TypeError,
                        "slice indices must be integers or "
                        "None or have an __index__ method");
        return NULL;
    }
}

/* Implementation of slice.indices. */

static PyObject*
slice_indices(PySliceObject* self, PyObject* len)
{
    PyObject *start=NULL, *stop=NULL, *step=NULL;
    PyObject *length=NULL, *upper=NULL, *lower=NULL, *zero=NULL;
    int step_is_negative, cmp;

    zero = PyLong_FromLong(0L);
    if (zero == NULL)
        return NULL;

    /* Compute step and length as integers. */
    length = PyNumber_Index(len);
    if (length == NULL)
        goto error;

    if (self->step == Py_None)
        step = PyLong_FromLong(1L);
    else
        step = evaluate_slice_index(self->step);
    if (step == NULL)
        goto error;

    /* Raise ValueError for negative length or zero step. */
    cmp = PyObject_RichCompareBool(length, zero, Py_LT);
    if (cmp < 0) {
        goto error;
    }
    if (cmp) {
        PyErr_SetString(PyExc_ValueError,
                        "length should not be negative");
        goto error;
    }

    cmp = PyObject_RichCompareBool(step, zero, Py_EQ);
    if (cmp < 0) {
        goto error;
    }
    if (cmp) {
        PyErr_SetString(PyExc_ValueError,
                        "slice step cannot be zero");
        goto error;
    }

    /* Find lower and upper bounds for start and stop. */
    step_is_negative = PyObject_RichCompareBool(step, zero, Py_LT);
    if (step_is_negative < 0) {
        goto error;
    }
    if (step_is_negative) {
        lower = PyLong_FromLong(-1L);
        if (lower == NULL)
            goto error;

        upper = PyNumber_Add(length, lower);
        if (upper == NULL)
            goto error;
    }
    else {
        lower = zero;
        Py_INCREF(lower);
        upper = length;
        Py_INCREF(upper);
    }

    /* Compute start. */
    if (self->start == Py_None) {
        start = step_is_negative ? upper : lower;
        Py_INCREF(start);
    }
    else {
        start = evaluate_slice_index(self->start);
        if (start == NULL)
            goto error;

        cmp = PyObject_RichCompareBool(start, zero, Py_LT);
        if (cmp < 0)
            goto error;
        if (cmp) {
            /* start += length */
            PyObject *tmp = PyNumber_Add(start, length);
            Py_DECREF(start);
            start = tmp;
            if (start == NULL)
                goto error;

            cmp = PyObject_RichCompareBool(start, lower, Py_LT);
            if (cmp < 0)
                goto error;
            if (cmp) {
                Py_INCREF(lower);
                Py_DECREF(start);
                start = lower;
            }
        }
        else {
            cmp = PyObject_RichCompareBool(start, upper, Py_GT);
            if (cmp < 0)
                goto error;
            if (cmp) {
                Py_INCREF(upper);
                Py_DECREF(start);
                start = upper;
            }
        }
    }

    /* Compute stop. */
    if (self->stop == Py_None) {
        stop = step_is_negative ? lower : upper;
        Py_INCREF(stop);
    }
    else {
        stop = evaluate_slice_index(self->stop);
        if (stop == NULL)
            goto error;

        cmp = PyObject_RichCompareBool(stop, zero, Py_LT);
        if (cmp < 0)
            goto error;
        if (cmp) {
            /* stop += length */
            PyObject *tmp = PyNumber_Add(stop, length);
            Py_DECREF(stop);
            stop = tmp;
            if (stop == NULL)
                goto error;

            cmp = PyObject_RichCompareBool(stop, lower, Py_LT);
            if (cmp < 0)
                goto error;
            if (cmp) {
                Py_INCREF(lower);
                Py_DECREF(stop);
                stop = lower;
            }
        }
        else {
            cmp = PyObject_RichCompareBool(stop, upper, Py_GT);
            if (cmp < 0)
                goto error;
            if (cmp) {
                Py_INCREF(upper);
                Py_DECREF(stop);
                stop = upper;
            }
        }
    }

    Py_DECREF(upper);
    Py_DECREF(lower);
    Py_DECREF(length);
    Py_DECREF(zero);
    return Py_BuildValue("(NNN)", start, stop, step);

  error:
    Py_XDECREF(start);
    Py_XDECREF(stop);
    Py_XDECREF(step);
    Py_XDECREF(upper);
    Py_XDECREF(lower);
    Py_XDECREF(length);
    Py_XDECREF(zero);
    return NULL;
}


PyDoc_STRVAR(slice_indices_doc,
"S.indices(len) -> (start, stop, stride)\n\
\n\
Assuming a sequence of length len, calculate the start and stop\n\
indices, and the stride length of the extended slice described by\n\
S. Out of bounds indices are clipped in a manner consistent with the\n\
handling of normal slices.");

static PyObject *
slice_reduce(PySliceObject* self)
{
    return Py_BuildValue("O(OOO)", Py_TYPE(self), self->start, self->stop, self->step);
}

PyDoc_STRVAR(reduce_doc, "Return state information for pickling.");

static PyMethodDef slice_methods[] = {
    {"indices",         (PyCFunction)slice_indices,
     METH_O,            slice_indices_doc},
    {"__reduce__",      (PyCFunction)slice_reduce,
     METH_NOARGS,       reduce_doc},
    {NULL, NULL}
};

static PyObject *
slice_richcompare(PyObject *v, PyObject *w, int op)
{
    PyObject *t1;
    PyObject *t2;
    PyObject *res;

    if (!PySlice_Check(v) || !PySlice_Check(w))
        Py_RETURN_NOTIMPLEMENTED;

    if (v == w) {
        /* XXX Do we really need this shortcut?
           There's a unit test for it, but is that fair? */
        switch (op) {
        case Py_EQ:
        case Py_LE:
        case Py_GE:
            res = Py_True;
            break;
        default:
            res = Py_False;
            break;
        }
        Py_INCREF(res);
        return res;
    }

    t1 = PyTuple_New(3);
    if (t1 == NULL)
        return NULL;
    t2 = PyTuple_New(3);
    if (t2 == NULL) {
        Py_DECREF(t1);
        return NULL;
    }

    PyTuple_SET_ITEM(t1, 0, ((PySliceObject *)v)->start);
    PyTuple_SET_ITEM(t1, 1, ((PySliceObject *)v)->stop);
    PyTuple_SET_ITEM(t1, 2, ((PySliceObject *)v)->step);
    PyTuple_SET_ITEM(t2, 0, ((PySliceObject *)w)->start);
    PyTuple_SET_ITEM(t2, 1, ((PySliceObject *)w)->stop);
    PyTuple_SET_ITEM(t2, 2, ((PySliceObject *)w)->step);

    res = PyObject_RichCompare(t1, t2, op);

    PyTuple_SET_ITEM(t1, 0, NULL);
    PyTuple_SET_ITEM(t1, 1, NULL);
    PyTuple_SET_ITEM(t1, 2, NULL);
    PyTuple_SET_ITEM(t2, 0, NULL);
    PyTuple_SET_ITEM(t2, 1, NULL);
    PyTuple_SET_ITEM(t2, 2, NULL);

    Py_DECREF(t1);
    Py_DECREF(t2);

    return res;
}

PyTypeObject PySlice_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "slice",                    /* Name of this type */
    sizeof(PySliceObject),      /* Basic object size */
    0,                          /* Item size for varobject */
    (destructor)slice_dealloc,                  /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    (reprfunc)slice_repr,                       /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    PyObject_HashNotImplemented,                /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    slice_doc,                                  /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    slice_richcompare,                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    slice_methods,                              /* tp_methods */
    slice_members,                              /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    0,                                          /* tp_alloc */
    slice_new,                                  /* tp_new */
};
