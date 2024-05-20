import itertools
import os
import rlcompleter
import sys
import tempfile
import unittest
from code import InteractiveConsole
from functools import partial
from unittest import TestCase
from unittest.mock import MagicMock, call, patch, ANY

from test.support import requires
from test.support.import_helper import import_module

# Optionally test pyrepl.  This currently requires that the
# 'curses' resource be given on the regrtest command line using the -u
# option.  Additionally, we need to attempt to import curses and readline.
requires("curses")
curses = import_module("curses")
readline = import_module("readline")

from _pyrepl.console import Console, Event
from _pyrepl.readline import ReadlineAlikeReader, ReadlineConfig
from _pyrepl.simple_interact import _strip_final_indent
from _pyrepl.unix_console import UnixConsole
from _pyrepl.unix_eventqueue import EventQueue
from _pyrepl.input import KeymapTranslator
from _pyrepl.keymap import parse_keys, compile_keymap


def more_lines(unicodetext, namespace=None):
    if namespace is None:
        namespace = {}
    src = _strip_final_indent(unicodetext)
    console = InteractiveConsole(namespace, filename="<stdin>")
    try:
        code = console.compile(src, "<stdin>", "single")
    except (OverflowError, SyntaxError, ValueError):
        return False
    else:
        return code is None


def multiline_input(reader, namespace=None):
    saved = reader.more_lines
    try:
        reader.more_lines = partial(more_lines, namespace=namespace)
        reader.ps1 = reader.ps2 = ">>>"
        reader.ps3 = reader.ps4 = "..."
        return reader.readline()
    finally:
        reader.more_lines = saved
        reader.paste_mode = False


def code_to_events(code):
    for c in code:
        yield Event(evt="key", data=c, raw=bytearray(c.encode("utf-8")))


def prepare_mock_console(events, **kwargs):
    console = MagicMock()
    console.get_event.side_effect = events
    console.height = 100
    console.width = 80
    for key, val in kwargs.items():
        setattr(console, key, val)
    return console


def prepare_fake_console(**kwargs):
    console = FakeConsole()
    for key, val in kwargs.items():
        setattr(console, key, val)
    return console


def prepare_reader(console, **kwargs):
    config = ReadlineConfig(readline_completer=None)
    reader = ReadlineAlikeReader(console=console, config=config)
    reader.more_lines = partial(more_lines, namespace=None)
    reader.paste_mode = True  # Avoid extra indents

    def get_prompt(lineno, cursor_on_line) -> str:
        return ""

    reader.get_prompt = get_prompt  # Remove prompt for easier calculations of (x, y)

    for key, val in kwargs.items():
        setattr(reader, key, val)

    return reader


def handle_all_events(
    events, prepare_console=prepare_mock_console, prepare_reader=prepare_reader
):
    console = prepare_console(events)
    reader = prepare_reader(console)
    try:
        while True:
            reader.handle1()
    except StopIteration:
        pass
    return reader, console


handle_events_narrow_console = partial(
    handle_all_events,
    prepare_console=partial(prepare_mock_console, width=10),
)


class FakeConsole(Console):
    def __init__(self, events, encoding="utf-8"):
        self.events = iter(events)
        self.encoding = encoding
        self.screen = []
        self.height = 100
        self.width = 80

    def get_event(self, block: bool = True) -> Event | None:
        return next(self.events)

    def getpending(self) -> Event:
        return self.get_event(block=False)

    def getheightwidth(self) -> tuple[int, int]:
        return self.height, self.width

    def refresh(self, screen: list[str], xy: tuple[int, int]) -> None:
        pass

    def prepare(self) -> None:
        pass

    def restore(self) -> None:
        pass

    def move_cursor(self, x: int, y: int) -> None:
        pass

    def set_cursor_vis(self, visible: bool) -> None:
        pass

    def push_char(self, char: int | bytes) -> None:
        pass

    def beep(self) -> None:
        pass

    def clear(self) -> None:
        pass

    def finish(self) -> None:
        pass

    def flushoutput(self) -> None:
        pass

    def forgetinput(self) -> None:
        pass

    def wait(self) -> None:
        pass

    def repaint(self) -> None:
        pass


class TestCursorPosition(TestCase):
    def test_up_arrow_simple(self):
        # fmt: off
        code = (
            'def f():\n'
            '  ...\n'
        )
        # fmt: on
        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
            ],
        )

        reader, console = handle_all_events(events)
        self.assertEqual(reader.cxy, (0, 1))
        console.move_cursor.assert_called_once_with(0, 1)

    def test_down_arrow_end_of_input(self):
        # fmt: off
        code = (
            'def f():\n'
            '  ...\n'
        )
        # fmt: on
        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
        )

        reader, console = handle_all_events(events)
        self.assertEqual(reader.cxy, (0, 2))
        console.move_cursor.assert_called_once_with(0, 2)

    def test_left_arrow_simple(self):
        events = itertools.chain(
            code_to_events("11+11"),
            [
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
            ],
        )

        reader, console = handle_all_events(events)
        self.assertEqual(reader.cxy, (4, 0))
        console.move_cursor.assert_called_once_with(4, 0)

    def test_right_arrow_end_of_line(self):
        events = itertools.chain(
            code_to_events("11+11"),
            [
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
            ],
        )

        reader, console = handle_all_events(events)
        self.assertEqual(reader.cxy, (5, 0))
        console.move_cursor.assert_called_once_with(5, 0)

    def test_cursor_position_simple_character(self):
        events = itertools.chain(code_to_events("k"))

        reader, _ = handle_all_events(events)
        self.assertEqual(reader.pos, 1)

        # 1 for simple character
        self.assertEqual(reader.cxy, (1, 0))

    def test_cursor_position_double_width_character(self):
        events = itertools.chain(code_to_events("樂"))

        reader, _ = handle_all_events(events)
        self.assertEqual(reader.pos, 1)

        # 2 for wide character
        self.assertEqual(reader.cxy, (2, 0))

    def test_cursor_position_double_width_character_move_left(self):
        events = itertools.chain(
            code_to_events("樂"),
            [
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
            ],
        )

        reader, _ = handle_all_events(events)
        self.assertEqual(reader.pos, 0)
        self.assertEqual(reader.cxy, (0, 0))

    def test_cursor_position_double_width_character_move_left_right(self):
        events = itertools.chain(
            code_to_events("樂"),
            [
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
            ],
        )

        reader, _ = handle_all_events(events)
        self.assertEqual(reader.pos, 1)

        # 2 for wide character
        self.assertEqual(reader.cxy, (2, 0))

    def test_cursor_position_double_width_characters_move_up(self):
        for_loop = "for _ in _:"

        # fmt: off
        code = (
           f"{for_loop}\n"
            "  ' 可口可乐; 可口可樂'"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
            ],
        )

        reader, _ = handle_all_events(events)

        # cursor at end of first line
        self.assertEqual(reader.pos, len(for_loop))
        self.assertEqual(reader.cxy, (len(for_loop), 0))

    def test_cursor_position_double_width_characters_move_up_down(self):
        for_loop = "for _ in _:"

        # fmt: off
        code = (
           f"{for_loop}\n"
            "  ' 可口可乐; 可口可樂'"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
        )

        reader, _ = handle_all_events(events)

        # cursor here (showing 2nd line only):
        # <  ' 可口可乐; 可口可樂'>
        #              ^
        self.assertEqual(reader.pos, 19)
        self.assertEqual(reader.cxy, (10, 1))

    def test_cursor_position_multiple_double_width_characters_move_left(self):
        events = itertools.chain(
            code_to_events("' 可口可乐; 可口可樂'"),
            [
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
            ],
        )

        reader, _ = handle_all_events(events)
        self.assertEqual(reader.pos, 10)

        # 1 for quote, 1 for space, 2 per wide character,
        # 1 for semicolon, 1 for space, 2 per wide character
        self.assertEqual(reader.cxy, (16, 0))

    def test_cursor_position_move_up_to_eol(self):
        first_line = "for _ in _:"
        second_line = "  hello"

        # fmt: off
        code = (
            f"{first_line}\n"
            f"{second_line}\n"
             "  h\n"
             "  hel"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
            ],
        )

        reader, _ = handle_all_events(events)

        # Cursor should be at end of line 1, even though line 2 is shorter
        # for _ in _:
        #   hello
        #   h
        #   hel
        self.assertEqual(
            reader.pos, len(first_line) + len(second_line) + 1
        )  # +1 for newline
        self.assertEqual(reader.cxy, (len(second_line), 1))

    def test_cursor_position_move_down_to_eol(self):
        last_line = "  hel"

        # fmt: off
        code = (
            "for _ in _:\n"
            "  hello\n"
            "  h\n"
           f"{last_line}"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
        )

        reader, _ = handle_all_events(events)

        # Cursor should be at end of line 3, even though line 2 is shorter
        # for _ in _:
        #   hello
        #   h
        #   hel
        self.assertEqual(reader.pos, len(code))
        self.assertEqual(reader.cxy, (len(last_line), 3))

    def test_cursor_position_multiple_mixed_lines_move_up(self):
        # fmt: off
        code = (
            "def foo():\n"
            "  x = '可口可乐; 可口可樂'\n"
            "  y = 'abckdfjskldfjslkdjf'"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            13 * [Event(evt="key", data="left", raw=bytearray(b"\x1bOD"))],
            [Event(evt="key", data="up", raw=bytearray(b"\x1bOA"))],
        )

        reader, _ = handle_all_events(events)

        # By moving left, we're before the s:
        # y = 'abckdfjskldfjslkdjf'
        #             ^
        # And we should move before the semi-colon despite the different offset
        # x = '可口可乐; 可口可樂'
        #            ^
        self.assertEqual(reader.pos, 22)
        self.assertEqual(reader.cxy, (15, 1))

    def test_cursor_position_after_wrap_and_move_up(self):
        # fmt: off
        code = (
            "def foo():\n"
            "  hello"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
            ],
        )
        reader, _ = handle_events_narrow_console(events)

        # The code looks like this:
        # def foo()\
        # :
        #   hello
        # After moving up we should be after the colon in line 2
        self.assertEqual(reader.pos, 10)
        self.assertEqual(reader.cxy, (1, 1))


class TestPyReplOutput(TestCase):
    def prepare_reader(self, events):
        console = FakeConsole(events)
        config = ReadlineConfig(readline_completer=None)
        reader = ReadlineAlikeReader(console=console, config=config)
        return reader

    def test_basic(self):
        reader = self.prepare_reader(code_to_events("1+1\n"))

        output = multiline_input(reader)
        self.assertEqual(output, "1+1")

    def test_multiline_edit(self):
        events = itertools.chain(
            code_to_events("def f():\n  ...\n\n"),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
                Event(evt="key", data="backspace", raw=bytearray(b"\x7f")),
                Event(evt="key", data="g", raw=bytearray(b"g")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
            ],
        )
        reader = self.prepare_reader(events)

        output = multiline_input(reader)
        self.assertEqual(output, "def f():\n  ...\n  ")
        output = multiline_input(reader)
        self.assertEqual(output, "def g():\n  ...\n  ")

    def test_history_navigation_with_up_arrow(self):
        events = itertools.chain(
            code_to_events("1+1\n2+2\n"),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
            ],
        )

        reader = self.prepare_reader(events)

        output = multiline_input(reader)
        self.assertEqual(output, "1+1")
        output = multiline_input(reader)
        self.assertEqual(output, "2+2")
        output = multiline_input(reader)
        self.assertEqual(output, "2+2")
        output = multiline_input(reader)
        self.assertEqual(output, "1+1")

    def test_history_navigation_with_down_arrow(self):
        events = itertools.chain(
            code_to_events("1+1\n2+2\n"),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
        )

        reader = self.prepare_reader(events)

        output = multiline_input(reader)
        self.assertEqual(output, "1+1")

    def test_history_search(self):
        events = itertools.chain(
            code_to_events("1+1\n2+2\n3+3\n"),
            [
                Event(evt="key", data="\x12", raw=bytearray(b"\x12")),
                Event(evt="key", data="1", raw=bytearray(b"1")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
                Event(evt="key", data="\n", raw=bytearray(b"\n")),
            ],
        )

        reader = self.prepare_reader(events)

        output = multiline_input(reader)
        self.assertEqual(output, "1+1")
        output = multiline_input(reader)
        self.assertEqual(output, "2+2")
        output = multiline_input(reader)
        self.assertEqual(output, "3+3")
        output = multiline_input(reader)
        self.assertEqual(output, "1+1")

    def test_control_character(self):
        events = code_to_events("c\x1d\n")
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, "c\x1d")


class TestPyReplCompleter(TestCase):
    def prepare_reader(self, events, namespace):
        console = FakeConsole(events)
        config = ReadlineConfig()
        config.readline_completer = rlcompleter.Completer(namespace).complete
        reader = ReadlineAlikeReader(console=console, config=config)
        return reader

    def test_simple_completion(self):
        events = code_to_events("os.geten\t\n")

        namespace = {"os": os}
        reader = self.prepare_reader(events, namespace)

        output = multiline_input(reader, namespace)
        self.assertEqual(output, "os.getenv")

    def test_completion_with_many_options(self):
        # Test with something that initially displays many options
        # and then complete from one of them. The first time tab is
        # pressed, the options are displayed (which corresponds to
        # when the repl shows [ not unique ]) and the second completes
        # from one of them.
        events = code_to_events("os.\t\tO_AP\t\n")

        namespace = {"os": os}
        reader = self.prepare_reader(events, namespace)

        output = multiline_input(reader, namespace)
        self.assertEqual(output, "os.O_APPEND")

    def test_empty_namespace_completion(self):
        events = code_to_events("os.geten\t\n")
        namespace = {}
        reader = self.prepare_reader(events, namespace)

        output = multiline_input(reader, namespace)
        self.assertEqual(output, "os.geten")

    def test_global_namespace_completion(self):
        events = code_to_events("py\t\n")
        namespace = {"python": None}
        reader = self.prepare_reader(events, namespace)
        output = multiline_input(reader, namespace)
        self.assertEqual(output, "python")

    def test_updown_arrow_with_completion_menu(self):
        """Up arrow in the middle of unfinished tab completion when the menu is displayed
        should work and trigger going back in history. Down arrow should subsequently
        get us back to the incomplete command."""
        code = "import os\nos.\t\t"
        namespace = {"os": os}

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
            code_to_events("\n"),
        )
        reader = self.prepare_reader(events, namespace=namespace)
        output = multiline_input(reader, namespace)
        # This is the first line, nothing to see here
        self.assertEqual(output, "import os")
        # This is the second line. We pressed up and down arrows
        # so we should end up where we were when we initiated tab completion.
        output = multiline_input(reader, namespace)
        self.assertEqual(output, "os.")


@patch("_pyrepl.curses.tigetstr", lambda x: b"")
class TestUnivEventQueue(TestCase):
    def setUp(self):
        self.file = tempfile.TemporaryFile()

    def tearDown(self) -> None:
        self.file.close()

    def test_get(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        event = Event("key", "a", b"a")
        eq.insert(event)
        self.assertEqual(eq.get(), event)

    def test_empty(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        self.assertTrue(eq.empty())
        eq.insert(Event("key", "a", b"a"))
        self.assertFalse(eq.empty())

    def test_flush_buf(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.buf.extend(b"test")
        self.assertEqual(eq.flush_buf(), b"test")
        self.assertEqual(eq.buf, bytearray())

    def test_insert(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        event = Event("key", "a", b"a")
        eq.insert(event)
        self.assertEqual(eq.events[0], event)

    @patch("_pyrepl.unix_eventqueue.keymap")
    def test_push_with_key_in_keymap(self, mock_keymap):
        mock_keymap.compile_keymap.return_value = {"a": "b"}
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {b"a": "b"}
        eq.push("a")
        mock_keymap.compile_keymap.assert_called()
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "b")

    @patch("_pyrepl.unix_eventqueue.keymap")
    def test_push_without_key_in_keymap(self, mock_keymap):
        mock_keymap.compile_keymap.return_value = {"a": "b"}
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {b"c": "d"}
        eq.push("a")
        mock_keymap.compile_keymap.assert_called()
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "a")

    @patch("_pyrepl.unix_eventqueue.keymap")
    def test_push_with_keymap_in_keymap(self, mock_keymap):
        mock_keymap.compile_keymap.return_value = {"a": "b"}
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {b"a": {b"b": "c"}}
        eq.push("a")
        mock_keymap.compile_keymap.assert_called()
        self.assertTrue(eq.empty())
        eq.push("b")
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "c")
        eq.push("d")
        self.assertEqual(eq.events[1].evt, "key")
        self.assertEqual(eq.events[1].data, "d")

    @patch("_pyrepl.unix_eventqueue.keymap")
    def test_push_with_keymap_in_keymap_and_escape(self, mock_keymap):
        mock_keymap.compile_keymap.return_value = {"a": "b"}
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {b"a": {b"b": "c"}}
        eq.push("a")
        mock_keymap.compile_keymap.assert_called()
        self.assertTrue(eq.empty())
        eq.flush_buf()
        eq.push("\033")
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "\033")
        eq.push("b")
        self.assertEqual(eq.events[1].evt, "key")
        self.assertEqual(eq.events[1].data, "b")

    def test_push_special_key(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {}
        eq.push("\x1b")
        eq.push("[")
        eq.push("A")
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "\x1b")

    def test_push_unrecognized_escape_sequence(self):
        eq = EventQueue(self.file.fileno(), "utf-8")
        eq.keymap = {}
        eq.push("\x1b")
        eq.push("[")
        eq.push("Z")
        self.assertEqual(len(eq.events), 3)
        self.assertEqual(eq.events[0].evt, "key")
        self.assertEqual(eq.events[0].data, "\x1b")
        self.assertEqual(eq.events[1].evt, "key")
        self.assertEqual(eq.events[1].data, "[")
        self.assertEqual(eq.events[2].evt, "key")
        self.assertEqual(eq.events[2].data, "Z")


class TestPasteEvent(TestCase):
    def prepare_reader(self, events):
        console = FakeConsole(events)
        config = ReadlineConfig(readline_completer=None)
        reader = ReadlineAlikeReader(console=console, config=config)
        return reader

    def test_paste(self):
        # fmt: off
        code = (
            'def a():\n'
            '  for x in range(10):\n'
            '    if x%2:\n'
            '      print(x)\n'
            '    else:\n'
            '      pass\n'
        )
        # fmt: on

        events = itertools.chain(
            [
                Event(evt="key", data="f3", raw=bytearray(b"\x1bOR")),
            ],
            code_to_events(code),
            [
                Event(evt="key", data="f3", raw=bytearray(b"\x1bOR")),
            ],
            code_to_events("\n"),
        )
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, code)

    def test_paste_mid_newlines(self):
        # fmt: off
        code = (
            'def f():\n'
            '  x = y\n'
            '  \n'
            '  y = z\n'
        )
        # fmt: on

        events = itertools.chain(
            [
                Event(evt="key", data="f3", raw=bytearray(b"\x1bOR")),
            ],
            code_to_events(code),
            [
                Event(evt="key", data="f3", raw=bytearray(b"\x1bOR")),
            ],
            code_to_events("\n"),
        )
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, code)

    def test_paste_mid_newlines_not_in_paste_mode(self):
        # fmt: off
        code = (
            'def f():\n'
            '  x = y\n'
            '  \n'
            '  y = z\n\n'
        )

        expected = (
            'def f():\n'
            '  x = y\n'
            '    '
        )
        # fmt: on

        events = code_to_events(code)
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, expected)

    def test_paste_not_in_paste_mode(self):
        # fmt: off
        input_code = (
            'def a():\n'
            '  for x in range(10):\n'
            '    if x%2:\n'
            '      print(x)\n'
            '    else:\n'
            '      pass\n\n'
        )

        output_code = (
            'def a():\n'
            '  for x in range(10):\n'
            '      if x%2:\n'
            '            print(x)\n'
            '                else:'
        )
        # fmt: on

        events = code_to_events(input_code)
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, output_code)

    def test_bracketed_paste(self):
        """Test that bracketed paste using \x1b[200~ and \x1b[201~ works."""
        # fmt: off
        input_code = (
            'def a():\n'
            '  for x in range(10):\n'
            '\n'
            '    if x%2:\n'
            '      print(x)\n'
            '\n'
            '    else:\n'
            '      pass\n'
        )

        output_code = (
            'def a():\n'
            '  for x in range(10):\n'
            '\n'
            '    if x%2:\n'
            '      print(x)\n'
            '\n'
            '    else:\n'
            '      pass\n'
        )
        # fmt: on

        paste_start = "\x1b[200~"
        paste_end = "\x1b[201~"

        events = itertools.chain(
            code_to_events(paste_start),
            code_to_events(input_code),
            code_to_events(paste_end),
            code_to_events("\n"),
        )
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, output_code)

    def test_bracketed_paste_single_line(self):
        input_code = "oneline"

        paste_start = "\x1b[200~"
        paste_end = "\x1b[201~"

        events = itertools.chain(
            code_to_events(paste_start),
            code_to_events(input_code),
            code_to_events(paste_end),
            code_to_events("\n"),
        )
        reader = self.prepare_reader(events)
        output = multiline_input(reader)
        self.assertEqual(output, input_code)


class TestReader(TestCase):
    def assert_screen_equals(self, reader, expected):
        actual = reader.calc_screen()
        expected = expected.split("\n")
        self.assertListEqual(actual, expected)

    def test_calc_screen_wrap_simple(self):
        events = code_to_events(10 * "a")
        reader, _ = handle_events_narrow_console(events)
        self.assert_screen_equals(reader, f"{9*"a"}\\\na")

    def test_calc_screen_wrap_wide_characters(self):
        events = code_to_events(8 * "a" + "樂")
        reader, _ = handle_events_narrow_console(events)
        self.assert_screen_equals(reader, f"{8*"a"}\\\n樂")

    def test_calc_screen_wrap_three_lines(self):
        events = code_to_events(20 * "a")
        reader, _ = handle_events_narrow_console(events)
        self.assert_screen_equals(reader, f"{9*"a"}\\\n{9*"a"}\\\naa")

    def test_calc_screen_wrap_three_lines_mixed_character(self):
        # fmt: off
        code = (
            "def f():\n"
           f"  {8*"a"}\n"
           f"  {5*"樂"}"
        )
        # fmt: on

        events = code_to_events(code)
        reader, _ = handle_events_narrow_console(events)

        # fmt: off
        self.assert_screen_equals(reader, (
            "def f():\n"
           f"  {7*"a"}\\\n"
            "a\n"
           f"  {3*"樂"}\\\n"
            "樂樂"
        ))
        # fmt: on

    def test_calc_screen_backspace(self):
        events = itertools.chain(
            code_to_events("aaa"),
            [
                Event(evt="key", data="backspace", raw=bytearray(b"\x7f")),
            ],
        )
        reader, _ = handle_all_events(events)
        self.assert_screen_equals(reader, "aa")

    def test_calc_screen_wrap_removes_after_backspace(self):
        events = itertools.chain(
            code_to_events(10 * "a"),
            [
                Event(evt="key", data="backspace", raw=bytearray(b"\x7f")),
            ],
        )
        reader, _ = handle_events_narrow_console(events)
        self.assert_screen_equals(reader, 9 * "a")

    def test_calc_screen_backspace_in_second_line_after_wrap(self):
        events = itertools.chain(
            code_to_events(11 * "a"),
            [
                Event(evt="key", data="backspace", raw=bytearray(b"\x7f")),
            ],
        )
        reader, _ = handle_events_narrow_console(events)
        self.assert_screen_equals(reader, f"{9*"a"}\\\na")

    def test_setpos_for_xy_simple(self):
        events = code_to_events("11+11")
        reader, _ = handle_all_events(events)
        reader.setpos_from_xy(0, 0)
        self.assertEqual(reader.pos, 0)

    def test_setpos_from_xy_multiple_lines(self):
        # fmt: off
        code = (
            "def foo():\n"
            "  return 1"
        )
        # fmt: on

        events = code_to_events(code)
        reader, _ = handle_all_events(events)
        reader.setpos_from_xy(2, 1)
        self.assertEqual(reader.pos, 13)

    def test_setpos_from_xy_after_wrap(self):
        # fmt: off
        code = (
            "def foo():\n"
            "  hello"
        )
        # fmt: on

        events = code_to_events(code)
        reader, _ = handle_events_narrow_console(events)
        reader.setpos_from_xy(2, 2)
        self.assertEqual(reader.pos, 13)

    def test_setpos_fromxy_in_wrapped_line(self):
        # fmt: off
        code = (
            "def foo():\n"
            "  hello"
        )
        # fmt: on

        events = code_to_events(code)
        reader, _ = handle_events_narrow_console(events)
        reader.setpos_from_xy(0, 1)
        self.assertEqual(reader.pos, 9)

    def test_up_arrow_after_ctrl_r(self):
        events = iter(
            [
                Event(evt="key", data="\x12", raw=bytearray(b"\x12")),
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
            ]
        )

        reader, _ = handle_all_events(events)
        self.assert_screen_equals(reader, "")


class KeymapTranslatorTests(unittest.TestCase):
    def test_push_single_key(self):
        keymap = [("a", "command_a")]
        translator = KeymapTranslator(keymap)
        evt = Event("key", "a")
        translator.push(evt)
        result = translator.get()
        self.assertEqual(result, ("command_a", ["a"]))

    def test_push_multiple_keys(self):
        keymap = [("ab", "command_ab")]
        translator = KeymapTranslator(keymap)
        evt1 = Event("key", "a")
        evt2 = Event("key", "b")
        translator.push(evt1)
        translator.push(evt2)
        result = translator.get()
        self.assertEqual(result, ("command_ab", ["a", "b"]))

    def test_push_invalid_key(self):
        keymap = [("a", "command_a")]
        translator = KeymapTranslator(keymap)
        evt = Event("key", "b")
        translator.push(evt)
        result = translator.get()
        self.assertEqual(result, (None, ["b"]))

    def test_push_invalid_key_with_stack(self):
        keymap = [("ab", "command_ab")]
        translator = KeymapTranslator(keymap)
        evt1 = Event("key", "a")
        evt2 = Event("key", "c")
        translator.push(evt1)
        translator.push(evt2)
        result = translator.get()
        self.assertEqual(result, (None, ["a", "c"]))

    def test_push_character_key(self):
        keymap = [("a", "command_a")]
        translator = KeymapTranslator(keymap)
        evt = Event("key", "a")
        translator.push(evt)
        result = translator.get()
        self.assertEqual(result, ("command_a", ["a"]))

    def test_push_character_key_with_stack(self):
        keymap = [("ab", "command_ab")]
        translator = KeymapTranslator(keymap)
        evt1 = Event("key", "a")
        evt2 = Event("key", "b")
        evt3 = Event("key", "c")
        translator.push(evt1)
        translator.push(evt2)
        translator.push(evt3)
        result = translator.get()
        self.assertEqual(result, ("command_ab", ["a", "b"]))

    def test_push_transition_key(self):
        keymap = [("a", {"b": "command_ab"})]
        translator = KeymapTranslator(keymap)
        evt1 = Event("key", "a")
        evt2 = Event("key", "b")
        translator.push(evt1)
        translator.push(evt2)
        result = translator.get()
        self.assertEqual(result, ("command_ab", ["a", "b"]))

    def test_push_transition_key_interrupted(self):
        keymap = [("a", {"b": "command_ab"})]
        translator = KeymapTranslator(keymap)
        evt1 = Event("key", "a")
        evt2 = Event("key", "c")
        evt3 = Event("key", "b")
        translator.push(evt1)
        translator.push(evt2)
        translator.push(evt3)
        result = translator.get()
        self.assertEqual(result, (None, ["a", "c"]))

    def test_push_invalid_key_with_unicode_category(self):
        keymap = [("a", "command_a")]
        translator = KeymapTranslator(keymap)
        evt = Event("key", "\u0003")  # Control character
        translator.push(evt)
        result = translator.get()
        self.assertEqual(result, (None, ["\u0003"]))

    def test_empty(self):
        keymap = [("a", "command_a")]
        translator = KeymapTranslator(keymap)
        self.assertTrue(translator.empty())
        evt = Event("key", "a")
        translator.push(evt)
        self.assertFalse(translator.empty())
        translator.get()
        self.assertTrue(translator.empty())


class TestParseKeys(unittest.TestCase):
    def test_single_character(self):
        self.assertEqual(parse_keys("a"), ["a"])
        self.assertEqual(parse_keys("b"), ["b"])
        self.assertEqual(parse_keys("1"), ["1"])

    def test_escape_sequences(self):
        self.assertEqual(parse_keys("\\n"), ["\n"])
        self.assertEqual(parse_keys("\\t"), ["\t"])
        self.assertEqual(parse_keys("\\\\"), ["\\"])
        self.assertEqual(parse_keys("\\'"), ["'"])
        self.assertEqual(parse_keys('\\"'), ['"'])

    def test_control_sequences(self):
        self.assertEqual(parse_keys("\\C-a"), ["\x01"])
        self.assertEqual(parse_keys("\\C-b"), ["\x02"])
        self.assertEqual(parse_keys("\\C-c"), ["\x03"])

    def test_meta_sequences(self):
        self.assertEqual(parse_keys("\\M-a"), ["\033", "a"])
        self.assertEqual(parse_keys("\\M-b"), ["\033", "b"])
        self.assertEqual(parse_keys("\\M-c"), ["\033", "c"])

    def test_keynames(self):
        self.assertEqual(parse_keys("\\<up>"), ["up"])
        self.assertEqual(parse_keys("\\<down>"), ["down"])
        self.assertEqual(parse_keys("\\<left>"), ["left"])
        self.assertEqual(parse_keys("\\<right>"), ["right"])

    def test_combinations(self):
        self.assertEqual(parse_keys("\\C-a\\n\\<up>"), ["\x01", "\n", "up"])
        self.assertEqual(parse_keys("\\M-a\\t\\<down>"), ["\033", "a", "\t", "down"])


class TestCompileKeymap(unittest.TestCase):
    def test_empty_keymap(self):
        keymap = {}
        result = compile_keymap(keymap)
        self.assertEqual(result, {})

    def test_single_keymap(self):
        keymap = {b"a": "action"}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": "action"})

    def test_nested_keymap(self):
        keymap = {b"a": {b"b": "action"}}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": {b"b": "action"}})

    def test_empty_value(self):
        keymap = {b"a": {b"": "action"}}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": {b"": "action"}})

    def test_multiple_empty_values(self):
        keymap = {b"a": {b"": "action1", b"b": "action2"}}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": {b"": "action1", b"b": "action2"}})

    def test_multiple_keymaps(self):
        keymap = {b"a": {b"b": "action1", b"c": "action2"}}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": {b"b": "action1", b"c": "action2"}})

    def test_nested_multiple_keymaps(self):
        keymap = {b"a": {b"b": {b"c": "action"}}}
        result = compile_keymap(keymap)
        self.assertEqual(result, {b"a": {b"b": {b"c": "action"}}})


def unix_console(events, **kwargs):
    console = UnixConsole()
    console.get_event = MagicMock(side_effect=events)

    height = kwargs.get("height", 25)
    width = kwargs.get("width", 80)
    console.getheightwidth = MagicMock(side_effect=lambda: (height, width))

    console.prepare()
    for key, val in kwargs.items():
        setattr(console, key, val)
    return console


handle_events_unix_console = partial(
    handle_all_events,
    prepare_console=partial(unix_console),
)
handle_events_narrow_unix_console = partial(
    handle_all_events,
    prepare_console=partial(unix_console, width=5),
)
handle_events_short_unix_console = partial(
    handle_all_events,
    prepare_console=partial(unix_console, height=1),
)
handle_events_unix_console_height_3 = partial(
    handle_all_events, prepare_console=partial(unix_console, height=3)
)


TERM_CAPABILITIES = {
    "bel": b"\x07",
    "civis": b"\x1b[?25l",
    "clear": b"\x1b[H\x1b[2J",
    "cnorm": b"\x1b[?12l\x1b[?25h",
    "cub": b"\x1b[%p1%dD",
    "cub1": b"\x08",
    "cud": b"\x1b[%p1%dB",
    "cud1": b"\n",
    "cuf": b"\x1b[%p1%dC",
    "cuf1": b"\x1b[C",
    "cup": b"\x1b[%i%p1%d;%p2%dH",
    "cuu": b"\x1b[%p1%dA",
    "cuu1": b"\x1b[A",
    "dch1": b"\x1b[P",
    "dch": b"\x1b[%p1%dP",
    "el": b"\x1b[K",
    "hpa": b"\x1b[%i%p1%dG",
    "ich": b"\x1b[%p1%d@",
    "ich1": None,
    "ind": b"\n",
    "pad": None,
    "ri": b"\x1bM",
    "rmkx": b"\x1b[?1l\x1b>",
    "smkx": b"\x1b[?1h\x1b=",
}


@patch("_pyrepl.curses.tigetstr", lambda s: TERM_CAPABILITIES.get(s))
@patch(
    "_pyrepl.curses.tparm",
    lambda s, *args: s + b":" + b",".join(str(i).encode() for i in args),
)
@patch("_pyrepl.curses.setupterm", lambda a, b: None)
@patch(
    "termios.tcgetattr",
    lambda _: [
        27394,
        3,
        19200,
        536872399,
        38400,
        38400,
        [
            b"\x04",
            b"\xff",
            b"\xff",
            b"\x7f",
            b"\x17",
            b"\x15",
            b"\x12",
            b"\x00",
            b"\x03",
            b"\x1c",
            b"\x1a",
            b"\x19",
            b"\x11",
            b"\x13",
            b"\x16",
            b"\x0f",
            b"\x01",
            b"\x00",
            b"\x14",
            b"\x00",
        ],
    ],
)
@patch("termios.tcsetattr", lambda a, b, c: None)
@patch("os.write")
class TestConsole(TestCase):
    def test_simple_addition(self, _os_write):
        code = "12+34"
        events = code_to_events(code)
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, b"1")
        _os_write.assert_any_call(ANY, b"2")
        _os_write.assert_any_call(ANY, b"+")
        _os_write.assert_any_call(ANY, b"3")
        _os_write.assert_any_call(ANY, b"4")

    def test_wrap(self, _os_write):
        code = "12+34"
        events = code_to_events(code)
        _, _ = handle_events_narrow_unix_console(events)
        _os_write.assert_any_call(ANY, b"1")
        _os_write.assert_any_call(ANY, b"2")
        _os_write.assert_any_call(ANY, b"+")
        _os_write.assert_any_call(ANY, b"3")
        _os_write.assert_any_call(ANY, b"\\")
        _os_write.assert_any_call(ANY, b"\n")
        _os_write.assert_any_call(ANY, b"4")

    def test_cursor_left(self, _os_write):
        code = "1"
        events = itertools.chain(
            code_to_events(code),
            [Event(evt="key", data="left", raw=bytearray(b"\x1bOD"))],
        )
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cub"] + b":1")

    def test_cursor_left_right(self, _os_write):
        code = "1"
        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="left", raw=bytearray(b"\x1bOD")),
                Event(evt="key", data="right", raw=bytearray(b"\x1bOC")),
            ],
        )
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cub"] + b":1")
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cuf"] + b":1")

    def test_cursor_up(self, _os_write):
        code = "1\n2+3"
        events = itertools.chain(
            code_to_events(code),
            [Event(evt="key", data="up", raw=bytearray(b"\x1bOA"))],
        )
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cuu"] + b":1")

    def test_cursor_up_down(self, _os_write):
        code = "1\n2+3"
        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
            ],
        )
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cuu"] + b":1")
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cud"] + b":1")

    def test_cursor_back_write(self, _os_write):
        events = itertools.chain(
            code_to_events("1"),
            [Event(evt="key", data="left", raw=bytearray(b"\x1bOD"))],
            code_to_events("2"),
        )
        _, _ = handle_events_unix_console(events)
        _os_write.assert_any_call(ANY, b"1")
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["cub"] + b":1")
        _os_write.assert_any_call(ANY, b"2")

    def test_multiline_function_move_up_short_terminal(self, _os_write):
        # fmt: off
        code = (
            "def f():\n"
            "  foo"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="scroll", data=None),
            ],
        )
        _, _ = handle_events_short_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["ri"] + b":")

    def test_multiline_function_move_up_down_short_terminal(self, _os_write):
        # fmt: off
        code = (
            "def f():\n"
            "  foo"
        )
        # fmt: on

        events = itertools.chain(
            code_to_events(code),
            [
                Event(evt="key", data="up", raw=bytearray(b"\x1bOA")),
                Event(evt="scroll", data=None),
                Event(evt="key", data="down", raw=bytearray(b"\x1bOB")),
                Event(evt="scroll", data=None),
            ],
        )
        _, _ = handle_events_short_unix_console(events)
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["ri"] + b":")
        _os_write.assert_any_call(ANY, TERM_CAPABILITIES["ind"] + b":")

    def test_resize_bigger_on_multiline_function(self, _os_write):
        # fmt: off
        code = (
            "def f():\n"
            "  foo"
        )
        # fmt: on

        events = itertools.chain(code_to_events(code))
        reader, console = handle_events_short_unix_console(events)

        console.height = 2
        console.getheightwidth = MagicMock(lambda _: (2, 80))

        def same_reader(_):
            return reader

        def same_console(events):
            console.get_event = MagicMock(side_effect=events)
            return console

        _, _ = handle_all_events(
            [Event(evt="resize", data=None)],
            prepare_reader=same_reader,
            prepare_console=same_console,
        )
        _os_write.assert_has_calls(
            [
                call(ANY, TERM_CAPABILITIES["ri"] + b":"),
                call(ANY, TERM_CAPABILITIES["cup"] + b":0,0"),
                call(ANY, b"def f():"),
            ]
        )

    def test_resize_smaller_on_multiline_function(self, _os_write):
        # fmt: off
        code = (
            "def f():\n"
            "  foo"
        )
        # fmt: on

        events = itertools.chain(code_to_events(code))
        reader, console = handle_events_unix_console_height_3(events)

        console.height = 1
        console.getheightwidth = MagicMock(lambda _: (1, 80))

        def same_reader(_):
            return reader

        def same_console(events):
            console.get_event = MagicMock(side_effect=events)
            return console

        _, _ = handle_all_events(
            [Event(evt="resize", data=None)],
            prepare_reader=same_reader,
            prepare_console=same_console,
        )
        _os_write.assert_has_calls(
            [
                call(ANY, TERM_CAPABILITIES["ind"] + b":"),
                call(ANY, TERM_CAPABILITIES["cup"] + b":0,0"),
                call(ANY, b"  foo"),
            ]
        )


if __name__ == "__main__":
    unittest.main()
