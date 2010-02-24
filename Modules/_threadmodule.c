
/* Thread module */
/* Interface to Sjoerd's portable C thread library */

#include "Python.h"
#include "structmember.h" /* offsetof */

#ifndef WITH_THREAD
#error "Error!  The rest of Python is not compiled with thread support."
#error "Rerun configure, adding a --with-threads option."
#error "Then run `make clean' followed by `make'."
#endif

#include "pythread.h"

static PyObject *ThreadError;
static long nb_threads = 0;

/* Lock objects */

typedef struct {
	PyObject_HEAD
	PyThread_type_lock lock_lock;
	PyObject *in_weakreflist;
} lockobject;

static void
lock_dealloc(lockobject *self)
{
	assert(self->lock_lock);
	if (self->in_weakreflist != NULL)
		PyObject_ClearWeakRefs((PyObject *) self);
	/* Unlock the lock so it's safe to free it */
	PyThread_acquire_lock(self->lock_lock, 0);
	PyThread_release_lock(self->lock_lock);
	
	PyThread_free_lock(self->lock_lock);
	PyObject_Del(self);
}

static PyObject *
lock_PyThread_acquire_lock(lockobject *self, PyObject *args)
{
	int i = 1;

	if (!PyArg_ParseTuple(args, "|i:acquire", &i))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	i = PyThread_acquire_lock(self->lock_lock, i);
	Py_END_ALLOW_THREADS

	return PyBool_FromLong((long)i);
}

PyDoc_STRVAR(acquire_doc,
"acquire([wait]) -> None or bool\n\
(acquire_lock() is an obsolete synonym)\n\
\n\
Lock the lock.  Without argument, this blocks if the lock is already\n\
locked (even by the same thread), waiting for another thread to release\n\
the lock, and return None once the lock is acquired.\n\
With an argument, this will only block if the argument is true,\n\
and the return value reflects whether the lock is acquired.\n\
The blocking operation is not interruptible.");

static PyObject *
lock_PyThread_release_lock(lockobject *self)
{
	/* Sanity check: the lock must be locked */
	if (PyThread_acquire_lock(self->lock_lock, 0)) {
		PyThread_release_lock(self->lock_lock);
		PyErr_SetString(ThreadError, "release unlocked lock");
		return NULL;
	}

	PyThread_release_lock(self->lock_lock);
	Py_INCREF(Py_None);
	return Py_None;
}

PyDoc_STRVAR(release_doc,
"release()\n\
(release_lock() is an obsolete synonym)\n\
\n\
Release the lock, allowing another thread that is blocked waiting for\n\
the lock to acquire the lock.  The lock must be in the locked state,\n\
but it needn't be locked by the same thread that unlocks it.");

static PyObject *
lock_locked_lock(lockobject *self)
{
	if (PyThread_acquire_lock(self->lock_lock, 0)) {
		PyThread_release_lock(self->lock_lock);
		return PyBool_FromLong(0L);
	}
	return PyBool_FromLong(1L);
}

PyDoc_STRVAR(locked_doc,
"locked() -> bool\n\
(locked_lock() is an obsolete synonym)\n\
\n\
Return whether the lock is in the locked state.");

static PyMethodDef lock_methods[] = {
	{"acquire_lock", (PyCFunction)lock_PyThread_acquire_lock, 
	 METH_VARARGS, acquire_doc},
	{"acquire",      (PyCFunction)lock_PyThread_acquire_lock, 
	 METH_VARARGS, acquire_doc},
	{"release_lock", (PyCFunction)lock_PyThread_release_lock, 
	 METH_NOARGS, release_doc},
	{"release",      (PyCFunction)lock_PyThread_release_lock, 
	 METH_NOARGS, release_doc},
	{"locked_lock",  (PyCFunction)lock_locked_lock,  
	 METH_NOARGS, locked_doc},
	{"locked",       (PyCFunction)lock_locked_lock,  
	 METH_NOARGS, locked_doc},
	{"__enter__",    (PyCFunction)lock_PyThread_acquire_lock,
	 METH_VARARGS, acquire_doc},
	{"__exit__",    (PyCFunction)lock_PyThread_release_lock,
	 METH_VARARGS, release_doc},
	{NULL,           NULL}		/* sentinel */
};

static PyTypeObject Locktype = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	"_thread.lock",			/*tp_name*/
	sizeof(lockobject),		/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)lock_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	0,				/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_reserved*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
	0,				/*tp_call*/
	0,				/*tp_str*/
	0,				/*tp_getattro*/
	0,				/*tp_setattro*/
	0,				/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,		/*tp_flags*/
	0,				/*tp_doc*/
	0,				/*tp_traverse*/
	0,				/*tp_clear*/
	0,				/*tp_richcompare*/
	offsetof(lockobject, in_weakreflist), /*tp_weaklistoffset*/
	0,				/*tp_iter*/
	0,				/*tp_iternext*/
	lock_methods,			/*tp_methods*/
};

/* Recursive lock objects */

typedef struct {
	PyObject_HEAD
	PyThread_type_lock rlock_lock;
	long rlock_owner;
	unsigned long rlock_count;
	PyObject *in_weakreflist;
} rlockobject;

static void
rlock_dealloc(rlockobject *self)
{
	assert(self->rlock_lock);
	if (self->in_weakreflist != NULL)
		PyObject_ClearWeakRefs((PyObject *) self);
	/* Unlock the lock so it's safe to free it */
	if (self->rlock_count > 0)
		PyThread_release_lock(self->rlock_lock);
	
	PyThread_free_lock(self->rlock_lock);
	Py_TYPE(self)->tp_free(self);
}

static PyObject *
rlock_acquire(rlockobject *self, PyObject *args, PyObject *kwds)
{
	char *kwlist[] = {"blocking", NULL};
	int blocking = 1;
	long tid;
	int r = 1;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i:acquire", kwlist,
				         &blocking))
		return NULL;

	tid = PyThread_get_thread_ident();
	if (self->rlock_count > 0 && tid == self->rlock_owner) {
		unsigned long count = self->rlock_count + 1;
		if (count <= self->rlock_count) {
			PyErr_SetString(PyExc_OverflowError,
					"Internal lock count overflowed");
			return NULL;
		}
		self->rlock_count = count;
		Py_RETURN_TRUE;
	}

	if (self->rlock_count > 0 ||
	    !PyThread_acquire_lock(self->rlock_lock, 0)) {
		if (!blocking) {
			Py_RETURN_FALSE;
		}
		Py_BEGIN_ALLOW_THREADS
		r = PyThread_acquire_lock(self->rlock_lock, blocking);
		Py_END_ALLOW_THREADS
	}
	if (r) {
		assert(self->rlock_count == 0);
		self->rlock_owner = tid;
		self->rlock_count = 1;
	}

	return PyBool_FromLong(r);
}

PyDoc_STRVAR(rlock_acquire_doc,
"acquire(blocking=True) -> bool\n\
\n\
Lock the lock.  `blocking` indicates whether we should wait\n\
for the lock to be available or not.  If `blocking` is False\n\
and another thread holds the lock, the method will return False\n\
immediately.  If `blocking` is True and another thread holds\n\
the lock, the method will wait for the lock to be released,\n\
take it and then return True.\n\
(note: the blocking operation is not interruptible.)\n\
\n\
In all other cases, the method will return True immediately.\n\
Precisely, if the current thread already holds the lock, its\n\
internal counter is simply incremented. If nobody holds the lock,\n\
the lock is taken and its internal counter initialized to 1.");

static PyObject *
rlock_release(rlockobject *self)
{
	long tid = PyThread_get_thread_ident();

	if (self->rlock_count == 0 || self->rlock_owner != tid) {
		PyErr_SetString(PyExc_RuntimeError,
				"cannot release un-acquired lock");
		return NULL;
	}
	if (--self->rlock_count == 0) {
		self->rlock_owner = 0;
		PyThread_release_lock(self->rlock_lock);
	}
	Py_RETURN_NONE;
}

PyDoc_STRVAR(rlock_release_doc,
"release()\n\
\n\
Release the lock, allowing another thread that is blocked waiting for\n\
the lock to acquire the lock.  The lock must be in the locked state,\n\
and must be locked by the same thread that unlocks it; otherwise a\n\
`RuntimeError` is raised.\n\
\n\
Do note that if the lock was acquire()d several times in a row by the\n\
current thread, release() needs to be called as many times for the lock\n\
to be available for other threads.");

static PyObject *
rlock_acquire_restore(rlockobject *self, PyObject *arg)
{
	long owner;
	unsigned long count;
	int r = 1;

	if (!PyArg_ParseTuple(arg, "kl:_acquire_restore", &count, &owner))
		return NULL;

	if (!PyThread_acquire_lock(self->rlock_lock, 0)) {
		Py_BEGIN_ALLOW_THREADS
		r = PyThread_acquire_lock(self->rlock_lock, 1);
		Py_END_ALLOW_THREADS
	}
	if (!r) {
		PyErr_SetString(ThreadError, "couldn't acquire lock");
		return NULL;
	}
	assert(self->rlock_count == 0);
	self->rlock_owner = owner;
	self->rlock_count = count;
	Py_RETURN_NONE;
}

PyDoc_STRVAR(rlock_acquire_restore_doc,
"_acquire_restore(state) -> None\n\
\n\
For internal use by `threading.Condition`.");

static PyObject *
rlock_release_save(rlockobject *self)
{
	long owner;
	unsigned long count;

	owner = self->rlock_owner;
	count = self->rlock_count;
	self->rlock_count = 0;
	self->rlock_owner = 0;
	PyThread_release_lock(self->rlock_lock);
	return Py_BuildValue("kl", count, owner);
}

PyDoc_STRVAR(rlock_release_save_doc,
"_release_save() -> tuple\n\
\n\
For internal use by `threading.Condition`.");


static PyObject *
rlock_is_owned(rlockobject *self)
{
	long tid = PyThread_get_thread_ident();
	
	if (self->rlock_count > 0 && self->rlock_owner == tid) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyDoc_STRVAR(rlock_is_owned_doc,
"_is_owned() -> bool\n\
\n\
For internal use by `threading.Condition`.");

static PyObject *
rlock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	rlockobject *self;

	self = (rlockobject *) type->tp_alloc(type, 0);
	if (self != NULL) {
		self->rlock_lock = PyThread_allocate_lock();
		if (self->rlock_lock == NULL) {
			type->tp_free(self);
			PyErr_SetString(ThreadError, "can't allocate lock");
			return NULL;
		}
		self->in_weakreflist = NULL;
		self->rlock_owner = 0;
		self->rlock_count = 0;
	}

	return (PyObject *) self;
}

static PyObject *
rlock_repr(rlockobject *self)
{
	return PyUnicode_FromFormat("<%s owner=%ld count=%lu>",
		Py_TYPE(self)->tp_name, self->rlock_owner, self->rlock_count);
}


static PyMethodDef rlock_methods[] = {
	{"acquire",      (PyCFunction)rlock_acquire, 
	 METH_VARARGS | METH_KEYWORDS, rlock_acquire_doc},
	{"release",      (PyCFunction)rlock_release, 
	 METH_NOARGS, rlock_release_doc},
	{"_is_owned",     (PyCFunction)rlock_is_owned,  
	 METH_NOARGS, rlock_is_owned_doc},
	{"_acquire_restore", (PyCFunction)rlock_acquire_restore,
	 METH_O, rlock_acquire_restore_doc},
	{"_release_save", (PyCFunction)rlock_release_save,
	 METH_NOARGS, rlock_release_save_doc},
	{"__enter__",    (PyCFunction)rlock_acquire,
	 METH_VARARGS | METH_KEYWORDS, rlock_acquire_doc},
	{"__exit__",    (PyCFunction)rlock_release,
	 METH_VARARGS, rlock_release_doc},
	{NULL,           NULL}		/* sentinel */
};


static PyTypeObject RLocktype = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	"_thread.RLock",		/*tp_name*/
	sizeof(rlockobject),		/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)rlock_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	0,				/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_reserved*/
	(reprfunc)rlock_repr,		/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
	0,				/*tp_call*/
	0,				/*tp_str*/
	0,				/*tp_getattro*/
	0,				/*tp_setattro*/
	0,				/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,				/*tp_doc*/
	0,				/*tp_traverse*/
	0,				/*tp_clear*/
	0,				/*tp_richcompare*/
	offsetof(rlockobject, in_weakreflist), /*tp_weaklistoffset*/
	0,				/*tp_iter*/
	0,				/*tp_iternext*/
	rlock_methods,			/*tp_methods*/
	0,				/* tp_members */
	0,				/* tp_getset */
	0,				/* tp_base */
	0,				/* tp_dict */
	0,				/* tp_descr_get */
	0,				/* tp_descr_set */
	0,				/* tp_dictoffset */
	0,				/* tp_init */
	PyType_GenericAlloc,		/* tp_alloc */
	rlock_new			/* tp_new */
};

static lockobject *
newlockobject(void)
{
	lockobject *self;
	self = PyObject_New(lockobject, &Locktype);
	if (self == NULL)
		return NULL;
	self->lock_lock = PyThread_allocate_lock();
	self->in_weakreflist = NULL;
	if (self->lock_lock == NULL) {
		PyObject_Del(self);
		self = NULL;
		PyErr_SetString(ThreadError, "can't allocate lock");
	}
	return self;
}

/* Thread-local objects */

#include "structmember.h"

typedef struct {
	PyObject_HEAD
	PyObject *key;
	PyObject *args;
	PyObject *kw;
	PyObject *dict;
} localobject;

static PyObject *
local_new(PyTypeObject *type, PyObject *args, PyObject *kw)
{
	localobject *self;
	PyObject *tdict;

	if (type->tp_init == PyBaseObject_Type.tp_init
	    && ((args && PyObject_IsTrue(args))
		|| (kw && PyObject_IsTrue(kw)))) {
		PyErr_SetString(PyExc_TypeError,
			  "Initialization arguments are not supported");
		return NULL;
	}

	self = (localobject *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	Py_XINCREF(args);
	self->args = args;
	Py_XINCREF(kw);
	self->kw = kw;
	self->dict = NULL;	/* making sure */
	self->key = PyUnicode_FromFormat("thread.local.%p", self);
	if (self->key == NULL) 
		goto err;

	self->dict = PyDict_New();
	if (self->dict == NULL)
		goto err;

	tdict = PyThreadState_GetDict();
	if (tdict == NULL) {
		PyErr_SetString(PyExc_SystemError,
				"Couldn't get thread-state dictionary");
		goto err;
	}

	if (PyDict_SetItem(tdict, self->key, self->dict) < 0)
		goto err;

	return (PyObject *)self;

  err:
	Py_DECREF(self);
	return NULL;
}

static int
local_traverse(localobject *self, visitproc visit, void *arg)
{
	Py_VISIT(self->args);
	Py_VISIT(self->kw);
	Py_VISIT(self->dict);
	return 0;
}

static int
local_clear(localobject *self)
{
	Py_CLEAR(self->args);
	Py_CLEAR(self->kw);
	Py_CLEAR(self->dict);
	return 0;
}

static void
local_dealloc(localobject *self)
{
	PyThreadState *tstate;
	if (self->key
	    && (tstate = PyThreadState_Get())
	    && tstate->interp) {
		for(tstate = PyInterpreterState_ThreadHead(tstate->interp);
		    tstate;
		    tstate = PyThreadState_Next(tstate)) 
			if (tstate->dict &&
			    PyDict_GetItem(tstate->dict, self->key))
				PyDict_DelItem(tstate->dict, self->key);
	}

	Py_XDECREF(self->key);
	local_clear(self);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
_ldict(localobject *self)
{
	PyObject *tdict, *ldict;

	tdict = PyThreadState_GetDict();
	if (tdict == NULL) {
		PyErr_SetString(PyExc_SystemError,
				"Couldn't get thread-state dictionary");
		return NULL;
	}

	ldict = PyDict_GetItem(tdict, self->key);
	if (ldict == NULL) {
		ldict = PyDict_New(); /* we own ldict */

		if (ldict == NULL)
			return NULL;
		else {
			int i = PyDict_SetItem(tdict, self->key, ldict);
			Py_DECREF(ldict); /* now ldict is borrowed */
			if (i < 0) 
				return NULL;
		}

		Py_CLEAR(self->dict);
		Py_INCREF(ldict);
		self->dict = ldict; /* still borrowed */

		if (Py_TYPE(self)->tp_init != PyBaseObject_Type.tp_init &&
		    Py_TYPE(self)->tp_init((PyObject*)self, 
					   self->args, self->kw) < 0) {
			/* we need to get rid of ldict from thread so
			   we create a new one the next time we do an attr
			   acces */
			PyDict_DelItem(tdict, self->key);
			return NULL;
		}
		
	}

	/* The call to tp_init above may have caused another thread to run.
	   Install our ldict again. */
	if (self->dict != ldict) {
		Py_CLEAR(self->dict);
		Py_INCREF(ldict);
		self->dict = ldict;
	}

	return ldict;
}

static int
local_setattro(localobject *self, PyObject *name, PyObject *v)
{
	PyObject *ldict;
	
	ldict = _ldict(self);
	if (ldict == NULL) 
		return -1;

	return PyObject_GenericSetAttr((PyObject *)self, name, v);
}

static PyObject *
local_getdict(localobject *self, void *closure)
{
	if (self->dict == NULL) {
		PyErr_SetString(PyExc_AttributeError, "__dict__");
		return NULL;
	}

	Py_INCREF(self->dict);
	return self->dict;
}

static PyGetSetDef local_getset[] = {
	{"__dict__", (getter)local_getdict, (setter)NULL,
	 "Local-data dictionary", NULL},
	{NULL}  /* Sentinel */
};

static PyObject *local_getattro(localobject *, PyObject *);

static PyTypeObject localtype = {
	PyVarObject_HEAD_INIT(NULL, 0)
	/* tp_name           */ "_thread._local",
	/* tp_basicsize      */ sizeof(localobject),
	/* tp_itemsize       */ 0,
	/* tp_dealloc        */ (destructor)local_dealloc,
	/* tp_print          */ 0,
	/* tp_getattr        */ 0,
	/* tp_setattr        */ 0,
	/* tp_reserved       */ 0,
	/* tp_repr           */ 0,
	/* tp_as_number      */ 0,
	/* tp_as_sequence    */ 0,
	/* tp_as_mapping     */ 0,
	/* tp_hash           */ 0,
	/* tp_call           */ 0,
	/* tp_str            */ 0,
	/* tp_getattro       */ (getattrofunc)local_getattro,
	/* tp_setattro       */ (setattrofunc)local_setattro,
	/* tp_as_buffer      */ 0,
	/* tp_flags          */ Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	/* tp_doc            */ "Thread-local data",
	/* tp_traverse       */ (traverseproc)local_traverse,
	/* tp_clear          */ (inquiry)local_clear,
	/* tp_richcompare    */ 0,
	/* tp_weaklistoffset */ 0,
	/* tp_iter           */ 0,
	/* tp_iternext       */ 0,
	/* tp_methods        */ 0,
	/* tp_members        */ 0,
	/* tp_getset         */ local_getset,
	/* tp_base           */ 0,
	/* tp_dict           */ 0, /* internal use */
	/* tp_descr_get      */ 0,
	/* tp_descr_set      */ 0,
	/* tp_dictoffset     */ offsetof(localobject, dict),
	/* tp_init           */ 0,
	/* tp_alloc          */ 0,
	/* tp_new            */ local_new,
	/* tp_free           */ 0, /* Low-level free-mem routine */
	/* tp_is_gc          */ 0, /* For PyObject_IS_GC */
};

static PyObject *
local_getattro(localobject *self, PyObject *name)
{
	PyObject *ldict, *value;

	ldict = _ldict(self);
	if (ldict == NULL) 
		return NULL;

	if (Py_TYPE(self) != &localtype)
		/* use generic lookup for subtypes */
		return PyObject_GenericGetAttr((PyObject *)self, name);

	/* Optimization: just look in dict ourselves */
	value = PyDict_GetItem(ldict, name);
	if (value == NULL) 
		/* Fall back on generic to get __class__ and __dict__ */
		return PyObject_GenericGetAttr((PyObject *)self, name);

	Py_INCREF(value);
	return value;
}

/* Module functions */

struct bootstate {
	PyInterpreterState *interp;
	PyObject *func;
	PyObject *args;
	PyObject *keyw;
};

static void
t_bootstrap(void *boot_raw)
{
	struct bootstate *boot = (struct bootstate *) boot_raw;
	PyThreadState *tstate;
	PyObject *res;

	tstate = PyThreadState_New(boot->interp);

	PyEval_AcquireThread(tstate);
	nb_threads++;
	res = PyEval_CallObjectWithKeywords(
		boot->func, boot->args, boot->keyw);
	if (res == NULL) {
		if (PyErr_ExceptionMatches(PyExc_SystemExit))
			PyErr_Clear();
		else {
			PyObject *file;
			PySys_WriteStderr(
				"Unhandled exception in thread started by ");
			file = PySys_GetObject("stderr");
			if (file != NULL && file != Py_None)
				PyFile_WriteObject(boot->func, file, 0);
			else
				PyObject_Print(boot->func, stderr, 0);
			PySys_WriteStderr("\n");
			PyErr_PrintEx(0);
		}
	}
	else
		Py_DECREF(res);
	Py_DECREF(boot->func);
	Py_DECREF(boot->args);
	Py_XDECREF(boot->keyw);
	PyMem_DEL(boot_raw);
	nb_threads--;
	PyThreadState_Clear(tstate);
	PyThreadState_DeleteCurrent();
	PyThread_exit_thread();
}

static PyObject *
thread_PyThread_start_new_thread(PyObject *self, PyObject *fargs)
{
	PyObject *func, *args, *keyw = NULL;
	struct bootstate *boot;
	long ident;

	if (!PyArg_UnpackTuple(fargs, "start_new_thread", 2, 3,
		               &func, &args, &keyw))
		return NULL;
	if (!PyCallable_Check(func)) {
		PyErr_SetString(PyExc_TypeError,
				"first arg must be callable");
		return NULL;
	}
	if (!PyTuple_Check(args)) {
		PyErr_SetString(PyExc_TypeError,
				"2nd arg must be a tuple");
		return NULL;
	}
	if (keyw != NULL && !PyDict_Check(keyw)) {
		PyErr_SetString(PyExc_TypeError,
				"optional 3rd arg must be a dictionary");
		return NULL;
	}
	boot = PyMem_NEW(struct bootstate, 1);
	if (boot == NULL)
		return PyErr_NoMemory();
	boot->interp = PyThreadState_GET()->interp;
	boot->func = func;
	boot->args = args;
	boot->keyw = keyw;
	Py_INCREF(func);
	Py_INCREF(args);
	Py_XINCREF(keyw);
	PyEval_InitThreads(); /* Start the interpreter's thread-awareness */
	ident = PyThread_start_new_thread(t_bootstrap, (void*) boot);
	if (ident == -1) {
		PyErr_SetString(ThreadError, "can't start new thread");
		Py_DECREF(func);
		Py_DECREF(args);
		Py_XDECREF(keyw);
		PyMem_DEL(boot);
		return NULL;
	}
	return PyLong_FromLong(ident);
}

PyDoc_STRVAR(start_new_doc,
"start_new_thread(function, args[, kwargs])\n\
(start_new() is an obsolete synonym)\n\
\n\
Start a new thread and return its identifier.  The thread will call the\n\
function with positional arguments from the tuple args and keyword arguments\n\
taken from the optional dictionary kwargs.  The thread exits when the\n\
function returns; the return value is ignored.  The thread will also exit\n\
when the function raises an unhandled exception; a stack trace will be\n\
printed unless the exception is SystemExit.\n");

static PyObject *
thread_PyThread_exit_thread(PyObject *self)
{
	PyErr_SetNone(PyExc_SystemExit);
	return NULL;
}

PyDoc_STRVAR(exit_doc,
"exit()\n\
(PyThread_exit_thread() is an obsolete synonym)\n\
\n\
This is synonymous to ``raise SystemExit''.  It will cause the current\n\
thread to exit silently unless the exception is caught.");

static PyObject *
thread_PyThread_interrupt_main(PyObject * self)
{
	PyErr_SetInterrupt();
	Py_INCREF(Py_None);
	return Py_None;
}

PyDoc_STRVAR(interrupt_doc,
"interrupt_main()\n\
\n\
Raise a KeyboardInterrupt in the main thread.\n\
A subthread can use this function to interrupt the main thread."
);

static lockobject *newlockobject(void);

static PyObject *
thread_PyThread_allocate_lock(PyObject *self)
{
	return (PyObject *) newlockobject();
}

PyDoc_STRVAR(allocate_doc,
"allocate_lock() -> lock object\n\
(allocate() is an obsolete synonym)\n\
\n\
Create a new lock object.  See LockType.__doc__ for information about locks.");

static PyObject *
thread_get_ident(PyObject *self)
{
	long ident;
	ident = PyThread_get_thread_ident();
	if (ident == -1) {
		PyErr_SetString(ThreadError, "no current thread ident");
		return NULL;
	}
	return PyLong_FromLong(ident);
}

PyDoc_STRVAR(get_ident_doc,
"get_ident() -> integer\n\
\n\
Return a non-zero integer that uniquely identifies the current thread\n\
amongst other threads that exist simultaneously.\n\
This may be used to identify per-thread resources.\n\
Even though on some platforms threads identities may appear to be\n\
allocated consecutive numbers starting at 1, this behavior should not\n\
be relied upon, and the number should be seen purely as a magic cookie.\n\
A thread's identity may be reused for another thread after it exits.");

static PyObject *
thread__count(PyObject *self)
{
	return PyLong_FromLong(nb_threads);
}

PyDoc_STRVAR(_count_doc,
"_count() -> integer\n\
\n\
\
Return the number of currently running Python threads, excluding \n\
the main thread. The returned number comprises all threads created\n\
through `start_new_thread()` as well as `threading.Thread`, and not\n\
yet finished.\n\
\n\
This function is meant for internal and specialized purposes only.\n\
In most applications `threading.enumerate()` should be used instead.");

static PyObject *
thread_stack_size(PyObject *self, PyObject *args)
{
	size_t old_size;
	Py_ssize_t new_size = 0;
	int rc;

	if (!PyArg_ParseTuple(args, "|n:stack_size", &new_size))
		return NULL;

	if (new_size < 0) {
		PyErr_SetString(PyExc_ValueError,
				"size must be 0 or a positive value");
		return NULL;
	}

	old_size = PyThread_get_stacksize();

	rc = PyThread_set_stacksize((size_t) new_size);
	if (rc == -1) {
		PyErr_Format(PyExc_ValueError,
			     "size not valid: %zd bytes",
			     new_size);
		return NULL;
	}
	if (rc == -2) {
		PyErr_SetString(ThreadError,
				"setting stack size not supported");
		return NULL;
	}

	return PyLong_FromSsize_t((Py_ssize_t) old_size);
}

PyDoc_STRVAR(stack_size_doc,
"stack_size([size]) -> size\n\
\n\
Return the thread stack size used when creating new threads.  The\n\
optional size argument specifies the stack size (in bytes) to be used\n\
for subsequently created threads, and must be 0 (use platform or\n\
configured default) or a positive integer value of at least 32,768 (32k).\n\
If changing the thread stack size is unsupported, a ThreadError\n\
exception is raised.  If the specified size is invalid, a ValueError\n\
exception is raised, and the stack size is unmodified.  32k bytes\n\
 currently the minimum supported stack size value to guarantee\n\
sufficient stack space for the interpreter itself.\n\
\n\
Note that some platforms may have particular restrictions on values for\n\
the stack size, such as requiring a minimum stack size larger than 32kB or\n\
requiring allocation in multiples of the system memory page size\n\
- platform documentation should be referred to for more information\n\
(4kB pages are common; using multiples of 4096 for the stack size is\n\
the suggested approach in the absence of more specific information).");

static PyMethodDef thread_methods[] = {
	{"start_new_thread",	(PyCFunction)thread_PyThread_start_new_thread,
	                        METH_VARARGS,
				start_new_doc},
	{"start_new",		(PyCFunction)thread_PyThread_start_new_thread, 
	                        METH_VARARGS,
				start_new_doc},
	{"allocate_lock",	(PyCFunction)thread_PyThread_allocate_lock, 
	 METH_NOARGS, allocate_doc},
	{"allocate",		(PyCFunction)thread_PyThread_allocate_lock, 
	 METH_NOARGS, allocate_doc},
	{"exit_thread",		(PyCFunction)thread_PyThread_exit_thread, 
	 METH_NOARGS, exit_doc},
	{"exit",		(PyCFunction)thread_PyThread_exit_thread, 
	 METH_NOARGS, exit_doc},
	{"interrupt_main",	(PyCFunction)thread_PyThread_interrupt_main,
	 METH_NOARGS, interrupt_doc},
	{"get_ident",		(PyCFunction)thread_get_ident, 
	 METH_NOARGS, get_ident_doc},
	{"_count",		(PyCFunction)thread__count, 
	 METH_NOARGS, _count_doc},
	{"stack_size",		(PyCFunction)thread_stack_size,
				METH_VARARGS,
				stack_size_doc},
	{NULL,			NULL}		/* sentinel */
};


/* Initialization function */

PyDoc_STRVAR(thread_doc,
"This module provides primitive operations to write multi-threaded programs.\n\
The 'threading' module provides a more convenient interface.");

PyDoc_STRVAR(lock_doc,
"A lock object is a synchronization primitive.  To create a lock,\n\
call the PyThread_allocate_lock() function.  Methods are:\n\
\n\
acquire() -- lock the lock, possibly blocking until it can be obtained\n\
release() -- unlock of the lock\n\
locked() -- test whether the lock is currently locked\n\
\n\
A lock is not owned by the thread that locked it; another thread may\n\
unlock it.  A thread attempting to lock a lock that it has already locked\n\
will block until another thread unlocks it.  Deadlocks may ensue.");

static struct PyModuleDef threadmodule = {
	PyModuleDef_HEAD_INIT,
	"_thread",
	thread_doc,
	-1,
	thread_methods,
	NULL,
	NULL,
	NULL,
	NULL
};


PyMODINIT_FUNC
PyInit__thread(void)
{
	PyObject *m, *d;
	
	/* Initialize types: */
	if (PyType_Ready(&localtype) < 0)
		return NULL;
	if (PyType_Ready(&Locktype) < 0)
		return NULL;
	if (PyType_Ready(&RLocktype) < 0)
		return NULL;

	/* Create the module and add the functions */
	m = PyModule_Create(&threadmodule);
	if (m == NULL)
		return NULL;

	/* Add a symbolic constant */
	d = PyModule_GetDict(m);
	ThreadError = PyErr_NewException("_thread.error", NULL, NULL);
	PyDict_SetItemString(d, "error", ThreadError);
	Locktype.tp_doc = lock_doc;
	Py_INCREF(&Locktype);
	PyDict_SetItemString(d, "LockType", (PyObject *)&Locktype);

	Py_INCREF(&RLocktype);
	if (PyModule_AddObject(m, "RLock", (PyObject *)&RLocktype) < 0)
		return NULL;

	Py_INCREF(&localtype);
	if (PyModule_AddObject(m, "_local", (PyObject *)&localtype) < 0)
		return NULL;

	nb_threads = 0;

	/* Initialize the C thread library */
	PyThread_init_thread();
	return m;
}
