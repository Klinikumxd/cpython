/* File object implementation */

#include "Python.h"
#include "structmember.h"

#ifndef DONT_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* DONT_HAVE_SYS_TYPES_H */

#ifdef MS_WINDOWS
#define fileno _fileno
/* can simulate truncate with Win32 API functions; see file_truncate */
#define HAVE_FTRUNCATE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _MSC_VER
/* Need GetVersion to see if on NT so safe to use _wfopen */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif /* _MSC_VER */

#ifdef macintosh
#ifdef USE_GUSI
#define HAVE_FTRUNCATE
#endif
#endif

#ifdef __MWERKS__
/* Mwerks fopen() doesn't always set errno */
#define NO_FOPEN_ERRNO
#endif

#if defined(PYOS_OS2) && defined(PYCC_GCC)
#include <io.h>
#endif

#define BUF(v) PyString_AS_STRING((PyStringObject *)v)

#ifndef DONT_HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_GETC_UNLOCKED
#define GETC(f) getc_unlocked(f)
#define FLOCKFILE(f) flockfile(f)
#define FUNLOCKFILE(f) funlockfile(f)
#else
#define GETC(f) getc(f)
#define FLOCKFILE(f)
#define FUNLOCKFILE(f)
#endif

#ifdef WITH_UNIVERSAL_NEWLINES
/* Bits in f_newlinetypes */
#define NEWLINE_UNKNOWN	0	/* No newline seen, yet */
#define NEWLINE_CR 1		/* \r newline seen */
#define NEWLINE_LF 2		/* \n newline seen */
#define NEWLINE_CRLF 4		/* \r\n newline seen */
#endif

FILE *
PyFile_AsFile(PyObject *f)
{
	if (f == NULL || !PyFile_Check(f))
		return NULL;
	else
		return ((PyFileObject *)f)->f_fp;
}

PyObject *
PyFile_Name(PyObject *f)
{
	if (f == NULL || !PyFile_Check(f))
		return NULL;
	else
		return ((PyFileObject *)f)->f_name;
}

/* On Unix, fopen will succeed for directories.
   In Python, there should be no file objects referring to
   directories, so we need a check.  */

static PyFileObject*
dircheck(PyFileObject* f)
{
#if defined(HAVE_FSTAT) && defined(S_IFDIR) && defined(EISDIR)
	struct stat buf;
	if (f->f_fp == NULL)
		return f;
	if (fstat(fileno(f->f_fp), &buf) == 0 &&
	    S_ISDIR(buf.st_mode)) {
#ifdef HAVE_STRERROR
		char *msg = strerror(EISDIR);
#else
		char *msg = "Is a directory";
#endif
		PyObject *exc = PyObject_CallFunction(PyExc_IOError, "(is)", 
						      EISDIR, msg);
		PyErr_SetObject(PyExc_IOError, exc);
		return NULL;
	}
#endif
	return f;
}


static PyObject *
fill_file_fields(PyFileObject *f, FILE *fp, char *name, char *mode,
		 int (*close)(FILE *), PyObject *wname)
{
	assert(f != NULL);
	assert(PyFile_Check(f));
	assert(f->f_fp == NULL);

	Py_DECREF(f->f_name);
	Py_DECREF(f->f_mode);
#ifdef Py_USING_UNICODE
	if (wname)
		f->f_name = PyUnicode_FromObject(wname);
	else
#endif
		f->f_name = PyString_FromString(name);
	f->f_mode = PyString_FromString(mode);

	f->f_close = close;
	f->f_softspace = 0;
	f->f_binary = strchr(mode,'b') != NULL;
	f->f_buf = NULL;
#ifdef WITH_UNIVERSAL_NEWLINES
	f->f_univ_newline = (strchr(mode, 'U') != NULL);
	f->f_newlinetypes = NEWLINE_UNKNOWN;
	f->f_skipnextlf = 0;
#endif

	if (f->f_name == NULL || f->f_mode == NULL)
		return NULL;
	f->f_fp = fp;
        f = dircheck(f);
	return (PyObject *) f;
}

static PyObject *
open_the_file(PyFileObject *f, char *name, char *mode)
{
	assert(f != NULL);
	assert(PyFile_Check(f));
#ifdef MS_WINDOWS
	/* windows ignores the passed name in order to support Unicode */
	assert(f->f_name != NULL);
#else
	assert(name != NULL);
#endif
	assert(mode != NULL);
	assert(f->f_fp == NULL);

	/* rexec.py can't stop a user from getting the file() constructor --
	   all they have to do is get *any* file object f, and then do
	   type(f).  Here we prevent them from doing damage with it. */
	if (PyEval_GetRestricted()) {
		PyErr_SetString(PyExc_IOError,
		"file() constructor not accessible in restricted mode");
		return NULL;
	}
	errno = 0;
#ifdef HAVE_FOPENRF
	if (*mode == '*') {
		FILE *fopenRF();
		f->f_fp = fopenRF(name, mode+1);
	}
	else
#endif
	{
#ifdef WITH_UNIVERSAL_NEWLINES
		if (strcmp(mode, "U") == 0 || strcmp(mode, "rU") == 0)
			mode = "rb";
#else
		/* Compatibility: specifying U in a Python without universal
		** newlines is allowed, and the file is opened as a normal text
		** file.
		*/
		if (strcmp(mode, "U") == 0 || strcmp(mode, "rU") == 0)
			mode = "r";
#endif
#ifdef MS_WINDOWS
		if (PyUnicode_Check(f->f_name)) {
			PyObject *wmode; 
			wmode = PyUnicode_DecodeASCII(mode, strlen(mode), NULL); 
			if (f->f_name && wmode) {
				Py_BEGIN_ALLOW_THREADS
				/* PyUnicode_AS_UNICODE OK without thread
				   lock as it is a simple dereference. */
				f->f_fp = _wfopen(PyUnicode_AS_UNICODE(f->f_name),
						  PyUnicode_AS_UNICODE(wmode));
				Py_END_ALLOW_THREADS
			}
			Py_XDECREF(wmode);
		}
#endif
		if (NULL == f->f_fp && NULL != name) {
			Py_BEGIN_ALLOW_THREADS
			f->f_fp = fopen(name, mode);
			Py_END_ALLOW_THREADS
		}
	}
	if (f->f_fp == NULL) {
#ifdef NO_FOPEN_ERRNO
		/* Metroworks only, wich does not always sets errno */
		if (errno == 0) {
			PyObject *v;
			v = Py_BuildValue("(is)", 0, "Cannot open file");
			if (v != NULL) {
				PyErr_SetObject(PyExc_IOError, v);
				Py_DECREF(v);
			}
			return NULL;
		}
#endif
#ifdef _MSC_VER
		/* MSVC 6 (Microsoft) leaves errno at 0 for bad mode strings,
		 * across all Windows flavors.  When it sets EINVAL varies
		 * across Windows flavors, the exact conditions aren't
		 * documented, and the answer lies in the OS's implementation
		 * of Win32's CreateFile function (whose source is secret).
		 * Seems the best we can do is map EINVAL to ENOENT.
		 */
		if (errno == 0)	/* bad mode string */
			errno = EINVAL;
		else if (errno == EINVAL) /* unknown, but not a mode string */
			errno = ENOENT;
#endif
		if (errno == EINVAL)
			PyErr_Format(PyExc_IOError, "invalid mode: %s",
				     mode);
		else
#ifdef MS_WINDOWS
			PyErr_SetFromErrnoWithFilenameObject(PyExc_IOError, f->f_name);
#else
			PyErr_SetFromErrnoWithFilename(PyExc_IOError, name);
#endif /* MS_WINDOWS */
		f = NULL;
	}
	if (f != NULL)
		f = dircheck(f);
	return (PyObject *)f;
}

PyObject *
PyFile_FromFile(FILE *fp, char *name, char *mode, int (*close)(FILE *))
{
	PyFileObject *f = (PyFileObject *)PyFile_Type.tp_new(&PyFile_Type,
							     NULL, NULL);
	if (f != NULL) {
		if (fill_file_fields(f, fp, name, mode, close, NULL) == NULL) {
			Py_DECREF(f);
			f = NULL;
		}
	}
	return (PyObject *) f;
}

PyObject *
PyFile_FromString(char *name, char *mode)
{
	extern int fclose(FILE *);
	PyFileObject *f;

	f = (PyFileObject *)PyFile_FromFile((FILE *)NULL, name, mode, fclose);
	if (f != NULL) {
		if (open_the_file(f, name, mode) == NULL) {
			Py_DECREF(f);
			f = NULL;
		}
	}
	return (PyObject *)f;
}

void
PyFile_SetBufSize(PyObject *f, int bufsize)
{
	if (bufsize >= 0) {
#ifdef HAVE_SETVBUF
		int type;
		switch (bufsize) {
		case 0:
			type = _IONBF;
			break;
		case 1:
			type = _IOLBF;
			bufsize = BUFSIZ;
			break;
		default:
			type = _IOFBF;
		}
		setvbuf(((PyFileObject *)f)->f_fp, (char *)NULL,
			type, bufsize);
#else /* !HAVE_SETVBUF */
		if (bufsize <= 1)
			setbuf(((PyFileObject *)f)->f_fp, (char *)NULL);
#endif /* !HAVE_SETVBUF */
	}
}

static PyObject *
err_closed(void)
{
	PyErr_SetString(PyExc_ValueError, "I/O operation on closed file");
	return NULL;
}

static void drop_readahead(PyFileObject *);

/* Methods */

static void
file_dealloc(PyFileObject *f)
{
	if (f->f_fp != NULL && f->f_close != NULL) {
		Py_BEGIN_ALLOW_THREADS
		(*f->f_close)(f->f_fp);
		Py_END_ALLOW_THREADS
	}
	Py_XDECREF(f->f_name);
	Py_XDECREF(f->f_mode);
	drop_readahead(f);
	f->ob_type->tp_free((PyObject *)f);
}

static PyObject *
file_repr(PyFileObject *f)
{
	if (PyUnicode_Check(f->f_name)) {
#ifdef Py_USING_UNICODE
		PyObject *ret = NULL;
		PyObject *name;
		name = PyUnicode_AsUnicodeEscapeString(f->f_name);
		ret = PyString_FromFormat("<%s file u'%s', mode '%s' at %p>",
				   f->f_fp == NULL ? "closed" : "open",
				   PyString_AsString(name),
				   PyString_AsString(f->f_mode),
				   f);
		Py_XDECREF(name);
		return ret;
#endif
	} else {
		return PyString_FromFormat("<%s file '%s', mode '%s' at %p>",
				   f->f_fp == NULL ? "closed" : "open",
				   PyString_AsString(f->f_name),
				   PyString_AsString(f->f_mode),
				   f);
	}
}

static PyObject *
file_close(PyFileObject *f)
{
	int sts = 0;
	if (f->f_fp != NULL) {
		if (f->f_close != NULL) {
			Py_BEGIN_ALLOW_THREADS
			errno = 0;
			sts = (*f->f_close)(f->f_fp);
			Py_END_ALLOW_THREADS
		}
		f->f_fp = NULL;
	}
	if (sts == EOF)
		return PyErr_SetFromErrno(PyExc_IOError);
	if (sts != 0)
		return PyInt_FromLong((long)sts);
	Py_INCREF(Py_None);
	return Py_None;
}


/* Our very own off_t-like type, 64-bit if possible */
#if !defined(HAVE_LARGEFILE_SUPPORT)
typedef off_t Py_off_t;
#elif SIZEOF_OFF_T >= 8
typedef off_t Py_off_t;
#elif SIZEOF_FPOS_T >= 8
typedef fpos_t Py_off_t;
#else
#error "Large file support, but neither off_t nor fpos_t is large enough."
#endif


/* a portable fseek() function
   return 0 on success, non-zero on failure (with errno set) */
static int
_portable_fseek(FILE *fp, Py_off_t offset, int whence)
{
#if !defined(HAVE_LARGEFILE_SUPPORT)
	return fseek(fp, offset, whence);
#elif defined(HAVE_FSEEKO) && SIZEOF_OFF_T >= 8
	return fseeko(fp, offset, whence);
#elif defined(HAVE_FSEEK64)
	return fseek64(fp, offset, whence);
#elif defined(__BEOS__)
	return _fseek(fp, offset, whence);
#elif SIZEOF_FPOS_T >= 8
	/* lacking a 64-bit capable fseek(), use a 64-bit capable fsetpos()
	   and fgetpos() to implement fseek()*/
	fpos_t pos;
	switch (whence) {
	case SEEK_END:
#ifdef MS_WINDOWS
		fflush(fp);
		if (_lseeki64(fileno(fp), 0, 2) == -1)
			return -1;
#else
		if (fseek(fp, 0, SEEK_END) != 0)
			return -1;
#endif
		/* fall through */
	case SEEK_CUR:
		if (fgetpos(fp, &pos) != 0)
			return -1;
		offset += pos;
		break;
	/* case SEEK_SET: break; */
	}
	return fsetpos(fp, &offset);
#else
#error "Large file support, but no way to fseek."
#endif
}


/* a portable ftell() function
   Return -1 on failure with errno set appropriately, current file
   position on success */
static Py_off_t
_portable_ftell(FILE* fp)
{
#if !defined(HAVE_LARGEFILE_SUPPORT)
	return ftell(fp);
#elif defined(HAVE_FTELLO) && SIZEOF_OFF_T >= 8
	return ftello(fp);
#elif defined(HAVE_FTELL64)
	return ftell64(fp);
#elif SIZEOF_FPOS_T >= 8
	fpos_t pos;
	if (fgetpos(fp, &pos) != 0)
		return -1;
	return pos;
#else
#error "Large file support, but no way to ftell."
#endif
}


static PyObject *
file_seek(PyFileObject *f, PyObject *args)
{
	int whence;
	int ret;
	Py_off_t offset;
	PyObject *offobj;

	if (f->f_fp == NULL)
		return err_closed();
	drop_readahead(f);
	whence = 0;
	if (!PyArg_ParseTuple(args, "O|i:seek", &offobj, &whence))
		return NULL;
#if !defined(HAVE_LARGEFILE_SUPPORT)
	offset = PyInt_AsLong(offobj);
#else
	offset = PyLong_Check(offobj) ?
		PyLong_AsLongLong(offobj) : PyInt_AsLong(offobj);
#endif
	if (PyErr_Occurred())
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	ret = _portable_fseek(f->f_fp, offset, whence);
	Py_END_ALLOW_THREADS

	if (ret != 0) {
		PyErr_SetFromErrno(PyExc_IOError);
		clearerr(f->f_fp);
		return NULL;
	}
#ifdef WITH_UNIVERSAL_NEWLINES
	f->f_skipnextlf = 0;
#endif
	Py_INCREF(Py_None);
	return Py_None;
}


#ifdef HAVE_FTRUNCATE
static PyObject *
file_truncate(PyFileObject *f, PyObject *args)
{
	int ret;
	Py_off_t newsize;
	PyObject *newsizeobj;

	if (f->f_fp == NULL)
		return err_closed();
	newsizeobj = NULL;
	if (!PyArg_UnpackTuple(args, "truncate", 0, 1, &newsizeobj))
		return NULL;

	/* Set newsize to current postion if newsizeobj NULL, else to the
	   specified value. */
	if (newsizeobj != NULL) {
#if !defined(HAVE_LARGEFILE_SUPPORT)
		newsize = PyInt_AsLong(newsizeobj);
#else
		newsize = PyLong_Check(newsizeobj) ?
				PyLong_AsLongLong(newsizeobj) :
				PyInt_AsLong(newsizeobj);
#endif
		if (PyErr_Occurred())
			return NULL;
	}
	else {
		/* Default to current position. */
		Py_BEGIN_ALLOW_THREADS
		errno = 0;
		newsize = _portable_ftell(f->f_fp);
		Py_END_ALLOW_THREADS
		if (newsize == -1)
			goto onioerror;
	}

	/* Flush the file. */
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	ret = fflush(f->f_fp);
	Py_END_ALLOW_THREADS
	if (ret != 0)
		goto onioerror;

#ifdef MS_WINDOWS
	/* MS _chsize doesn't work if newsize doesn't fit in 32 bits,
	   so don't even try using it. */
	{
		Py_off_t current;	/* current file position */
		HANDLE hFile;
		int error;

		/* current <- current file postion. */
		if (newsizeobj == NULL)
			current = newsize;
		else {
			Py_BEGIN_ALLOW_THREADS
			errno = 0;
			current = _portable_ftell(f->f_fp);
			Py_END_ALLOW_THREADS
			if (current == -1)
				goto onioerror;
		}

		/* Move to newsize. */
		if (current != newsize) {
			Py_BEGIN_ALLOW_THREADS
			errno = 0;
			error = _portable_fseek(f->f_fp, newsize, SEEK_SET)
				!= 0;
			Py_END_ALLOW_THREADS
			if (error)
				goto onioerror;
		}

		/* Truncate.  Note that this may grow the file! */
		Py_BEGIN_ALLOW_THREADS
		errno = 0;
		hFile = (HANDLE)_get_osfhandle(fileno(f->f_fp));
		error = hFile == (HANDLE)-1;
		if (!error) {
			error = SetEndOfFile(hFile) == 0;
			if (error)
				errno = EACCES;
		}
		Py_END_ALLOW_THREADS
		if (error)
			goto onioerror;

		/* Restore original file position. */
		if (current != newsize) {
			Py_BEGIN_ALLOW_THREADS
			errno = 0;
			error = _portable_fseek(f->f_fp, current, SEEK_SET)
				!= 0;
			Py_END_ALLOW_THREADS
			if (error)
				goto onioerror;
		}
	}
#else
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	ret = ftruncate(fileno(f->f_fp), newsize);
	Py_END_ALLOW_THREADS
	if (ret != 0) goto onioerror;
#endif /* !MS_WINDOWS */

	Py_INCREF(Py_None);
	return Py_None;

onioerror:
	PyErr_SetFromErrno(PyExc_IOError);
	clearerr(f->f_fp);
	return NULL;
}
#endif /* HAVE_FTRUNCATE */

static PyObject *
file_tell(PyFileObject *f)
{
	Py_off_t pos;

	if (f->f_fp == NULL)
		return err_closed();
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	pos = _portable_ftell(f->f_fp);
	Py_END_ALLOW_THREADS
	if (pos == -1) {
		PyErr_SetFromErrno(PyExc_IOError);
		clearerr(f->f_fp);
		return NULL;
	}
#ifdef WITH_UNIVERSAL_NEWLINES
	if (f->f_skipnextlf) {
		int c;
		c = GETC(f->f_fp);
		if (c == '\n') {
			pos++;
			f->f_skipnextlf = 0;
		} else if (c != EOF) ungetc(c, f->f_fp);
	}
#endif
#if !defined(HAVE_LARGEFILE_SUPPORT)
	return PyInt_FromLong(pos);
#else
	return PyLong_FromLongLong(pos);
#endif
}

static PyObject *
file_fileno(PyFileObject *f)
{
	if (f->f_fp == NULL)
		return err_closed();
	return PyInt_FromLong((long) fileno(f->f_fp));
}

static PyObject *
file_flush(PyFileObject *f)
{
	int res;

	if (f->f_fp == NULL)
		return err_closed();
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	res = fflush(f->f_fp);
	Py_END_ALLOW_THREADS
	if (res != 0) {
		PyErr_SetFromErrno(PyExc_IOError);
		clearerr(f->f_fp);
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
file_isatty(PyFileObject *f)
{
	long res;
	if (f->f_fp == NULL)
		return err_closed();
	Py_BEGIN_ALLOW_THREADS
	res = isatty((int)fileno(f->f_fp));
	Py_END_ALLOW_THREADS
	return PyBool_FromLong(res);
}


#if BUFSIZ < 8192
#define SMALLCHUNK 8192
#else
#define SMALLCHUNK BUFSIZ
#endif

#if SIZEOF_INT < 4
#define BIGCHUNK  (512 * 32)
#else
#define BIGCHUNK  (512 * 1024)
#endif

static size_t
new_buffersize(PyFileObject *f, size_t currentsize)
{
#ifdef HAVE_FSTAT
	off_t pos, end;
	struct stat st;
	if (fstat(fileno(f->f_fp), &st) == 0) {
		end = st.st_size;
		/* The following is not a bug: we really need to call lseek()
		   *and* ftell().  The reason is that some stdio libraries
		   mistakenly flush their buffer when ftell() is called and
		   the lseek() call it makes fails, thereby throwing away
		   data that cannot be recovered in any way.  To avoid this,
		   we first test lseek(), and only call ftell() if lseek()
		   works.  We can't use the lseek() value either, because we
		   need to take the amount of buffered data into account.
		   (Yet another reason why stdio stinks. :-) */
#ifdef USE_GUSI2
		pos = lseek(fileno(f->f_fp), 1L, SEEK_CUR);
		pos = lseek(fileno(f->f_fp), -1L, SEEK_CUR);
#else
		pos = lseek(fileno(f->f_fp), 0L, SEEK_CUR);
#endif
		if (pos >= 0) {
			pos = ftell(f->f_fp);
		}
		if (pos < 0)
			clearerr(f->f_fp);
		if (end > pos && pos >= 0)
			return currentsize + end - pos + 1;
		/* Add 1 so if the file were to grow we'd notice. */
	}
#endif
	if (currentsize > SMALLCHUNK) {
		/* Keep doubling until we reach BIGCHUNK;
		   then keep adding BIGCHUNK. */
		if (currentsize <= BIGCHUNK)
			return currentsize + currentsize;
		else
			return currentsize + BIGCHUNK;
	}
	return currentsize + SMALLCHUNK;
}

#if defined(EWOULDBLOCK) && defined(EAGAIN) && EWOULDBLOCK != EAGAIN
#define BLOCKED_ERRNO(x) ((x) == EWOULDBLOCK || (x) == EAGAIN)
#else
#ifdef EWOULDBLOCK
#define BLOCKED_ERRNO(x) ((x) == EWOULDBLOCK)
#else
#ifdef EAGAIN
#define BLOCKED_ERRNO(x) ((x) == EAGAIN)
#else
#define BLOCKED_ERRNO(x) 0
#endif
#endif
#endif

static PyObject *
file_read(PyFileObject *f, PyObject *args)
{
	long bytesrequested = -1;
	size_t bytesread, buffersize, chunksize;
	PyObject *v;

	if (f->f_fp == NULL)
		return err_closed();
	if (!PyArg_ParseTuple(args, "|l:read", &bytesrequested))
		return NULL;
	if (bytesrequested < 0)
		buffersize = new_buffersize(f, (size_t)0);
	else
		buffersize = bytesrequested;
	if (buffersize > INT_MAX) {
		PyErr_SetString(PyExc_OverflowError,
	"requested number of bytes is more than a Python string can hold");
		return NULL;
	}
	v = PyString_FromStringAndSize((char *)NULL, buffersize);
	if (v == NULL)
		return NULL;
	bytesread = 0;
	for (;;) {
		Py_BEGIN_ALLOW_THREADS
		errno = 0;
		chunksize = Py_UniversalNewlineFread(BUF(v) + bytesread,
			  buffersize - bytesread, f->f_fp, (PyObject *)f);
		Py_END_ALLOW_THREADS
		if (chunksize == 0) {
			if (!ferror(f->f_fp))
				break;
			clearerr(f->f_fp);
			/* When in non-blocking mode, data shouldn't
			 * be discarded if a blocking signal was
			 * received. That will also happen if
			 * chunksize != 0, but bytesread < buffersize. */
			if (bytesread > 0 && BLOCKED_ERRNO(errno))
				break;
			PyErr_SetFromErrno(PyExc_IOError);
			Py_DECREF(v);
			return NULL;
		}
		bytesread += chunksize;
		if (bytesread < buffersize) {
			clearerr(f->f_fp);
			break;
		}
		if (bytesrequested < 0) {
			buffersize = new_buffersize(f, buffersize);
			if (_PyString_Resize(&v, buffersize) < 0)
				return NULL;
		} else {
			/* Got what was requested. */
			break;
		}
	}
	if (bytesread != buffersize)
		_PyString_Resize(&v, bytesread);
	return v;
}

static PyObject *
file_readinto(PyFileObject *f, PyObject *args)
{
	char *ptr;
	int ntodo;
	size_t ndone, nnow;

	if (f->f_fp == NULL)
		return err_closed();
	if (!PyArg_ParseTuple(args, "w#", &ptr, &ntodo))
		return NULL;
	ndone = 0;
	while (ntodo > 0) {
		Py_BEGIN_ALLOW_THREADS
		errno = 0;
		nnow = Py_UniversalNewlineFread(ptr+ndone, ntodo, f->f_fp, 
						(PyObject *)f);
		Py_END_ALLOW_THREADS
		if (nnow == 0) {
			if (!ferror(f->f_fp))
				break;
			PyErr_SetFromErrno(PyExc_IOError);
			clearerr(f->f_fp);
			return NULL;
		}
		ndone += nnow;
		ntodo -= nnow;
	}
	return PyInt_FromLong((long)ndone);
}

/**************************************************************************
Routine to get next line using platform fgets().

Under MSVC 6:

+ MS threadsafe getc is very slow (multiple layers of function calls before+
  after each character, to lock+unlock the stream).
+ The stream-locking functions are MS-internal -- can't access them from user
  code.
+ There's nothing Tim could find in the MS C or platform SDK libraries that
  can worm around this.
+ MS fgets locks/unlocks only once per line; it's the only hook we have.

So we use fgets for speed(!), despite that it's painful.

MS realloc is also slow.

Reports from other platforms on this method vs getc_unlocked (which MS doesn't
have):
	Linux		a wash
	Solaris		a wash
	Tru64 Unix	getline_via_fgets significantly faster

CAUTION:  The C std isn't clear about this:  in those cases where fgets
writes something into the buffer, can it write into any position beyond the
required trailing null byte?  MSVC 6 fgets does not, and no platform is (yet)
known on which it does; and it would be a strange way to code fgets. Still,
getline_via_fgets may not work correctly if it does.  The std test
test_bufio.py should fail if platform fgets() routinely writes beyond the
trailing null byte.  #define DONT_USE_FGETS_IN_GETLINE to disable this code.
**************************************************************************/

/* Use this routine if told to, or by default on non-get_unlocked()
 * platforms unless told not to.  Yikes!  Let's spell that out:
 * On a platform with getc_unlocked():
 *     By default, use getc_unlocked().
 *     If you want to use fgets() instead, #define USE_FGETS_IN_GETLINE.
 * On a platform without getc_unlocked():
 *     By default, use fgets().
 *     If you don't want to use fgets(), #define DONT_USE_FGETS_IN_GETLINE.
 */
#if !defined(USE_FGETS_IN_GETLINE) && !defined(HAVE_GETC_UNLOCKED)
#define USE_FGETS_IN_GETLINE
#endif

#if defined(DONT_USE_FGETS_IN_GETLINE) && defined(USE_FGETS_IN_GETLINE)
#undef USE_FGETS_IN_GETLINE
#endif

#ifdef USE_FGETS_IN_GETLINE
static PyObject*
getline_via_fgets(FILE *fp)
{
/* INITBUFSIZE is the maximum line length that lets us get away with the fast
 * no-realloc, one-fgets()-call path.  Boosting it isn't free, because we have
 * to fill this much of the buffer with a known value in order to figure out
 * how much of the buffer fgets() overwrites.  So if INITBUFSIZE is larger
 * than "most" lines, we waste time filling unused buffer slots.  100 is
 * surely adequate for most peoples' email archives, chewing over source code,
 * etc -- "regular old text files".
 * MAXBUFSIZE is the maximum line length that lets us get away with the less
 * fast (but still zippy) no-realloc, two-fgets()-call path.  See above for
 * cautions about boosting that.  300 was chosen because the worst real-life
 * text-crunching job reported on Python-Dev was a mail-log crawler where over
 * half the lines were 254 chars.
 */
#define INITBUFSIZE 100
#define MAXBUFSIZE 300
	char* p;	/* temp */
	char buf[MAXBUFSIZE];
	PyObject* v;	/* the string object result */
	char* pvfree;	/* address of next free slot */
	char* pvend;    /* address one beyond last free slot */
	size_t nfree;	/* # of free buffer slots; pvend-pvfree */
	size_t total_v_size;  /* total # of slots in buffer */
	size_t increment;	/* amount to increment the buffer */

	/* Optimize for normal case:  avoid _PyString_Resize if at all
	 * possible via first reading into stack buffer "buf".
	 */
	total_v_size = INITBUFSIZE;	/* start small and pray */
	pvfree = buf;
	for (;;) {
		Py_BEGIN_ALLOW_THREADS
		pvend = buf + total_v_size;
		nfree = pvend - pvfree;
		memset(pvfree, '\n', nfree);
		p = fgets(pvfree, nfree, fp);
		Py_END_ALLOW_THREADS

		if (p == NULL) {
			clearerr(fp);
			if (PyErr_CheckSignals())
				return NULL;
			v = PyString_FromStringAndSize(buf, pvfree - buf);
			return v;
		}
		/* fgets read *something* */
		p = memchr(pvfree, '\n', nfree);
		if (p != NULL) {
			/* Did the \n come from fgets or from us?
			 * Since fgets stops at the first \n, and then writes
			 * \0, if it's from fgets a \0 must be next.  But if
			 * that's so, it could not have come from us, since
			 * the \n's we filled the buffer with have only more
			 * \n's to the right.
			 */
			if (p+1 < pvend && *(p+1) == '\0') {
				/* It's from fgets:  we win!  In particular,
				 * we haven't done any mallocs yet, and can
				 * build the final result on the first try.
				 */
				++p;	/* include \n from fgets */
			}
			else {
				/* Must be from us:  fgets didn't fill the
				 * buffer and didn't find a newline, so it
				 * must be the last and newline-free line of
				 * the file.
				 */
				assert(p > pvfree && *(p-1) == '\0');
				--p;	/* don't include \0 from fgets */
			}
			v = PyString_FromStringAndSize(buf, p - buf);
			return v;
		}
		/* yuck:  fgets overwrote all the newlines, i.e. the entire
		 * buffer.  So this line isn't over yet, or maybe it is but
		 * we're exactly at EOF.  If we haven't already, try using the
		 * rest of the stack buffer.
		 */
		assert(*(pvend-1) == '\0');
		if (pvfree == buf) {
			pvfree = pvend - 1;	/* overwrite trailing null */
			total_v_size = MAXBUFSIZE;
		}
		else
			break;
	}

	/* The stack buffer isn't big enough; malloc a string object and read
	 * into its buffer.
	 */
	total_v_size = MAXBUFSIZE << 1;
	v = PyString_FromStringAndSize((char*)NULL, (int)total_v_size);
	if (v == NULL)
		return v;
	/* copy over everything except the last null byte */
	memcpy(BUF(v), buf, MAXBUFSIZE-1);
	pvfree = BUF(v) + MAXBUFSIZE - 1;

	/* Keep reading stuff into v; if it ever ends successfully, break
	 * after setting p one beyond the end of the line.  The code here is
	 * very much like the code above, except reads into v's buffer; see
	 * the code above for detailed comments about the logic.
	 */
	for (;;) {
		Py_BEGIN_ALLOW_THREADS
		pvend = BUF(v) + total_v_size;
		nfree = pvend - pvfree;
		memset(pvfree, '\n', nfree);
		p = fgets(pvfree, nfree, fp);
		Py_END_ALLOW_THREADS

		if (p == NULL) {
			clearerr(fp);
			if (PyErr_CheckSignals()) {
				Py_DECREF(v);
				return NULL;
			}
			p = pvfree;
			break;
		}
		p = memchr(pvfree, '\n', nfree);
		if (p != NULL) {
			if (p+1 < pvend && *(p+1) == '\0') {
				/* \n came from fgets */
				++p;
				break;
			}
			/* \n came from us; last line of file, no newline */
			assert(p > pvfree && *(p-1) == '\0');
			--p;
			break;
		}
		/* expand buffer and try again */
		assert(*(pvend-1) == '\0');
		increment = total_v_size >> 2;	/* mild exponential growth */
		total_v_size += increment;
		if (total_v_size > INT_MAX) {
			PyErr_SetString(PyExc_OverflowError,
			    "line is longer than a Python string can hold");
			Py_DECREF(v);
			return NULL;
		}
		if (_PyString_Resize(&v, (int)total_v_size) < 0)
			return NULL;
		/* overwrite the trailing null byte */
		pvfree = BUF(v) + (total_v_size - increment - 1);
	}
	if (BUF(v) + total_v_size != p)
		_PyString_Resize(&v, p - BUF(v));
	return v;
#undef INITBUFSIZE
#undef MAXBUFSIZE
}
#endif	/* ifdef USE_FGETS_IN_GETLINE */

/* Internal routine to get a line.
   Size argument interpretation:
   > 0: max length;
   <= 0: read arbitrary line
*/

static PyObject *
get_line(PyFileObject *f, int n)
{
	FILE *fp = f->f_fp;
	int c;
	char *buf, *end;
	size_t total_v_size;	/* total # of slots in buffer */
	size_t used_v_size;	/* # used slots in buffer */
	size_t increment;       /* amount to increment the buffer */
	PyObject *v;
#ifdef WITH_UNIVERSAL_NEWLINES
	int newlinetypes = f->f_newlinetypes;
	int skipnextlf = f->f_skipnextlf;
	int univ_newline = f->f_univ_newline;
#endif

#if defined(USE_FGETS_IN_GETLINE)
#ifdef WITH_UNIVERSAL_NEWLINES
	if (n <= 0 && !univ_newline )
#else
	if (n <= 0)
#endif
		return getline_via_fgets(fp);
#endif
	total_v_size = n > 0 ? n : 100;
	v = PyString_FromStringAndSize((char *)NULL, total_v_size);
	if (v == NULL)
		return NULL;
	buf = BUF(v);
	end = buf + total_v_size;

	for (;;) {
		Py_BEGIN_ALLOW_THREADS
		FLOCKFILE(fp);
#ifdef WITH_UNIVERSAL_NEWLINES
		if (univ_newline) {
			c = 'x'; /* Shut up gcc warning */
			while ( buf != end && (c = GETC(fp)) != EOF ) {
				if (skipnextlf ) {
					skipnextlf = 0;
					if (c == '\n') {
						/* Seeing a \n here with 
						 * skipnextlf true means we 
						 * saw a \r before.
						 */
						newlinetypes |= NEWLINE_CRLF;
						c = GETC(fp);
						if (c == EOF) break;
					} else {
						newlinetypes |= NEWLINE_CR;
					}
				}
				if (c == '\r') {
					skipnextlf = 1;
					c = '\n';
				} else if ( c == '\n')
					newlinetypes |= NEWLINE_LF;
				*buf++ = c;
				if (c == '\n') break;
			}
			if ( c == EOF && skipnextlf )
				newlinetypes |= NEWLINE_CR;
		} else /* If not universal newlines use the normal loop */
#endif
		while ((c = GETC(fp)) != EOF &&
		       (*buf++ = c) != '\n' &&
			buf != end)
			;
		FUNLOCKFILE(fp);
		Py_END_ALLOW_THREADS
#ifdef WITH_UNIVERSAL_NEWLINES
		f->f_newlinetypes = newlinetypes;
		f->f_skipnextlf = skipnextlf;
#endif
		if (c == '\n')
			break;
		if (c == EOF) {
			if (ferror(fp)) {
				PyErr_SetFromErrno(PyExc_IOError);
				clearerr(fp);
				Py_DECREF(v);
				return NULL;
			}
			clearerr(fp);
			if (PyErr_CheckSignals()) {
				Py_DECREF(v);
				return NULL;
			}
			break;
		}
		/* Must be because buf == end */
		if (n > 0)
			break;
		used_v_size = total_v_size;
		increment = total_v_size >> 2; /* mild exponential growth */
		total_v_size += increment;
		if (total_v_size > INT_MAX) {
			PyErr_SetString(PyExc_OverflowError,
			    "line is longer than a Python string can hold");
			Py_DECREF(v);
			return NULL;
		}
		if (_PyString_Resize(&v, total_v_size) < 0)
			return NULL;
		buf = BUF(v) + used_v_size;
		end = BUF(v) + total_v_size;
	}

	used_v_size = buf - BUF(v);
	if (used_v_size != total_v_size)
		_PyString_Resize(&v, used_v_size);
	return v;
}

/* External C interface */

PyObject *
PyFile_GetLine(PyObject *f, int n)
{
	PyObject *result;

	if (f == NULL) {
		PyErr_BadInternalCall();
		return NULL;
	}

	if (PyFile_Check(f)) {
		if (((PyFileObject*)f)->f_fp == NULL)
			return err_closed();
		result = get_line((PyFileObject *)f, n);
	}
	else {
		PyObject *reader;
		PyObject *args;

		reader = PyObject_GetAttrString(f, "readline");
		if (reader == NULL)
			return NULL;
		if (n <= 0)
			args = Py_BuildValue("()");
		else
			args = Py_BuildValue("(i)", n);
		if (args == NULL) {
			Py_DECREF(reader);
			return NULL;
		}
		result = PyEval_CallObject(reader, args);
		Py_DECREF(reader);
		Py_DECREF(args);
		if (result != NULL && !PyString_Check(result) &&
		    !PyUnicode_Check(result)) {
			Py_DECREF(result);
			result = NULL;
			PyErr_SetString(PyExc_TypeError,
				   "object.readline() returned non-string");
		}
	}

	if (n < 0 && result != NULL && PyString_Check(result)) {
		char *s = PyString_AS_STRING(result);
		int len = PyString_GET_SIZE(result);
		if (len == 0) {
			Py_DECREF(result);
			result = NULL;
			PyErr_SetString(PyExc_EOFError,
					"EOF when reading a line");
		}
		else if (s[len-1] == '\n') {
			if (result->ob_refcnt == 1)
				_PyString_Resize(&result, len-1);
			else {
				PyObject *v;
				v = PyString_FromStringAndSize(s, len-1);
				Py_DECREF(result);
				result = v;
			}
		}
	}
#ifdef Py_USING_UNICODE
	if (n < 0 && result != NULL && PyUnicode_Check(result)) {
		Py_UNICODE *s = PyUnicode_AS_UNICODE(result);
		int len = PyUnicode_GET_SIZE(result);
		if (len == 0) {
			Py_DECREF(result);
			result = NULL;
			PyErr_SetString(PyExc_EOFError,
					"EOF when reading a line");
		}
		else if (s[len-1] == '\n') {
			if (result->ob_refcnt == 1)
				PyUnicode_Resize(&result, len-1);
			else {
				PyObject *v;
				v = PyUnicode_FromUnicode(s, len-1);
				Py_DECREF(result);
				result = v;
			}
		}
	}
#endif
	return result;
}

/* Python method */

static PyObject *
file_readline(PyFileObject *f, PyObject *args)
{
	int n = -1;

	if (f->f_fp == NULL)
		return err_closed();
	if (!PyArg_ParseTuple(args, "|i:readline", &n))
		return NULL;
	if (n == 0)
		return PyString_FromString("");
	if (n < 0)
		n = 0;
	return get_line(f, n);
}

static PyObject *
file_readlines(PyFileObject *f, PyObject *args)
{
	long sizehint = 0;
	PyObject *list;
	PyObject *line;
	char small_buffer[SMALLCHUNK];
	char *buffer = small_buffer;
	size_t buffersize = SMALLCHUNK;
	PyObject *big_buffer = NULL;
	size_t nfilled = 0;
	size_t nread;
	size_t totalread = 0;
	char *p, *q, *end;
	int err;
	int shortread = 0;

	if (f->f_fp == NULL)
		return err_closed();
	if (!PyArg_ParseTuple(args, "|l:readlines", &sizehint))
		return NULL;
	if ((list = PyList_New(0)) == NULL)
		return NULL;
	for (;;) {
		if (shortread)
			nread = 0;
		else {
			Py_BEGIN_ALLOW_THREADS
			errno = 0;
			nread = Py_UniversalNewlineFread(buffer+nfilled,
				buffersize-nfilled, f->f_fp, (PyObject *)f);
			Py_END_ALLOW_THREADS
			shortread = (nread < buffersize-nfilled);
		}
		if (nread == 0) {
			sizehint = 0;
			if (!ferror(f->f_fp))
				break;
			PyErr_SetFromErrno(PyExc_IOError);
			clearerr(f->f_fp);
		  error:
			Py_DECREF(list);
			list = NULL;
			goto cleanup;
		}
		totalread += nread;
		p = memchr(buffer+nfilled, '\n', nread);
		if (p == NULL) {
			/* Need a larger buffer to fit this line */
			nfilled += nread;
			buffersize *= 2;
			if (buffersize > INT_MAX) {
				PyErr_SetString(PyExc_OverflowError,
			    "line is longer than a Python string can hold");
				goto error;
			}
			if (big_buffer == NULL) {
				/* Create the big buffer */
				big_buffer = PyString_FromStringAndSize(
					NULL, buffersize);
				if (big_buffer == NULL)
					goto error;
				buffer = PyString_AS_STRING(big_buffer);
				memcpy(buffer, small_buffer, nfilled);
			}
			else {
				/* Grow the big buffer */
				if ( _PyString_Resize(&big_buffer, buffersize) < 0 )
					goto error;
				buffer = PyString_AS_STRING(big_buffer);
			}
			continue;
		}
		end = buffer+nfilled+nread;
		q = buffer;
		do {
			/* Process complete lines */
			p++;
			line = PyString_FromStringAndSize(q, p-q);
			if (line == NULL)
				goto error;
			err = PyList_Append(list, line);
			Py_DECREF(line);
			if (err != 0)
				goto error;
			q = p;
			p = memchr(q, '\n', end-q);
		} while (p != NULL);
		/* Move the remaining incomplete line to the start */
		nfilled = end-q;
		memmove(buffer, q, nfilled);
		if (sizehint > 0)
			if (totalread >= (size_t)sizehint)
				break;
	}
	if (nfilled != 0) {
		/* Partial last line */
		line = PyString_FromStringAndSize(buffer, nfilled);
		if (line == NULL)
			goto error;
		if (sizehint > 0) {
			/* Need to complete the last line */
			PyObject *rest = get_line(f, 0);
			if (rest == NULL) {
				Py_DECREF(line);
				goto error;
			}
			PyString_Concat(&line, rest);
			Py_DECREF(rest);
			if (line == NULL)
				goto error;
		}
		err = PyList_Append(list, line);
		Py_DECREF(line);
		if (err != 0)
			goto error;
	}
  cleanup:
	Py_XDECREF(big_buffer);
	return list;
}

static PyObject *
file_write(PyFileObject *f, PyObject *args)
{
	char *s;
	int n, n2;
	if (f->f_fp == NULL)
		return err_closed();
	if (!PyArg_ParseTuple(args, f->f_binary ? "s#" : "t#", &s, &n))
		return NULL;
	f->f_softspace = 0;
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	n2 = fwrite(s, 1, n, f->f_fp);
	Py_END_ALLOW_THREADS
	if (n2 != n) {
		PyErr_SetFromErrno(PyExc_IOError);
		clearerr(f->f_fp);
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
file_writelines(PyFileObject *f, PyObject *seq)
{
#define CHUNKSIZE 1000
	PyObject *list, *line;
	PyObject *it;	/* iter(seq) */
	PyObject *result;
	int i, j, index, len, nwritten, islist;

	assert(seq != NULL);
	if (f->f_fp == NULL)
		return err_closed();

	result = NULL;
	list = NULL;
	islist = PyList_Check(seq);
	if  (islist)
		it = NULL;
	else {
		it = PyObject_GetIter(seq);
		if (it == NULL) {
			PyErr_SetString(PyExc_TypeError,
				"writelines() requires an iterable argument");
			return NULL;
		}
		/* From here on, fail by going to error, to reclaim "it". */
		list = PyList_New(CHUNKSIZE);
		if (list == NULL)
			goto error;
	}

	/* Strategy: slurp CHUNKSIZE lines into a private list,
	   checking that they are all strings, then write that list
	   without holding the interpreter lock, then come back for more. */
	for (index = 0; ; index += CHUNKSIZE) {
		if (islist) {
			Py_XDECREF(list);
			list = PyList_GetSlice(seq, index, index+CHUNKSIZE);
			if (list == NULL)
				goto error;
			j = PyList_GET_SIZE(list);
		}
		else {
			for (j = 0; j < CHUNKSIZE; j++) {
				line = PyIter_Next(it);
				if (line == NULL) {
					if (PyErr_Occurred())
						goto error;
					break;
				}
				PyList_SetItem(list, j, line);
			}
		}
		if (j == 0)
			break;

		/* Check that all entries are indeed strings. If not,
		   apply the same rules as for file.write() and
		   convert the results to strings. This is slow, but
		   seems to be the only way since all conversion APIs
		   could potentially execute Python code. */
		for (i = 0; i < j; i++) {
			PyObject *v = PyList_GET_ITEM(list, i);
			if (!PyString_Check(v)) {
			    	const char *buffer;
			    	int len;
				if (((f->f_binary &&
				      PyObject_AsReadBuffer(v,
					      (const void**)&buffer,
							    &len)) ||
				     PyObject_AsCharBuffer(v,
							   &buffer,
							   &len))) {
					PyErr_SetString(PyExc_TypeError,
			"writelines() argument must be a sequence of strings");
					goto error;
				}
				line = PyString_FromStringAndSize(buffer,
								  len);
				if (line == NULL)
					goto error;
				Py_DECREF(v);
				PyList_SET_ITEM(list, i, line);
			}
		}

		/* Since we are releasing the global lock, the
		   following code may *not* execute Python code. */
		Py_BEGIN_ALLOW_THREADS
		f->f_softspace = 0;
		errno = 0;
		for (i = 0; i < j; i++) {
		    	line = PyList_GET_ITEM(list, i);
			len = PyString_GET_SIZE(line);
			nwritten = fwrite(PyString_AS_STRING(line),
					  1, len, f->f_fp);
			if (nwritten != len) {
				Py_BLOCK_THREADS
				PyErr_SetFromErrno(PyExc_IOError);
				clearerr(f->f_fp);
				goto error;
			}
		}
		Py_END_ALLOW_THREADS

		if (j < CHUNKSIZE)
			break;
	}

	Py_INCREF(Py_None);
	result = Py_None;
  error:
	Py_XDECREF(list);
  	Py_XDECREF(it);
	return result;
#undef CHUNKSIZE
}

static PyObject *
file_getiter(PyFileObject *f)
{
	if (f->f_fp == NULL)
		return err_closed();
	Py_INCREF(f);
	return (PyObject *)f;
}

PyDoc_STRVAR(readline_doc,
"readline([size]) -> next line from the file, as a string.\n"
"\n"
"Retain newline.  A non-negative size argument limits the maximum\n"
"number of bytes to return (an incomplete line may be returned then).\n"
"Return an empty string at EOF.");

PyDoc_STRVAR(read_doc,
"read([size]) -> read at most size bytes, returned as a string.\n"
"\n"
"If the size argument is negative or omitted, read until EOF is reached.\n"
"Notice that when in non-blocking mode, less data than what was requested\n"
"may be returned, even if no size parameter was given.");

PyDoc_STRVAR(write_doc,
"write(str) -> None.  Write string str to file.\n"
"\n"
"Note that due to buffering, flush() or close() may be needed before\n"
"the file on disk reflects the data written.");

PyDoc_STRVAR(fileno_doc,
"fileno() -> integer \"file descriptor\".\n"
"\n"
"This is needed for lower-level file interfaces, such os.read().");

PyDoc_STRVAR(seek_doc,
"seek(offset[, whence]) -> None.  Move to new file position.\n"
"\n"
"Argument offset is a byte count.  Optional argument whence defaults to\n"
"0 (offset from start of file, offset should be >= 0); other values are 1\n"
"(move relative to current position, positive or negative), and 2 (move\n"
"relative to end of file, usually negative, although many platforms allow\n"
"seeking beyond the end of a file).\n"
"\n"
"Note that not all file objects are seekable.");

#ifdef HAVE_FTRUNCATE
PyDoc_STRVAR(truncate_doc,
"truncate([size]) -> None.  Truncate the file to at most size bytes.\n"
"\n"
"Size defaults to the current file position, as returned by tell().");
#endif

PyDoc_STRVAR(tell_doc,
"tell() -> current file position, an integer (may be a long integer).");

PyDoc_STRVAR(readinto_doc,
"readinto() -> Undocumented.  Don't use this; it may go away.");

PyDoc_STRVAR(readlines_doc,
"readlines([size]) -> list of strings, each a line from the file.\n"
"\n"
"Call readline() repeatedly and return a list of the lines so read.\n"
"The optional size argument, if given, is an approximate bound on the\n"
"total number of bytes in the lines returned.");

PyDoc_STRVAR(xreadlines_doc,
"xreadlines() -> returns self.\n"
"\n"
"For backward compatibility. File objects now include the performance\n"
"optimizations previously implemented in the xreadlines module.");

PyDoc_STRVAR(writelines_doc,
"writelines(sequence_of_strings) -> None.  Write the strings to the file.\n"
"\n"
"Note that newlines are not added.  The sequence can be any iterable object\n"
"producing strings. This is equivalent to calling write() for each string.");

PyDoc_STRVAR(flush_doc,
"flush() -> None.  Flush the internal I/O buffer.");

PyDoc_STRVAR(close_doc,
"close() -> None or (perhaps) an integer.  Close the file.\n"
"\n"
"Sets data attribute .closed to True.  A closed file cannot be used for\n"
"further I/O operations.  close() may be called more than once without\n"
"error.  Some kinds of file objects (for example, opened by popen())\n"
"may return an exit status upon closing.");

PyDoc_STRVAR(isatty_doc,
"isatty() -> true or false.  True if the file is connected to a tty device.");

static PyMethodDef file_methods[] = {
	{"readline",  (PyCFunction)file_readline, METH_VARARGS, readline_doc},
	{"read",      (PyCFunction)file_read,     METH_VARARGS, read_doc},
	{"write",     (PyCFunction)file_write,    METH_VARARGS, write_doc},
	{"fileno",    (PyCFunction)file_fileno,   METH_NOARGS,  fileno_doc},
	{"seek",      (PyCFunction)file_seek,     METH_VARARGS, seek_doc},
#ifdef HAVE_FTRUNCATE
	{"truncate",  (PyCFunction)file_truncate, METH_VARARGS, truncate_doc},
#endif
	{"tell",      (PyCFunction)file_tell,     METH_NOARGS,  tell_doc},
	{"readinto",  (PyCFunction)file_readinto, METH_VARARGS, readinto_doc},
	{"readlines", (PyCFunction)file_readlines,METH_VARARGS, readlines_doc},
	{"xreadlines",(PyCFunction)file_getiter,  METH_NOARGS, xreadlines_doc},
	{"writelines",(PyCFunction)file_writelines, METH_O,    writelines_doc},
	{"flush",     (PyCFunction)file_flush,    METH_NOARGS,  flush_doc},
	{"close",     (PyCFunction)file_close,    METH_NOARGS,  close_doc},
	{"isatty",    (PyCFunction)file_isatty,   METH_NOARGS,  isatty_doc},
	{NULL,	      NULL}		/* sentinel */
};

#define OFF(x) offsetof(PyFileObject, x)

static PyMemberDef file_memberlist[] = {
	{"softspace",	T_INT,		OFF(f_softspace), 0,
	 "flag indicating that a space needs to be printed; used by print"},
	{"mode",	T_OBJECT,	OFF(f_mode),	RO,
	 "file mode ('r', 'U', 'w', 'a', possibly with 'b' or '+' added)"},
	{"name",	T_OBJECT,	OFF(f_name),	RO,
	 "file name"},
	/* getattr(f, "closed") is implemented without this table */
	{NULL}	/* Sentinel */
};

static PyObject *
get_closed(PyFileObject *f, void *closure)
{
	return PyBool_FromLong((long)(f->f_fp == 0));
}
#ifdef WITH_UNIVERSAL_NEWLINES
static PyObject *
get_newlines(PyFileObject *f, void *closure)
{
	switch (f->f_newlinetypes) {
	case NEWLINE_UNKNOWN:
		Py_INCREF(Py_None);
		return Py_None;
	case NEWLINE_CR:
		return PyString_FromString("\r");
	case NEWLINE_LF:
		return PyString_FromString("\n");
	case NEWLINE_CR|NEWLINE_LF:
		return Py_BuildValue("(ss)", "\r", "\n");
	case NEWLINE_CRLF:
		return PyString_FromString("\r\n");
	case NEWLINE_CR|NEWLINE_CRLF:
		return Py_BuildValue("(ss)", "\r", "\r\n");
	case NEWLINE_LF|NEWLINE_CRLF:
		return Py_BuildValue("(ss)", "\n", "\r\n");
	case NEWLINE_CR|NEWLINE_LF|NEWLINE_CRLF:
		return Py_BuildValue("(sss)", "\r", "\n", "\r\n");
	default:
		PyErr_Format(PyExc_SystemError, 
			     "Unknown newlines value 0x%x\n", 
			     f->f_newlinetypes);
		return NULL;
	}
}
#endif

static PyGetSetDef file_getsetlist[] = {
	{"closed", (getter)get_closed, NULL, "True if the file is closed"},
#ifdef WITH_UNIVERSAL_NEWLINES
	{"newlines", (getter)get_newlines, NULL, 
	 "end-of-line convention used in this file"},
#endif
	{0},
};

static void
drop_readahead(PyFileObject *f)
{
	if (f->f_buf != NULL) {
		PyMem_Free(f->f_buf);
		f->f_buf = NULL;
	}
}

/* Make sure that file has a readahead buffer with at least one byte 
   (unless at EOF) and no more than bufsize.  Returns negative value on 
   error */
static int
readahead(PyFileObject *f, int bufsize)
{
	int chunksize;

	if (f->f_buf != NULL) {
		if( (f->f_bufend - f->f_bufptr) >= 1) 
			return 0;
		else
			drop_readahead(f);
	}
	if ((f->f_buf = PyMem_Malloc(bufsize)) == NULL) {
		return -1;
	}
	Py_BEGIN_ALLOW_THREADS
	errno = 0;
	chunksize = Py_UniversalNewlineFread(
		f->f_buf, bufsize, f->f_fp, (PyObject *)f);
	Py_END_ALLOW_THREADS
	if (chunksize == 0) {
		if (ferror(f->f_fp)) {
			PyErr_SetFromErrno(PyExc_IOError);
			clearerr(f->f_fp);
			drop_readahead(f);
			return -1;
		}
	}
	f->f_bufptr = f->f_buf;
	f->f_bufend = f->f_buf + chunksize;
	return 0;
}

/* Used by file_iternext.  The returned string will start with 'skip'
   uninitialized bytes followed by the remainder of the line. Don't be 
   horrified by the recursive call: maximum recursion depth is limited by 
   logarithmic buffer growth to about 50 even when reading a 1gb line. */

static PyStringObject *
readahead_get_line_skip(PyFileObject *f, int skip, int bufsize)
{
	PyStringObject* s;
	char *bufptr;
	char *buf;
	int len;

	if (f->f_buf == NULL)
		if (readahead(f, bufsize) < 0) 
			return NULL;

	len = f->f_bufend - f->f_bufptr;
	if (len == 0) 
		return (PyStringObject *)
			PyString_FromStringAndSize(NULL, skip);
	bufptr = memchr(f->f_bufptr, '\n', len);
	if (bufptr != NULL) {
		bufptr++;			/* Count the '\n' */
		len = bufptr - f->f_bufptr;
		s = (PyStringObject *)
			PyString_FromStringAndSize(NULL, skip+len);
		if (s == NULL) 
			return NULL;
		memcpy(PyString_AS_STRING(s)+skip, f->f_bufptr, len);
		f->f_bufptr = bufptr;
		if (bufptr == f->f_bufend)
			drop_readahead(f);
	} else {
		bufptr = f->f_bufptr;
		buf = f->f_buf;
		f->f_buf = NULL; 	/* Force new readahead buffer */
                s = readahead_get_line_skip(
			f, skip+len, bufsize + (bufsize>>2) );
		if (s == NULL) {
		        PyMem_Free(buf);
			return NULL;
		}
		memcpy(PyString_AS_STRING(s)+skip, bufptr, len);
		PyMem_Free(buf);
	}
	return s;
}

/* A larger buffer size may actually decrease performance. */
#define READAHEAD_BUFSIZE 8192

static PyObject *
file_iternext(PyFileObject *f)
{
	PyStringObject* l;

	if (f->f_fp == NULL)
		return err_closed();

	l = readahead_get_line_skip(f, 0, READAHEAD_BUFSIZE);
	if (l == NULL || PyString_GET_SIZE(l) == 0) {
		Py_XDECREF(l);
		return NULL;
	}
	return (PyObject *)l;
}


static PyObject *
file_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyObject *self;
	static PyObject *not_yet_string;

	assert(type != NULL && type->tp_alloc != NULL);

	if (not_yet_string == NULL) {
		not_yet_string = PyString_FromString("<uninitialized file>");
		if (not_yet_string == NULL)
			return NULL;
	}

	self = type->tp_alloc(type, 0);
	if (self != NULL) {
		/* Always fill in the name and mode, so that nobody else
		   needs to special-case NULLs there. */
		Py_INCREF(not_yet_string);
		((PyFileObject *)self)->f_name = not_yet_string;
		Py_INCREF(not_yet_string);
		((PyFileObject *)self)->f_mode = not_yet_string;
	}
	return self;
}

static int
file_init(PyObject *self, PyObject *args, PyObject *kwds)
{
	PyFileObject *foself = (PyFileObject *)self;
	int ret = 0;
	static char *kwlist[] = {"name", "mode", "buffering", 0};
	char *name = NULL;
	char *mode = "r";
	int bufsize = -1;
	int wideargument = 0;

	assert(PyFile_Check(self));
	if (foself->f_fp != NULL) {
		/* Have to close the existing file first. */
		PyObject *closeresult = file_close(foself);
		if (closeresult == NULL)
			return -1;
		Py_DECREF(closeresult);
	}

#ifdef Py_WIN_WIDE_FILENAMES
	if (GetVersion() < 0x80000000) {    /* On NT, so wide API available */
		PyObject *po;
		if (PyArg_ParseTupleAndKeywords(args, kwds, "U|si:file",
						kwlist, &po, &mode, &bufsize)) {
			wideargument = 1;
			if (fill_file_fields(foself, NULL, name, mode,
					     fclose, po) == NULL)
				goto Error;
		} else {
			/* Drop the argument parsing error as narrow
			   strings are also valid. */
			PyErr_Clear();
		}
	}
#endif

	if (!wideargument) {
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "et|si:file", kwlist,
						 Py_FileSystemDefaultEncoding,
						 &name,
						 &mode, &bufsize))
			return -1;
		if (fill_file_fields(foself, NULL, name, mode,
				     fclose, NULL) == NULL)
			goto Error;
	}
	if (open_the_file(foself, name, mode) == NULL)
		goto Error;
	PyFile_SetBufSize(self, bufsize);
	goto Done;

Error:
	ret = -1;
	/* fall through */
Done:
	PyMem_Free(name); /* free the encoded string */
	return ret;
}

PyDoc_VAR(file_doc) =
PyDoc_STR(
"file(name[, mode[, buffering]]) -> file object\n"
"\n"
"Open a file.  The mode can be 'r', 'w' or 'a' for reading (default),\n"
"writing or appending.  The file will be created if it doesn't exist\n"
"when opened for writing or appending; it will be truncated when\n"
"opened for writing.  Add a 'b' to the mode for binary files.\n"
"Add a '+' to the mode to allow simultaneous reading and writing.\n"
"If the buffering argument is given, 0 means unbuffered, 1 means line\n"
"buffered, and larger numbers specify the buffer size.\n"
)
#ifdef WITH_UNIVERSAL_NEWLINES
PyDoc_STR(
"Add a 'U' to mode to open the file for input with universal newline\n"
"support.  Any line ending in the input file will be seen as a '\\n'\n"
"in Python.  Also, a file so opened gains the attribute 'newlines';\n"
"the value for this attribute is one of None (no newline read yet),\n"
"'\\r', '\\n', '\\r\\n' or a tuple containing all the newline types seen.\n"
"\n"
"'U' cannot be combined with 'w' or '+' mode.\n"
)
#endif /* WITH_UNIVERSAL_NEWLINES */
PyDoc_STR(
"\n"
"Note:  open() is an alias for file()."
);

PyTypeObject PyFile_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"file",
	sizeof(PyFileObject),
	0,
	(destructor)file_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,			 		/* tp_getattr */
	0,			 		/* tp_setattr */
	0,					/* tp_compare */
	(reprfunc)file_repr, 			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	file_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	(getiterfunc)file_getiter,		/* tp_iter */
	(iternextfunc)file_iternext,		/* tp_iternext */
	file_methods,				/* tp_methods */
	file_memberlist,			/* tp_members */
	file_getsetlist,			/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc)file_init,			/* tp_init */
	PyType_GenericAlloc,			/* tp_alloc */
	file_new,				/* tp_new */
	PyObject_Del,                           /* tp_free */
};

/* Interface for the 'soft space' between print items. */

int
PyFile_SoftSpace(PyObject *f, int newflag)
{
	int oldflag = 0;
	if (f == NULL) {
		/* Do nothing */
	}
	else if (PyFile_Check(f)) {
		oldflag = ((PyFileObject *)f)->f_softspace;
		((PyFileObject *)f)->f_softspace = newflag;
	}
	else {
		PyObject *v;
		v = PyObject_GetAttrString(f, "softspace");
		if (v == NULL)
			PyErr_Clear();
		else {
			if (PyInt_Check(v))
				oldflag = PyInt_AsLong(v);
			Py_DECREF(v);
		}
		v = PyInt_FromLong((long)newflag);
		if (v == NULL)
			PyErr_Clear();
		else {
			if (PyObject_SetAttrString(f, "softspace", v) != 0)
				PyErr_Clear();
			Py_DECREF(v);
		}
	}
	return oldflag;
}

/* Interfaces to write objects/strings to file-like objects */

int
PyFile_WriteObject(PyObject *v, PyObject *f, int flags)
{
	PyObject *writer, *value, *args, *result;
	if (f == NULL) {
		PyErr_SetString(PyExc_TypeError, "writeobject with NULL file");
		return -1;
	}
	else if (PyFile_Check(f)) {
		FILE *fp = PyFile_AsFile(f);
		if (fp == NULL) {
			err_closed();
			return -1;
		}
		return PyObject_Print(v, fp, flags);
	}
	writer = PyObject_GetAttrString(f, "write");
	if (writer == NULL)
		return -1;
	if (flags & Py_PRINT_RAW) {
                if (PyUnicode_Check(v)) {
                        value = v;
                        Py_INCREF(value);
                } else
                        value = PyObject_Str(v);
	}
        else
		value = PyObject_Repr(v);
	if (value == NULL) {
		Py_DECREF(writer);
		return -1;
	}
	args = Py_BuildValue("(O)", value);
	if (args == NULL) {
		Py_DECREF(value);
		Py_DECREF(writer);
		return -1;
	}
	result = PyEval_CallObject(writer, args);
	Py_DECREF(args);
	Py_DECREF(value);
	Py_DECREF(writer);
	if (result == NULL)
		return -1;
	Py_DECREF(result);
	return 0;
}

int
PyFile_WriteString(const char *s, PyObject *f)
{
	if (f == NULL) {
		/* Should be caused by a pre-existing error */
		if (!PyErr_Occurred())
			PyErr_SetString(PyExc_SystemError,
					"null file for PyFile_WriteString");
		return -1;
	}
	else if (PyFile_Check(f)) {
		FILE *fp = PyFile_AsFile(f);
		if (fp == NULL) {
			err_closed();
			return -1;
		}
		fputs(s, fp);
		return 0;
	}
	else if (!PyErr_Occurred()) {
		PyObject *v = PyString_FromString(s);
		int err;
		if (v == NULL)
			return -1;
		err = PyFile_WriteObject(v, f, Py_PRINT_RAW);
		Py_DECREF(v);
		return err;
	}
	else
		return -1;
}

/* Try to get a file-descriptor from a Python object.  If the object
   is an integer or long integer, its value is returned.  If not, the
   object's fileno() method is called if it exists; the method must return
   an integer or long integer, which is returned as the file descriptor value.
   -1 is returned on failure.
*/

int PyObject_AsFileDescriptor(PyObject *o)
{
	int fd;
	PyObject *meth;

	if (PyInt_Check(o)) {
		fd = PyInt_AsLong(o);
	}
	else if (PyLong_Check(o)) {
		fd = PyLong_AsLong(o);
	}
	else if ((meth = PyObject_GetAttrString(o, "fileno")) != NULL)
	{
		PyObject *fno = PyEval_CallObject(meth, NULL);
		Py_DECREF(meth);
		if (fno == NULL)
			return -1;

		if (PyInt_Check(fno)) {
			fd = PyInt_AsLong(fno);
			Py_DECREF(fno);
		}
		else if (PyLong_Check(fno)) {
			fd = PyLong_AsLong(fno);
			Py_DECREF(fno);
		}
		else {
			PyErr_SetString(PyExc_TypeError,
					"fileno() returned a non-integer");
			Py_DECREF(fno);
			return -1;
		}
	}
	else {
		PyErr_SetString(PyExc_TypeError,
				"argument must be an int, or have a fileno() method.");
		return -1;
	}

	if (fd < 0) {
		PyErr_Format(PyExc_ValueError,
			     "file descriptor cannot be a negative integer (%i)",
			     fd);
		return -1;
	}
	return fd;
}

#ifdef WITH_UNIVERSAL_NEWLINES
/* From here on we need access to the real fgets and fread */
#undef fgets
#undef fread

/*
** Py_UniversalNewlineFgets is an fgets variation that understands
** all of \r, \n and \r\n conventions.
** The stream should be opened in binary mode.
** If fobj is NULL the routine always does newline conversion, and
** it may peek one char ahead to gobble the second char in \r\n.
** If fobj is non-NULL it must be a PyFileObject. In this case there
** is no readahead but in stead a flag is used to skip a following
** \n on the next read. Also, if the file is open in binary mode
** the whole conversion is skipped. Finally, the routine keeps track of
** the different types of newlines seen.
** Note that we need no error handling: fgets() treats error and eof
** identically.
*/
char *
Py_UniversalNewlineFgets(char *buf, int n, FILE *stream, PyObject *fobj)
{
	char *p = buf;
	int c;
	int newlinetypes = 0;
	int skipnextlf = 0;
	int univ_newline = 1;

	if (fobj) {
		if (!PyFile_Check(fobj)) {
			errno = ENXIO;	/* What can you do... */
			return NULL;
		}
		univ_newline = ((PyFileObject *)fobj)->f_univ_newline;
		if ( !univ_newline )
			return fgets(buf, n, stream);
		newlinetypes = ((PyFileObject *)fobj)->f_newlinetypes;
		skipnextlf = ((PyFileObject *)fobj)->f_skipnextlf;
	}
	FLOCKFILE(stream);
	c = 'x'; /* Shut up gcc warning */
	while (--n > 0 && (c = GETC(stream)) != EOF ) {
		if (skipnextlf ) {
			skipnextlf = 0;
			if (c == '\n') {
				/* Seeing a \n here with skipnextlf true
				** means we saw a \r before.
				*/
				newlinetypes |= NEWLINE_CRLF;
				c = GETC(stream);
				if (c == EOF) break;
			} else {
				/*
				** Note that c == EOF also brings us here,
				** so we're okay if the last char in the file
				** is a CR.
				*/
				newlinetypes |= NEWLINE_CR;
			}
		}
		if (c == '\r') {
			/* A \r is translated into a \n, and we skip
			** an adjacent \n, if any. We don't set the
			** newlinetypes flag until we've seen the next char.
			*/
			skipnextlf = 1;
			c = '\n';
		} else if ( c == '\n') {
			newlinetypes |= NEWLINE_LF;
		}
		*p++ = c;
		if (c == '\n') break;
	}
	if ( c == EOF && skipnextlf )
		newlinetypes |= NEWLINE_CR;
	FUNLOCKFILE(stream);
	*p = '\0';
	if (fobj) {
		((PyFileObject *)fobj)->f_newlinetypes = newlinetypes;
		((PyFileObject *)fobj)->f_skipnextlf = skipnextlf;
	} else if ( skipnextlf ) {
		/* If we have no file object we cannot save the
		** skipnextlf flag. We have to readahead, which
		** will cause a pause if we're reading from an
		** interactive stream, but that is very unlikely
		** unless we're doing something silly like
		** execfile("/dev/tty").
		*/
		c = GETC(stream);
		if ( c != '\n' )
			ungetc(c, stream);
	}
	if (p == buf)
		return NULL;
	return buf;
}

/*
** Py_UniversalNewlineFread is an fread variation that understands
** all of \r, \n and \r\n conventions.
** The stream should be opened in binary mode.
** fobj must be a PyFileObject. In this case there
** is no readahead but in stead a flag is used to skip a following
** \n on the next read. Also, if the file is open in binary mode
** the whole conversion is skipped. Finally, the routine keeps track of
** the different types of newlines seen.
*/
size_t
Py_UniversalNewlineFread(char *buf, size_t n,
			 FILE *stream, PyObject *fobj)
{
	char *dst = buf;
	PyFileObject *f = (PyFileObject *)fobj;
	int newlinetypes, skipnextlf;

	assert(buf != NULL);
	assert(stream != NULL);

	if (!fobj || !PyFile_Check(fobj)) {
		errno = ENXIO;	/* What can you do... */
		return 0;
	}
	if (!f->f_univ_newline)
		return fread(buf, 1, n, stream);
	newlinetypes = f->f_newlinetypes;
	skipnextlf = f->f_skipnextlf;
	/* Invariant:  n is the number of bytes remaining to be filled
	 * in the buffer.
	 */
	while (n) {
		size_t nread;
		int shortread;
		char *src = dst;

		nread = fread(dst, 1, n, stream);
		assert(nread <= n);
		if (nread == 0)
			break;

		n -= nread; /* assuming 1 byte out for each in; will adjust */
		shortread = n != 0;	/* true iff EOF or error */
		while (nread--) {
			char c = *src++;
			if (c == '\r') {
				/* Save as LF and set flag to skip next LF. */
				*dst++ = '\n';
				skipnextlf = 1;
			}
			else if (skipnextlf && c == '\n') {
				/* Skip LF, and remember we saw CR LF. */
				skipnextlf = 0;
				newlinetypes |= NEWLINE_CRLF;
				++n;
			}
			else {
				/* Normal char to be stored in buffer.  Also
				 * update the newlinetypes flag if either this
				 * is an LF or the previous char was a CR.
				 */
				if (c == '\n')
					newlinetypes |= NEWLINE_LF;
				else if (skipnextlf)
					newlinetypes |= NEWLINE_CR;
				*dst++ = c;
				skipnextlf = 0;
			}
		}
		if (shortread) {
			/* If this is EOF, update type flags. */
			if (skipnextlf && feof(stream))
				newlinetypes |= NEWLINE_CR;
			break;
		}
	}
	f->f_newlinetypes = newlinetypes;
	f->f_skipnextlf = skipnextlf;
	return dst - buf;
}
#endif
