import contextlib
import io
import os.path
import re

SCRIPT_NAME = 'Tools/build/generate_global_objects.py'
__file__ = os.path.abspath(__file__)
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
INTERNAL = os.path.join(ROOT, 'Include', 'internal')


IGNORED = {
    'ACTION',  # Python/_warnings.c
    'ATTR',  # Python/_warnings.c and Objects/funcobject.c
    'DUNDER',  # Objects/typeobject.c
    'RDUNDER',  # Objects/typeobject.c
    'SPECIAL',  # Objects/weakrefobject.c
    'NAME',  # Objects/typeobject.c
}
IDENTIFIERS = [
    # from ADD() Python/_warnings.c
    'default',
    'ignore',

    # from GET_WARNINGS_ATTR() in Python/_warnings.c
    'WarningMessage',
    '_showwarnmsg',
    '_warn_unawaited_coroutine',
    'defaultaction',
    'filters',
    'onceregistry',

    # from WRAP_METHOD() in Objects/weakrefobject.c
    '__bytes__',
    '__reversed__',

    # from COPY_ATTR() in Objects/funcobject.c
    '__module__',
    '__name__',
    '__qualname__',
    '__doc__',
    '__annotations__',

    # from SLOT* in Objects/typeobject.c
    '__abs__',
    '__add__',
    '__aiter__',
    '__and__',
    '__anext__',
    '__await__',
    '__bool__',
    '__call__',
    '__contains__',
    '__del__',
    '__delattr__',
    '__delete__',
    '__delitem__',
    '__eq__',
    '__float__',
    '__floordiv__',
    '__ge__',
    '__get__',
    '__getattr__',
    '__getattribute__',
    '__getitem__',
    '__gt__',
    '__hash__',
    '__iadd__',
    '__iand__',
    '__ifloordiv__',
    '__ilshift__',
    '__imatmul__',
    '__imod__',
    '__imul__',
    '__index__',
    '__init__',
    '__int__',
    '__invert__',
    '__ior__',
    '__ipow__',
    '__irshift__',
    '__isub__',
    '__iter__',
    '__itruediv__',
    '__ixor__',
    '__le__',
    '__len__',
    '__lshift__',
    '__lt__',
    '__matmul__',
    '__mod__',
    '__mul__',
    '__ne__',
    '__neg__',
    '__new__',
    '__next__',
    '__or__',
    '__pos__',
    '__pow__',
    '__radd__',
    '__rand__',
    '__repr__',
    '__rfloordiv__',
    '__rlshift__',
    '__rmatmul__',
    '__rmod__',
    '__rmul__',
    '__ror__',
    '__rpow__',
    '__rrshift__',
    '__rshift__',
    '__rsub__',
    '__rtruediv__',
    '__rxor__',
    '__set__',
    '__setattr__',
    '__setitem__',
    '__str__',
    '__sub__',
    '__truediv__',
    '__xor__',
    '__divmod__',
    '__rdivmod__',
]


#######################################
# helpers

def iter_files():
    for name in ('Modules', 'Objects', 'Parser', 'PC', 'Programs', 'Python'):
        root = os.path.join(ROOT, name)
        for dirname, _, files in os.walk(root):
            for name in files:
                if not name.endswith(('.c', '.h')):
                    continue
                yield os.path.join(dirname, name)


def iter_global_strings():
    id_regex = re.compile(r'\b_Py_ID\((\w+)\)')
    str_regex = re.compile(r'\b_Py_DECLARE_STR\((\w+), "(.*?)"\)')
    for filename in iter_files():
        try:
            infile = open(filename, encoding='utf-8')
        except FileNotFoundError:
            # The file must have been a temporary file.
            continue
        with infile:
            for lno, line in enumerate(infile, 1):
                for m in id_regex.finditer(line):
                    identifier, = m.groups()
                    yield identifier, None, filename, lno, line
                for m in str_regex.finditer(line):
                    varname, string = m.groups()
                    yield varname, string, filename, lno, line


def iter_to_marker(lines, marker):
    for line in lines:
        if line.rstrip() == marker:
            break
        yield line


class Printer:

    def __init__(self, file):
        self.level = 0
        self.file = file
        self.continuation = [False]

    @contextlib.contextmanager
    def indent(self):
        save_level = self.level
        try:
            self.level += 1
            yield
        finally:
            self.level = save_level

    def write(self, arg):
        eol = '\n'
        if self.continuation[-1]:
            eol = f' \\{eol}' if arg else f'\\{eol}'
        self.file.writelines(("    "*self.level, arg, eol))

    @contextlib.contextmanager
    def block(self, prefix, suffix="", *, continuation=None):
        if continuation is None:
            continuation = self.continuation[-1]
        self.continuation.append(continuation)

        self.write(prefix + " {")
        with self.indent():
            yield
        self.continuation.pop()
        self.write("}" + suffix)


@contextlib.contextmanager
def open_for_changes(filename, orig):
    """Like open() but only write to the file if it changed."""
    outfile = io.StringIO()
    yield outfile
    text = outfile.getvalue()
    if text != orig:
        with open(filename, 'w', encoding='utf-8') as outfile:
            outfile.write(text)
    else:
        print(f'# not changed: {filename}')


#######################################
# the global objects

START = f'/* The following is auto-generated by {SCRIPT_NAME}. */'
END = '/* End auto-generated code */'


def generate_global_strings(identifiers, strings):
    filename = os.path.join(INTERNAL, 'pycore_global_strings.h')

    # Read the non-generated part of the file.
    with open(filename) as infile:
        orig = infile.read()
    lines = iter(orig.rstrip().splitlines())
    before = '\n'.join(iter_to_marker(lines, START))
    for _ in iter_to_marker(lines, END):
        pass
    after = '\n'.join(lines)

    # Generate the file.
    with open_for_changes(filename, orig) as outfile:
        printer = Printer(outfile)
        printer.write(before)
        printer.write(START)
        with printer.block('struct _Py_global_strings', ';'):
            with printer.block('struct', ' literals;'):
                for literal, name in sorted(strings.items(), key=lambda x: x[1]):
                    printer.write(f'STRUCT_FOR_STR({name}, "{literal}")')
            outfile.write('\n')
            with printer.block('struct', ' identifiers;'):
                for name in sorted(identifiers):
                    assert name.isidentifier(), name
                    printer.write(f'STRUCT_FOR_ID({name})')
            with printer.block('struct', ' ascii[128];'):
                printer.write("PyASCIIObject _ascii;")
                printer.write("uint8_t _data[2];")
            with printer.block('struct', ' latin1[128];'):
                printer.write("PyCompactUnicodeObject _latin1;")
                printer.write("uint8_t _data[2];")
        printer.write(END)
        printer.write(after)


def generate_runtime_init(identifiers, strings):
    # First get some info from the declarations.
    nsmallposints = None
    nsmallnegints = None
    with open(os.path.join(INTERNAL, 'pycore_global_objects.h')) as infile:
        for line in infile:
            if line.startswith('#define _PY_NSMALLPOSINTS'):
                nsmallposints = int(line.split()[-1])
            elif line.startswith('#define _PY_NSMALLNEGINTS'):
                nsmallnegints = int(line.split()[-1])
                break
        else:
            raise NotImplementedError
    assert nsmallposints and nsmallnegints

    # Then target the runtime initializer.
    filename = os.path.join(INTERNAL, 'pycore_runtime_init_generated.h')

    # Read the non-generated part of the file.
    with open(filename) as infile:
        orig = infile.read()
    lines = iter(orig.rstrip().splitlines())
    before = '\n'.join(iter_to_marker(lines, START))
    for _ in iter_to_marker(lines, END):
        pass
    after = '\n'.join(lines)

    # Generate the file.
    with open_for_changes(filename, orig) as outfile:
        immortal_objects = []
        printer = Printer(outfile)
        printer.write(before)
        printer.write(START)
        with printer.block('#define _Py_global_objects_INIT', continuation=True):
            with printer.block('.singletons =', ','):
                # Global int objects.
                with printer.block('.small_ints =', ','):
                    for i in range(-nsmallnegints, nsmallposints):
                        printer.write(f'_PyLong_DIGIT_INIT({i}),')
                        immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(small_ints)[_PY_NSMALLNEGINTS + {i}]')
                printer.write('')
                # Global bytes objects.
                printer.write('.bytes_empty = _PyBytes_SIMPLE_INIT(0, 0),')
                immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(bytes_empty)')
                with printer.block('.bytes_characters =', ','):
                    for i in range(256):
                        printer.write(f'_PyBytes_CHAR_INIT({i}),')
                        immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(bytes_characters)[{i}]')
                printer.write('')
                # Global strings.
                with printer.block('.strings =', ','):
                    with printer.block('.literals =', ','):
                        for literal, name in sorted(strings.items(), key=lambda x: x[1]):
                            printer.write(f'INIT_STR({name}, "{literal}"),')
                            immortal_objects.append(f'(PyObject *)&_Py_STR({name})')
                    with printer.block('.identifiers =', ','):
                        for name in sorted(identifiers):
                            assert name.isidentifier(), name
                            printer.write(f'INIT_ID({name}),')
                            immortal_objects.append(f'(PyObject *)&_Py_ID({name})')
                    with printer.block('.ascii =', ','):
                        for i in range(128):
                            printer.write(f'_PyASCIIObject_INIT("\\x{i:02x}"),')
                            immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(strings).ascii[{i}]')
                    with printer.block('.latin1 =', ','):
                        for i in range(128, 256):
                            utf8 = ['"']
                            for c in chr(i).encode('utf-8'):
                                utf8.append(f"\\x{c:02x}")
                            utf8.append('"')
                            printer.write(f'_PyUnicode_LATIN1_INIT("\\x{i:02x}", {"".join(utf8)}),')
                            immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(strings).latin1[{i} - 128]')
                printer.write('')
                with printer.block('.tuple_empty =', ','):
                    printer.write('.ob_base = _PyVarObject_IMMORTAL_INIT(&PyTuple_Type, 0)')
                    immortal_objects.append(f'(PyObject *)&_Py_SINGLETON(tuple_empty)')
        printer.write(END)
        printer.write(after)
        return immortal_objects


def generate_static_strings_initializer(identifiers, strings):
    # Target the runtime initializer.
    filename = os.path.join(INTERNAL, 'pycore_unicodeobject_generated.h')

    # Read the non-generated part of the file.
    with open(filename) as infile:
        orig = infile.read()
    lines = iter(orig.rstrip().splitlines())
    before = '\n'.join(iter_to_marker(lines, START))
    for _ in iter_to_marker(lines, END):
        pass
    after = '\n'.join(lines)

    # Generate the file.
    with open_for_changes(filename, orig) as outfile:
        printer = Printer(outfile)
        printer.write(before)
        printer.write(START)
        printer.write("static inline void")
        with printer.block("_PyUnicode_InitStaticStrings(void)"):
            printer.write(f'PyObject *string;')
            for i in sorted(identifiers):
                # This use of _Py_ID() is ignored by iter_global_strings()
                # since iter_files() ignores .h files.
                printer.write(f'string = &_Py_ID({i});')
                printer.write(f'PyUnicode_InternInPlace(&string);')
            # XXX What about "strings"?
        printer.write(END)
        printer.write(after)


def generate_global_object_finalizers(immortal_objects):
    # Target the runtime initializer.
    filename = os.path.join(INTERNAL, 'pycore_global_objects_fini_generated.h')

    # Read the non-generated part of the file.
    with open(filename) as infile:
        orig = infile.read()
    lines = iter(orig.rstrip().splitlines())
    before = '\n'.join(iter_to_marker(lines, START))
    for _ in iter_to_marker(lines, END):
        pass
    after = '\n'.join(lines)

    # Generate the file.
    with open_for_changes(filename, orig) as outfile:
        printer = Printer(outfile)
        printer.write(before)
        printer.write(START)
        printer.write('#ifdef Py_DEBUG')
        printer.write("static inline void")
        with printer.block("_PyStaticObjects_CheckRefcnt(void)"):
            for i in immortal_objects:
                printer.write(f'_PyStaticObject_CheckRefcnt({i});')
        printer.write('#endif  // Py_DEBUG')
        printer.write(END)
        printer.write(after)


def get_identifiers_and_strings() -> 'tuple[set[str], dict[str, str]]':
    identifiers = set(IDENTIFIERS)
    strings = {}
    for name, string, *_ in iter_global_strings():
        if string is None:
            if name not in IGNORED:
                identifiers.add(name)
        else:
            if string not in strings:
                strings[string] = name
            elif name != strings[string]:
                raise ValueError(f'string mismatch for {name!r} ({string!r} != {strings[name]!r}')
    return identifiers, strings


#######################################
# the script

def main() -> None:
    identifiers, strings = get_identifiers_and_strings()

    generate_global_strings(identifiers, strings)
    immortal_objects = generate_runtime_init(identifiers, strings)
    generate_static_strings_initializer(identifiers, strings)
    generate_global_object_finalizers(immortal_objects)


if __name__ == '__main__':
    main()
