import errno
import hotshot
import hotshot.stats
import os
import sys
import test.pystone


if sys.argv[1:]:
    logfile = sys.argv[1]
else:
    import tempfile
    logf = tempfile.NamedTemporaryFile()
    logfile = logf.name

p = hotshot.Profile(logfile)
benchtime, stones = p.runcall(test.pystone.pystones)
p.close()

print "Pystone(%s) time for %d passes = %g" % \
      (test.pystone.__version__, test.pystone.LOOPS, benchtime)
print "This machine benchmarks at %g pystones/second" % stones

stats = hotshot.stats.load(logfile)
stats.strip_dirs()
stats.sort_stats('time', 'calls')
try:
    stats.print_stats(20)
except IOError, e:
    if e.errno != errno.EPIPE:
        raise
