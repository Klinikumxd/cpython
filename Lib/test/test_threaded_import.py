# This is a variant of the very old (early 90's) file
# Demo/threads/bug.py.  It simply provokes a number of threads into
# trying to import the same module "at the same time".
# There are no pleasant failure modes -- most likely is that Python
# complains several times about module random having no attribute
# randrange, and then Python hangs.

import thread
from test_support import verbose

critical_section = thread.allocate_lock()
done = thread.allocate_lock()

def task():
    global N, critical_section, done
    import random
    x = random.randrange(1, 3)
    critical_section.acquire()
    N -= 1
    if N == 0:
        done.release()
    critical_section.release()

# Tricky:  When regrtest imports this module, the thread running regrtest
# grabs the import lock and won't let go of it until this module returns.
# All other threads attempting an import hang for the duration.  Since
# this test spawns threads that do little *but* import, we can't do that
# successfully until after this module finishes importing and regrtest
# regains control.  To make this work, a special case was added to
# regrtest to invoke a module's "test_main" function (if any) after
# importing it.

def test_main():        # magic name!  see above
    global N, done
    done.acquire()
    for N in (20, 50) * 3:
        if verbose:
            print "Trying", N, "threads ...",
        for i in range(N):
            thread.start_new_thread(task, ())
        done.acquire()
        if verbose:
            print "OK."

if __name__ == "__main__":
    test_main()
