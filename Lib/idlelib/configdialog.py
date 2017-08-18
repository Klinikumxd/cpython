"""IDLE Configuration Dialog: support user customization of IDLE by GUI

Customize font faces, sizes, and colorization attributes.  Set indentation
defaults.  Customize keybindings.  Colorization and keybindings can be
saved as user defined sets.  Select startup options including shell/editor
and default window size.  Define additional help sources.

Note that tab width in IDLE is currently fixed at eight due to Tk issues.
Refer to comments in EditorWindow autoindent code for details.

"""
from tkinter import (Toplevel, Frame, LabelFrame, Listbox, Label, Button,
                     Entry, Text, Scale, Radiobutton, Checkbutton, Canvas,
                     StringVar, BooleanVar, IntVar, TRUE, FALSE,
                     TOP, BOTTOM, RIGHT, LEFT, SOLID, GROOVE, NORMAL, DISABLED,
                     NONE, BOTH, X, Y, W, E, EW, NS, NSEW, NW,
                     HORIZONTAL, VERTICAL, ANCHOR, ACTIVE, END)
from tkinter.ttk import Notebook, Scrollbar
import tkinter.colorchooser as tkColorChooser
import tkinter.font as tkFont
import tkinter.messagebox as tkMessageBox

from idlelib.config import idleConf, ConfigChanges
from idlelib.config_key import GetKeysDialog
from idlelib.dynoption import DynOptionMenu
from idlelib import macosx
from idlelib.query import SectionName, HelpSource
from idlelib.tabbedpages import TabbedPageSet
from idlelib.textview import view_text

changes = ConfigChanges()


class ConfigDialog(Toplevel):
    """Config dialog for IDLE.
    """

    def __init__(self, parent, title='', _htest=False, _utest=False):
        """Show the tabbed dialog for user configuration.

        Args:
            parent - parent of this dialog
            title - string which is the title of this popup dialog
            _htest - bool, change box location when running htest
            _utest - bool, don't wait_window when running unittest

        Note: Focus set on font page fontlist.

        Methods:
            create_widgets
            cancel: Bound to DELETE_WINDOW protocol.
        """
        Toplevel.__init__(self, parent)
        self.parent = parent
        if _htest:
            parent.instance_dict = {}
        if not _utest:
            self.withdraw()

        self.configure(borderwidth=5)
        self.title(title or 'IDLE Preferences')
        x = parent.winfo_rootx() + 20
        y = parent.winfo_rooty() + (30 if not _htest else 150)
        self.geometry(f'+{x}+{y}')
        # Each theme element key is its display name.
        # The first value of the tuple is the sample area tag name.
        # The second value is the display name list sort index.
        self.create_widgets()
        self.resizable(height=FALSE, width=FALSE)
        self.transient(parent)
        self.protocol("WM_DELETE_WINDOW", self.cancel)
        self.fontpage.fontlist.focus_set()
        # XXX Decide whether to keep or delete these key bindings.
        # Key bindings for this dialog.
        # self.bind('<Escape>', self.Cancel) #dismiss dialog, no save
        # self.bind('<Alt-a>', self.Apply) #apply changes, save
        # self.bind('<F1>', self.Help) #context help
        self.load_configs()
        # Avoid callbacks during load_configs.
        tracers.attach()

        if not _utest:
            self.grab_set()
            self.wm_deiconify()
            self.wait_window()


    def create_widgets(self):
        """Create and place widgets for tabbed dialog.

        Widgets Bound to self:
            note: Notebook
            highpage: self.create_page_highlight
            fontpage: FontPage
            keyspage: KeysPage
            genpage: GenPage
            extpage: self.create_page_extensions

        Methods:
            create_action_buttons
            load_configs: Load pages except for extensions.
            activate_config_changes: Tell editors to reload.
        """
        self.note = note = Notebook(self, width=450, height=450)
        self.highpage = self.create_page_highlight()
        self.fontpage = FontPage(note, self.highpage)
        self.keyspage = KeysPage(note)
        self.genpage = GenPage(note)
        self.extpage = self.create_page_extensions()
        note.add(self.fontpage, text='Fonts/Tabs')
        note.add(self.highpage, text='Highlights')
        note.add(self.keyspage, text=' Keys ')
        note.add(self.genpage, text=' General ')
        note.add(self.extpage, text='Extensions')
        note.enable_traversal()
        note.pack(side=TOP, expand=TRUE, fill=BOTH)
        self.create_action_buttons().pack(side=BOTTOM)

    def load_configs(self):
        """Load configuration for each page.

        Load configuration from default and user config files and populate
        the widgets on the config dialog pages.

        Methods:
            load_font_cfg
            load_tab_cfg
            load_theme_cfg
            load_key_cfg
            load_general_cfg
        """
        #self.load_font_cfg()
        #self.load_tab_cfg()
        self.load_theme_cfg()
        # self.load_key_cfg()
        # self.load_general_cfg()
        # note: extension page handled separately

    def create_action_buttons(self):
        """Return frame of action buttons for dialog.

        Methods:
            ok
            apply
            cancel
            help

        Widget Structure:
            outer: Frame
                buttons: Frame
                    (no assignment): Button (ok)
                    (no assignment): Button (apply)
                    (no assignment): Button (cancel)
                    (no assignment): Button (help)
                (no assignment): Frame
        """
        if macosx.isAquaTk():
            # Changing the default padding on OSX results in unreadable
            # text in the buttons.
            padding_args = {}
        else:
            padding_args = {'padx':6, 'pady':3}
        outer = Frame(self, pady=2)
        buttons = Frame(outer, pady=2)
        for txt, cmd in (
            ('Ok', self.ok),
            ('Apply', self.apply),
            ('Cancel', self.cancel),
            ('Help', self.help)):
            Button(buttons, text=txt, command=cmd, takefocus=FALSE,
                   **padding_args).pack(side=LEFT, padx=5)
        # Add space above buttons.
        Frame(outer, height=2, borderwidth=0).pack(side=TOP)
        buttons.pack(side=BOTTOM)
        return outer

    def ok(self):
        """Apply config changes, then dismiss dialog.

        Methods:
            apply
            destroy: inherited
        """
        self.apply()
        self.destroy()

    def apply(self):
        """Apply config changes and leave dialog open.

        Methods:
            deactivate_current_config
            save_all_changed_extensions
            activate_config_changes
        """
        self.deactivate_current_config()
        changes.save_all()
        self.save_all_changed_extensions()
        self.activate_config_changes()

    def cancel(self):
        """Dismiss config dialog.

        Methods:
            destroy: inherited
        """
        self.destroy()

    def help(self):
        """Create textview for config dialog help.

        Attrbutes accessed:
            tab_pages

        Methods:
            view_text: Method from textview module.
        """
        page = self.tab_pages._current_page
        view_text(self, title='Help for IDLE preferences',
                 text=help_common+help_pages.get(page, ''))


    def create_page_highlight(self):
        """Return frame of widgets for Highlighting tab.

        Enable users to provisionally change foreground and background
        colors applied to textual tags.  Color mappings are stored in
        complete listings called themes.  Built-in themes in
        idlelib/config-highlight.def are fixed as far as the dialog is
        concerned. Any theme can be used as the base for a new custom
        theme, stored in .idlerc/config-highlight.cfg.

        Function load_theme_cfg() initializes tk variables and theme
        lists and calls paint_theme_sample() and set_highlight_target()
        for the current theme.  Radiobuttons builtin_theme_on and
        custom_theme_on toggle var theme_source, which controls if the
        current set of colors are from a builtin or custom theme.
        DynOptionMenus builtinlist and customlist contain lists of the
        builtin and custom themes, respectively, and the current item
        from each list is stored in vars builtin_name and custom_name.

        Function paint_theme_sample() applies the colors from the theme
        to the tags in text widget highlight_sample and then invokes
        set_color_sample().  Function set_highlight_target() sets the state
        of the radiobuttons fg_on and bg_on based on the tag and it also
        invokes set_color_sample().

        Function set_color_sample() sets the background color for the frame
        holding the color selector.  This provides a larger visual of the
        color for the current tag and plane (foreground/background).

        Note: set_color_sample() is called from many places and is often
        called more than once when a change is made.  It is invoked when
        foreground or background is selected (radiobuttons), from
        paint_theme_sample() (theme is changed or load_cfg is called), and
        from set_highlight_target() (target tag is changed or load_cfg called).

        Button delete_custom invokes delete_custom() to delete
        a custom theme from idleConf.userCfg['highlight'] and changes.
        Button save_custom invokes save_as_new_theme() which calls
        get_new_theme_name() and create_new() to save a custom theme
        and its colors to idleConf.userCfg['highlight'].

        Radiobuttons fg_on and bg_on toggle var fg_bg_toggle to control
        if the current selected color for a tag is for the foreground or
        background.

        DynOptionMenu targetlist contains a readable description of the
        tags applied to Python source within IDLE.  Selecting one of the
        tags from this list populates highlight_target, which has a callback
        function set_highlight_target().

        Text widget highlight_sample displays a block of text (which is
        mock Python code) in which is embedded the defined tags and reflects
        the color attributes of the current theme and changes for those tags.
        Mouse button 1 allows for selection of a tag and updates
        highlight_target with that tag value.

        Note: The font in highlight_sample is set through the config in
        the fonts tab.

        In other words, a tag can be selected either from targetlist or
        by clicking on the sample text within highlight_sample.  The
        plane (foreground/background) is selected via the radiobutton.
        Together, these two (tag and plane) control what color is
        shown in set_color_sample() for the current theme.  Button set_color
        invokes get_color() which displays a ColorChooser to change the
        color for the selected tag/plane.  If a new color is picked,
        it will be saved to changes and the highlight_sample and
        frame background will be updated.

        Tk Variables:
            color: Color of selected target.
            builtin_name: Menu variable for built-in theme.
            custom_name: Menu variable for custom theme.
            fg_bg_toggle: Toggle for foreground/background color.
                Note: this has no callback.
            theme_source: Selector for built-in or custom theme.
            highlight_target: Menu variable for the highlight tag target.

        Instance Data Attributes:
            theme_elements: Dictionary of tags for text highlighting.
                The key is the display name and the value is a tuple of
                (tag name, display sort order).

        Methods [attachment]:
            load_theme_cfg: Load current highlight colors.
            get_color: Invoke colorchooser [button_set_color].
            set_color_sample_binding: Call set_color_sample [fg_bg_toggle].
            set_highlight_target: set fg_bg_toggle, set_color_sample().
            set_color_sample: Set frame background to target.
            on_new_color_set: Set new color and add option.
            paint_theme_sample: Recolor sample.
            get_new_theme_name: Get from popup.
            create_new: Combine theme with changes and save.
            save_as_new_theme: Save [button_save_custom].
            set_theme_type: Command for [theme_source].
            delete_custom: Activate default [button_delete_custom].
            save_new: Save to userCfg['theme'] (is function).

        Widgets of highlights page frame:  (*) widgets bound to self
            frame_custom: LabelFrame
                (*)highlight_sample: Text
                (*)frame_color_set: Frame
                    (*)button_set_color: Button
                    (*)targetlist: DynOptionMenu - highlight_target
                frame_fg_bg_toggle: Frame
                    (*)fg_on: Radiobutton - fg_bg_toggle
                    (*)bg_on: Radiobutton - fg_bg_toggle
                (*)button_save_custom: Button
            frame_theme: LabelFrame
                theme_type_title: Label
                (*)builtin_theme_on: Radiobutton - theme_source
                (*)custom_theme_on: Radiobutton - theme_source
                (*)builtinlist: DynOptionMenu - builtin_name
                (*)customlist: DynOptionMenu - custom_name
                (*)button_delete_custom: Button
                (*)theme_message: Label
        """
        self.theme_elements={
            'Normal Text': ('normal', '00'),
            'Python Keywords': ('keyword', '01'),
            'Python Definitions': ('definition', '02'),
            'Python Builtins': ('builtin', '03'),
            'Python Comments': ('comment', '04'),
            'Python Strings': ('string', '05'),
            'Selected Text': ('hilite', '06'),
            'Found Text': ('hit', '07'),
            'Cursor': ('cursor', '08'),
            'Editor Breakpoint': ('break', '09'),
            'Shell Normal Text': ('console', '10'),
            'Shell Error Text': ('error', '11'),
            'Shell Stdout Text': ('stdout', '12'),
            'Shell Stderr Text': ('stderr', '13'),
            }
        parent = self.parent
        self.builtin_name = tracers.add(
                StringVar(parent), self.var_changed_builtin_name)
        self.custom_name = tracers.add(
                StringVar(parent), self.var_changed_custom_name)
        self.fg_bg_toggle = BooleanVar(parent)
        self.color = tracers.add(
                StringVar(parent), self.var_changed_color)
        self.theme_source = tracers.add(
                BooleanVar(parent), self.var_changed_theme_source)
        self.highlight_target = tracers.add(
                StringVar(parent), self.var_changed_highlight_target)

        # Create widgets:
        # body frame and section frames.
        frame = Frame(self.note)
        frame_custom = LabelFrame(frame, borderwidth=2, relief=GROOVE,
                                 text=' Custom Highlighting ')
        frame_theme = LabelFrame(frame, borderwidth=2, relief=GROOVE,
                                text=' Highlighting Theme ')
        # frame_custom.
        text = self.highlight_sample = frame.highlight_sample = Text(
                frame_custom, relief=SOLID, borderwidth=1,
                font=('courier', 12, ''), cursor='hand2', width=21, height=13,
                takefocus=FALSE, highlightthickness=0, wrap=NONE)
        text.bind('<Double-Button-1>', lambda e: 'break')
        text.bind('<B1-Motion>', lambda e: 'break')
        text_and_tags=(('\n', 'normal'),
            ('#you can click here', 'comment'), ('\n', 'normal'),
            ('#to choose items', 'comment'), ('\n', 'normal'),
            ('def', 'keyword'), (' ', 'normal'),
            ('func', 'definition'), ('(param):\n  ', 'normal'),
            ('"""string"""', 'string'), ('\n  var0 = ', 'normal'),
            ("'string'", 'string'), ('\n  var1 = ', 'normal'),
            ("'selected'", 'hilite'), ('\n  var2 = ', 'normal'),
            ("'found'", 'hit'), ('\n  var3 = ', 'normal'),
            ('list', 'builtin'), ('(', 'normal'),
            ('None', 'keyword'), (')\n', 'normal'),
            ('  breakpoint("line")', 'break'), ('\n\n', 'normal'),
            (' error ', 'error'), (' ', 'normal'),
            ('cursor |', 'cursor'), ('\n ', 'normal'),
            ('shell', 'console'), (' ', 'normal'),
            ('stdout', 'stdout'), (' ', 'normal'),
            ('stderr', 'stderr'), ('\n\n', 'normal'))
        for texttag in text_and_tags:
            text.insert(END, texttag[0], texttag[1])
        for element in self.theme_elements:
            def tem(event, elem=element):
                event.widget.winfo_toplevel().highlight_target.set(elem)
            text.tag_bind(
                    self.theme_elements[element][0], '<ButtonPress-1>', tem)
        text['state'] = DISABLED
        self.frame_color_set = Frame(frame_custom, relief=SOLID, borderwidth=1)
        frame_fg_bg_toggle = Frame(frame_custom)
        self.button_set_color = Button(
                self.frame_color_set, text='Choose Color for :',
                command=self.get_color, highlightthickness=0)
        self.targetlist = DynOptionMenu(
                self.frame_color_set, self.highlight_target, None,
                highlightthickness=0) #, command=self.set_highlight_targetBinding
        self.fg_on = Radiobutton(
                frame_fg_bg_toggle, variable=self.fg_bg_toggle, value=1,
                text='Foreground', command=self.set_color_sample_binding)
        self.bg_on = Radiobutton(
                frame_fg_bg_toggle, variable=self.fg_bg_toggle, value=0,
                text='Background', command=self.set_color_sample_binding)
        self.fg_bg_toggle.set(1)
        self.button_save_custom = Button(
                frame_custom, text='Save as New Custom Theme',
                command=self.save_as_new_theme)
        # frame_theme.
        theme_type_title = Label(frame_theme, text='Select : ')
        self.builtin_theme_on = Radiobutton(
                frame_theme, variable=self.theme_source, value=1,
                command=self.set_theme_type, text='a Built-in Theme')
        self.custom_theme_on = Radiobutton(
                frame_theme, variable=self.theme_source, value=0,
                command=self.set_theme_type, text='a Custom Theme')
        self.builtinlist = DynOptionMenu(
                frame_theme, self.builtin_name, None, command=None)
        self.customlist = DynOptionMenu(
                frame_theme, self.custom_name, None, command=None)
        self.button_delete_custom = Button(
                frame_theme, text='Delete Custom Theme',
                command=self.delete_custom)
        self.theme_message = Label(frame_theme, bd=2)

        # Pack widgets:
        # body.
        frame_custom.pack(side=LEFT, padx=5, pady=5, expand=TRUE, fill=BOTH)
        frame_theme.pack(side=LEFT, padx=5, pady=5, fill=Y)
        # frame_custom.
        self.frame_color_set.pack(side=TOP, padx=5, pady=5, expand=TRUE, fill=X)
        frame_fg_bg_toggle.pack(side=TOP, padx=5, pady=0)
        self.highlight_sample.pack(
                side=TOP, padx=5, pady=5, expand=TRUE, fill=BOTH)
        self.button_set_color.pack(side=TOP, expand=TRUE, fill=X, padx=8, pady=4)
        self.targetlist.pack(side=TOP, expand=TRUE, fill=X, padx=8, pady=3)
        self.fg_on.pack(side=LEFT, anchor=E)
        self.bg_on.pack(side=RIGHT, anchor=W)
        self.button_save_custom.pack(side=BOTTOM, fill=X, padx=5, pady=5)
        # frame_theme.
        theme_type_title.pack(side=TOP, anchor=W, padx=5, pady=5)
        self.builtin_theme_on.pack(side=TOP, anchor=W, padx=5)
        self.custom_theme_on.pack(side=TOP, anchor=W, padx=5, pady=2)
        self.builtinlist.pack(side=TOP, fill=X, padx=5, pady=5)
        self.customlist.pack(side=TOP, fill=X, anchor=W, padx=5, pady=5)
        self.button_delete_custom.pack(side=TOP, fill=X, padx=5, pady=5)
        self.theme_message.pack(side=TOP, fill=X, pady=5)
        return frame

    def load_theme_cfg(self):
        """Load current configuration settings for the theme options.

        Based on the theme_source toggle, the theme is set as
        either builtin or custom and the initial widget values
        reflect the current settings from idleConf.

        Attributes updated:
            theme_source: Set from idleConf.
            builtinlist: List of default themes from idleConf.
            customlist: List of custom themes from idleConf.
            custom_theme_on: Disabled if there are no custom themes.
            custom_theme: Message with additional information.
            targetlist: Create menu from self.theme_elements.

        Methods:
            set_theme_type
            paint_theme_sample
            set_highlight_target
        """
        # Set current theme type radiobutton.
        self.theme_source.set(idleConf.GetOption(
                'main', 'Theme', 'default', type='bool', default=1))
        # Set current theme.
        current_option = idleConf.CurrentTheme()
        # Load available theme option menus.
        if self.theme_source.get():  # Default theme selected.
            item_list = idleConf.GetSectionList('default', 'highlight')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('user', 'highlight')
            item_list.sort()
            if not item_list:
                self.custom_theme_on['state'] = DISABLED
                self.custom_name.set('- no custom themes -')
            else:
                self.customlist.SetMenu(item_list, item_list[0])
        else:  # User theme selected.
            item_list = idleConf.GetSectionList('user', 'highlight')
            item_list.sort()
            self.customlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('default', 'highlight')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, item_list[0])
        self.set_theme_type()
        # Load theme element option menu.
        theme_names = list(self.theme_elements.keys())
        theme_names.sort(key=lambda x: self.theme_elements[x][1])
        self.targetlist.SetMenu(theme_names, theme_names[0])
        self.paint_theme_sample()
        self.set_highlight_target()

    def var_changed_builtin_name(self, *params):
        """Process new builtin theme selection.

        Add the changed theme's name to the changed_items and recreate
        the sample with the values from the selected theme.
        """
        old_themes = ('IDLE Classic', 'IDLE New')
        value = self.builtin_name.get()
        if value not in old_themes:
            if idleConf.GetOption('main', 'Theme', 'name') not in old_themes:
                changes.add_option('main', 'Theme', 'name', old_themes[0])
            changes.add_option('main', 'Theme', 'name2', value)
            self.theme_message['text'] = 'New theme, see Help'
            self.theme_message['fg'] = '#500000'
        else:
            changes.add_option('main', 'Theme', 'name', value)
            changes.add_option('main', 'Theme', 'name2', '')
            self.theme_message['text'] = ''
            self.theme_message['fg'] = 'black'
        self.paint_theme_sample()

    def var_changed_custom_name(self, *params):
        """Process new custom theme selection.

        If a new custom theme is selected, add the name to the
        changed_items and apply the theme to the sample.
        """
        value = self.custom_name.get()
        if value != '- no custom themes -':
            changes.add_option('main', 'Theme', 'name', value)
            self.paint_theme_sample()

    def var_changed_theme_source(self, *params):
        """Process toggle between builtin and custom theme.

        Update the default toggle value and apply the newly
        selected theme type.
        """
        value = self.theme_source.get()
        changes.add_option('main', 'Theme', 'default', value)
        if value:
            self.var_changed_builtin_name()
        else:
            self.var_changed_custom_name()

    def var_changed_color(self, *params):
        "Process change to color choice."
        self.on_new_color_set()

    def var_changed_highlight_target(self, *params):
        "Process selection of new target tag for highlighting."
        self.set_highlight_target()

    def set_theme_type(self):
        """Set available screen options based on builtin or custom theme.

        Attributes accessed:
            theme_source

        Attributes updated:
            builtinlist
            customlist
            button_delete_custom
            custom_theme_on

        Called from:
            handler for builtin_theme_on and custom_theme_on
            delete_custom
            create_new
            load_theme_cfg
        """
        if self.theme_source.get():
            self.builtinlist['state'] = NORMAL
            self.customlist['state'] = DISABLED
            self.button_delete_custom['state'] = DISABLED
        else:
            self.builtinlist['state'] = DISABLED
            self.custom_theme_on['state'] = NORMAL
            self.customlist['state'] = NORMAL
            self.button_delete_custom['state'] = NORMAL

    def get_color(self):
        """Handle button to select a new color for the target tag.

        If a new color is selected while using a builtin theme, a
        name must be supplied to create a custom theme.

        Attributes accessed:
            highlight_target
            frame_color_set
            theme_source

        Attributes updated:
            color

        Methods:
            get_new_theme_name
            create_new
        """
        target = self.highlight_target.get()
        prev_color = self.frame_color_set.cget('bg')
        rgbTuplet, color_string = tkColorChooser.askcolor(
                parent=self, title='Pick new color for : '+target,
                initialcolor=prev_color)
        if color_string and (color_string != prev_color):
            # User didn't cancel and they chose a new color.
            if self.theme_source.get():  # Current theme is a built-in.
                message = ('Your changes will be saved as a new Custom Theme. '
                           'Enter a name for your new Custom Theme below.')
                new_theme = self.get_new_theme_name(message)
                if not new_theme:  # User cancelled custom theme creation.
                    return
                else:  # Create new custom theme based on previously active theme.
                    self.create_new(new_theme)
                    self.color.set(color_string)
            else:  # Current theme is user defined.
                self.color.set(color_string)

    def on_new_color_set(self):
        "Display sample of new color selection on the dialog."
        new_color = self.color.get()
        self.frame_color_set['bg'] = new_color  # Set sample.
        plane = 'foreground' if self.fg_bg_toggle.get() else 'background'
        sample_element = self.theme_elements[self.highlight_target.get()][0]
        self.highlight_sample.tag_config(sample_element, **{plane: new_color})
        theme = self.custom_name.get()
        theme_element = sample_element + '-' + plane
        changes.add_option('highlight', theme, theme_element, new_color)

    def get_new_theme_name(self, message):
        "Return name of new theme from query popup."
        used_names = (idleConf.GetSectionList('user', 'highlight') +
                idleConf.GetSectionList('default', 'highlight'))
        new_theme = SectionName(
                self, 'New Custom Theme', message, used_names).result
        return new_theme

    def save_as_new_theme(self):
        """Prompt for new theme name and create the theme.

        Methods:
            get_new_theme_name
            create_new
        """
        new_theme_name = self.get_new_theme_name('New Theme Name:')
        if new_theme_name:
            self.create_new(new_theme_name)

    def create_new(self, new_theme_name):
        """Create a new custom theme with the given name.

        Create the new theme based on the previously active theme
        with the current changes applied.  Once it is saved, then
        activate the new theme.

        Attributes accessed:
            builtin_name
            custom_name

        Attributes updated:
            customlist
            theme_source

        Method:
            save_new
            set_theme_type
        """
        if self.theme_source.get():
            theme_type = 'default'
            theme_name = self.builtin_name.get()
        else:
            theme_type = 'user'
            theme_name = self.custom_name.get()
        new_theme = idleConf.GetThemeDict(theme_type, theme_name)
        # Apply any of the old theme's unsaved changes to the new theme.
        if theme_name in changes['highlight']:
            theme_changes = changes['highlight'][theme_name]
            for element in theme_changes:
                new_theme[element] = theme_changes[element]
        # Save the new theme.
        self.save_new(new_theme_name, new_theme)
        # Change GUI over to the new theme.
        custom_theme_list = idleConf.GetSectionList('user', 'highlight')
        custom_theme_list.sort()
        self.customlist.SetMenu(custom_theme_list, new_theme_name)
        self.theme_source.set(0)
        self.set_theme_type()

    def set_highlight_target(self):
        """Set fg/bg toggle and color based on highlight tag target.

        Instance variables accessed:
            highlight_target

        Attributes updated:
            fg_on
            bg_on
            fg_bg_toggle

        Methods:
            set_color_sample

        Called from:
            var_changed_highlight_target
            load_theme_cfg
        """
        if self.highlight_target.get() == 'Cursor':  # bg not possible
            self.fg_on['state'] = DISABLED
            self.bg_on['state'] = DISABLED
            self.fg_bg_toggle.set(1)
        else:  # Both fg and bg can be set.
            self.fg_on['state'] = NORMAL
            self.bg_on['state'] = NORMAL
            self.fg_bg_toggle.set(1)
        self.set_color_sample()

    def set_color_sample_binding(self, *args):
        """Change color sample based on foreground/background toggle.

        Methods:
            set_color_sample
        """
        self.set_color_sample()

    def set_color_sample(self):
        """Set the color of the frame background to reflect the selected target.

        Instance variables accessed:
            theme_elements
            highlight_target
            fg_bg_toggle
            highlight_sample

        Attributes updated:
            frame_color_set
        """
        # Set the color sample area.
        tag = self.theme_elements[self.highlight_target.get()][0]
        plane = 'foreground' if self.fg_bg_toggle.get() else 'background'
        color = self.highlight_sample.tag_cget(tag, plane)
        self.frame_color_set['bg'] = color

    def paint_theme_sample(self):
        """Apply the theme colors to each element tag in the sample text.

        Instance attributes accessed:
            theme_elements
            theme_source
            builtin_name
            custom_name

        Attributes updated:
            highlight_sample: Set the tag elements to the theme.

        Methods:
            set_color_sample

        Called from:
            var_changed_builtin_name
            var_changed_custom_name
            load_theme_cfg
        """
        if self.theme_source.get():  # Default theme
            theme = self.builtin_name.get()
        else:  # User theme
            theme = self.custom_name.get()
        for element_title in self.theme_elements:
            element = self.theme_elements[element_title][0]
            colors = idleConf.GetHighlight(theme, element)
            if element == 'cursor':  # Cursor sample needs special painting.
                colors['background'] = idleConf.GetHighlight(
                        theme, 'normal', fgBg='bg')
            # Handle any unsaved changes to this theme.
            if theme in changes['highlight']:
                theme_dict = changes['highlight'][theme]
                if element + '-foreground' in theme_dict:
                    colors['foreground'] = theme_dict[element + '-foreground']
                if element + '-background' in theme_dict:
                    colors['background'] = theme_dict[element + '-background']
            self.highlight_sample.tag_config(element, **colors)
        self.set_color_sample()

    def save_new(self, theme_name, theme):
        """Save a newly created theme to idleConf.

        theme_name - string, the name of the new theme
        theme - dictionary containing the new theme
        """
        if not idleConf.userCfg['highlight'].has_section(theme_name):
            idleConf.userCfg['highlight'].add_section(theme_name)
        for element in theme:
            value = theme[element]
            idleConf.userCfg['highlight'].SetOption(theme_name, element, value)

    def delete_custom(self):
        """Handle event to delete custom theme.

        The current theme is deactivated and the default theme is
        activated.  The custom theme is permanently removed from
        the config file.

        Attributes accessed:
            custom_name

        Attributes updated:
            custom_theme_on
            customlist
            theme_source
            builtin_name

        Methods:
            deactivate_current_config
            save_all_changed_extensions
            activate_config_changes
            set_theme_type
        """
        theme_name = self.custom_name.get()
        delmsg = 'Are you sure you wish to delete the theme %r ?'
        if not tkMessageBox.askyesno(
                'Delete Theme',  delmsg % theme_name, parent=self):
            return
        self.deactivate_current_config()
        # Remove theme from changes, config, and file.
        changes.delete_section('highlight', theme_name)
        # Reload user theme list.
        item_list = idleConf.GetSectionList('user', 'highlight')
        item_list.sort()
        if not item_list:
            self.custom_theme_on['state'] = DISABLED
            self.customlist.SetMenu(item_list, '- no custom themes -')
        else:
            self.customlist.SetMenu(item_list, item_list[0])
        # Revert to default theme.
        self.theme_source.set(idleConf.defaultCfg['main'].Get('Theme', 'default'))
        self.builtin_name.set(idleConf.defaultCfg['main'].Get('Theme', 'name'))
        # User can't back out of these changes, they must be applied now.
        changes.save_all()
        self.save_all_changed_extensions()
        self.activate_config_changes()
        self.set_theme_type()

    def deactivate_current_config(self):
        """Remove current key bindings.

        Iterate over window instances defined in parent and remove
        the keybindings.
        """
        # Before a config is saved, some cleanup of current
        # config must be done - remove the previous keybindings.
        win_instances = self.parent.instance_dict.keys()
        for instance in win_instances:
            instance.RemoveKeybindings()

    def activate_config_changes(self):
        """Apply configuration changes to current windows.

        Dynamically update the current parent window instances
        with some of the configuration changes.
        """
        win_instances = self.parent.instance_dict.keys()
        for instance in win_instances:
            instance.ResetColorizer()
            instance.ResetFont()
            instance.set_notabs_indentwidth()
            instance.ApplyKeybindings()
            instance.reset_help_menu_entries()

    def create_page_extensions(self):
        """Part of the config dialog used for configuring IDLE extensions.

        This code is generic - it works for any and all IDLE extensions.

        IDLE extensions save their configuration options using idleConf.
        This code reads the current configuration using idleConf, supplies a
        GUI interface to change the configuration values, and saves the
        changes using idleConf.

        Not all changes take effect immediately - some may require restarting IDLE.
        This depends on each extension's implementation.

        All values are treated as text, and it is up to the user to supply
        reasonable values. The only exception to this are the 'enable*' options,
        which are boolean, and can be toggled with a True/False button.

        Methods:
            load_extensions:
            extension_selected: Handle selection from list.
            create_extension_frame: Hold widgets for one extension.
            set_extension_value: Set in userCfg['extensions'].
            save_all_changed_extensions: Call extension page Save().
        """
        parent = self.parent
        frame = Frame(self.note)
        self.ext_defaultCfg = idleConf.defaultCfg['extensions']
        self.ext_userCfg = idleConf.userCfg['extensions']
        self.is_int = self.register(is_int)
        self.load_extensions()
        # Create widgets - a listbox shows all available extensions, with the
        # controls for the extension selected in the listbox to the right.
        self.extension_names = StringVar(self)
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(2, weight=1)
        self.extension_list = Listbox(frame, listvariable=self.extension_names,
                                      selectmode='browse')
        self.extension_list.bind('<<ListboxSelect>>', self.extension_selected)
        scroll = Scrollbar(frame, command=self.extension_list.yview)
        self.extension_list.yscrollcommand=scroll.set
        self.details_frame = LabelFrame(frame, width=250, height=250)
        self.extension_list.grid(column=0, row=0, sticky='nws')
        scroll.grid(column=1, row=0, sticky='ns')
        self.details_frame.grid(column=2, row=0, sticky='nsew', padx=[10, 0])
        frame.configure(padx=10, pady=10)
        self.config_frame = {}
        self.current_extension = None

        self.outerframe = self                      # TEMPORARY
        self.tabbed_page_set = self.extension_list  # TEMPORARY

        # Create the frame holding controls for each extension.
        ext_names = ''
        for ext_name in sorted(self.extensions):
            self.create_extension_frame(ext_name)
            ext_names = ext_names + '{' + ext_name + '} '
        self.extension_names.set(ext_names)
        self.extension_list.selection_set(0)
        self.extension_selected(None)

        return frame

    def load_extensions(self):
        "Fill self.extensions with data from the default and user configs."
        self.extensions = {}
        for ext_name in idleConf.GetExtensions(active_only=False):
            self.extensions[ext_name] = []

        for ext_name in self.extensions:
            opt_list = sorted(self.ext_defaultCfg.GetOptionList(ext_name))

            # Bring 'enable' options to the beginning of the list.
            enables = [opt_name for opt_name in opt_list
                       if opt_name.startswith('enable')]
            for opt_name in enables:
                opt_list.remove(opt_name)
            opt_list = enables + opt_list

            for opt_name in opt_list:
                def_str = self.ext_defaultCfg.Get(
                        ext_name, opt_name, raw=True)
                try:
                    def_obj = {'True':True, 'False':False}[def_str]
                    opt_type = 'bool'
                except KeyError:
                    try:
                        def_obj = int(def_str)
                        opt_type = 'int'
                    except ValueError:
                        def_obj = def_str
                        opt_type = None
                try:
                    value = self.ext_userCfg.Get(
                            ext_name, opt_name, type=opt_type, raw=True,
                            default=def_obj)
                except ValueError:  # Need this until .Get fixed.
                    value = def_obj  # Bad values overwritten by entry.
                var = StringVar(self)
                var.set(str(value))

                self.extensions[ext_name].append({'name': opt_name,
                                                  'type': opt_type,
                                                  'default': def_str,
                                                  'value': value,
                                                  'var': var,
                                                 })

    def extension_selected(self, event):
        "Handle selection of an extension from the list."
        newsel = self.extension_list.curselection()
        if newsel:
            newsel = self.extension_list.get(newsel)
        if newsel is None or newsel != self.current_extension:
            if self.current_extension:
                self.details_frame.config(text='')
                self.config_frame[self.current_extension].grid_forget()
                self.current_extension = None
        if newsel:
            self.details_frame.config(text=newsel)
            self.config_frame[newsel].grid(column=0, row=0, sticky='nsew')
            self.current_extension = newsel

    def create_extension_frame(self, ext_name):
        """Create a frame holding the widgets to configure one extension"""
        f = VerticalScrolledFrame(self.details_frame, height=250, width=250)
        self.config_frame[ext_name] = f
        entry_area = f.interior
        # Create an entry for each configuration option.
        for row, opt in enumerate(self.extensions[ext_name]):
            # Create a row with a label and entry/checkbutton.
            label = Label(entry_area, text=opt['name'])
            label.grid(row=row, column=0, sticky=NW)
            var = opt['var']
            if opt['type'] == 'bool':
                Checkbutton(entry_area, textvariable=var, variable=var,
                            onvalue='True', offvalue='False',
                            indicatoron=FALSE, selectcolor='', width=8
                            ).grid(row=row, column=1, sticky=W, padx=7)
            elif opt['type'] == 'int':
                Entry(entry_area, textvariable=var, validate='key',
                      validatecommand=(self.is_int, '%P')
                      ).grid(row=row, column=1, sticky=NSEW, padx=7)

            else:
                Entry(entry_area, textvariable=var
                      ).grid(row=row, column=1, sticky=NSEW, padx=7)
        return

    def set_extension_value(self, section, opt):
        """Return True if the configuration was added or changed.

        If the value is the same as the default, then remove it
        from user config file.
        """
        name = opt['name']
        default = opt['default']
        value = opt['var'].get().strip() or default
        opt['var'].set(value)
        # if self.defaultCfg.has_section(section):
        # Currently, always true; if not, indent to return.
        if (value == default):
            return self.ext_userCfg.RemoveOption(section, name)
        # Set the option.
        return self.ext_userCfg.SetOption(section, name, value)

    def save_all_changed_extensions(self):
        """Save configuration changes to the user config file.

        Attributes accessed:
            extensions

        Methods:
            set_extension_value
        """
        has_changes = False
        for ext_name in self.extensions:
            options = self.extensions[ext_name]
            for opt in options:
                if self.set_extension_value(ext_name, opt):
                    has_changes = True
        if has_changes:
            self.ext_userCfg.Save()


# class TabPage(Frame):  # A template for Page classes.
#     def __init__(self, master):
#         super().__init__(master)
#         self.create_page_tab()
#         self.load_tab_cfg()
#     def create_page_tab(self):
#         # Define tk vars and register var and callback with tracers.
#         # Create subframes and widgets.
#         # Pack widgets.
#     def load_tab_cfg(self):
#         # Initialize widgets with data from idleConf.
#     def var_changed_var_name():
#         # For each tk var that needs other than default callback.
#     def other_methods():
#         # Define tab-specific behavior.


class FontPage(Frame):

    def __init__(self, master, highpage):
        super().__init__(master)
        self.highlight_sample = highpage.highlight_sample
        self.create_page_font_tab()
        self.load_font_cfg()
        self.load_tab_cfg()

    def create_page_font_tab(self):
        """Return frame of widgets for Font/Tabs tab.

        Fonts: Enable users to provisionally change font face, size, or
        boldness and to see the consequence of proposed choices.  Each
        action set 3 options in changes structuree and changes the
        corresponding aspect of the font sample on this page and
        highlight sample on highlight page.

        Function load_font_cfg initializes font vars and widgets from
        idleConf entries and tk.

        Fontlist: mouse button 1 click or up or down key invoke
        on_fontlist_select(), which sets var font_name.

        Sizelist: clicking the menubutton opens the dropdown menu. A
        mouse button 1 click or return key sets var font_size.

        Bold_toggle: clicking the box toggles var font_bold.

        Changing any of the font vars invokes var_changed_font, which
        adds all 3 font options to changes and calls set_samples.
        Set_samples applies a new font constructed from the font vars to
        font_sample and to highlight_sample on the hightlight page.

        Tabs: Enable users to change spaces entered for indent tabs.
        Changing indent_scale value with the mouse sets Var space_num,
        which invokes the default callback to add an entry to
        changes.  Load_tab_cfg initializes space_num to default.

        Widgets for FontPage(Frame):  (*) widgets bound to self
            frame_font: LabelFrame
                frame_font_name: Frame
                    font_name_title: Label
                    (*)fontlist: ListBox - font_name
                    scroll_font: Scrollbar
                frame_font_param: Frame
                    font_size_title: Label
                    (*)sizelist: DynOptionMenu - font_size
                    (*)bold_toggle: Checkbutton - font_bold
                frame_font_sample: Frame
                    (*)font_sample: Label
            frame_indent: LabelFrame
                    indent_title: Label
                    (*)indent_scale: Scale - space_num
        """
        self.font_name = tracers.add(StringVar(self), self.var_changed_font)
        self.font_size = tracers.add(StringVar(self), self.var_changed_font)
        self.font_bold = tracers.add(BooleanVar(self), self.var_changed_font)
        self.space_num = tracers.add(IntVar(self), ('main', 'Indent', 'num-spaces'))

        # Create widgets:
        # body and body section frames.
        frame_font = LabelFrame(
                self, borderwidth=2, relief=GROOVE, text=' Base Editor Font ')
        frame_indent = LabelFrame(
                self, borderwidth=2, relief=GROOVE, text=' Indentation Width ')
        # frame_font.
        frame_font_name = Frame(frame_font)
        frame_font_param = Frame(frame_font)
        font_name_title = Label(
                frame_font_name, justify=LEFT, text='Font Face :')
        self.fontlist = Listbox(frame_font_name, height=5,
                                takefocus=True, exportselection=FALSE)
        self.fontlist.bind('<ButtonRelease-1>', self.on_fontlist_select)
        self.fontlist.bind('<KeyRelease-Up>', self.on_fontlist_select)
        self.fontlist.bind('<KeyRelease-Down>', self.on_fontlist_select)
        scroll_font = Scrollbar(frame_font_name)
        scroll_font.config(command=self.fontlist.yview)
        self.fontlist.config(yscrollcommand=scroll_font.set)
        font_size_title = Label(frame_font_param, text='Size :')
        self.sizelist = DynOptionMenu(frame_font_param, self.font_size, None)
        self.bold_toggle = Checkbutton(
                frame_font_param, variable=self.font_bold,
                onvalue=1, offvalue=0, text='Bold')
        frame_font_sample = Frame(frame_font, relief=SOLID, borderwidth=1)
        temp_font = tkFont.Font(self, ('courier', 10, 'normal'))
        self.font_sample = Label(
                frame_font_sample, justify=LEFT, font=temp_font,
                text='AaBbCcDdEe\nFfGgHhIiJj\n1234567890\n#:+=(){}[]')
        # frame_indent.
        indent_title = Label(
                frame_indent, justify=LEFT,
                text='Python Standard: 4 Spaces!')
        self.indent_scale = Scale(
                frame_indent, variable=self.space_num,
                orient='horizontal', tickinterval=2, from_=2, to=16)

        # Pack widgets:
        # body.
        frame_font.pack(side=LEFT, padx=5, pady=5, expand=TRUE, fill=BOTH)
        frame_indent.pack(side=LEFT, padx=5, pady=5, fill=Y)
        # frame_font.
        frame_font_name.pack(side=TOP, padx=5, pady=5, fill=X)
        frame_font_param.pack(side=TOP, padx=5, pady=5, fill=X)
        font_name_title.pack(side=TOP, anchor=W)
        self.fontlist.pack(side=LEFT, expand=TRUE, fill=X)
        scroll_font.pack(side=LEFT, fill=Y)
        font_size_title.pack(side=LEFT, anchor=W)
        self.sizelist.pack(side=LEFT, anchor=W)
        self.bold_toggle.pack(side=LEFT, anchor=W, padx=20)
        frame_font_sample.pack(side=TOP, padx=5, pady=5, expand=TRUE, fill=BOTH)
        self.font_sample.pack(expand=TRUE, fill=BOTH)
        # frame_indent.
        frame_indent.pack(side=TOP, fill=X)
        indent_title.pack(side=TOP, anchor=W, padx=5)
        self.indent_scale.pack(side=TOP, padx=5, fill=X)

    def load_font_cfg(self):
        """Load current configuration settings for the font options.

        Retrieve current font with idleConf.GetFont and font families
        from tk. Setup fontlist and set font_name.  Setup sizelist,
        which sets font_size.  Set font_bold.  Call set_samples.
        """
        configured_font = idleConf.GetFont(self, 'main', 'EditorWindow')
        font_name = configured_font[0].lower()
        font_size = configured_font[1]
        font_bold  = configured_font[2]=='bold'

        # Set editor font selection list and font_name.
        fonts = list(tkFont.families(self))
        fonts.sort()
        for font in fonts:
            self.fontlist.insert(END, font)
        self.font_name.set(font_name)
        lc_fonts = [s.lower() for s in fonts]
        try:
            current_font_index = lc_fonts.index(font_name)
            self.fontlist.see(current_font_index)
            self.fontlist.select_set(current_font_index)
            self.fontlist.select_anchor(current_font_index)
            self.fontlist.activate(current_font_index)
        except ValueError:
            pass
        # Set font size dropdown.
        self.sizelist.SetMenu(('7', '8', '9', '10', '11', '12', '13', '14',
                               '16', '18', '20', '22', '25', '29', '34', '40'),
                              font_size)
        # Set font weight.
        self.font_bold.set(font_bold)
        self.set_samples()

    def var_changed_font(self, *params):
        """Store changes to font attributes.

        When one font attribute changes, save them all, as they are
        not independent from each other. In particular, when we are
        overriding the default font, we need to write out everything.
        """
        value = self.font_name.get()
        changes.add_option('main', 'EditorWindow', 'font', value)
        value = self.font_size.get()
        changes.add_option('main', 'EditorWindow', 'font-size', value)
        value = self.font_bold.get()
        changes.add_option('main', 'EditorWindow', 'font-bold', value)
        self.set_samples()

    def on_fontlist_select(self, event):
        """Handle selecting a font from the list.

        Event can result from either mouse click or Up or Down key.
        Set font_name and example displays to selection.
        """
        font = self.fontlist.get(
                ACTIVE if event.type.name == 'KeyRelease' else ANCHOR)
        self.font_name.set(font.lower())

    def set_samples(self, event=None):
        """Update update both screen samples with the font settings.

        Called on font initialization and change events.
        Accesses font_name, font_size, and font_bold Variables.
        Updates font_sample and hightlight page highlight_sample.
        """
        font_name = self.font_name.get()
        font_weight = tkFont.BOLD if self.font_bold.get() else tkFont.NORMAL
        new_font = (font_name, self.font_size.get(), font_weight)
        self.font_sample['font'] = new_font
        self.highlight_sample['font'] = new_font

    def load_tab_cfg(self):
        """Load current configuration settings for the tab options.

        Attributes updated:
            space_num: Set to value from idleConf.
        """
        # Set indent sizes.
        space_num = idleConf.GetOption(
            'main', 'Indent', 'num-spaces', default=4, type='int')
        self.space_num.set(space_num)

    def var_changed_space_num(self, *params):
        "Store change to indentation size."
        value = self.space_num.get()
        changes.add_option('main', 'Indent', 'num-spaces', value)


class HighPage(Frame):

    def __init__(self, master):
        super().__init__(master)
        self.cd = master.master
        self.create_page_highlight()
        self.load_theme_cfg()

    def create_page_highlight(self):
        """Return frame of widgets for Highlighting tab.

        Enable users to provisionally change foreground and background
        colors applied to textual tags.  Color mappings are stored in
        complete listings called themes.  Built-in themes in
        idlelib/config-highlight.def are fixed as far as the dialog is
        concerned. Any theme can be used as the base for a new custom
        theme, stored in .idlerc/config-highlight.cfg.

        Function load_theme_cfg() initializes tk variables and theme
        lists and calls paint_theme_sample() and set_highlight_target()
        for the current theme.  Radiobuttons builtin_theme_on and
        custom_theme_on toggle var theme_source, which controls if the
        current set of colors are from a builtin or custom theme.
        DynOptionMenus builtinlist and customlist contain lists of the
        builtin and custom themes, respectively, and the current item
        from each list is stored in vars builtin_name and custom_name.

        Function paint_theme_sample() applies the colors from the theme
        to the tags in text widget highlight_sample and then invokes
        set_color_sample().  Function set_highlight_target() sets the state
        of the radiobuttons fg_on and bg_on based on the tag and it also
        invokes set_color_sample().

        Function set_color_sample() sets the background color for the frame
        holding the color selector.  This provides a larger visual of the
        color for the current tag and plane (foreground/background).

        Note: set_color_sample() is called from many places and is often
        called more than once when a change is made.  It is invoked when
        foreground or background is selected (radiobuttons), from
        paint_theme_sample() (theme is changed or load_cfg is called), and
        from set_highlight_target() (target tag is changed or load_cfg called).

        Button delete_custom invokes delete_custom() to delete
        a custom theme from idleConf.userCfg['highlight'] and changes.
        Button save_custom invokes save_as_new_theme() which calls
        get_new_theme_name() and create_new() to save a custom theme
        and its colors to idleConf.userCfg['highlight'].

        Radiobuttons fg_on and bg_on toggle var fg_bg_toggle to control
        if the current selected color for a tag is for the foreground or
        background.

        DynOptionMenu targetlist contains a readable description of the
        tags applied to Python source within IDLE.  Selecting one of the
        tags from this list populates highlight_target, which has a callback
        function set_highlight_target().

        Text widget highlight_sample displays a block of text (which is
        mock Python code) in which is embedded the defined tags and reflects
        the color attributes of the current theme and changes for those tags.
        Mouse button 1 allows for selection of a tag and updates
        highlight_target with that tag value.

        Note: The font in highlight_sample is set through the config in
        the fonts tab.

        In other words, a tag can be selected either from targetlist or
        by clicking on the sample text within highlight_sample.  The
        plane (foreground/background) is selected via the radiobutton.
        Together, these two (tag and plane) control what color is
        shown in set_color_sample() for the current theme.  Button set_color
        invokes get_color() which displays a ColorChooser to change the
        color for the selected tag/plane.  If a new color is picked,
        it will be saved to changes and the highlight_sample and
        frame background will be updated.

        Tk Variables:
            color: Color of selected target.
            builtin_name: Menu variable for built-in theme.
            custom_name: Menu variable for custom theme.
            fg_bg_toggle: Toggle for foreground/background color.
                Note: this has no callback.
            theme_source: Selector for built-in or custom theme.
            highlight_target: Menu variable for the highlight tag target.

        Instance Data Attributes:
            theme_elements: Dictionary of tags for text highlighting.
                The key is the display name and the value is a tuple of
                (tag name, display sort order).

        Methods [attachment]:
            load_theme_cfg: Load current highlight colors.
            get_color: Invoke colorchooser [button_set_color].
            set_color_sample_binding: Call set_color_sample [fg_bg_toggle].
            set_highlight_target: set fg_bg_toggle, set_color_sample().
            set_color_sample: Set frame background to target.
            on_new_color_set: Set new color and add option.
            paint_theme_sample: Recolor sample.
            get_new_theme_name: Get from popup.
            create_new: Combine theme with changes and save.
            save_as_new_theme: Save [button_save_custom].
            set_theme_type: Command for [theme_source].
            delete_custom: Activate default [button_delete_custom].
            save_new: Save to userCfg['theme'] (is function).

        Widgets of highlights page frame:  (*) widgets bound to self
            frame_custom: LabelFrame
                (*)highlight_sample: Text
                (*)frame_color_set: Frame
                    (*)button_set_color: Button
                    (*)targetlist: DynOptionMenu - highlight_target
                frame_fg_bg_toggle: Frame
                    (*)fg_on: Radiobutton - fg_bg_toggle
                    (*)bg_on: Radiobutton - fg_bg_toggle
                (*)button_save_custom: Button
            frame_theme: LabelFrame
                theme_type_title: Label
                (*)builtin_theme_on: Radiobutton - theme_source
                (*)custom_theme_on: Radiobutton - theme_source
                (*)builtinlist: DynOptionMenu - builtin_name
                (*)customlist: DynOptionMenu - custom_name
                (*)button_delete_custom: Button
                (*)theme_message: Label
        """
        self.theme_elements = {
            'Normal Text': ('normal', '00'),
            'Python Keywords': ('keyword', '01'),
            'Python Definitions': ('definition', '02'),
            'Python Builtins': ('builtin', '03'),
            'Python Comments': ('comment', '04'),
            'Python Strings': ('string', '05'),
            'Selected Text': ('hilite', '06'),
            'Found Text': ('hit', '07'),
            'Cursor': ('cursor', '08'),
            'Editor Breakpoint': ('break', '09'),
            'Shell Normal Text': ('console', '10'),
            'Shell Error Text': ('error', '11'),
            'Shell Stdout Text': ('stdout', '12'),
            'Shell Stderr Text': ('stderr', '13'),
            }
        self.builtin_name = tracers.add(
                StringVar(self), self.var_changed_builtin_name)
        self.custom_name = tracers.add(
                StringVar(self), self.var_changed_custom_name)
        self.fg_bg_toggle = BooleanVar(self)
        self.color = tracers.add(
                StringVar(self), self.var_changed_color)
        self.theme_source = tracers.add(
                BooleanVar(self), self.var_changed_theme_source)
        self.highlight_target = tracers.add(
                StringVar(self), self.var_changed_highlight_target)

        # Create widgets:
        # body frame and section frames.
        frame_custom = LabelFrame(self, borderwidth=2, relief=GROOVE,
                                  text=' Custom Highlighting ')
        frame_theme = LabelFrame(self, borderwidth=2, relief=GROOVE,
                                 text=' Highlighting Theme ')
        # frame_custom.
        text = self.highlight_sample = Text(
                frame_custom, relief=SOLID, borderwidth=1,
                font=('courier', 12, ''), cursor='hand2', width=21, height=13,
                takefocus=FALSE, highlightthickness=0, wrap=NONE)
        text.bind('<Double-Button-1>', lambda e: 'break')
        text.bind('<B1-Motion>', lambda e: 'break')
        text_and_tags=(('\n', 'normal'),
            ('#you can click here', 'comment'), ('\n', 'normal'),
            ('#to choose items', 'comment'), ('\n', 'normal'),
            ('def', 'keyword'), (' ', 'normal'),
            ('func', 'definition'), ('(param):\n  ', 'normal'),
            ('"""string"""', 'string'), ('\n  var0 = ', 'normal'),
            ("'string'", 'string'), ('\n  var1 = ', 'normal'),
            ("'selected'", 'hilite'), ('\n  var2 = ', 'normal'),
            ("'found'", 'hit'), ('\n  var3 = ', 'normal'),
            ('list', 'builtin'), ('(', 'normal'),
            ('None', 'keyword'), (')\n', 'normal'),
            ('  breakpoint("line")', 'break'), ('\n\n', 'normal'),
            (' error ', 'error'), (' ', 'normal'),
            ('cursor |', 'cursor'), ('\n ', 'normal'),
            ('shell', 'console'), (' ', 'normal'),
            ('stdout', 'stdout'), (' ', 'normal'),
            ('stderr', 'stderr'), ('\n\n', 'normal'))
        for texttag in text_and_tags:
            text.insert(END, texttag[0], texttag[1])
        for element in self.theme_elements:
            def tem(event, elem=element):
                event.widget.winfo_toplevel().highlight_target.set(elem)
            text.tag_bind(
                    self.theme_elements[element][0], '<ButtonPress-1>', tem)
        text['state'] = DISABLED
        self.frame_color_set = Frame(frame_custom, relief=SOLID, borderwidth=1)
        frame_fg_bg_toggle = Frame(frame_custom)
        self.button_set_color = Button(
                self.frame_color_set, text='Choose Color for :',
                command=self.get_color, highlightthickness=0)
        self.targetlist = DynOptionMenu(
                self.frame_color_set, self.highlight_target, None,
                highlightthickness=0) #, command=self.set_highlight_targetBinding
        self.fg_on = Radiobutton(
                frame_fg_bg_toggle, variable=self.fg_bg_toggle, value=1,
                text='Foreground', command=self.set_color_sample_binding)
        self.bg_on = Radiobutton(
                frame_fg_bg_toggle, variable=self.fg_bg_toggle, value=0,
                text='Background', command=self.set_color_sample_binding)
        self.fg_bg_toggle.set(1)
        self.button_save_custom = Button(
                frame_custom, text='Save as New Custom Theme',
                command=self.save_as_new_theme)
        # frame_theme.
        theme_type_title = Label(frame_theme, text='Select : ')
        self.builtin_theme_on = Radiobutton(
                frame_theme, variable=self.theme_source, value=1,
                command=self.set_theme_type, text='a Built-in Theme')
        self.custom_theme_on = Radiobutton(
                frame_theme, variable=self.theme_source, value=0,
                command=self.set_theme_type, text='a Custom Theme')
        self.builtinlist = DynOptionMenu(
                frame_theme, self.builtin_name, None, command=None)
        self.customlist = DynOptionMenu(
                frame_theme, self.custom_name, None, command=None)
        self.button_delete_custom = Button(
                frame_theme, text='Delete Custom Theme',
                command=self.delete_custom)
        self.theme_message = Label(frame_theme, bd=2)

        # Pack widgets:
        # body.
        frame_custom.pack(side=LEFT, padx=5, pady=5, expand=TRUE, fill=BOTH)
        frame_theme.pack(side=LEFT, padx=5, pady=5, fill=Y)
        # frame_custom.
        self.frame_color_set.pack(side=TOP, padx=5, pady=5, expand=TRUE, fill=X)
        frame_fg_bg_toggle.pack(side=TOP, padx=5, pady=0)
        self.highlight_sample.pack(
                side=TOP, padx=5, pady=5, expand=TRUE, fill=BOTH)
        self.button_set_color.pack(side=TOP, expand=TRUE, fill=X, padx=8, pady=4)
        self.targetlist.pack(side=TOP, expand=TRUE, fill=X, padx=8, pady=3)
        self.fg_on.pack(side=LEFT, anchor=E)
        self.bg_on.pack(side=RIGHT, anchor=W)
        self.button_save_custom.pack(side=BOTTOM, fill=X, padx=5, pady=5)
        # frame_theme.
        theme_type_title.pack(side=TOP, anchor=W, padx=5, pady=5)
        self.builtin_theme_on.pack(side=TOP, anchor=W, padx=5)
        self.custom_theme_on.pack(side=TOP, anchor=W, padx=5, pady=2)
        self.builtinlist.pack(side=TOP, fill=X, padx=5, pady=5)
        self.customlist.pack(side=TOP, fill=X, anchor=W, padx=5, pady=5)
        self.button_delete_custom.pack(side=TOP, fill=X, padx=5, pady=5)
        self.theme_message.pack(side=TOP, fill=X, pady=5)

    def load_theme_cfg(self):
        """Load current configuration settings for the theme options.

        Based on the theme_source toggle, the theme is set as
        either builtin or custom and the initial widget values
        reflect the current settings from idleConf.

        Attributes updated:
            theme_source: Set from idleConf.
            builtinlist: List of default themes from idleConf.
            customlist: List of custom themes from idleConf.
            custom_theme_on: Disabled if there are no custom themes.
            custom_theme: Message with additional information.
            targetlist: Create menu from self.theme_elements.

        Methods:
            set_theme_type
            paint_theme_sample
            set_highlight_target
        """
        # Set current theme type radiobutton.
        self.theme_source.set(idleConf.GetOption(
                'main', 'Theme', 'default', type='bool', default=1))
        # Set current theme.
        current_option = idleConf.CurrentTheme()
        # Load available theme option menus.
        if self.theme_source.get():  # Default theme selected.
            item_list = idleConf.GetSectionList('default', 'highlight')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('user', 'highlight')
            item_list.sort()
            if not item_list:
                self.custom_theme_on['state'] = DISABLED
                self.custom_name.set('- no custom themes -')
            else:
                self.customlist.SetMenu(item_list, item_list[0])
        else:  # User theme selected.
            item_list = idleConf.GetSectionList('user', 'highlight')
            item_list.sort()
            self.customlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('default', 'highlight')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, item_list[0])
        self.set_theme_type()
        # Load theme element option menu.
        theme_names = list(self.theme_elements.keys())
        theme_names.sort(key=lambda x: self.theme_elements[x][1])
        self.targetlist.SetMenu(theme_names, theme_names[0])
        self.paint_theme_sample()
        self.set_highlight_target()

    def var_changed_builtin_name(self, *params):
        """Process new builtin theme selection.

        Add the changed theme's name to the changed_items and recreate
        the sample with the values from the selected theme.
        """
        old_themes = ('IDLE Classic', 'IDLE New')
        value = self.builtin_name.get()
        if value not in old_themes:
            if idleConf.GetOption('main', 'Theme', 'name') not in old_themes:
                changes.add_option('main', 'Theme', 'name', old_themes[0])
            changes.add_option('main', 'Theme', 'name2', value)
            self.theme_message['text'] = 'New theme, see Help'
            self.theme_message['fg'] = '#500000'
        else:
            changes.add_option('main', 'Theme', 'name', value)
            changes.add_option('main', 'Theme', 'name2', '')
            self.theme_message['text'] = ''
            self.theme_message['fg'] = 'black'
        self.paint_theme_sample()

    def var_changed_custom_name(self, *params):
        """Process new custom theme selection.

        If a new custom theme is selected, add the name to the
        changed_items and apply the theme to the sample.
        """
        value = self.custom_name.get()
        if value != '- no custom themes -':
            changes.add_option('main', 'Theme', 'name', value)
            self.paint_theme_sample()

    def var_changed_theme_source(self, *params):
        """Process toggle between builtin and custom theme.

        Update the default toggle value and apply the newly
        selected theme type.
        """
        value = self.theme_source.get()
        changes.add_option('main', 'Theme', 'default', value)
        if value:
            self.var_changed_builtin_name()
        else:
            self.var_changed_custom_name()

    def var_changed_color(self, *params):
        "Process change to color choice."
        self.on_new_color_set()

    def var_changed_highlight_target(self, *params):
        "Process selection of new target tag for highlighting."
        self.set_highlight_target()

    def set_theme_type(self):
        """Set available screen options based on builtin or custom theme.

        Attributes accessed:
            theme_source

        Attributes updated:
            builtinlist
            customlist
            button_delete_custom
            custom_theme_on

        Called from:
            handler for builtin_theme_on and custom_theme_on
            delete_custom
            create_new
            load_theme_cfg
        """
        if self.theme_source.get():
            self.builtinlist['state'] = NORMAL
            self.customlist['state'] = DISABLED
            self.button_delete_custom['state'] = DISABLED
        else:
            self.builtinlist['state'] = DISABLED
            self.custom_theme_on['state'] = NORMAL
            self.customlist['state'] = NORMAL
            self.button_delete_custom['state'] = NORMAL

    def get_color(self):
        """Handle button to select a new color for the target tag.

        If a new color is selected while using a builtin theme, a
        name must be supplied to create a custom theme.

        Attributes accessed:
            highlight_target
            frame_color_set
            theme_source

        Attributes updated:
            color

        Methods:
            get_new_theme_name
            create_new
        """
        target = self.highlight_target.get()
        prev_color = self.frame_color_set.cget('bg')
        rgbTuplet, color_string = tkColorChooser.askcolor(
                parent=self, title='Pick new color for : '+target,
                initialcolor=prev_color)
        if color_string and (color_string != prev_color):
            # User didn't cancel and they chose a new color.
            if self.theme_source.get():  # Current theme is a built-in.
                message = ('Your changes will be saved as a new Custom Theme. '
                           'Enter a name for your new Custom Theme below.')
                new_theme = self.get_new_theme_name(message)
                if not new_theme:  # User cancelled custom theme creation.
                    return
                else:  # Create new custom theme based on previously active theme.
                    self.create_new(new_theme)
                    self.color.set(color_string)
            else:  # Current theme is user defined.
                self.color.set(color_string)

    def on_new_color_set(self):
        "Display sample of new color selection on the dialog."
        new_color = self.color.get()
        self.frame_color_set['bg'] = new_color  # Set sample.
        plane = 'foreground' if self.fg_bg_toggle.get() else 'background'
        sample_element = self.theme_elements[self.highlight_target.get()][0]
        self.highlight_sample.tag_config(sample_element, **{plane: new_color})
        theme = self.custom_name.get()
        theme_element = sample_element + '-' + plane
        changes.add_option('highlight', theme, theme_element, new_color)

    def get_new_theme_name(self, message):
        "Return name of new theme from query popup."
        used_names = (idleConf.GetSectionList('user', 'highlight') +
                idleConf.GetSectionList('default', 'highlight'))
        new_theme = SectionName(
                self, 'New Custom Theme', message, used_names).result
        return new_theme

    def save_as_new_theme(self):
        """Prompt for new theme name and create the theme.

        Methods:
            get_new_theme_name
            create_new
        """
        new_theme_name = self.get_new_theme_name('New Theme Name:')
        if new_theme_name:
            self.create_new(new_theme_name)

    def create_new(self, new_theme_name):
        """Create a new custom theme with the given name.

        Create the new theme based on the previously active theme
        with the current changes applied.  Once it is saved, then
        activate the new theme.

        Attributes accessed:
            builtin_name
            custom_name

        Attributes updated:
            customlist
            theme_source

        Method:
            save_new
            set_theme_type
        """
        if self.theme_source.get():
            theme_type = 'default'
            theme_name = self.builtin_name.get()
        else:
            theme_type = 'user'
            theme_name = self.custom_name.get()
        new_theme = idleConf.GetThemeDict(theme_type, theme_name)
        # Apply any of the old theme's unsaved changes to the new theme.
        if theme_name in changes['highlight']:
            theme_changes = changes['highlight'][theme_name]
            for element in theme_changes:
                new_theme[element] = theme_changes[element]
        # Save the new theme.
        self.save_new(new_theme_name, new_theme)
        # Change GUI over to the new theme.
        custom_theme_list = idleConf.GetSectionList('user', 'highlight')
        custom_theme_list.sort()
        self.customlist.SetMenu(custom_theme_list, new_theme_name)
        self.theme_source.set(0)
        self.set_theme_type()

    def set_highlight_target(self):
        """Set fg/bg toggle and color based on highlight tag target.

        Instance variables accessed:
            highlight_target

        Attributes updated:
            fg_on
            bg_on
            fg_bg_toggle

        Methods:
            set_color_sample

        Called from:
            var_changed_highlight_target
            load_theme_cfg
        """
        if self.highlight_target.get() == 'Cursor':  # bg not possible
            self.fg_on['state'] = DISABLED
            self.bg_on['state'] = DISABLED
            self.fg_bg_toggle.set(1)
        else:  # Both fg and bg can be set.
            self.fg_on['state'] = NORMAL
            self.bg_on['state'] = NORMAL
            self.fg_bg_toggle.set(1)
        self.set_color_sample()

    def set_color_sample_binding(self, *args):
        """Change color sample based on foreground/background toggle.

        Methods:
            set_color_sample
        """
        self.set_color_sample()

    def set_color_sample(self):
        """Set the color of the frame background to reflect the selected target.

        Instance variables accessed:
            theme_elements
            highlight_target
            fg_bg_toggle
            highlight_sample

        Attributes updated:
            frame_color_set
        """
        # Set the color sample area.
        tag = self.theme_elements[self.highlight_target.get()][0]
        plane = 'foreground' if self.fg_bg_toggle.get() else 'background'
        color = self.highlight_sample.tag_cget(tag, plane)
        self.frame_color_set['bg'] = color

    def paint_theme_sample(self):
        """Apply the theme colors to each element tag in the sample text.

        Instance attributes accessed:
            theme_elements
            theme_source
            builtin_name
            custom_name

        Attributes updated:
            highlight_sample: Set the tag elements to the theme.

        Methods:
            set_color_sample

        Called from:
            var_changed_builtin_name
            var_changed_custom_name
            load_theme_cfg
        """
        if self.theme_source.get():  # Default theme
            theme = self.builtin_name.get()
        else:  # User theme
            theme = self.custom_name.get()
        for element_title in self.theme_elements:
            element = self.theme_elements[element_title][0]
            colors = idleConf.GetHighlight(theme, element)
            if element == 'cursor':  # Cursor sample needs special painting.
                colors['background'] = idleConf.GetHighlight(
                        theme, 'normal', fgBg='bg')
            # Handle any unsaved changes to this theme.
            if theme in changes['highlight']:
                theme_dict = changes['highlight'][theme]
                if element + '-foreground' in theme_dict:
                    colors['foreground'] = theme_dict[element + '-foreground']
                if element + '-background' in theme_dict:
                    colors['background'] = theme_dict[element + '-background']
            self.highlight_sample.tag_config(element, **colors)
        self.set_color_sample()

    def save_new(self, theme_name, theme):
        """Save a newly created theme to idleConf.

        theme_name - string, the name of the new theme
        theme - dictionary containing the new theme
        """
        if not idleConf.userCfg['highlight'].has_section(theme_name):
            idleConf.userCfg['highlight'].add_section(theme_name)
        for element in theme:
            value = theme[element]
            idleConf.userCfg['highlight'].SetOption(theme_name, element, value)

    def delete_custom(self):
        """Handle event to delete custom theme.

        The current theme is deactivated and the default theme is
        activated.  The custom theme is permanently removed from
        the config file.

        Attributes accessed:
            custom_name

        Attributes updated:
            custom_theme_on
            customlist
            theme_source
            builtin_name

        Methods:
            deactivate_current_config
            save_all_changed_extensions
            activate_config_changes
            set_theme_type
        """
        theme_name = self.custom_name.get()
        delmsg = 'Are you sure you wish to delete the theme %r ?'
        if not tkMessageBox.askyesno(
                'Delete Theme',  delmsg % theme_name, parent=self):
            return
        cd.deactivate_current_config()
        # Remove theme from changes, config, and file.
        changes.delete_section('highlight', theme_name)
        # Reload user theme list.
        item_list = idleConf.GetSectionList('user', 'highlight')
        item_list.sort()
        if not item_list:
            self.custom_theme_on['state'] = DISABLED
            self.customlist.SetMenu(item_list, '- no custom themes -')
        else:
            self.customlist.SetMenu(item_list, item_list[0])
        # Revert to default theme.
        self.theme_source.set(idleConf.defaultCfg['main'].Get('Theme', 'default'))
        self.builtin_name.set(idleConf.defaultCfg['main'].Get('Theme', 'name'))
        # User can't back out of these changes, they must be applied now.
        changes.save_all()
        cd.save_all_changed_extensions()
        cd.activate_config_changes()
        self.set_theme_type()


class KeysPage(Frame):

    def __init__(self, master):
        super().__init__(master)
        self.cd = master.master
        self.create_page_keys()
        self.load_key_cfg()

    def create_page_keys(self):
        """Return frame of widgets for Keys tab.

        Enable users to provisionally change both individual and sets of
        keybindings (shortcut keys). Except for features implemented as
        extensions, keybindings are stored in complete sets called
        keysets. Built-in keysets in idlelib/config-keys.def are fixed
        as far as the dialog is concerned. Any keyset can be used as the
        base for a new custom keyset, stored in .idlerc/config-keys.cfg.

        Function load_key_cfg() initializes tk variables and keyset
        lists and calls load_keys_list for the current keyset.
        Radiobuttons builtin_keyset_on and custom_keyset_on toggle var
        keyset_source, which controls if the current set of keybindings
        are from a builtin or custom keyset. DynOptionMenus builtinlist
        and customlist contain lists of the builtin and custom keysets,
        respectively, and the current item from each list is stored in
        vars builtin_name and custom_name.

        Button delete_custom_keys invokes delete_custom_keys() to delete
        a custom keyset from idleConf.userCfg['keys'] and changes.  Button
        save_custom_keys invokes save_as_new_key_set() which calls
        get_new_keys_name() and create_new_key_set() to save a custom keyset
        and its keybindings to idleConf.userCfg['keys'].

        Listbox bindingslist contains all of the keybindings for the
        selected keyset.  The keybindings are loaded in load_keys_list()
        and are pairs of (event, [keys]) where keys can be a list
        of one or more key combinations to bind to the same event.
        Mouse button 1 click invokes on_bindingslist_select(), which
        allows button_new_keys to be clicked.

        So, an item is selected in listbindings, which activates
        button_new_keys, and clicking button_new_keys calls function
        get_new_keys().  Function get_new_keys() gets the key mappings from the
        current keyset for the binding event item that was selected.  The
        function then displays another dialog, GetKeysDialog, with the
        selected binding event and current keys and allows new key sequences
        to be entered for that binding event.  If the keys aren't
        changed, nothing happens.  If the keys are changed and the keyset
        is a builtin, function get_new_keys_name() will be called
        for input of a custom keyset name.  If no name is given, then the
        change to the keybinding will abort and no updates will be made.  If
        a custom name is entered in the prompt or if the current keyset was
        already custom (and thus didn't require a prompt), then
        idleConf.userCfg['keys'] is updated in function create_new_key_set()
        with the change to the event binding.  The item listing in bindingslist
        is updated with the new keys.  Var keybinding is also set which invokes
        the callback function, var_changed_keybinding, to add the change to
        the 'keys' or 'extensions' changes tracker based on the binding type.

        Tk Variables:
            keybinding: Action/key bindings.

        Methods:
            load_keys_list: Reload active set.
            create_new_key_set: Combine active keyset and changes.
            set_keys_type: Command for keyset_source.
            save_new_key_set: Save to idleConf.userCfg['keys'] (is function).
            deactivate_current_config: Remove keys bindings in editors.

        Widgets for KeysPage(frame):  (*) widgets bound to self
            frame_key_sets: LabelFrame
                frames[0]: Frame
                    (*)builtin_keyset_on: Radiobutton - var keyset_source
                    (*)custom_keyset_on: Radiobutton - var keyset_source
                    (*)builtinlist: DynOptionMenu - var builtin_name,
                            func keybinding_selected
                    (*)customlist: DynOptionMenu - var custom_name,
                            func keybinding_selected
                    (*)keys_message: Label
                frames[1]: Frame
                    (*)button_delete_custom_keys: Button - delete_custom_keys
                    (*)button_save_custom_keys: Button -  save_as_new_key_set
            frame_custom: LabelFrame
                frame_target: Frame
                    target_title: Label
                    scroll_target_y: Scrollbar
                    scroll_target_x: Scrollbar
                    (*)bindingslist: ListBox - on_bindingslist_select
                    (*)button_new_keys: Button - get_new_keys & ..._name
        """
        self.builtin_name = tracers.add(
                StringVar(self), self.var_changed_builtin_name)
        self.custom_name = tracers.add(
                StringVar(self), self.var_changed_custom_name)
        self.keyset_source = tracers.add(
                BooleanVar(self), self.var_changed_keyset_source)
        self.keybinding = tracers.add(
                StringVar(self), self.var_changed_keybinding)

        # Create widgets:
        # body and section frames.
        frame_custom = LabelFrame(
                self, borderwidth=2, relief=GROOVE,
                text=' Custom Key Bindings ')
        frame_key_sets = LabelFrame(
                self, borderwidth=2, relief=GROOVE, text=' Key Set ')
        # frame_custom.
        frame_target = Frame(frame_custom)
        target_title = Label(frame_target, text='Action - Key(s)')
        scroll_target_y = Scrollbar(frame_target)
        scroll_target_x = Scrollbar(frame_target, orient=HORIZONTAL)
        self.bindingslist = Listbox(
                frame_target, takefocus=FALSE, exportselection=FALSE)
        self.bindingslist.bind('<ButtonRelease-1>',
                               self.on_bindingslist_select)
        scroll_target_y['command'] = self.bindingslist.yview
        scroll_target_x['command'] = self.bindingslist.xview
        self.bindingslist['yscrollcommand'] = scroll_target_y.set
        self.bindingslist['xscrollcommand'] = scroll_target_x.set
        self.button_new_keys = Button(
                frame_custom, text='Get New Keys for Selection',
                command=self.get_new_keys, state=DISABLED)
        # frame_key_sets.
        frames = [Frame(frame_key_sets, padx=2, pady=2, borderwidth=0)
                  for i in range(2)]
        self.builtin_keyset_on = Radiobutton(
                frames[0], variable=self.keyset_source, value=1,
                command=self.set_keys_type, text='Use a Built-in Key Set')
        self.custom_keyset_on = Radiobutton(
                frames[0], variable=self.keyset_source, value=0,
                command=self.set_keys_type, text='Use a Custom Key Set')
        self.builtinlist = DynOptionMenu(
                frames[0], self.builtin_name, None, command=None)
        self.customlist = DynOptionMenu(
                frames[0], self.custom_name, None, command=None)
        self.button_delete_custom_keys = Button(
                frames[1], text='Delete Custom Key Set',
                command=self.delete_custom_keys)
        self.button_save_custom_keys = Button(
                frames[1], text='Save as New Custom Key Set',
                command=self.save_as_new_key_set)
        self.keys_message = Label(frames[0], bd=2)

        # Pack widgets:
        # body.
        frame_custom.pack(side=BOTTOM, padx=5, pady=5, expand=TRUE, fill=BOTH)
        frame_key_sets.pack(side=BOTTOM, padx=5, pady=5, fill=BOTH)
        # frame_custom.
        self.button_new_keys.pack(side=BOTTOM, fill=X, padx=5, pady=5)
        frame_target.pack(side=LEFT, padx=5, pady=5, expand=TRUE, fill=BOTH)
        # frame_target.
        frame_target.columnconfigure(0, weight=1)
        frame_target.rowconfigure(1, weight=1)
        target_title.grid(row=0, column=0, columnspan=2, sticky=W)
        self.bindingslist.grid(row=1, column=0, sticky=NSEW)
        scroll_target_y.grid(row=1, column=1, sticky=NS)
        scroll_target_x.grid(row=2, column=0, sticky=EW)
        # frame_key_sets.
        self.builtin_keyset_on.grid(row=0, column=0, sticky=W+NS)
        self.custom_keyset_on.grid(row=1, column=0, sticky=W+NS)
        self.builtinlist.grid(row=0, column=1, sticky=NSEW)
        self.customlist.grid(row=1, column=1, sticky=NSEW)
        self.keys_message.grid(row=0, column=2, sticky=NSEW, padx=5, pady=5)
        self.button_delete_custom_keys.pack(side=LEFT, fill=X, expand=True, padx=2)
        self.button_save_custom_keys.pack(side=LEFT, fill=X, expand=True, padx=2)
        frames[0].pack(side=TOP, fill=BOTH, expand=True)
        frames[1].pack(side=TOP, fill=X, expand=True, pady=2)

    def load_key_cfg(self):
        "Load current configuration settings for the keybinding options."
        # Set current keys type radiobutton.
        self.keyset_source.set(idleConf.GetOption(
                'main', 'Keys', 'default', type='bool', default=1))
        # Set current keys.
        current_option = idleConf.CurrentKeys()
        # Load available keyset option menus.
        if self.keyset_source.get():  # Default theme selected.
            item_list = idleConf.GetSectionList('default', 'keys')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('user', 'keys')
            item_list.sort()
            if not item_list:
                self.custom_keyset_on['state'] = DISABLED
                self.custom_name.set('- no custom keys -')
            else:
                self.customlist.SetMenu(item_list, item_list[0])
        else:  # User key set selected.
            item_list = idleConf.GetSectionList('user', 'keys')
            item_list.sort()
            self.customlist.SetMenu(item_list, current_option)
            item_list = idleConf.GetSectionList('default', 'keys')
            item_list.sort()
            self.builtinlist.SetMenu(item_list, idleConf.default_keys())
        self.set_keys_type()
        # Load keyset element list.
        keyset_name = idleConf.CurrentKeys()
        self.load_keys_list(keyset_name)

    def var_changed_builtin_name(self, *params):
        "Process selection of builtin key set."
        old_keys = (
            'IDLE Classic Windows',
            'IDLE Classic Unix',
            'IDLE Classic Mac',
            'IDLE Classic OSX',
        )
        value = self.builtin_name.get()
        if value not in old_keys:
            if idleConf.GetOption('main', 'Keys', 'name') not in old_keys:
                changes.add_option('main', 'Keys', 'name', old_keys[0])
            changes.add_option('main', 'Keys', 'name2', value)
            self.keys_message['text'] = 'New key set, see Help'
            self.keys_message['fg'] = '#500000'
        else:
            changes.add_option('main', 'Keys', 'name', value)
            changes.add_option('main', 'Keys', 'name2', '')
            self.keys_message['text'] = ''
            self.keys_message['fg'] = 'black'
        self.load_keys_list(value)

    def var_changed_custom_name(self, *params):
        "Process selection of custom key set."
        value = self.custom_name.get()
        if value != '- no custom keys -':
            changes.add_option('main', 'Keys', 'name', value)
            self.load_keys_list(value)

    def var_changed_keyset_source(self, *params):
        "Process toggle between builtin key set and custom key set."
        value = self.keyset_source.get()
        changes.add_option('main', 'Keys', 'default', value)
        if value:
            self.var_changed_builtin_name()
        else:
            self.var_changed_custom_name()

    def var_changed_keybinding(self, *params):
        "Store change to a keybinding."
        value = self.keybinding.get()
        key_set = self.custom_name.get()
        event = self.bindingslist.get(ANCHOR).split()[0]
        if idleConf.IsCoreBinding(event):
            changes.add_option('keys', key_set, event, value)
        else:  # Event is an extension binding.
            ext_name = idleConf.GetExtnNameForEvent(event)
            ext_keybind_section = ext_name + '_cfgBindings'
            changes.add_option('extensions', ext_keybind_section, event, value)

    def set_keys_type(self):
        "Set available screen options based on builtin or custom key set."
        if self.keyset_source.get():
            self.builtinlist['state'] = NORMAL
            self.customlist['state'] = DISABLED
            self.button_delete_custom_keys['state'] = DISABLED
        else:
            self.builtinlist['state'] = DISABLED
            self.custom_keyset_on['state'] = NORMAL
            self.customlist['state'] = NORMAL
            self.button_delete_custom_keys['state'] = NORMAL

    def get_new_keys(self):
        """Handle event to change key binding for selected line.

        A selection of a key/binding in the list of current
        bindings pops up a dialog to enter a new binding.  If
        the current key set is builtin and a binding has
        changed, then a name for a custom key set needs to be
        entered for the change to be applied.
        """
        list_index = self.bindingslist.index(ANCHOR)
        binding = self.bindingslist.get(list_index)
        bind_name = binding.split()[0]
        if self.keyset_source.get():
            current_key_set_name = self.builtin_name.get()
        else:
            current_key_set_name = self.custom_name.get()
        current_bindings = idleConf.GetCurrentKeySet()
        if current_key_set_name in changes['keys']:  # unsaved changes
            key_set_changes = changes['keys'][current_key_set_name]
            for event in key_set_changes:
                current_bindings[event] = key_set_changes[event].split()
        current_key_sequences = list(current_bindings.values())
        new_keys = GetKeysDialog(self, 'Get New Keys', bind_name,
                current_key_sequences).result
        if new_keys:
            if self.keyset_source.get():  # Current key set is a built-in.
                message = ('Your changes will be saved as a new Custom Key Set.'
                           ' Enter a name for your new Custom Key Set below.')
                new_keyset = self.get_new_keys_name(message)
                if not new_keyset:  # User cancelled custom key set creation.
                    self.bindingslist.select_set(list_index)
                    self.bindingslist.select_anchor(list_index)
                    return
                else:  # Create new custom key set based on previously active key set.
                    self.create_new_key_set(new_keyset)
            self.bindingslist.delete(list_index)
            self.bindingslist.insert(list_index, bind_name+' - '+new_keys)
            self.bindingslist.select_set(list_index)
            self.bindingslist.select_anchor(list_index)
            self.keybinding.set(new_keys)
        else:
            self.bindingslist.select_set(list_index)
            self.bindingslist.select_anchor(list_index)

    def get_new_keys_name(self, message):
        "Return new key set name from query popup."
        used_names = (idleConf.GetSectionList('user', 'keys') +
                idleConf.GetSectionList('default', 'keys'))
        new_keyset = SectionName(
                self, 'New Custom Key Set', message, used_names).result
        return new_keyset

    def save_as_new_key_set(self):
        "Prompt for name of new key set and save changes using that name."
        new_keys_name = self.get_new_keys_name('New Key Set Name:')
        if new_keys_name:
            self.create_new_key_set(new_keys_name)

    def on_bindingslist_select(self, event):
        "Activate button to assign new keys to selected action."
        self.button_new_keys['state'] = NORMAL

    def create_new_key_set(self, new_key_set_name):
        """Create a new custom key set with the given name.

        Copy the bindings/keys from the previously active keyset
        to the new keyset and activate the new custom keyset.
        """
        if self.keyset_source.get():
            prev_key_set_name = self.builtin_name.get()
        else:
            prev_key_set_name = self.custom_name.get()
        prev_keys = idleConf.GetCoreKeys(prev_key_set_name)
        new_keys = {}
        for event in prev_keys:  # Add key set to changed items.
            event_name = event[2:-2]  # Trim off the angle brackets.
            binding = ' '.join(prev_keys[event])
            new_keys[event_name] = binding
        # Handle any unsaved changes to prev key set.
        if prev_key_set_name in changes['keys']:
            key_set_changes = changes['keys'][prev_key_set_name]
            for event in key_set_changes:
                new_keys[event] = key_set_changes[event]
        # Save the new key set.
        self.save_new_key_set(new_key_set_name, new_keys)
        # Change GUI over to the new key set.
        custom_key_list = idleConf.GetSectionList('user', 'keys')
        custom_key_list.sort()
        self.customlist.SetMenu(custom_key_list, new_key_set_name)
        self.keyset_source.set(0)
        self.set_keys_type()

    def load_keys_list(self, keyset_name):
        """Reload the list of action/key binding pairs for the active key set.

        An action/key binding can be selected to change the key binding.
        """
        reselect = False
        if self.bindingslist.curselection():
            reselect = True
            list_index = self.bindingslist.index(ANCHOR)
        keyset = idleConf.GetKeySet(keyset_name)
        bind_names = list(keyset.keys())
        bind_names.sort()
        self.bindingslist.delete(0, END)
        for bind_name in bind_names:
            key = ' '.join(keyset[bind_name])
            bind_name = bind_name[2:-2]  # Trim off the angle brackets.
            if keyset_name in changes['keys']:
                # Handle any unsaved changes to this key set.
                if bind_name in changes['keys'][keyset_name]:
                    key = changes['keys'][keyset_name][bind_name]
            self.bindingslist.insert(END, bind_name+' - '+key)
        if reselect:
            self.bindingslist.see(list_index)
            self.bindingslist.select_set(list_index)
            self.bindingslist.select_anchor(list_index)

    @staticmethod
    def save_new_key_set(keyset_name, keyset):
        """Save a newly created core key set.

        Add keyset to idleConf.userCfg['keys'], not to disk.
        If the keyset doesn't exist, it is created.  The
        binding/keys are taken from the keyset argument.

        keyset_name - string, the name of the new key set
        keyset - dictionary containing the new keybindings
        """
        if not idleConf.userCfg['keys'].has_section(keyset_name):
            idleConf.userCfg['keys'].add_section(keyset_name)
        for event in keyset:
            value = keyset[event]
            idleConf.userCfg['keys'].SetOption(keyset_name, event, value)

    def delete_custom_keys(self):
        """Handle event to delete a custom key set.

        Applying the delete deactivates the current configuration and
        reverts to the default.  The custom key set is permanently
        deleted from the config file.
        """
        keyset_name = self.custom_name.get()
        delmsg = 'Are you sure you wish to delete the key set %r ?'
        if not tkMessageBox.askyesno(
                'Delete Key Set',  delmsg % keyset_name, parent=self):
            return
        self.cd.deactivate_current_config()
        # Remove key set from changes, config, and file.
        changes.delete_section('keys', keyset_name)
        # Reload user key set list.
        item_list = idleConf.GetSectionList('user', 'keys')
        item_list.sort()
        if not item_list:
            self.custom_keyset_on['state'] = DISABLED
            self.customlist.SetMenu(item_list, '- no custom keys -')
        else:
            self.customlist.SetMenu(item_list, item_list[0])
        # Revert to default key set.
        self.keyset_source.set(idleConf.defaultCfg['main']
                                .Get('Keys', 'default'))
        self.builtin_name.set(idleConf.defaultCfg['main'].Get('Keys', 'name')
                             or idleConf.default_keys())
        # User can't back out of these changes, they must be applied now.
        changes.save_all()
        self.cd.save_all_changed_extensions()
        self.cd.activate_config_changes()
        self.set_keys_type()


class GenPage(Frame):

    def __init__(self, master):
        super().__init__(master)
        self.create_page_general()
        self.load_general_cfg()

    def create_page_general(self):
        """Return frame of widgets for General tab.

        Enable users to provisionally change general options. Function
        load_general_cfg intializes tk variables and helplist using
        idleConf.  Radiobuttons startup_shell_on and startup_editor_on
        set var startup_edit. Radiobuttons save_ask_on and save_auto_on
        set var autosave. Entry boxes win_width_int and win_height_int
        set var win_width and win_height.  Setting var_name invokes the
        default callback that adds option to changes.

        Helplist: load_general_cfg loads list user_helplist with
        name, position pairs and copies names to listbox helplist.
        Clicking a name invokes help_source selected. Clicking
        button_helplist_name invokes helplist_item_name, which also
        changes user_helplist.  These functions all call
        set_add_delete_state. All but load call update_help_changes to
        rewrite changes['main']['HelpFiles'].

        Widgets for GenPage(Frame):  (*) widgets bound to self
            frame_run: LabelFrame
                startup_title: Label
                (*)startup_editor_on: Radiobutton - startup_edit
                (*)startup_shell_on: Radiobutton - startup_edit
            frame_save: LabelFrame
                run_save_title: Label
                (*)save_ask_on: Radiobutton - autosave
                (*)save_auto_on: Radiobutton - autosave
            frame_win_size: LabelFrame
                win_size_title: Label
                win_width_title: Label
                (*)win_width_int: Entry - win_width
                win_height_title: Label
                (*)win_height_int: Entry - win_height
            frame_help: LabelFrame
                frame_helplist: Frame
                    frame_helplist_buttons: Frame
                        (*)button_helplist_edit
                        (*)button_helplist_add
                        (*)button_helplist_remove
                    (*)helplist: ListBox
                    scroll_helplist: Scrollbar
        """
        self.startup_edit = tracers.add(
                IntVar(self), ('main', 'General', 'editor-on-startup'))
        self.autosave = tracers.add(
                IntVar(self), ('main', 'General', 'autosave'))
        self.win_width = tracers.add(
                StringVar(self), ('main', 'EditorWindow', 'width'))
        self.win_height = tracers.add(
                StringVar(self), ('main', 'EditorWindow', 'height'))

        # Create widgets:
        # Section frames.
        frame_run = LabelFrame(self, borderwidth=2, relief=GROOVE,
                              text=' Startup Preferences ')
        frame_save = LabelFrame(self, borderwidth=2, relief=GROOVE,
                               text=' autosave Preferences ')
        frame_win_size = Frame(self, borderwidth=2, relief=GROOVE)
        frame_help = LabelFrame(self, borderwidth=2, relief=GROOVE,
                               text=' Additional Help Sources ')
        # frame_run.
        startup_title = Label(frame_run, text='At Startup')
        self.startup_editor_on = Radiobutton(
                frame_run, variable=self.startup_edit, value=1,
                text="Open Edit Window")
        self.startup_shell_on = Radiobutton(
                frame_run, variable=self.startup_edit, value=0,
                text='Open Shell Window')
        # frame_save.
        run_save_title = Label(frame_save, text='At Start of Run (F5)  ')
        self.save_ask_on = Radiobutton(
                frame_save, variable=self.autosave, value=0,
                text="Prompt to Save")
        self.save_auto_on = Radiobutton(
                frame_save, variable=self.autosave, value=1,
                text='No Prompt')
        # frame_win_size.
        win_size_title = Label(
                frame_win_size, text='Initial Window Size  (in characters)')
        win_width_title = Label(frame_win_size, text='Width')
        self.win_width_int = Entry(
                frame_win_size, textvariable=self.win_width, width=3)
        win_height_title = Label(frame_win_size, text='Height')
        self.win_height_int = Entry(
                frame_win_size, textvariable=self.win_height, width=3)
        # frame_help.
        frame_helplist = Frame(frame_help)
        frame_helplist_buttons = Frame(frame_helplist)
        self.helplist = Listbox(
                frame_helplist, height=5, takefocus=True,
                exportselection=FALSE)
        scroll_helplist = Scrollbar(frame_helplist)
        scroll_helplist['command'] = self.helplist.yview
        self.helplist['yscrollcommand'] = scroll_helplist.set
        self.helplist.bind('<ButtonRelease-1>', self.help_source_selected)
        self.button_helplist_edit = Button(
                frame_helplist_buttons, text='Edit', state=DISABLED,
                width=8, command=self.helplist_item_edit)
        self.button_helplist_add = Button(
                frame_helplist_buttons, text='Add',
                width=8, command=self.helplist_item_add)
        self.button_helplist_remove = Button(
                frame_helplist_buttons, text='Remove', state=DISABLED,
                width=8, command=self.helplist_item_remove)

        # Pack widgets:
        # body.
        frame_run.pack(side=TOP, padx=5, pady=5, fill=X)
        frame_save.pack(side=TOP, padx=5, pady=5, fill=X)
        frame_win_size.pack(side=TOP, padx=5, pady=5, fill=X)
        frame_help.pack(side=TOP, padx=5, pady=5, expand=TRUE, fill=BOTH)
        # frame_run.
        startup_title.pack(side=LEFT, anchor=W, padx=5, pady=5)
        self.startup_shell_on.pack(side=RIGHT, anchor=W, padx=5, pady=5)
        self.startup_editor_on.pack(side=RIGHT, anchor=W, padx=5, pady=5)
        # frame_save.
        run_save_title.pack(side=LEFT, anchor=W, padx=5, pady=5)
        self.save_auto_on.pack(side=RIGHT, anchor=W, padx=5, pady=5)
        self.save_ask_on.pack(side=RIGHT, anchor=W, padx=5, pady=5)
        # frame_win_size.
        win_size_title.pack(side=LEFT, anchor=W, padx=5, pady=5)
        self.win_height_int.pack(side=RIGHT, anchor=E, padx=10, pady=5)
        win_height_title.pack(side=RIGHT, anchor=E, pady=5)
        self.win_width_int.pack(side=RIGHT, anchor=E, padx=10, pady=5)
        win_width_title.pack(side=RIGHT, anchor=E, pady=5)
        # frame_help.
        frame_helplist_buttons.pack(side=RIGHT, padx=5, pady=5, fill=Y)
        frame_helplist.pack(side=TOP, padx=5, pady=5, expand=TRUE, fill=BOTH)
        scroll_helplist.pack(side=RIGHT, anchor=W, fill=Y)
        self.helplist.pack(side=LEFT, anchor=E, expand=TRUE, fill=BOTH)
        self.button_helplist_edit.pack(side=TOP, anchor=W, pady=5)
        self.button_helplist_add.pack(side=TOP, anchor=W)
        self.button_helplist_remove.pack(side=TOP, anchor=W, pady=5)

    def load_general_cfg(self):
        "Load current configuration settings for the general options."
        # Set startup state.
        self.startup_edit.set(idleConf.GetOption(
                'main', 'General', 'editor-on-startup', default=0, type='bool'))
        # Set autosave state.
        self.autosave.set(idleConf.GetOption(
                'main', 'General', 'autosave', default=0, type='bool'))
        # Set initial window size.
        self.win_width.set(idleConf.GetOption(
                'main', 'EditorWindow', 'width', type='int'))
        self.win_height.set(idleConf.GetOption(
                'main', 'EditorWindow', 'height', type='int'))
        # Set additional help sources.
        self.user_helplist = idleConf.GetAllExtraHelpSourcesList()
        self.helplist.delete(0, 'end')
        for help_item in self.user_helplist:
            self.helplist.insert(END, help_item[0])
        self.set_add_delete_state()

    def help_source_selected(self, event):
        "Handle event for selecting additional help."
        self.set_add_delete_state()

    def set_add_delete_state(self):
        "Toggle the state for the help list buttons based on list entries."
        if self.helplist.size() < 1:  # No entries in list.
            self.button_helplist_edit['state'] = DISABLED
            self.button_helplist_remove['state'] = DISABLED
        else:  # Some entries.
            if self.helplist.curselection():  # There currently is a selection.
                self.button_helplist_edit['state'] = NORMAL
                self.button_helplist_remove['state'] = NORMAL
            else:  # There currently is not a selection.
                self.button_helplist_edit['state'] = DISABLED
                self.button_helplist_remove['state'] = DISABLED

    def helplist_item_add(self):
        """Handle add button for the help list.

        Query for name and location of new help sources and add
        them to the list.
        """
        help_source = HelpSource(self, 'New Help Source').result
        if help_source:
            self.user_helplist.append(help_source)
            self.helplist.insert(END, help_source[0])
            self.update_help_changes()

    def helplist_item_edit(self):
        """Handle edit button for the help list.

        Query with existing help source information and update
        config if the values are changed.
        """
        item_index = self.helplist.index(ANCHOR)
        help_source = self.user_helplist[item_index]
        new_help_source = HelpSource(
                self, 'Edit Help Source',
                menuitem=help_source[0],
                filepath=help_source[1],
                ).result
        if new_help_source and new_help_source != help_source:
            self.user_helplist[item_index] = new_help_source
            self.helplist.delete(item_index)
            self.helplist.insert(item_index, new_help_source[0])
            self.update_help_changes()
            self.set_add_delete_state()  # Selected will be un-selected

    def helplist_item_remove(self):
        """Handle remove button for the help list.

        Delete the help list item from config.
        """
        item_index = self.helplist.index(ANCHOR)
        del(self.user_helplist[item_index])
        self.helplist.delete(item_index)
        self.update_help_changes()
        self.set_add_delete_state()

    def update_help_changes(self):
        "Clear and rebuild the HelpFiles section in changes"
        changes['main']['HelpFiles'] = {}
        for num in range(1, len(self.user_helplist) + 1):
            changes.add_option(
                    'main', 'HelpFiles', str(num),
                    ';'.join(self.user_helplist[num-1][:2]))


class VarTrace:
    """Maintain Tk variables trace state."""

    def __init__(self):
        """Store Tk variables and callbacks.

        untraced: List of tuples (var, callback)
            that do not have the callback attached
            to the Tk var.
        traced: List of tuples (var, callback) where
            that callback has been attached to the var.
        """
        self.untraced = []
        self.traced = []

    def clear(self):
        "Clear lists (for tests)."
        # Call after all tests in a module to avoid memory leaks.
        self.untraced.clear()
        self.traced.clear()

    def add(self, var, callback):
        """Add (var, callback) tuple to untraced list.

        Args:
            var: Tk variable instance.
            callback: Either function name to be used as a callback
                or a tuple with IdleConf config-type, section, and
                option names used in the default callback.

        Return:
            Tk variable instance.
        """
        if isinstance(callback, tuple):
            callback = self.make_callback(var, callback)
        self.untraced.append((var, callback))
        return var

    @staticmethod
    def make_callback(var, config):
        "Return default callback function to add values to changes instance."
        def default_callback(*params):
            "Add config values to changes instance."
            changes.add_option(*config, var.get())
        return default_callback

    def attach(self):
        "Attach callback to all vars that are not traced."
        while self.untraced:
            var, callback = self.untraced.pop()
            var.trace_add('write', callback)
            self.traced.append((var, callback))

    def detach(self):
        "Remove callback from traced vars."
        while self.traced:
            var, callback = self.traced.pop()
            var.trace_remove('write', var.trace_info()[0][1])
            self.untraced.append((var, callback))


tracers = VarTrace()

help_common = '''\
When you click either the Apply or Ok buttons, settings in this
dialog that are different from IDLE's default are saved in
a .idlerc directory in your home directory. Except as noted,
these changes apply to all versions of IDLE installed on this
machine. Some do not take affect until IDLE is restarted.
[Cancel] only cancels changes made since the last save.
'''
help_pages = {
    'Highlighting': '''
Highlighting:
The IDLE Dark color theme is new in October 2015.  It can only
be used with older IDLE releases if it is saved as a custom
theme, with a different name.
''',
    'Keys': '''
Keys:
The IDLE Modern Unix key set is new in June 2016.  It can only
be used with older IDLE releases if it is saved as a custom
key set, with a different name.
''',
    'Extensions': '''
Extensions:

Autocomplete: Popupwait is milleseconds to wait after key char, without
cursor movement, before popping up completion box.  Key char is '.' after
identifier or a '/' (or '\\' on Windows) within a string.

FormatParagraph: Max-width is max chars in lines after re-formatting.
Use with paragraphs in both strings and comment blocks.

ParenMatch: Style indicates what is highlighted when closer is entered:
'opener' - opener '({[' corresponding to closer; 'parens' - both chars;
'expression' (default) - also everything in between.  Flash-delay is how
long to highlight if cursor is not moved (0 means forever).
'''
}


def is_int(s):
    "Return 's is blank or represents an int'"
    if not s:
        return True
    try:
        int(s)
        return True
    except ValueError:
        return False


class VerticalScrolledFrame(Frame):
    """A pure Tkinter vertically scrollable frame.

    * Use the 'interior' attribute to place widgets inside the scrollable frame
    * Construct and pack/place/grid normally
    * This frame only allows vertical scrolling
    """
    def __init__(self, parent, *args, **kw):
        Frame.__init__(self, parent, *args, **kw)

        # Create a canvas object and a vertical scrollbar for scrolling it.
        vscrollbar = Scrollbar(self, orient=VERTICAL)
        vscrollbar.pack(fill=Y, side=RIGHT, expand=FALSE)
        canvas = Canvas(self, bd=0, highlightthickness=0,
                        yscrollcommand=vscrollbar.set, width=240)
        canvas.pack(side=LEFT, fill=BOTH, expand=TRUE)
        vscrollbar.config(command=canvas.yview)

        # Reset the view.
        canvas.xview_moveto(0)
        canvas.yview_moveto(0)

        # Create a frame inside the canvas which will be scrolled with it.
        self.interior = interior = Frame(canvas)
        interior_id = canvas.create_window(0, 0, window=interior, anchor=NW)

        # Track changes to the canvas and frame width and sync them,
        # also updating the scrollbar.
        def _configure_interior(event):
            # Update the scrollbars to match the size of the inner frame.
            size = (interior.winfo_reqwidth(), interior.winfo_reqheight())
            canvas.config(scrollregion="0 0 %s %s" % size)
        interior.bind('<Configure>', _configure_interior)

        def _configure_canvas(event):
            if interior.winfo_reqwidth() != canvas.winfo_width():
                # Update the inner frame's width to fill the canvas.
                canvas.itemconfigure(interior_id, width=canvas.winfo_width())
        canvas.bind('<Configure>', _configure_canvas)

        return


if __name__ == '__main__':
    import unittest
    unittest.main('idlelib.idle_test.test_configdialog',
                  verbosity=2, exit=False)
    from idlelib.idle_test.htest import run
    run(ConfigDialog)
