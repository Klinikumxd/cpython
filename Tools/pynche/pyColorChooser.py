"""Color chooser implementing (almost) the tkColorColor interface
"""

import os
from PyncheWidget import PyncheWidget
import Main
import ColorDB

class Chooser:
    """Ask for a color"""
    def __init__(self,
                 master = None,
                 initialcolor = None,
                 databasefile = None,
                 initfile = None,
                 ignore = None):
        self.__master = master
        self.__initialcolor = initialcolor
        self.__databasefile = databasefile
        self.__initfile = initfile or os.path.expanduser('~/.pynche')
        self.__ignore = ignore
        self.__pw = None

    def show(self):
        if not self.__pw:
            self.__pw, self.__sb = \
                       Main.build(master = self.__master,
                                  initialcolor = self.__initialcolor,
                                  initfile = self.__initfile,
                                  ignore = self.__ignore)
        Main.run(self.__pw, self.__sb)
        rgbtuple = self.__sb.current_rgb()
        self.__pw.withdraw()
        # check to see if the cancel button was pushed
        if self.__sb.canceled_p():
            return None, None
        colordb = self.__sb.colordb()
        # try to return the color name from the database if there is an exact
        # match, otherwise use the "#rrggbb" spec.  TBD: Forget about color
        # aliases for now, maybe later we should return these too.
        try:
            name = colordb.find_byrgb(rgbtuple)[0]
        except ColorDB.BadColor:
            name = ColorDB.triplet_to_rrggbb(rgbtuple)
        return rgbtuple, name



# convenience stuff
def askcolor(color = None, **options):
    """Ask for a color"""
    return apply(Chooser, (), options).show()



# test stuff
if __name__ == '__main__':
    class Tester:
        def __init__(self):
            from Tkinter import *
            self.__root = tk = Tk()
            b = Button(tk, text='Choose Color...', command=self.__choose)
            b.pack()
            self.__l = Label(tk)
            self.__l.pack()
            q = Button(tk, text='Quit', command=self.__quit)
            q.pack()

        def __choose(self, event=None):
            rgb, name = askcolor(master=self.__root)
            if rgb is None:
                text = 'You hit CANCEL!'
            else:
                r, g, b = rgb
                text = 'You picked %s (%3d/%3d/%3d)' % (name, r, g, b)
            self.__l.configure(text=text)

        def __quit(self, event=None):
            self.__root.quit()

        def run(self):
            self.__root.mainloop()
    t = Tester()
    t.run()
    # simpler
##    print 'color:', askcolor()
##    print 'color:', askcolor()
