#ifndef Py_LONGOBJECT_H
#define Py_LONGOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
Copyright 1991, 1992, 1993, 1994 by Stichting Mathematisch Centrum,
Amsterdam, The Netherlands.

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

/* Long (arbitrary precision) integer object interface */

typedef struct _longobject longobject; /* Revealed in longintrepr.h */

extern DL_IMPORT typeobject Longtype;

#define is_longobject(op) ((op)->ob_type == &Longtype)

extern object *newlongobject PROTO((long));
extern object *dnewlongobject PROTO((double));
extern long getlongvalue PROTO((object *));
extern double dgetlongvalue PROTO((object *));

object *long_scan PROTO((char *, int));
object *long_escan PROTO((char *, char **, int));

#ifdef __cplusplus
}
#endif
#endif /* !Py_LONGOBJECT_H */
