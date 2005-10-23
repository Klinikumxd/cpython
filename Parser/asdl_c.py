#! /usr/bin/env python
"""Generate C code from an ASDL description."""

# TO DO
# handle fields that have a type but no name

import os, sys, traceback

import asdl

TABSIZE = 8
MAX_COL = 80

def get_c_type(name):
    """Return a string for the C name of the type.

    This function special cases the default types provided by asdl:
    identifier, string, int, bool.
    """
    # XXX ack!  need to figure out where Id is useful and where string
    if isinstance(name, asdl.Id):
        name = name.value
    if name in asdl.builtin_types:
        return name
    else:
        return "%s_ty" % name

def reflow_lines(s, depth):
    """Reflow the line s indented depth tabs.

    Return a sequence of lines where no line extends beyond MAX_COL
    when properly indented.  The first line is properly indented based
    exclusively on depth * TABSIZE.  All following lines -- these are
    the reflowed lines generated by this function -- start at the same
    column as the first character beyond the opening { in the first
    line.
    """
    size = MAX_COL - depth * TABSIZE
    if len(s) < size:
        return [s]

    lines = []
    cur = s
    padding = ""
    while len(cur) > size:
        i = cur.rfind(' ', 0, size)
        # XXX this should be fixed for real
        if i == -1 and 'GeneratorExp' in cur:
            i = size + 3
        assert i != -1, "Impossible line %d to reflow: %s" % (size, `s`)
        lines.append(padding + cur[:i])
        if len(lines) == 1:
            # find new size based on brace
            j = cur.find('{', 0, i)
            if j >= 0:
                j += 2 # account for the brace and the space after it
                size -= j
                padding = " " * j
            else:
                j = cur.find('(', 0, i)
                if j >= 0:
                    j += 1 # account for the paren (no space after it)
                    size -= j
                    padding = " " * j
        cur = cur[i+1:]
    else:
        lines.append(padding + cur)
    return lines

def is_simple(sum):
    """Return True if a sum is a simple.

    A sum is simple if its types have no fields, e.g.
    unaryop = Invert | Not | UAdd | USub
    """

    for t in sum.types:
        if t.fields:
            return False
    return True

class EmitVisitor(asdl.VisitorBase):
    """Visit that emits lines"""

    def __init__(self, file):
        self.file = file
        super(EmitVisitor, self).__init__()

    def emit(self, s, depth, reflow=1):
        # XXX reflow long lines?
        if reflow:
            lines = reflow_lines(s, depth)
        else:
            lines = [s]
        for line in lines:
            line = (" " * TABSIZE * depth) + line + "\n"
            self.file.write(line)

class TypeDefVisitor(EmitVisitor):
    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type, depth=0):
        self.visit(type.value, type.name, depth)

    def visitSum(self, sum, name, depth):
        if is_simple(sum):
            self.simple_sum(sum, name, depth)
        else:
            self.sum_with_constructors(sum, name, depth)

    def simple_sum(self, sum, name, depth):
        enum = []
        for i in range(len(sum.types)):
            type = sum.types[i]
            enum.append("%s=%d" % (type.name, i + 1))
        enums = ", ".join(enum)
        ctype = get_c_type(name)
        s = "typedef enum _%s { %s } %s;" % (name, enums, ctype)
        self.emit(s, depth)
        self.emit("", depth)

    def sum_with_constructors(self, sum, name, depth):
        ctype = get_c_type(name)
        s = "typedef struct _%(name)s *%(ctype)s;" % locals()
        self.emit(s, depth)
        self.emit("", depth)

    def visitProduct(self, product, name, depth):
        ctype = get_c_type(name)
        s = "typedef struct _%(name)s *%(ctype)s;" % locals()
        self.emit(s, depth)
        self.emit("", depth)

class StructVisitor(EmitVisitor):
    """Visitor to generate typdefs for AST."""

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type, depth=0):
        self.visit(type.value, type.name, depth)

    def visitSum(self, sum, name, depth):
        if not is_simple(sum):
            self.sum_with_constructors(sum, name, depth)

    def sum_with_constructors(self, sum, name, depth):
        def emit(s, depth=depth):
            self.emit(s % sys._getframe(1).f_locals, depth)
        enum = []
        for i in range(len(sum.types)):
            type = sum.types[i]
            enum.append("%s_kind=%d" % (type.name, i + 1))

        emit("struct _%(name)s {")
        emit("enum { " + ", ".join(enum) + " } kind;", depth + 1)
        emit("union {", depth + 1)
        for t in sum.types:
            self.visit(t, depth + 2)
        emit("} v;", depth + 1)
        for field in sum.attributes:
            # rudimentary attribute handling
            type = str(field.type)
            assert type in asdl.builtin_types, type
            emit("%s %s;" % (type, field.name), depth + 1);
        emit("};")
        emit("")

    def visitConstructor(self, cons, depth):
        if cons.fields:
            self.emit("struct {", depth)
            for f in cons.fields:
                self.visit(f, depth + 1)
            self.emit("} %s;" % cons.name, depth)
            self.emit("", depth)
        else:
            # XXX not sure what I want here, nothing is probably fine
            pass

    def visitField(self, field, depth):
        # XXX need to lookup field.type, because it might be something
        # like a builtin...
        ctype = get_c_type(field.type)
        name = field.name
        if field.seq:
            self.emit("asdl_seq *%(name)s;" % locals(), depth)
        else:
            self.emit("%(ctype)s %(name)s;" % locals(), depth)

    def visitProduct(self, product, name, depth):
        self.emit("struct _%(name)s {" % locals(), depth)
        for f in product.fields:
            self.visit(f, depth + 1)
        self.emit("};", depth)
        self.emit("", depth)

class PrototypeVisitor(EmitVisitor):
    """Generate function prototypes for the .h file"""

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, type.name)

    def visitSum(self, sum, name):
        if is_simple(sum):
            pass # XXX
        else:
            for t in sum.types:
                self.visit(t, name, sum.attributes)

    def get_args(self, fields):
        """Return list of C argument into, one for each field.

        Argument info is 3-tuple of a C type, variable name, and flag
        that is true if type can be NULL.
        """
        args = []
        unnamed = {}
        for f in fields:
            if f.name is None:
                name = f.type
                c = unnamed[name] = unnamed.get(name, 0) + 1
                if c > 1:
                    name = "name%d" % (c - 1)
            else:
                name = f.name
            # XXX should extend get_c_type() to handle this
            if f.seq:
                ctype = "asdl_seq *"
            else:
                ctype = get_c_type(f.type)
            args.append((ctype, name, f.opt or f.seq))
        return args

    def visitConstructor(self, cons, type, attrs):
        args = self.get_args(cons.fields)
        attrs = self.get_args(attrs)
        ctype = get_c_type(type)
        self.emit_function(cons.name, ctype, args, attrs)

    def emit_function(self, name, ctype, args, attrs, union=1):
        args = args + attrs
        if args:
            argstr = ", ".join(["%s %s" % (atype, aname)
                                for atype, aname, opt in args])
        else:
            argstr = "void"
        self.emit("%s %s(%s);" % (ctype, name, argstr), 0)

    def visitProduct(self, prod, name):
        self.emit_function(name, get_c_type(name),
                           self.get_args(prod.fields), [], union=0)

class FunctionVisitor(PrototypeVisitor):
    """Visitor to generate constructor functions for AST."""

    def emit_function(self, name, ctype, args, attrs, union=1):
        def emit(s, depth=0, reflow=1):
            self.emit(s, depth, reflow)
        argstr = ", ".join(["%s %s" % (atype, aname)
                            for atype, aname, opt in args + attrs])
        self.emit("%s" % ctype, 0)
        emit("%s(%s)" % (name, argstr))
        emit("{")
        emit("%s p;" % ctype, 1)
        for argtype, argname, opt in args:
            # XXX hack alert: false is allowed for a bool
            if not opt and not argtype == "bool":
                emit("if (!%s) {" % argname, 1)
                emit("PyErr_SetString(PyExc_ValueError,", 2)
                msg = "field %s is required for %s" % (argname, name)
                emit('                "%s");' % msg,
                     2, reflow=0)
                emit('return NULL;', 2)
                emit('}', 1)

        emit("p = (%s)malloc(sizeof(*p));" % ctype, 1)
        emit("if (!p) {", 1)
        emit("PyErr_NoMemory();", 2)
        emit("return NULL;", 2)
        emit("}", 1)
        if union:
            self.emit_body_union(name, args, attrs)
        else:
            self.emit_body_struct(name, args, attrs)
        emit("return p;", 1)
        emit("}")
        emit("")

    def emit_body_union(self, name, args, attrs):
        def emit(s, depth=0, reflow=1):
            self.emit(s, depth, reflow)
        emit("p->kind = %s_kind;" % name, 1)
        for argtype, argname, opt in args:
            emit("p->v.%s.%s = %s;" % (name, argname, argname), 1)
        for argtype, argname, opt in attrs:
            emit("p->%s = %s;" % (argname, argname), 1)

    def emit_body_struct(self, name, args, attrs):
        def emit(s, depth=0, reflow=1):
            self.emit(s, depth, reflow)
        for argtype, argname, opt in args:
            emit("p->%s = %s;" % (argname, argname), 1)
        assert not attrs

class PickleVisitor(EmitVisitor):

    def visitModule(self, mod):
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, type.name)

    def visitSum(self, sum, name):
        pass

    def visitProduct(self, sum, name):
        pass

    def visitConstructor(self, cons, name):
        pass

    def visitField(self, sum):
        pass

class MarshalPrototypeVisitor(PickleVisitor):

    def prototype(self, sum, name):
        ctype = get_c_type(name)
        self.emit("int marshal_write_%s(PyObject **, int *, %s);"
                  % (name, ctype), 0)

    visitProduct = visitSum = prototype

class FreePrototypeVisitor(PickleVisitor):

    def prototype(self, sum, name):
        ctype = get_c_type(name)
        self.emit("void free_%s(%s);" % (name, ctype), 0)

    visitProduct = visitSum = prototype

_SPECIALIZED_SEQUENCES = ('stmt', 'expr')

def find_sequence(fields, doing_specialization):
    """Return True if any field uses a sequence."""
    for f in fields:
        if f.seq:
            if not doing_specialization:
                return True
            if str(f.type) not in _SPECIALIZED_SEQUENCES:
                return True
    return False

def has_sequence(types, doing_specialization):
    for t in types:
        if find_sequence(t.fields, doing_specialization):
            return True
    return False


class StaticVisitor(PickleVisitor):
    '''Very simple, always emit this static code'''

    CODE = '''static void
free_seq_exprs(asdl_seq *seq)
{
        int i, n;
        n = asdl_seq_LEN(seq);
        for (i = 0; i < n; i++)
                free_expr((expr_ty)asdl_seq_GET(seq, i));
        asdl_seq_free(seq);
}

static void
free_seq_stmts(asdl_seq *seq)
{
        int i, n;
        n = asdl_seq_LEN(seq);
        for (i = 0; i < n; i++)
                free_stmt((stmt_ty)asdl_seq_GET(seq, i));
        asdl_seq_free(seq);
}
'''

    def visit(self, object):
        self.emit(self.CODE, 0, reflow=False)


class FreeVisitor(PickleVisitor):

    def func_begin(self, name, has_seq):
        ctype = get_c_type(name)
        self.emit("void", 0)
        self.emit("free_%s(%s o)" % (name, ctype), 0)
        self.emit("{", 0)
        if has_seq:
            self.emit("int i, n;", 1)
            self.emit("asdl_seq *seq;", 1)
            self.emit('', 0)
        self.emit('if (!o)', 1)
        self.emit('return;', 2)
        self.emit('', 0)

    def func_end(self):
        self.emit("}", 0)
        self.emit("", 0)

    def visitSum(self, sum, name):
        has_seq = has_sequence(sum.types, True)
        self.func_begin(name, has_seq)
        if not is_simple(sum):
            self.emit("switch (o->kind) {", 1)
            for i in range(len(sum.types)):
                t = sum.types[i]
                self.visitConstructor(t, i + 1, name)
            self.emit("}", 1)
            self.emit("", 0)
            self.emit("free(o);", 1)
        self.func_end()

    def visitProduct(self, prod, name):
        self.func_begin(name, find_sequence(prod.fields, True))
        for field in prod.fields:
            self.visitField(field, name, 1, True)
        self.emit("", 0)
        self.emit("free(o);", 1)
        self.func_end()
        
    def visitConstructor(self, cons, enum, name):
        self.emit("case %s_kind:" % cons.name, 1)
        for f in cons.fields:
            self.visitField(f, cons.name, 2, False)
        self.emit("break;", 2)

    def visitField(self, field, name, depth, product):
        def emit(s, d):
            self.emit(s, depth + d)
        if product:
            value = "o->%s" % field.name
        else:
            value = "o->v.%s.%s" % (name, field.name)
        if field.seq:
            self.emitSeq(field, value, depth, emit)

        # XXX need to know the simple types in advance, so that we
        # don't call free_TYPE() for them.

        elif field.opt:
            emit("if (%s) {" % value, 0)
            self.free(field, value, depth + 1)
            emit("}", 0)
        else:
            self.free(field, value, depth)

    def emitSeq(self, field, value, depth, emit):
        # specialize for freeing sequences of statements and expressions
        if str(field.type) in _SPECIALIZED_SEQUENCES:
            c_code = "free_seq_%ss(%s);" % (field.type, value)
            emit(c_code, 0)
        else:
            emit("seq = %s;" % value, 0)
            emit("n = asdl_seq_LEN(seq);", 0)
            emit("for (i = 0; i < n; i++)", 0)
            self.free(field, "asdl_seq_GET(seq, i)", depth + 1)
            emit("asdl_seq_free(seq);", 0)

    def free(self, field, value, depth):
        if str(field.type) in ("identifier", "string", "object"):
            ctype = get_c_type(field.type)
            self.emit("Py_DECREF((%s)%s);" % (ctype, value), depth)
        elif str(field.type) == "bool":
            return
        else:
            ctype = get_c_type(field.type)
            self.emit("free_%s((%s)%s);" % (field.type, ctype, value), depth)
        

class MarshalFunctionVisitor(PickleVisitor):

    def func_begin(self, name, has_seq):
        ctype = get_c_type(name)
        self.emit("int", 0)
        self.emit("marshal_write_%s(PyObject **buf, int *off, %s o)" %
                  (name, ctype), 0)
        self.emit("{", 0)
        if has_seq:
            self.emit("int i;", 1)

    def func_end(self):
        self.emit("return 1;", 1)
        self.emit("}", 0)
        self.emit("", 0)
    
    def visitSum(self, sum, name):
        self.func_begin(name, has_sequence(sum.types, False))
        simple = is_simple(sum)
        if simple:
            self.emit("switch (o) {", 1)
        else:
            self.emit("switch (o->kind) {", 1)
        for i in range(len(sum.types)):
            t = sum.types[i]
            self.visitConstructor(t, i + 1, name, simple)
        self.emit("}", 1)
        self.func_end()

    def visitProduct(self, prod, name):
        self.func_begin(name, find_sequence(prod.fields, False))
        for field in prod.fields:
            self.visitField(field, name, 1, 1)
        self.func_end()
            
    def visitConstructor(self, cons, enum, name, simple):
        if simple:
            self.emit("case %s:" % cons.name, 1)
            self.emit("marshal_write_int(buf, off, %d);" % enum, 2);
            self.emit("break;", 2)
        else:
            self.emit("case %s_kind:" % cons.name, 1)
            self.emit("marshal_write_int(buf, off, %d);" % enum, 2)
            for f in cons.fields:
                self.visitField(f, cons.name, 2, 0)
            self.emit("break;", 2)

    def visitField(self, field, name, depth, product):
        def emit(s, d):
            self.emit(s, depth + d)
        if product:
            value = "o->%s" % field.name
        else:
            value = "o->v.%s.%s" % (name, field.name)
        if field.seq:
            emit("marshal_write_int(buf, off, asdl_seq_LEN(%s));" % value, 0)
            emit("for (i = 0; i < asdl_seq_LEN(%s); i++) {" % value, 0)
            emit("void *elt = asdl_seq_GET(%s, i);" % value, 1);
            ctype = get_c_type(field.type);
            emit("marshal_write_%s(buf, off, (%s)elt);" % (field.type,
                    ctype), 1)
            emit("}", 0)
        elif field.opt:
            emit("if (%s) {" % value, 0)
            emit("marshal_write_int(buf, off, 1);", 1)
            emit("marshal_write_%s(buf, off, %s);" % (field.type, value), 1)
            emit("}", 0)
            emit("else {", 0)
            emit("marshal_write_int(buf, off, 0);", 1)
            emit("}", 0)
        else:
            emit("marshal_write_%s(buf, off, %s);" % (field.type, value), 0)

class ChainOfVisitors:
    def __init__(self, *visitors):
        self.visitors = visitors

    def visit(self, object):
        for v in self.visitors:
            v.visit(object)

def main(srcfile):
    auto_gen_msg = '/* File automatically generated by %s */\n' % sys.argv[0]
    mod = asdl.parse(srcfile)
    if not asdl.check(mod):
        sys.exit(1)
    if INC_DIR:
        p = "%s/%s-ast.h" % (INC_DIR, mod.name)
    else:
        p = "%s-ast.h" % mod.name
    f = open(p, "wb")
    print >> f, auto_gen_msg
    print >> f, '#include "asdl.h"\n'
    c = ChainOfVisitors(TypeDefVisitor(f),
                        StructVisitor(f),
                        PrototypeVisitor(f),
                        FreePrototypeVisitor(f),
                        MarshalPrototypeVisitor(f),
                        )
    c.visit(mod)
    f.close()

    if SRC_DIR:
        p = "%s/%s-ast.c" % (SRC_DIR, mod.name)
    else:
        p = "%s-ast.c" % mod.name
    f = open(p, "wb")
    print >> f, auto_gen_msg
    print >> f, '#include "Python.h"'
    print >> f, '#include "%s-ast.h"' % mod.name
    print >> f
    v = ChainOfVisitors(FunctionVisitor(f),
                        StaticVisitor(f),
                        FreeVisitor(f),
                        MarshalFunctionVisitor(f),
                        )
    v.visit(mod)
    f.close()

if __name__ == "__main__":
    import sys
    import getopt

    INC_DIR = ''
    SRC_DIR = ''
    opts, args = getopt.getopt(sys.argv[1:], "h:c:")
    for o, v in opts:
        if o == '-h':
            INC_DIR = v
        if o == '-c':
            SRC_DIR = v
    if len(args) != 1:
        print "Must specify single input file"
    main(args[0])
