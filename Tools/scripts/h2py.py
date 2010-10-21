#! /usr/bin/env python3

# Read #define's and translate to Python code.
# Handle #include statements.
# Handle #define macros with one argument.
# Anything that isn't recognized or doesn't translate into valid
# Python is ignored.

# Without filename arguments, acts as a filter.
# If one or more filenames are given, output is written to corresponding
# filenames in the local directory, translated to all uppercase, with
# the extension replaced by ".py".

# By passing one or more options of the form "-i regular_expression"
# you can specify additional strings to be ignored.  This is useful
# e.g. to ignore casts to u_long: simply specify "-i '(u_long)'".

# XXX To do:
# - turn trailing C comments into Python comments
# - turn C Boolean operators "&& || !" into Python "and or not"
# - what to do about #if(def)?
# - what to do about macros with multiple parameters?

import sys, re, getopt, os

p_define = re.compile('^[\t ]*#[\t ]*define[\t ]+([a-zA-Z0-9_]+)[\t ]+')

p_macro = re.compile(
  '^[\t ]*#[\t ]*define[\t ]+'
  '([a-zA-Z0-9_]+)\(([_a-zA-Z][_a-zA-Z0-9]*)\)[\t ]+')

p_include = re.compile('^[\t ]*#[\t ]*include[\t ]+<([a-zA-Z0-9_/\.]+)')

p_comment = re.compile(r'/\*([^*]+|\*+[^/])*(\*+/)?')
p_cpp_comment = re.compile('//.*')

ignores = [p_comment, p_cpp_comment]

p_char = re.compile(r"'(\\.[^\\]*|[^\\])'")

p_hex = re.compile(r"0x([0-9a-fA-F]+)L?")

filedict = {}
importable = {}

try:
    searchdirs=os.environ['include'].split(';')
except KeyError:
    try:
        searchdirs=os.environ['INCLUDE'].split(';')
    except KeyError:
        searchdirs=['/usr/include']

def main():
    global filedict
    opts, args = getopt.getopt(sys.argv[1:], 'i:')
    for o, a in opts:
        if o == '-i':
            ignores.append(re.compile(a))
    if not args:
        args = ['-']
    for filename in args:
        if filename == '-':
            sys.stdout.write('# Generated by h2py from stdin\n')
            process(sys.stdin, sys.stdout)
        else:
            fp = open(filename, 'r')
            outfile = os.path.basename(filename)
            i = outfile.rfind('.')
            if i > 0: outfile = outfile[:i]
            modname = outfile.upper()
            outfile = modname + '.py'
            outfp = open(outfile, 'w')
            outfp.write('# Generated by h2py from %s\n' % filename)
            filedict = {}
            for dir in searchdirs:
                if filename[:len(dir)] == dir:
                    filedict[filename[len(dir)+1:]] = None  # no '/' trailing
                    importable[filename[len(dir)+1:]] = modname
                    break
            process(fp, outfp)
            outfp.close()
            fp.close()

def pytify(body):
    # replace ignored patterns by spaces
    for p in ignores:
        body = p.sub(' ', body)
    # replace char literals by ord(...)
    body = p_char.sub("ord('\\1')", body)
    # Compute negative hexadecimal constants
    start = 0
    UMAX = 2*(sys.maxsize+1)
    while 1:
        m = p_hex.search(body, start)
        if not m: break
        s,e = m.span()
        val = int(body[slice(*m.span(1))], 16)
        if val > sys.maxsize:
            val -= UMAX
            body = body[:s] + "(" + str(val) + ")" + body[e:]
        start = s + 1
    return body

def process(fp, outfp, env = {}):
    lineno = 0
    while 1:
        line = fp.readline()
        if not line: break
        lineno = lineno + 1
        match = p_define.match(line)
        if match:
            # gobble up continuation lines
            while line[-2:] == '\\\n':
                nextline = fp.readline()
                if not nextline: break
                lineno = lineno + 1
                line = line + nextline
            name = match.group(1)
            body = line[match.end():]
            body = pytify(body)
            ok = 0
            stmt = '%s = %s\n' % (name, body.strip())
            try:
                exec(stmt, env)
            except:
                sys.stderr.write('Skipping: %s' % stmt)
            else:
                outfp.write(stmt)
        match = p_macro.match(line)
        if match:
            macro, arg = match.group(1, 2)
            body = line[match.end():]
            body = pytify(body)
            stmt = 'def %s(%s): return %s\n' % (macro, arg, body)
            try:
                exec(stmt, env)
            except:
                sys.stderr.write('Skipping: %s' % stmt)
            else:
                outfp.write(stmt)
        match = p_include.match(line)
        if match:
            regs = match.regs
            a, b = regs[1]
            filename = line[a:b]
            if filename in importable:
                outfp.write('from %s import *\n' % importable[filename])
            elif filename not in filedict:
                filedict[filename] = None
                inclfp = None
                for dir in searchdirs:
                    try:
                        inclfp = open(dir + '/' + filename)
                        break
                    except IOError:
                        pass
                if inclfp:
                    outfp.write(
                            '\n# Included from %s\n' % filename)
                    process(inclfp, outfp, env)
                else:
                    sys.stderr.write('Warning - could not find file %s\n' %
                                     filename)

if __name__ == '__main__':
    main()
