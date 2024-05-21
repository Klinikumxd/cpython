import itertools
import os
import rlcompleter
import unittest
from unittest import TestCase

from .support import FakeConsole, handle_all_events, handle_events_narrow_console, multiline_input, code_to_events
from _pyrepl.console import Event
from _pyrepl.readline import ReadlineAlikeReader, ReadlineConfig


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


if __name__ == "__main__":
    unittest.main()
