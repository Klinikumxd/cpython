"""curses.wrapper

Contains one function, wrapper(), which runs another function which
should be the rest of your curses-based application.  If the
application raises an exception, wrapper() will restore the terminal
to a sane state so you can read the resulting traceback.

"""

import sys, curses

def wrapper(func, *rest):
    """Wrapper function that initializes curses and calls another function,
    restoring normal keyboard/screen behavior on error.
    The callable object 'func' is then passed the main window 'stdscr'
    as its first argument, followed by any other arguments passed to
    wrapper().
    """
    
    res = None
    try:
	# Initialize curses
	stdscr=curses.initscr()
	# Turn off echoing of keys, and enter cbreak mode,
	# where no buffering is performed on keyboard input
	curses.noecho() ; curses.cbreak()

	# In keypad mode, escape sequences for special keys
	# (like the cursor keys) will be interpreted and
	# a special value like curses.KEY_LEFT will be returned
        stdscr.keypad(1)

        res = apply(func, (stdscr,) + rest)
    except:
	# In the event of an error, restore the terminal
	# to a sane state.
	stdscr.keypad(0)
	curses.echo() ; curses.nocbreak()
	curses.endwin()
        # Pass the exception upwards
        (exc_type, exc_value, exc_traceback) = sys.exc_info()
        raise exc_type, exc_value, exc_traceback
    else:
	# Set everything back to normal
	stdscr.keypad(0)
	curses.echo() ; curses.nocbreak()
	curses.endwin()		 # Terminate curses

        return res

