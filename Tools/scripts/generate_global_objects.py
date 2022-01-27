import argparse
import ast
import builtins
import collections
import contextlib
import os.path
import sys


assert os.path.isabs(__file__), __file__
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
INTERNAL = os.path.join(ROOT, 'Include', 'internal')


#######################################
# helpers

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


#######################################
# the global objects

START = '/* The following is auto-generated by Tools/scripts/generate_global_objects.py. */'
END = '/* End auto-generated code */'


def generate_runtime_init():
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
    filename = os.path.join(INTERNAL, 'pycore_runtime_init.h')

    # Read the non-generated part of the file.
    with open(filename) as infile:
        before = ''.join(iter_to_marker(infile, START))[:-1]
        for _ in iter_to_marker(infile, END):
            pass
        after = infile.read()[:-1]

    # Generate the file.
    with open(filename, 'w', encoding='utf-8') as outfile:
        printer = Printer(outfile)
        printer.write(before)
        printer.write(START)
        with printer.block('#define _Py_global_objects_INIT', continuation=True):
            with printer.block('.singletons =', ','):
                # Global int objects.
                with printer.block('.small_ints =', ','):
                    for i in range(-nsmallnegints, nsmallposints):
                        printer.write(f'_PyLong_DIGIT_INIT({i}),')
                printer.write('')
                # Global bytes objects.
                printer.write('.bytes_empty = _PyBytes_SIMPLE_INIT(0, 0),')
                with printer.block('.bytes_characters =', ','):
                    for i in range(256):
                        printer.write(f'_PyBytes_CHAR_INIT({i}),')
        printer.write(END)
        printer.write(after)


#######################################
# the script

def main() -> None:
    generate_runtime_init()


if __name__ == '__main__':
    argv = sys.argv[1:]
    if argv:
        sys.exit(f'ERROR: got unexpected args {argv}')
    main()
