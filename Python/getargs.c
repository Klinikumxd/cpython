/***********************************************************
Copyright 1991-1995 by Stichting Mathematisch Centrum, Amsterdam,
The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.

STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/* New getargs implementation */

/* XXX There are several unchecked sprintf or strcat calls in this file.
   XXX The only way these can become a danger is if some C code in the
   XXX Python source (or in an extension) uses ridiculously long names
   XXX or riduculously deep nesting in format strings. */

#include "allobjects.h"


int getargs PROTO((object *, char *, ...));
int newgetargs PROTO((object *, char *, ...));
int vgetargs PROTO((object *, char *, va_list));

int PyArg_ParseTupleAndKeywords PROTO((object *, object *,
				       char *, char **, ...));

/* Forward */
static int vgetargs1 PROTO((object *, char *, va_list *, int));
static void seterror PROTO((int, char *, int *, char *, char *));
static char *convertitem PROTO((object *, char **, va_list *, int *, char *));
static char *converttuple PROTO((object *, char **, va_list *,
				 int *, char *, int));
static char *convertsimple PROTO((object *, char **, va_list *, char *));
static char *convertsimple1 PROTO((object *, char **, va_list *));

static int vgetargskeywords PROTO((object *, object *,
				   char *, char **, va_list *));
static char *skipitem PROTO((char **, va_list *));

#ifdef HAVE_STDARG_PROTOTYPES
/* VARARGS2 */
int getargs(object *args, char *format, ...)
#else
/* VARARGS */
int getargs(va_alist) va_dcl
#endif
{
	int retval;
	va_list va;
#ifdef HAVE_STDARG_PROTOTYPES
	
	va_start(va, format);
#else
	object *args;
	char *format;
	
	va_start(va);
	args = va_arg(va, object *);
	format = va_arg(va, char *);
#endif
	retval = vgetargs1(args, format, &va, 1);
	va_end(va);
	return retval;
}


#ifdef HAVE_STDARG_PROTOTYPES
/* VARARGS2 */
int newgetargs(object *args, char *format, ...)
#else
/* VARARGS */
int newgetargs(va_alist) va_dcl
#endif
{
	int retval;
	va_list va;
#ifdef HAVE_STDARG_PROTOTYPES
	
	va_start(va, format);
#else
	object *args;
	char *format;
	
	va_start(va);
	args = va_arg(va, object *);
	format = va_arg(va, char *);
#endif
	retval = vgetargs1(args, format, &va, 0);
	va_end(va);
	return retval;
}


int
vgetargs(args, format, va)
	object *args;
	char *format;
	va_list va;
{
	va_list lva;

#ifdef VA_LIST_IS_ARRAY
	memcpy(lva, va, sizeof(va_list));
#else
	lva = va;
#endif

	return vgetargs1(args, format, &lva, 0);
}


static int
vgetargs1(args, format, p_va, compat)
	object *args;
	char *format;
	va_list *p_va;
	int compat;
{
	char msgbuf[256];
	int levels[32];
	char *fname = NULL;
	char *message = NULL;
	int min = -1;
	int max = 0;
	int level = 0;
	char *formatsave = format;
	int i, len;
	char *msg;
	
	for (;;) {
		int c = *format++;
		if (c == '(' /* ')' */) {
			if (level == 0)
				max++;
			level++;
		}
		else if (/* '(' */ c == ')') {
			if (level == 0)
				fatal(/* '(' */
				      "excess ')' in getargs format");
			else
				level--;
		}
		else if (c == '\0')
			break;
		else if (c == ':') {
			fname = format;
			break;
		}
		else if (c == ';') {
			message = format;
			break;
		}
		else if (level != 0)
			; /* Pass */
		else if (isalpha(c))
			max++;
		else if (c == '|')
			min = max;
	}
	
	if (level != 0)
		fatal(/* '(' */ "missing ')' in getargs format");
	
	if (min < 0)
		min = max;
	
	format = formatsave;
	
	if (compat) {
		if (max == 0) {
			if (args == NULL)
				return 1;
			sprintf(msgbuf, "%s requires no arguments",
				fname==NULL ? "function" : fname);
			err_setstr(TypeError, msgbuf);
			return 0;
		}
		else if (min == 1 && max == 1) {
			if (args == NULL) {
				sprintf(msgbuf,
					"%s requires at least one argument",
					fname==NULL ? "function" : fname);
				err_setstr(TypeError, msgbuf);
				return 0;
			}
			msg = convertitem(args, &format, p_va, levels, msgbuf);
			if (msg == NULL)
				return 1;
			seterror(levels[0], msg, levels+1, fname, message);
			return 0;
		}
		else {
			err_setstr(SystemError,
			    "old style getargs format uses new features");
			return 0;
		}
	}
	
	if (!is_tupleobject(args)) {
		err_setstr(SystemError,
		    "new style getargs format but argument is not a tuple");
		return 0;
	}
	
	len = gettuplesize(args);
	
	if (len < min || max < len) {
		if (message == NULL) {
			sprintf(msgbuf,
				"%s requires %s %d argument%s; %d given",
				fname==NULL ? "function" : fname,
				min==max ? "exactly"
				         : len < min ? "at least" : "at most",
				len < min ? min : max,
				(len < min ? min : max) == 1 ? "" : "s",
				len);
			message = msgbuf;
		}
		err_setstr(TypeError, message);
		return 0;
	}
	
	for (i = 0; i < len; i++) {
		if (*format == '|')
			format++;
		msg = convertitem(gettupleitem(args, i), &format, p_va,
				 levels, msgbuf);
		if (msg) {
			seterror(i+1, msg, levels, fname, message);
			return 0;
		}
	}
	
	return 1;
}



static void
seterror(iarg, msg, levels, fname, message)
	int iarg;
	char *msg;
	int *levels;
	char *fname;
	char *message;
{
	char buf[256];
	int i;
	char *p = buf;

	if (err_occurred())
		return;
	if (iarg == 0 && message == NULL)
		message = msg;
	else if (message == NULL) {
		if (fname != NULL) {
			sprintf(p, "%s, ", fname);
			p += strlen(p);
		}
		sprintf(p, "argument %d", iarg);
		i = 0;
		p += strlen(p);
		while (levels[i] > 0) {
			sprintf(p, ", item %d", levels[i]-1);
			p += strlen(p);
			i++;
		}
		sprintf(p, ": expected %s found", msg);
		message = buf;
	}
	err_setstr(TypeError, message);
}


/* Convert a tuple argument.
   On entry, *p_format points to the character _after_ the opening '('.
   On successful exit, *p_format points to the closing ')'.
   If successful:
      *p_format and *p_va are updated,
      *levels and *msgbuf are untouched,
      and NULL is returned.
   If the argument is invalid:
      *p_format is unchanged,
      *p_va is undefined,
      *levels is a 0-terminated list of item numbers,
      *msgbuf contains an error message, whose format is:
         "<typename1>, <typename2>", where:
            <typename1> is the name of the expected type, and
            <typename2> is the name of the actual type,
         (so you can surround it by "expected ... found"),
      and msgbuf is returned.
*/

static char *
converttuple(arg, p_format, p_va, levels, msgbuf, toplevel)
	object *arg;
	char **p_format;
	va_list *p_va;
	int *levels;
	char *msgbuf;
	int toplevel;
{
	int level = 0;
	int n = 0;
	char *format = *p_format;
	int i;
	
	for (;;) {
		int c = *format++;
		if (c == '(') {
			if (level == 0)
				n++;
			level++;
		}
		else if (c == ')') {
			if (level == 0)
				break;
			level--;
		}
		else if (c == ':' || c == ';' || c == '\0')
			break;
		else if (level == 0 && isalpha(c))
			n++;
	}
	
	if (!is_tupleobject(arg)) {
		levels[0] = 0;
		sprintf(msgbuf,
			toplevel ? "%d arguments, %s" : "%d-tuple, %s",
			n, arg == None ? "None" : arg->ob_type->tp_name);
		return msgbuf;
	}
	
	if ((i = gettuplesize(arg)) != n) {
		levels[0] = 0;
		sprintf(msgbuf,
			toplevel ? "%d arguments, %d" : "%d-tuple, %d-tuple",
			n, i);
		return msgbuf;
	}
	
	format = *p_format;
	for (i = 0; i < n; i++) {
		char *msg;
		msg = convertitem(gettupleitem(arg, i), &format, p_va,
				 levels+1, msgbuf);
		if (msg != NULL) {
			levels[0] = i+1;
			return msg;
		}
	}
	
	*p_format = format;
	return NULL;
}


/* Convert a single item. */

static char *
convertitem(arg, p_format, p_va, levels, msgbuf)
	object *arg;
	char **p_format;
	va_list *p_va;
	int *levels;
	char *msgbuf;
{
	char *msg;
	char *format = *p_format;
	
	if (*format == '(' /* ')' */) {
		format++;
		msg = converttuple(arg, &format, p_va, levels, msgbuf, 0);
		if (msg == NULL)
			format++;
	}
	else {
		msg = convertsimple(arg, &format, p_va, msgbuf);
		if (msg != NULL)
			levels[0] = 0;
	}
	if (msg == NULL)
		*p_format = format;
	return msg;
}


/* Convert a non-tuple argument.  Adds to convertsimple1 functionality
   by appending ", <actual argument type>" to error message. */

static char *
convertsimple(arg, p_format, p_va, msgbuf)
	object *arg;
	char **p_format;
	va_list *p_va;
	char *msgbuf;
{
	char *msg = convertsimple1(arg, p_format, p_va);
	if (msg != NULL) {
		sprintf(msgbuf, "%.50s, %.50s", msg,
			arg == None ? "None" : arg->ob_type->tp_name);
		msg = msgbuf;
	}
	return msg;
}


/* Convert a non-tuple argument.  Return NULL if conversion went OK,
   or a string representing the expected type if the conversion failed.
   When failing, an exception may or may not have been raised.
   Don't call if a tuple is expected. */

static char *
convertsimple1(arg, p_format, p_va)
	object *arg;
	char **p_format;
	va_list *p_va;
{
	char *format = *p_format;
	char c = *format++;
	
	switch (c) {
	
	case 'b': /* byte -- very short int */
		{
			char *p = va_arg(*p_va, char *);
			long ival = getintvalue(arg);
			if (ival == -1 && err_occurred())
				return "integer<b>";
			else
				*p = ival;
			break;
		}
	
	case 'h': /* short int */
		{
			short *p = va_arg(*p_va, short *);
			long ival = getintvalue(arg);
			if (ival == -1 && err_occurred())
				return "integer<h>";
			else
				*p = ival;
			break;
		}
	
	case 'i': /* int */
		{
			int *p = va_arg(*p_va, int *);
			long ival = getintvalue(arg);
			if (ival == -1 && err_occurred())
				return "integer<i>";
			else
				*p = ival;
			break;
		}
	
	case 'l': /* long int */
		{
			long *p = va_arg(*p_va, long *);
			long ival = getintvalue(arg);
			if (ival == -1 && err_occurred())
				return "integer<l>";
			else
				*p = ival;
			break;
		}
	
	case 'f': /* float */
		{
			float *p = va_arg(*p_va, float *);
			double dval = getfloatvalue(arg);
			if (err_occurred())
				return "float<f>";
			else
				*p = dval;
			break;
		}
	
	case 'd': /* double */
		{
			double *p = va_arg(*p_va, double *);
			double dval = getfloatvalue(arg);
			if (err_occurred())
				return "float<d>";
			else
				*p = dval;
			break;
		}
	
#ifndef WITHOUT_COMPLEX
	case 'D': /* complex double */
		{
			Py_complex *p = va_arg(*p_va, Py_complex *);
			Py_complex cval;
			cval = PyComplex_AsCComplex(arg);
			if (err_occurred())
				return "complex<D>";
			else
				*p = cval;
			break;
		}
#endif /* WITHOUT_COMPLEX */
	
	case 'c': /* char */
		{
			char *p = va_arg(*p_va, char *);
			if (is_stringobject(arg) && getstringsize(arg) == 1)
				*p = getstringvalue(arg)[0];
			else
				return "char";
			break;
		}
	
	case 's': /* string */
		{
			char **p = va_arg(*p_va, char **);
			if (is_stringobject(arg))
				*p = getstringvalue(arg);
			else
				return "string";
			if (*format == '#') {
				int *q = va_arg(*p_va, int *);
				*q = getstringsize(arg);
				format++;
			}
			else if (strlen(*p) != getstringsize(arg))
				return "string without null bytes";
			break;
		}
	
	case 'z': /* string, may be NULL (None) */
		{
			char **p = va_arg(*p_va, char **);
			if (arg == None)
				*p = 0;
			else if (is_stringobject(arg))
				*p = getstringvalue(arg);
			else
				return "None or string";
			if (*format == '#') {
				int *q = va_arg(*p_va, int *);
				if (arg == None)
					*q = 0;
				else
					*q = getstringsize(arg);
				format++;
			}
			else if (*p != NULL &&
				 strlen(*p) != getstringsize(arg))
				return "None or string without null bytes";
			break;
		}
	
	case 'S': /* string object */
		{
			object **p = va_arg(*p_va, object **);
			if (is_stringobject(arg))
				*p = arg;
			else
				return "string";
			break;
		}
	
	case 'O': /* object */
		{
			typeobject *type;
			object **p;
			if (*format == '!') {
				format++;
				type = va_arg(*p_va, typeobject*);
				if (arg->ob_type != type)
					return type->tp_name;
				else {
					p = va_arg(*p_va, object **);
					*p = arg;
				}
			}
			else if (*format == '?') {
				inquiry pred = va_arg(*p_va, inquiry);
				format++;
				if ((*pred)(arg)) {
					p = va_arg(*p_va, object **);
					*p = arg;
				}
			}
			else if (*format == '&') {
				typedef int (*converter)
					PROTO((object *, void *));
				converter convert = va_arg(*p_va, converter);
				void *addr = va_arg(*p_va, void *);
				format++;
				if (! (*convert)(arg, addr))
					return "(unspecified)";
			}
			else {
				p = va_arg(*p_va, object **);
				*p = arg;
			}
			break;
		}
	
	default:
		return "impossible<bad format char>";
	
	}
	
	*p_format = format;
	return NULL;
}


/* Support for keyword arguments donated by
   Geoff Philbrick <philbric@delphi.hks.com> */

#ifdef HAVE_STDARG_PROTOTYPES
/* VARARGS2 */
int PyArg_ParseTupleAndKeywords(object *args,
				object *keywords,
				char *format, 
				char **kwlist, ...)
#else
/* VARARGS */
int PyArg_ParseTupleAndKeywords(va_alist) va_dcl
#endif
{
	int retval;
	va_list va;
#ifdef HAVE_STDARG_PROTOTYPES
	
	va_start(va, kwlist);
#else
	object *args;
	object *keywords;
	char *format;
	char **kwlist;
	
	va_start(va);
	args = va_arg(va, object *);
	keywords = va_arg(va, object *);
	format = va_arg(va, char *);
	kwlist = va_arg(va, char **);
#endif
	retval = vgetargskeywords(args, keywords, format, kwlist, &va);	
	va_end(va);
	return retval;
}


static int
vgetargskeywords(args, keywords, format, kwlist, p_va)
	object *args;
	object *keywords;
	char *format;
	char **kwlist;
	va_list *p_va;
{
	char msgbuf[256];
	int levels[32];
	char *fname = NULL;
	char *message = NULL;
	int min = -1;
	int max = 0;
	int level = 0;
	char *formatsave = format;
	int i, len, tplen, kwlen;
	char *msg, *ks, **p;
	int nkwds, pos, match, converted;
	object *key, *value, *item;
	
	/* nested tuples cannot be parsed when using keyword arguments */
	
	for (;;) {
		int c = *format++;
		if (c == '(') {
			PyErr_SetString(PyExc_SystemError,
		      "tuple found in format when using keyword arguments");
			return 0;
		}
		else if (c == '\0')
			break;
		else if (c == ':') {
			fname = format;
			break;
		}
		else if (c == ';') {
			message = format;
			break;
		}
		else if (isalpha(c))
			max++;
		else if (c == '|')
			min = max;
	}	
	
	if (min < 0)
		min = max;
	
	format = formatsave;
	
	if (!PyTuple_Check(args)) {
		PyErr_SetString(PyExc_SystemError,
		    "new style getargs format but argument is not a tuple");
		return 0;
	}	
	
	tplen = PyTuple_Size(args);
	
	/* do a cursory check of the keywords just to see how many we got */
	   
	if (keywords) { 	
		if (!PyDict_Check(keywords)) {
			PyErr_SetString(PyExc_SystemError,
	  "non-dictionary object received when keyword dictionary expected");
			return 0;
		}	
		kwlen = PyDict_Size(keywords);
	}
	else {
		kwlen = 0;
	}
			
	/* make sure there are no duplicate values for an argument;
	   its not clear when to use the term "keyword argument vs. 
	   keyword parameter in messages */
	
	if (keywords) {
		for (i = 0; i < tplen; i++) {
			if (PyMapping_HasKeyString(keywords, kwlist[i])) {
				sprintf(msgbuf,
					"keyword parameter %s redefined",
					kwlist[i]);
				PyErr_SetString(PyExc_TypeError, msgbuf);
				return 0;
			}
		}
	}
	PyErr_Clear(); /* I'm not which Py functions set the error string */
		
	/* required arguments missing from args can be supplied by keyword 
	   arguments */
	
	len = tplen;
	if (keywords && tplen < min) {
		for (i = tplen; i < min; i++) {
		  if (PyMapping_HasKeyString(keywords, kwlist[i])) {
				len++;
		  }
		}
	}
	PyErr_Clear();	
	
	/* make sure we got an acceptable number of arguments; the message
	   is a little confusing with keywords since keyword arguments
	   which are supplied, but don't match the required arguments
	   are not included in the "%d given" part of the message */

	if (len < min || max < len) {
		if (message == NULL) {
			sprintf(msgbuf,
				"%s requires %s %d argument%s; %d given",
				fname==NULL ? "function" : fname,
				min==max ? "exactly"
				         : len < min ? "at least" : "at most",
				len < min ? min : max,
				(len < min ? min : max) == 1 ? "" : "s",
				len);
			message = msgbuf;
		}
		PyErr_SetString(PyExc_TypeError, message);
		return 0;
	}
	
	for (i = 0; i < tplen; i++) {
		if (*format == '|')
			format++;
		msg = convertitem(PyTuple_GetItem(args, i), &format, p_va,
				 levels, msgbuf);
		if (msg) {
			seterror(i+1, msg, levels, fname, message);
			return 0;
		}
	}

	/* handle no keyword parameters in call  */	
	   	   
	if (!keywords) return 1; 
		
	/* make sure the number of keywords in the keyword list matches the 
	   number of items in the format string */
	  
	nkwds = 0;
	p =  kwlist;
	for (;;) {
		if (!*(p++)) break;
		nkwds++;
	}

	if (nkwds != max) {
		PyErr_SetString(PyExc_SystemError,
	  "number of items in format string and keyword list do not match");
		return 0;
	}	  	  
			
	/* convert the keyword arguments; this uses the format 
	   string where it was left after processing args */
	
	converted = 0;
	for (i = tplen; i < nkwds; i++) {
		if (*format == '|')
			format++;
		if (item = PyMapping_GetItemString(keywords, kwlist[i])) {
			msg = convertitem(item, &format, p_va, levels, msgbuf);
			if (msg) {
				seterror(i+1, msg, levels, fname, message);
				return 0;
			}
			converted++;
		}
		else {
			PyErr_Clear();
			msg = skipitem(&format, p_va);
			if (msg) {
				seterror(i+1, msg, levels, fname, message);
				return 0;
			}
		}
	}
	
	/* make sure there are no extraneous keyword arguments */
	
	pos = 0;
	if (converted < kwlen) {
		while (PyDict_Next(keywords, &pos, &key, &value)) {
			match = 0;
			ks = PyString_AsString(key);
			for (i = 0; i < nkwds; i++) {
				if (!strcmp(ks, kwlist[i])) {
					match = 1;
					break;
				}
			}
			if (!match) {
				sprintf(msgbuf,
		         "%s is an invalid keyword argument for this function",
					ks);
				PyErr_SetString(PyExc_TypeError, msgbuf);
				return 0;
			}
		}
	}
	
	return 1;
}


static char *
skipitem(p_format, p_va)
	char **p_format;
	va_list *p_va;
{
	char *format = *p_format;
	char c = *format++;
	
	switch (c) {
	
	case 'b': /* byte -- very short int */
		{
			va_arg(*p_va, char *);
			break;
		}
	
	case 'h': /* short int */
		{
			va_arg(*p_va, short *);
			break;
		}
	
	case 'i': /* int */
		{
			va_arg(*p_va, int *);
			break;
		}
	
	case 'l': /* long int */
		{
			va_arg(*p_va, long *);
			break;
		}
	
	case 'f': /* float */
		{
			va_arg(*p_va, float *);
			break;
		}
	
	case 'd': /* double */
		{
			va_arg(*p_va, double *);
			break;
		}
	
#ifndef WITHOUT_COMPLEX
	case 'D': /* complex double */
		{
			va_arg(*p_va, Py_complex *);
			break;
		}
#endif /* WITHOUT_COMPLEX */
	
	case 'c': /* char */
		{
			va_arg(*p_va, char *);
			break;
		}
	
	case 's': /* string */
		{
			va_arg(*p_va, char **);
			if (*format == '#') {
				va_arg(*p_va, int *);
				format++;
			}
			break;
		}
	
	case 'z': /* string */
		{
			va_arg(*p_va, char **);
			if (*format == '#') {
				va_arg(*p_va, int *);
				format++;
			}
			break;
		}
	
	case 'S': /* string object */
		{
			va_arg(*p_va, object **);
			break;
		}
	
	case 'O': /* object */
		{
			typeobject *type;
			object **p;
			if (*format == '!') {
				format++;
				va_arg(*p_va, typeobject*);
				va_arg(*p_va, object **);
			}
#if 0
/* I don't know what this is for */
			else if (*format == '?') {
				inquiry pred = va_arg(*p_va, inquiry);
				format++;
				if ((*pred)(arg)) {
					va_arg(*p_va, object **);
				}
			}
#endif
			else if (*format == '&') {
				typedef int (*converter)
					PROTO((object *, void *));
				va_arg(*p_va, converter);
				va_arg(*p_va, void *);
				format++;
			}
			else {
				va_arg(*p_va, object **);
			}
			break;
		}
	
	default:
		return "impossible<bad format char>";
	
	}
	
	*p_format = format;
	return NULL;
}
