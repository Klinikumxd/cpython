'''Parse a Python file and retrieve classes and methods.

Parse enough of a Python file to recognize class and method
definitions and to find out the superclasses of a class.

The interface consists of a single function:
	readmodule(module, path)
module is the name of a Python module, path is an optional list of
directories where the module is to be searched.  If present, path is
prepended to the system search path sys.path.
The return value is a dictionary.  The keys of the dictionary are
the names of the classes defined in the module (including classes
that are defined via the from XXX import YYY construct).  The values
are class instances of the class Class defined here.

A class is described by the class Class in this module.  Instances
of this class have the following instance variables:
	name -- the name of the class
	super -- a list of super classes (Class instances)
	methods -- a dictionary of methods
	file -- the file in which the class was defined
	lineno -- the line in the file on which the class statement occurred
The dictionary of methods uses the method names as keys and the line
numbers on which the method was defined as values.
If the name of a super class is not recognized, the corresponding
entry in the list of super classes is not a class instance but a
string giving the name of the super class.  Since import statements
are recognized and imported modules are scanned as well, this
shouldn't happen often.

BUGS
Continuation lines are not dealt with at all and strings may confuse
the hell out of the parser, but it usually works.'''

import os
import sys
import imp
import regex
import string

id = '\\(<id>[A-Za-z_][A-Za-z0-9_]*\\)'	# match identifier
blank_line = regex.compile('^[ \t]*\\($\\|#\\)')
is_class = regex.symcomp('^class[ \t]+'+id+'[ \t]*\\(<sup>([^)]*)\\)?[ \t]*:')
is_method = regex.symcomp('^[ \t]+def[ \t]+'+id+'[ \t]*(')
is_import = regex.symcomp('^import[ \t]*\\(<imp>[^#]+\\)')
is_from = regex.symcomp('^from[ \t]+'+id+'[ \t]+import[ \t]+\\(<imp>[^#]+\\)')
dedent = regex.compile('^[^ \t]')
indent = regex.compile('^[^ \t]*')

_modules = {}				# cache of modules we've seen

# each Python class is represented by an instance of this class
class Class:
	'''Class to represent a Python class.'''
	def __init__(self, name, super, file, lineno):
		self.name = name
		if super is None:
			super = []
		self.super = super
		self.methods = {}
		self.file = file
		self.lineno = lineno

	def _addmethod(self, name, lineno):
		self.methods[name] = lineno

def readmodule(module, path = []):
	'''Read a module file and return a dictionary of classes.

	Search for MODULE in PATH and sys.path, read and parse the
	module and return a dictionary with one entry for each class
	found in the module.'''

	if _modules.has_key(module):
		# we've seen this module before...
		return _modules[module]
	if module in sys.builtin_module_names:
		# this is a built-in module
		dict = {}
		_modules[module] = dict
		return dict

	# search the path for the module
	f = None
	suffixes = imp.get_suffixes()
	for dir in path + sys.path:
		for suff, mode, type in suffixes:
			file = os.path.join(dir, module + suff)
			try:
				f = open(file, mode)
			except IOError:
				pass
			else:
				# found the module
				break
		if f:
			break
	if not f:
		raise IOError, 'module ' + module + ' not found'
	if type != imp.PY_SOURCE:
		# not Python source, can't do anything with this module
		f.close()
		dict = {}
		_modules[module] = dict
		return dict

	cur_class = None
	dict = {}
	_modules[module] = dict
	imports = []
	lineno = 0
	while 1:
		line = f.readline()
		if not line:
			break
		lineno = lineno + 1	# count lines
		line = line[:-1]	# remove line feed
		if blank_line.match(line) >= 0:
			# ignore blank (and comment only) lines
			continue
##		if indent.match(line) >= 0:
##			indentation = len(string.expandtabs(indent.group(0), 8))
		if is_import.match(line) >= 0:
			# import module
			for n in string.splitfields(is_import.group('imp'), ','):
				n = string.strip(n)
				try:
					# recursively read the
					# imported module
					d = readmodule(n, path)
				except:
					print 'module',n,'not found'
					pass
			continue
		if is_from.match(line) >= 0:
			# from module import stuff
			mod = is_from.group('id')
			names = string.splitfields(is_from.group('imp'), ',')
			try:
				# recursively read the imported module
				d = readmodule(mod, path)
			except:
				print 'module',mod,'not found'
				continue
			# add any classes that were defined in the
			# imported module to our name space if they
			# were mentioned in the list
			for n in names:
				n = string.strip(n)
				if d.has_key(n):
					dict[n] = d[n]
				elif n == '*':
					# only add a name if not
					# already there (to mimic what
					# Python does internally)
					for n in d.keys():
						if not dict.has_key(n):
							dict[n] = d[n]
			continue
		if is_class.match(line) >= 0:
			# we found a class definition
			class_name = is_class.group('id')
			inherit = is_class.group('sup')
			if inherit:
				# the class inherits from other classes
				inherit = string.strip(inherit[1:-1])
				names = []
				for n in string.splitfields(inherit, ','):
					n = string.strip(n)
					if dict.has_key(n):
						# we know this super class
						n = dict[n]
					else:
						c = string.splitfields(n, '.')
						if len(c) > 1:
							# super class
							# is of the
							# form module.class:
							# look in
							# module for class
							m = c[-2]
							c = c[-1]
							if _modules.has_key(m):
								d = _modules[m]
								if d.has_key(c):
									n = d[c]
					names.append(n)
				inherit = names
			# remember this class
			cur_class = Class(class_name, inherit, file, lineno)
			dict[class_name] = cur_class
			continue
		if is_method.match(line) >= 0:
			# found a method definition
			if cur_class:
				# and we know the class it belongs to
				meth_name = is_method.group('id')
				cur_class._addmethod(meth_name, lineno)
			continue
		if dedent.match(line) >= 0:
			# end of class definition
			cur_class = None
	f.close()
	return dict
