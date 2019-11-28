"""Tests for sys.audit and sys.addaudithook
"""

import subprocess
import sys
import unittest
from test import support

if not hasattr(sys, "addaudithook") or not hasattr(sys, "audit"):
    raise unittest.SkipTest("test only relevant when sys.audit is available")

AUDIT_TESTS_PY = support.findfile("audit-tests.py")


class AuditTest(unittest.TestCase):
    def do_test(self, *args):
        with subprocess.Popen(
            [sys.executable, "-X utf8", AUDIT_TESTS_PY, *args],
            encoding="utf-8",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ) as p:
            p.wait()
            sys.stdout.writelines(p.stdout)
            sys.stderr.writelines(p.stderr)
            if p.returncode:
                self.fail("".join(p.stderr))

    def run_python(self, *args):
        events = []
        with subprocess.Popen(
            [sys.executable, "-X utf8", AUDIT_TESTS_PY, *args],
            encoding="utf-8",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ) as p:
            p.wait()
            sys.stderr.writelines(p.stderr)
            return (
                p.returncode,
                [line.strip().partition(" ") for line in p.stdout],
                "".join(p.stderr),
            )

    def test_basic(self):
        self.do_test("test_basic")

    def test_block_add_hook(self):
        self.do_test("test_block_add_hook")

    def test_block_add_hook_baseexception(self):
        self.do_test("test_block_add_hook_baseexception")

    def test_finalize_hooks(self):
        returncode, events, stderr = self.run_python("test_finalize_hooks")
        if stderr:
            print(stderr, file=sys.stderr)
        if returncode:
            self.fail(stderr)

        firstId = events[0][2]
        self.assertSequenceEqual(
            [
                ("Created", " ", firstId),
                ("cpython._PySys_ClearAuditHooks", " ", firstId),
            ],
            events,
        )

    def test_pickle(self):
        support.import_module("pickle")

        self.do_test("test_pickle")

    def test_monkeypatch(self):
        self.do_test("test_monkeypatch")

    def test_open(self):
        self.do_test("test_open", support.TESTFN)

    def test_cantrace(self):
        self.do_test("test_cantrace")

    def test_mmap(self):
        self.do_test("test_mmap")

    def test_excepthook(self):
        returncode, events, stderr = self.run_python("test_excepthook")
        if not returncode:
            self.fail(f"Expected fatal exception\n{stderr}")

        self.assertSequenceEqual(
            [("sys.excepthook", " ", "RuntimeError('fatal-error')")], events
        )

    def test_unraisablehook(self):
        returncode, events, stderr = self.run_python("test_unraisablehook")
        if returncode:
            self.fail(stderr)

        self.assertEqual(events[0][0], "sys.unraisablehook")
        self.assertEqual(
            events[0][2],
            "RuntimeError('nonfatal-error') Exception ignored for audit hook test",
        )


if __name__ == "__main__":
    unittest.main()
