#ifndef Py_STRUCTMEMBER_H
#define Py_STRUCTMEMBER_H
#ifdef __cplusplus
extern "C" {
#endif


/* Interface to map C struct members to Python object attributes */

#ifdef HAVE_STDDEF_H
#include <stddef.h> /* For offsetof */
#endif

/* The offsetof() macro calculates the offset of a structure member
   in its structure.  Unfortunately this cannot be written down
   portably, hence it is provided by a Standard C header file.
   For pre-Standard C compilers, here is a version that usually works
   (but watch out!): */

#ifndef offsetof
#define offsetof(type, member) ( (int) & ((type*)0) -> member )
#endif

/* An array of memberlist structures defines the name, type and offset
   of selected members of a C structure.  These can be read by
   PyMember_Get() and set by PyMember_Set() (except if their READONLY flag
   is set).  The array must be terminated with an entry whose name
   pointer is NULL. */

struct memberlist {
	char *name;
	int type;
	int offset;
	int readonly;
};

/* Types */
#define T_SHORT		0
#define T_INT		1
#define T_LONG		2
#define T_FLOAT		3
#define T_DOUBLE	4
#define T_STRING	5
#define T_OBJECT	6
/* XXX the ordering here is weird for binary compatibility */
#define T_CHAR		7	/* 1-character string */
#define T_BYTE		8	/* 8-bit signed int */
/* unsigned variants: */
#define T_UBYTE		9
#define T_USHORT	10
#define T_UINT		11
#define T_ULONG		12

/* Added by Jack: strings contained in the structure */
#define T_STRING_INPLACE	13
#ifdef macintosh
#define T_PSTRING	14	/* macintosh pascal-style counted string */
#define T_PSTRING_INPLACE	15
#endif /* macintosh */

/* Readonly flag */
#define READONLY	1
#define RO		READONLY		/* Shorthand */

DL_IMPORT(PyObject *) PyMember_Get(char *, struct memberlist *, char *);
DL_IMPORT(int) PyMember_Set(char *, struct memberlist *, char *, PyObject *);

#ifdef __cplusplus
}
#endif
#endif /* !Py_STRUCTMEMBER_H */
