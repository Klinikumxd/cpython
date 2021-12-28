# This script lists the names of standard library modules
# to update Python/stdlib_mod_names.h
import _imp
import os.path
import re
import subprocess
import sys
import sysconfig


SRC_DIR = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
STDLIB_PATH = os.path.join(SRC_DIR, 'Lib')
MODULES_SETUP = os.path.join(SRC_DIR, 'Modules', 'Setup')
SETUP_PY = os.path.join(SRC_DIR, 'setup.py')

IGNORE = {
    '__init__',
    '__pycache__',
    'site-packages',

    # Test modules and packages
    '__hello__',
    '__phello__',
    '__hello_alias__',
    '__phello_alias__',
    '__hello_only__',
    '_ctypes_test',
    '_testbuffer',
    '_testcapi',
    '_testconsole',
    '_testimportmultiple',
    '_testinternalcapi',
    '_testmultiphase',
    '_xxsubinterpreters',
    '_xxtestfuzz',
    'distutils.tests',
    'idlelib.idle_test',
    'lib2to3.tests',
    'test',
    'xxlimited',
    'xxlimited_35',
    'xxsubtype',
}

# Windows extension modules
WINDOWS_MODULES = (
    '_msi',
    '_overlapped',
    '_testconsole',
    '_winapi',
    'msvcrt',
    'nt',
    'winreg',
    'winsound'
)

# macOS extension modules
MACOS_MODULES = (
    '_scproxy',
)

# Pure Python modules (Lib/*.py)
def list_python_modules(names):
    for filename in os.listdir(STDLIB_PATH):
        if not filename.endswith(".py"):
            continue
        name = filename.removesuffix(".py")
        names.add(name)


# Packages in Lib/
def list_packages(names):
    for name in os.listdir(STDLIB_PATH):
        if name in IGNORE:
            continue
        package_path = os.path.join(STDLIB_PATH, name)
        if not os.path.isdir(package_path):
            continue
        if any(package_file.endswith(".py")
               for package_file in os.listdir(package_path)):
            names.add(name)


# Extension modules built by setup.py
def list_setup_extensions(names):
    cmd = [sys.executable, SETUP_PY, "-q", "build", "--list-module-names"]
    output = subprocess.check_output(cmd)
    output = output.decode("utf8")
    extensions = output.splitlines()
    names |= set(extensions)


# Built-in and extension modules built by Modules/Setup
def list_modules_setup_extensions(names):
    assign_var = re.compile("^[A-Z]+=")

    with open(MODULES_SETUP, encoding="utf-8") as modules_fp:
        for line in modules_fp:
            # Strip comment
            line = line.partition("#")[0]
            line = line.rstrip()
            if not line:
                continue
            if assign_var.match(line):
                # Ignore "VAR=VALUE"
                continue
            if line in ("*disabled*", "*shared*"):
                continue
            parts = line.split()
            if len(parts) < 2:
                continue
            # "errno errnomodule.c" => write "errno"
            name = parts[0]
            names.add(name)


# List frozen modules of the PyImport_FrozenModules list (Python/frozen.c).
# Use the "./Programs/_testembed list_frozen" command.
def list_frozen(names):
    submodules = set()
    for name in _imp._frozen_module_names():
        # To skip __hello__, __hello_alias__ and etc.
        if name.startswith('__'):
            continue
        if '.' in name:
            submodules.add(name)
        else:
            names.add(name)
    # Make sure all frozen submodules have a known parent.
    for name in list(submodules):
        if name.partition('.')[0] in names:
            submodules.remove(name)
    if submodules:
        raise Exception(f'unexpected frozen submodules: {sorted(submodules)}')


def list_modules():
    names = set(sys.builtin_module_names) | set(WINDOWS_MODULES) | set(MACOS_MODULES)
    list_modules_setup_extensions(names)
    list_setup_extensions(names)
    list_packages(names)
    list_python_modules(names)
    list_frozen(names)

    # Remove ignored packages and modules
    for name in list(names):
        package_name = name.split('.')[0]
        # package_name can be equal to name
        if package_name in IGNORE:
            names.discard(name)

    for name in names:
        if "." in name:
            raise Exception("sub-modules must not be listed")

    return names


def write_modules(fp, names):
    print("// Auto-generated by Tools/scripts/generate_stdlib_module_names.py.",
          file=fp)
    print("// List used to create sys.stdlib_module_names.", file=fp)
    print(file=fp)
    print("static const char* _Py_stdlib_module_names[] = {", file=fp)
    for name in sorted(names):
        print(f'"{name}",', file=fp)
    print("};", file=fp)


def main():
    if not sysconfig.is_python_build():
        print(f"ERROR: {sys.executable} is not a Python build",
              file=sys.stderr)
        sys.exit(1)

    fp = sys.stdout
    names = list_modules()
    write_modules(fp, names)


if __name__ == "__main__":
    main()
