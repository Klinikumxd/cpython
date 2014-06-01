# Tkinter font wrapper
#
# written by Fredrik Lundh, February 1998
#

__version__ = "0.9"

import itertools
import tkinter


# weight/slant
NORMAL = "normal"
ROMAN = "roman"
BOLD   = "bold"
ITALIC = "italic"


def nametofont(name):
    """Given the name of a tk named font, returns a Font representation.
    """
    return Font(name=name, exists=True)


class Font:
    """Represents a named font.

    Constructor options are:

    font -- font specifier (name, system font, or (family, size, style)-tuple)
    name -- name to use for this font configuration (defaults to a unique name)
    exists -- does a named font by this name already exist?
       Creates a new named font if False, points to the existing font if True.
       Raises _tkinter.TclError if the assertion is false.

       the following are ignored if font is specified:

    family -- font 'family', e.g. Courier, Times, Helvetica
    size -- font size in points
    weight -- font thickness: NORMAL, BOLD
    slant -- font slant: ROMAN, ITALIC
    underline -- font underlining: false (0), true (1)
    overstrike -- font strikeout: false (0), true (1)

    """

    counter = itertools.count(1)

    def _set(self, kw):
        options = []
        for k, v in kw.items():
            options.append("-"+k)
            options.append(str(v))
        return tuple(options)

    def _get(self, args):
        options = []
        for k in args:
            options.append("-"+k)
        return tuple(options)

    def _mkdict(self, args):
        options = {}
        for i in range(0, len(args), 2):
            options[args[i][1:]] = args[i+1]
        return options

    def __init__(self, root=None, font=None, name=None, exists=False,
                 **options):
        if not root:
            root = tkinter._default_root
        if font:
            # get actual settings corresponding to the given font
            font = root.tk.splitlist(root.tk.call("font", "actual", font))
        else:
            font = self._set(options)
        if not name:
            name = "font" + str(next(self.counter))
        self.name = name

        if exists:
            self.delete_font = False
            # confirm font exists
            if self.name not in root.tk.splitlist(
                    root.tk.call("font", "names")):
                raise tkinter._tkinter.TclError(
                    "named font %s does not already exist" % (self.name,))
            # if font config info supplied, apply it
            if font:
                root.tk.call("font", "configure", self.name, *font)
        else:
            # create new font (raises TclError if the font exists)
            root.tk.call("font", "create", self.name, *font)
            self.delete_font = True
        # backlinks!
        self._root  = root
        self._split = root.tk.splitlist
        self._call  = root.tk.call

    def __str__(self):
        return self.name

    def __eq__(self, other):
        return isinstance(other, Font) and self.name == other.name

    def __getitem__(self, key):
        return self.cget(key)

    def __setitem__(self, key, value):
        self.configure(**{key: value})

    def __del__(self):
        try:
            if self.delete_font:
                self._call("font", "delete", self.name)
        except (KeyboardInterrupt, SystemExit):
            raise
        except Exception:
            pass

    def copy(self):
        "Return a distinct copy of the current font"
        return Font(self._root, **self.actual())

    def actual(self, option=None, displayof=None):
        "Return actual font attributes"
        args = ()
        if displayof:
            args = ('-displayof', displayof)
        if option:
            args = args + ('-' + option, )
            return self._call("font", "actual", self.name, *args)
        else:
            return self._mkdict(
                self._split(self._call("font", "actual", self.name, *args)))

    def cget(self, option):
        "Get font attribute"
        return self._call("font", "config", self.name, "-"+option)

    def config(self, **options):
        "Modify font attributes"
        if options:
            self._call("font", "config", self.name,
                  *self._set(options))
        else:
            return self._mkdict(
                self._split(self._call("font", "config", self.name)))

    configure = config

    def measure(self, text, displayof=None):
        "Return text width"
        args = (text,)
        if displayof:
            args = ('-displayof', displayof, text)
        return int(self._call("font", "measure", self.name, *args))

    def metrics(self, *options, **kw):
        """Return font metrics.

        For best performance, create a dummy widget
        using this font before calling this method."""
        args = ()
        displayof = kw.pop('displayof', None)
        if displayof:
            args = ('-displayof', displayof)
        if options:
            args = args + self._get(options)
            return int(
                self._call("font", "metrics", self.name, *args))
        else:
            res = self._split(self._call("font", "metrics", self.name, *args))
            options = {}
            for i in range(0, len(res), 2):
                options[res[i][1:]] = int(res[i+1])
            return options


def families(root=None, displayof=None):
    "Get font families (as a tuple)"
    if not root:
        root = tkinter._default_root
    args = ()
    if displayof:
        args = ('-displayof', displayof)
    return root.tk.splitlist(root.tk.call("font", "families", *args))


def names(root=None):
    "Get names of defined fonts (as a tuple)"
    if not root:
        root = tkinter._default_root
    return root.tk.splitlist(root.tk.call("font", "names"))


# --------------------------------------------------------------------
# test stuff

if __name__ == "__main__":

    root = tkinter.Tk()

    # create a font
    f = Font(family="times", size=30, weight=NORMAL)

    print(f.actual())
    print(f.actual("family"))
    print(f.actual("weight"))

    print(f.config())
    print(f.cget("family"))
    print(f.cget("weight"))

    print(names())

    print(f.measure("hello"), f.metrics("linespace"))

    print(f.metrics(displayof=root))

    f = Font(font=("Courier", 20, "bold"))
    print(f.measure("hello"), f.metrics("linespace", displayof=root))

    w = tkinter.Label(root, text="Hello, world", font=f)
    w.pack()

    w = tkinter.Button(root, text="Quit!", command=root.destroy)
    w.pack()

    fb = Font(font=w["font"]).copy()
    fb.config(weight=BOLD)

    w.config(font=fb)

    tkinter.mainloop()
