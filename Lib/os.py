# os.py -- either mac or posix depending on what system we're on.

# This exports:
# - all functions from either posix or mac, e.g., os.unlink, os.stat, etc.
# - os.path is either module path or macpath
# - os.name is either 'posix' or 'mac'
# - os.curdir is a string representing the current directory ('.' or ':')

# Programs that import and use 'os' stand a better chance of being
# portable between different platforms.  Of course, they must then
# only use functions that are defined by all platforms (e.g., unlink
# and opendir), and leave all pathname manipulation to os.path
# (e.g., split and join).

try:
	from posix import *
	name = 'posix'
	curdir = '.'
	import path
except ImportError:
	from mac import *
	name = 'mac'
	curdir = ':'
	import macpath
	path = macpath
	del macpath
