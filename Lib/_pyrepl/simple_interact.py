#   Copyright 2000-2010 Michael Hudson-Doyle <micahel@gmail.com>
#                       Armin Rigo
#
#                        All Rights Reserved
#
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose is hereby granted without fee,
# provided that the above copyright notice appear in all copies and
# that both that copyright notice and this permission notice appear in
# supporting documentation.
#
# THE AUTHOR MICHAEL HUDSON DISCLAIMS ALL WARRANTIES WITH REGARD TO
# THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS, IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
# INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
# CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

"""This is an alternative to python_reader which tries to emulate
the CPython prompt as closely as possible, with the exception of
allowing multiline input and multiline history entries.
"""

from __future__ import annotations

import _sitebuiltins
import linecache
import functools
import sys
import code

from .readline import _get_reader, multiline_input

TYPE_CHECKING = False

if TYPE_CHECKING:
    from typing import Any


_error: tuple[type[Exception], ...] | type[Exception]
try:
    from .unix_console import _error
except ModuleNotFoundError:
    from .windows_console import _error

def check() -> str:
    """Returns the error message if there is a problem initializing the state."""
    try:
        _get_reader()
    except _error as e:
        return str(e) or repr(e) or "unknown error"
    return ""


def _strip_final_indent(text: str) -> str:
    # kill spaces and tabs at the end, but only if they follow '\n'.
    # meant to remove the auto-indentation only (although it would of
    # course also remove explicitly-added indentation).
    short = text.rstrip(" \t")
    n = len(short)
    if n > 0 and text[n - 1] == "\n":
        return short
    return text


def _clear_screen():
    reader = _get_reader()
    reader.scheduled_commands.append("clear_screen")


REPL_COMMANDS = {
    "exit": _sitebuiltins.Quitter('exit', ''),
    "quit": _sitebuiltins.Quitter('quit' ,''),
    "copyright": _sitebuiltins._Printer('copyright', sys.copyright),
    "help": "help",
    "clear": _clear_screen,
}


def _more_lines(console: code.InteractiveConsole, unicodetext: str) -> bool:
    # ooh, look at the hack:
    src = _strip_final_indent(unicodetext)
    try:
        code = console.compile(src, "<stdin>", "single")
    except (OverflowError, SyntaxError, ValueError):
        lines = src.splitlines(keepends=True)
        if len(lines) == 1:
            return False

        last_line = lines[-1]
        was_indented = last_line.startswith((" ", "\t"))
        not_empty = last_line.strip() != ""
        incomplete = not last_line.endswith("\n")
        return (was_indented or not_empty) and incomplete
    else:
        return code is None


def run_multiline_interactive_console(
    console: code.InteractiveConsole,
    *,
    future_flags: int = 0,
) -> None:
    from .readline import _setup
    _setup(console.locals)
    if future_flags:
        console.compile.compiler.flags |= future_flags

    more_lines = functools.partial(_more_lines, console)
    input_n = 0

    def maybe_run_command(statement: str) -> bool:
        statement = statement.strip()
        if statement in console.locals or statement not in REPL_COMMANDS:
            return False

        reader = _get_reader()
        reader.history.pop()  # skip internal commands in history
        command = REPL_COMMANDS[statement]
        if callable(command):
            command()
            return True

        if isinstance(command, str):
            # Internal readline commands require a prepared reader like
            # inside multiline_input.
            reader.prepare()
            reader.refresh()
            reader.do_cmd((command, [statement]))
            reader.restore()
            return True

        return False

    while 1:
        try:
            try:
                sys.stdout.flush()
            except Exception:
                pass

            ps1 = getattr(sys, "ps1", ">>> ")
            ps2 = getattr(sys, "ps2", "... ")
            try:
                statement = multiline_input(more_lines, ps1, ps2)
            except EOFError:
                break

            if maybe_run_command(statement):
                continue

            input_name = f"<python-input-{input_n}>"
            linecache._register_code(input_name, statement, "<stdin>")  # type: ignore[attr-defined]
            more = console.push(_strip_final_indent(statement), filename=input_name, _symbol="single")  # type: ignore[call-arg]
            assert not more
            input_n += 1
        except KeyboardInterrupt:
            r = _get_reader()
            if r.last_command and 'isearch' in r.last_command.__name__:
                r.isearch_direction = ''
                r.console.forgetinput()
                r.pop_input_trans()
                r.dirty = True
            r.refresh()
            r.in_bracketed_paste = False
            console.write("\nKeyboardInterrupt\n")
            console.resetbuffer()
        except MemoryError:
            console.write("\nMemoryError\n")
            console.resetbuffer()
