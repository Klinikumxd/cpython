/**********************************************************
Copyright 1991-1995 by Stichting Mathematisch Centrum, Amsterdam,
The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI or Corporation for National Research Initiatives or
CNRI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

While CWI is the initial source for this software, a modified version
is made available by the Corporation for National Research Initiatives
(CNRI) at the Internet address ftp://ftp.python.org.

STICHTING MATHEMATISCH CENTRUM AND CNRI DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH
CENTRUM OR CNRI BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/* AL module -- interface to Mark Callow's Audio Library (AL). */

#include <audio.h>

/* Check which version audio library we have: */
#ifdef AL_ERROR_NUMBER
#define AL_405
/* XXXX 4.0.5 libaudio also allows us to provide better error
** handling (with ALseterrorhandler). We should implement that
** sometime.
*/

#endif

#include "allobjects.h"
#include "import.h"
#include "modsupport.h"
#include "structmember.h"
#include "ceval.h"


/* Config objects */

typedef struct {
	OB_HEAD
	ALconfig ob_config;
} configobject;

staticforward typeobject Configtype;

#define is_configobject(v) ((v)->ob_type == &Configtype)

/* Forward */
static int getconfigarg PROTO((object *, ALconfig *));
static int getstrstrconfigarg PROTO((object *, char **, char **, ALconfig *));

static object *
setConfig (self, args, func)
	configobject *self;
	object *args;
	void (*func)(ALconfig, long);
{
	long par;

	if (!getlongarg (args, &par)) return NULL;

	(*func) (self-> ob_config, par);

	INCREF (None);
	return None;
}

static object *
getConfig (self, args, func)
	configobject *self;
	object *args;
	long (*func)(ALconfig);
{	
	long par;

	if (!getnoarg (args)) return NULL;
	
	par = (*func) (self-> ob_config);

	return newintobject (par);
}

static object *
al_setqueuesize (self, args)
	configobject *self;
	object *args;
{
	return (setConfig (self, args, ALsetqueuesize));
}

static object *
al_getqueuesize (self, args)
	configobject *self;
	object *args;
{
	return (getConfig (self, args, ALgetqueuesize));
}

static object *
al_setwidth (self, args)
	configobject *self;
	object *args;
{
	return (setConfig (self, args, ALsetwidth));
}

static object *
al_getwidth (self, args)
	configobject *self;
	object *args;
{
	return (getConfig (self, args, ALgetwidth));	
}

static object *
al_getchannels (self, args)
	configobject *self;
	object *args;
{
	return (getConfig (self, args, ALgetchannels));	
}

static object *
al_setchannels (self, args)
	configobject *self;
	object *args;
{
	return (setConfig (self, args, ALsetchannels));
}

#ifdef AL_405

static object *
al_getsampfmt (self, args)
	configobject *self;
	object *args;
{
	return (getConfig (self, args, ALgetsampfmt));	
}

static object *
al_setsampfmt (self, args)
	configobject *self;
	object *args;
{
	return (setConfig (self, args, ALsetsampfmt));
}

static object *
al_getfloatmax(self, args)
	configobject *self;
	object *args;
{
	double arg;

	if ( !getnoarg(args) )
	  return 0;
	arg = ALgetfloatmax(self->ob_config);
	return newfloatobject(arg);
}

static object *
al_setfloatmax(self, args)
	configobject *self;
	object *args;
{
	double arg;

	if ( !getargs(args, "d", &arg) )
	  return 0;
	ALsetfloatmax(self->ob_config, arg);
	INCREF(None);
	return None;
}
#endif /* AL_405 */
	
static struct methodlist config_methods[] = {
	{"getqueuesize",	(method)al_getqueuesize},
	{"setqueuesize",	(method)al_setqueuesize},
	{"getwidth",		(method)al_getwidth},
	{"setwidth",		(method)al_setwidth},
	{"getchannels",		(method)al_getchannels},
	{"setchannels",		(method)al_setchannels},
#ifdef AL_405
	{"getsampfmt",		(method)al_getsampfmt},
	{"setsampfmt",		(method)al_setsampfmt},
	{"getfloatmax",		(method)al_getfloatmax},
	{"setfloatmax",		(method)al_setfloatmax},
#endif /* AL_405 */
	{NULL,			NULL}		/* sentinel */
};

static void
config_dealloc(self)
	configobject *self;
{
	ALfreeconfig(self->ob_config);
	DEL(self);
}

static object *
config_getattr(self, name)
	configobject *self;
	char *name;
{
	return findmethod(config_methods, (object *)self, name);
}

static typeobject Configtype = {
	OB_HEAD_INIT(&Typetype)
	0,			/*ob_size*/
	"config",		/*tp_name*/
	sizeof(configobject),	/*tp_size*/
	0,			/*tp_itemsize*/
	/* methods */
	(destructor)config_dealloc, /*tp_dealloc*/
	0,			/*tp_print*/
	(getattrfunc)config_getattr, /*tp_getattr*/
	0,			/*tp_setattr*/
	0,			/*tp_compare*/
	0,			/*tp_repr*/
};

static object *
newconfigobject(config)
	ALconfig config;
{
	configobject *p;
	
	p = NEWOBJ(configobject, &Configtype);
	if (p == NULL)
		return NULL;
	p->ob_config = config;
	return (object *)p;
}

/* Port objects */

typedef struct {
	OB_HEAD
	ALport ob_port;
} portobject;

staticforward typeobject Porttype;

#define is_portobject(v) ((v)->ob_type == &Porttype)

static object *
al_closeport (self, args)
	portobject *self;
	object *args;
{
	if (!getnoarg (args)) return NULL;

	if (self->ob_port != NULL) {
		ALcloseport (self-> ob_port);
		self->ob_port = NULL;
		/* XXX Using a closed port may dump core! */
	}

	INCREF (None);
	return None;
}

static object *
al_getfd (self, args)
	portobject *self;
	object *args;
{
	int fd;

	if (!getnoarg (args)) return NULL;

	fd = ALgetfd (self-> ob_port);

	return newintobject (fd);
}

static object *
al_getfilled (self, args)
	portobject *self;
	object *args;
{
	long count;

	if (!getnoarg (args)) return NULL;
	
	count = ALgetfilled (self-> ob_port);

	return newintobject (count);
}

static object *
al_getfillable (self, args)
	portobject *self;
	object *args;
{
	long count;

	if (!getnoarg (args)) return NULL;
	
	count = ALgetfillable (self-> ob_port);

	return newintobject (count);
}

static object *
al_readsamps (self, args)
	portobject *self;
	object *args;
{
	long count;
	object *v;
	ALconfig c;
	int width;

	if (!getlongarg (args, &count)) return NULL;

	if (count <= 0)
	{
		err_setstr (RuntimeError, "al.readsamps : arg <= 0");
		return NULL;
	}

	c = ALgetconfig(self->ob_port);
#ifdef AL_405
	width = ALgetsampfmt(c);
	if ( width == AL_SAMPFMT_FLOAT )
	  width = sizeof(float);
	else if ( width == AL_SAMPFMT_DOUBLE )
	  width = sizeof(double);
	else
	  width = ALgetwidth(c);
#else
	width = ALgetwidth(c);
#endif /* AL_405 */
	ALfreeconfig(c);
	v = newsizedstringobject ((char *)NULL, width * count);
	if (v == NULL) return NULL;

	BGN_SAVE
	ALreadsamps (self-> ob_port, (void *) getstringvalue(v), count);
	END_SAVE

	return (v);
}

static object *
al_writesamps (self, args)
	portobject *self;
	object *args;
{
	char *buf;
	int size, width;
	ALconfig c;

	if (!getargs (args, "s#", &buf, &size)) return NULL;

	c = ALgetconfig(self->ob_port);
#ifdef AL_405
	width = ALgetsampfmt(c);
	if ( width == AL_SAMPFMT_FLOAT )
	  width = sizeof(float);
	else if ( width == AL_SAMPFMT_DOUBLE )
	  width = sizeof(double);
	else
	  width = ALgetwidth(c);
#else
	width = ALgetwidth(c);
#endif /* AL_405 */
	ALfreeconfig(c);
	BGN_SAVE
	ALwritesamps (self-> ob_port, (void *) buf, (long) size / width);
	END_SAVE

	INCREF (None);
	return None;
}

static object *
al_getfillpoint (self, args)
	portobject *self;
	object *args;
{
	long count;

	if (!getnoarg (args)) return NULL;
	
	count = ALgetfillpoint (self-> ob_port);

	return newintobject (count);
}

static object *
al_setfillpoint (self, args)
	portobject *self;
	object *args;
{
	long count;

	if (!getlongarg (args, &count)) return NULL;
	
	ALsetfillpoint (self-> ob_port, count);

	INCREF (None);
	return (None);
}

static object *
al_setconfig (self, args)
	portobject *self;
	object *args;
{
	ALconfig config;

	if (!getconfigarg (args, &config)) return NULL;
	
	ALsetconfig (self-> ob_port, config);

	INCREF (None);
	return (None);
}

static object *
al_getconfig (self, args)
	portobject *self;
	object *args;
{
	ALconfig config;

	if (!getnoarg (args)) return NULL;
	
	config = ALgetconfig (self-> ob_port);

	return newconfigobject (config);
}

#ifdef AL_405
static object *
al_getstatus (self, args)
	portobject *self;
	object *args;
{
	object *list, *v;
	long *PVbuffer;
	long length;
	int i;
	
	if (!getargs(args, "O", &list))
		return NULL;
	if (!is_listobject(list)) {
		err_badarg();
		return NULL;
	}
	length = getlistsize(list);
	PVbuffer = NEW(long, length);
	if (PVbuffer == NULL)
		return err_nomem();
	for (i = 0; i < length; i++) {
		v = getlistitem(list, i);
		if (!is_intobject(v)) {
			DEL(PVbuffer);
			err_badarg();
			return NULL;
		}
		PVbuffer[i] = getintvalue(v);
	}

	ALgetstatus(self->ob_port, PVbuffer, length);

	for (i = 0; i < length; i++)
	  setlistitem(list, i, newintobject(PVbuffer[i]));

	DEL(PVbuffer);

	INCREF(None);
	return None;
}
#endif /* AL_405 */

static struct methodlist port_methods[] = {
	{"closeport",		(method)al_closeport},
	{"getfd",		(method)al_getfd},
        {"fileno",		(method)al_getfd},
	{"getfilled",		(method)al_getfilled},
	{"getfillable",		(method)al_getfillable},
	{"readsamps",		(method)al_readsamps},
	{"writesamps",		(method)al_writesamps},
	{"setfillpoint",	(method)al_setfillpoint},
	{"getfillpoint",	(method)al_getfillpoint},
	{"setconfig",		(method)al_setconfig},
	{"getconfig",		(method)al_getconfig},
#ifdef AL_405
	{"getstatus",		(method)al_getstatus},
#endif /* AL_405 */	    
	{NULL,			NULL}		/* sentinel */
};

static void
port_dealloc(p)
	portobject *p;
{
	if (p->ob_port != NULL)
		ALcloseport(p->ob_port);
	DEL(p);
}

static object *
port_getattr(p, name)
	portobject *p;
	char *name;
{
	return findmethod(port_methods, (object *)p, name);
}

static typeobject Porttype = {
	OB_HEAD_INIT(&Typetype)
	0,			/*ob_size*/
	"port",			/*tp_name*/
	sizeof(portobject),	/*tp_size*/
	0,			/*tp_itemsize*/
	/* methods */
	(destructor)port_dealloc, /*tp_dealloc*/
	0,			/*tp_print*/
	(getattrfunc)port_getattr, /*tp_getattr*/
	0,			/*tp_setattr*/
	0,			/*tp_compare*/
	0,			/*tp_repr*/
};

static object *
newportobject(port)
	ALport port;
{
	portobject *p;
	
	p = NEWOBJ(portobject, &Porttype);
	if (p == NULL)
		return NULL;
	p->ob_port = port;
	return (object *)p;
}

/* the module al */

static object *
al_openport (self, args)
	object *self, *args;
{
	char *name, *dir;
	ALport port;
	ALconfig config = NULL;
	int size;

	if (args == NULL || !is_tupleobject(args)) {
		err_badarg ();
		return NULL;
	}
	size = gettuplesize(args);
	if (size == 2) {
		if (!getargs (args, "(ss)", &name, &dir))
			return NULL;
	}
	else if (size == 3) {
		if (!getstrstrconfigarg (args, &name, &dir, &config))
			return NULL;
	}
	else {
		err_badarg ();
		return NULL;
	}

	port = ALopenport(name, dir, config);

	if (port == NULL) {
		err_errno(RuntimeError);
		return NULL;
	}

	return newportobject (port);
}

static object *
al_newconfig (self, args)
	object *self, *args;
{
	ALconfig config;

	if (!getnoarg (args)) return NULL;

	config = ALnewconfig ();
	if (config == NULL) {
		err_errno(RuntimeError);
		return NULL;
	}

	return newconfigobject (config);
}

static object *
al_queryparams(self, args)
	object *self, *args;
{
	long device;
	long length;
	long *PVbuffer;
	long PVdummy[2];
	object *v;

	if (!getlongarg (args, &device))
		return NULL;
	length = ALqueryparams(device, PVdummy, 2L);
	PVbuffer = NEW(long, length);
	if (PVbuffer == NULL)
		return err_nomem();
	(void) ALqueryparams(device, PVbuffer, length);
	v = newlistobject((int)length);
	if (v != NULL) {
		int i;
		for (i = 0; i < length; i++)
			setlistitem(v, i, newintobject(PVbuffer[i]));
	}
	DEL(PVbuffer);
	return v;
}

static object *
doParams(args, func, modified)
	object *args;
	void (*func)(long, long *, long);
	int modified;
{
	long device;
	object *list, *v;
	long *PVbuffer;
	long length;
	int i;
	
	if (!getargs(args, "(lO)", &device, &list))
		return NULL;
	if (!is_listobject(list)) {
		err_badarg();
		return NULL;
	}
	length = getlistsize(list);
	PVbuffer = NEW(long, length);
	if (PVbuffer == NULL)
		return err_nomem();
	for (i = 0; i < length; i++) {
		v = getlistitem(list, i);
		if (!is_intobject(v)) {
			DEL(PVbuffer);
			err_badarg();
			return NULL;
		}
		PVbuffer[i] = getintvalue(v);
	}

	(*func)(device, PVbuffer, length);

	if (modified) {
		for (i = 0; i < length; i++)
			setlistitem(list, i, newintobject(PVbuffer[i]));
	}

	DEL(PVbuffer);

	INCREF(None);
	return None;
}

static object *
al_getparams(self, args)
	object *self, *args;
{
	return doParams(args, ALgetparams, 1);
}

static object *
al_setparams(self, args)
	object *self, *args;
{
	return doParams(args, ALsetparams, 0);
}

static object *
al_getname(self, args)
	object *self, *args;
{
	long device, descriptor;
	char *name;
	if (!getargs(args, "(ll)", &device, &descriptor))
		return NULL;
	name = ALgetname(device, descriptor);
	if (name == NULL) {
		err_setstr(ValueError, "al.getname: bad descriptor");
		return NULL;
	}
	return newstringobject(name);
}

static object *
al_getdefault(self, args)
	object *self, *args;
{
	long device, descriptor, value;
	if (!getargs(args, "(ll)", &device, &descriptor))
		return NULL;
	value = ALgetdefault(device, descriptor);
	return newlongobject(value);
}

static object *
al_getminmax(self, args)
	object *self, *args;
{
	long device, descriptor, min, max;
	if (!getargs(args, "(ll)", &device, &descriptor))
		return NULL;
	min = -1;
	max = -1;
	ALgetminmax(device, descriptor, &min, &max);
	return mkvalue("ll", min, max);
}

static struct methodlist al_methods[] = {
	{"openport",		(method)al_openport},
	{"newconfig",		(method)al_newconfig},
	{"queryparams",		(method)al_queryparams},
	{"getparams",		(method)al_getparams},
	{"setparams",		(method)al_setparams},
	{"getname",		(method)al_getname},
	{"getdefault",		(method)al_getdefault},
	{"getminmax",		(method)al_getminmax},
	{NULL,			NULL}		/* sentinel */
};

void
inital()
{
	initmodule("al", al_methods);
}

static int
getconfigarg(o, conf)
	object *o;
	ALconfig *conf;
{
	if (o == NULL || !is_configobject(o))
		return err_badarg ();
	
	*conf = ((configobject *) o) -> ob_config;
	
	return 1;
}

static int
getstrstrconfigarg(v, a, b, c)
	object *v;
	char **a;
	char **b;
	ALconfig *c;
{
	object *o;
	return getargs(v, "(ssO)", a, b, &o) && getconfigarg(o, c);
}
