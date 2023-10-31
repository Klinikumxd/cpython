"""Test suite for the sys.monitoring."""

import collections
import dis
import functools
import operator
import sys
import textwrap
import types
import unittest
import asyncio

PAIR = (0,1)

def f1():
    pass

def f2():
    len([])
    sys.getsizeof(0)

def floop():
    for item in PAIR:
        pass

def gen():
    yield
    yield

def g1():
    for _ in gen():
        pass

TEST_TOOL = 2
TEST_TOOL2 = 3
TEST_TOOL3 = 4

class MonitoringBasicTest(unittest.TestCase):

    def test_has_objects(self):
        m = sys.monitoring
        m.events
        m.use_tool_id
        m.free_tool_id
        m.get_tool
        m.get_events
        m.set_events
        m.get_local_events
        m.set_local_events
        m.register_callback
        m.restart_events
        m.DISABLE
        m.MISSING
        m.events.NO_EVENTS

    def test_tool(self):
        sys.monitoring.use_tool_id(TEST_TOOL, "MonitoringTest.Tool")
        self.assertEqual(sys.monitoring.get_tool(TEST_TOOL), "MonitoringTest.Tool")
        sys.monitoring.set_events(TEST_TOOL, 15)
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL), 15)
        sys.monitoring.set_events(TEST_TOOL, 0)
        with self.assertRaises(ValueError):
            sys.monitoring.set_events(TEST_TOOL, sys.monitoring.events.C_RETURN)
        with self.assertRaises(ValueError):
            sys.monitoring.set_events(TEST_TOOL, sys.monitoring.events.C_RAISE)
        sys.monitoring.free_tool_id(TEST_TOOL)
        self.assertEqual(sys.monitoring.get_tool(TEST_TOOL), None)
        with self.assertRaises(ValueError):
            sys.monitoring.set_events(TEST_TOOL, sys.monitoring.events.CALL)


class MonitoringTestBase:

    def setUp(self):
        # Check that a previous test hasn't left monitoring on.
        for tool in range(6):
            self.assertEqual(sys.monitoring.get_events(tool), 0)
        self.assertIs(sys.monitoring.get_tool(TEST_TOOL), None)
        self.assertIs(sys.monitoring.get_tool(TEST_TOOL2), None)
        self.assertIs(sys.monitoring.get_tool(TEST_TOOL3), None)
        sys.monitoring.use_tool_id(TEST_TOOL, "test " + self.__class__.__name__)
        sys.monitoring.use_tool_id(TEST_TOOL2, "test2 " + self.__class__.__name__)
        sys.monitoring.use_tool_id(TEST_TOOL3, "test3 " + self.__class__.__name__)

    def tearDown(self):
        # Check that test hasn't left monitoring on.
        for tool in range(6):
            self.assertEqual(sys.monitoring.get_events(tool), 0)
        sys.monitoring.free_tool_id(TEST_TOOL)
        sys.monitoring.free_tool_id(TEST_TOOL2)
        sys.monitoring.free_tool_id(TEST_TOOL3)


class MonitoringCountTest(MonitoringTestBase, unittest.TestCase):

    def check_event_count(self, func, event, expected):

        class Counter:
            def __init__(self):
                self.count = 0
            def __call__(self, *args):
                self.count += 1

        counter = Counter()
        sys.monitoring.register_callback(TEST_TOOL, event, counter)
        if event == E.C_RETURN or event == E.C_RAISE:
            sys.monitoring.set_events(TEST_TOOL, E.CALL)
        else:
            sys.monitoring.set_events(TEST_TOOL, event)
        self.assertEqual(counter.count, 0)
        counter.count = 0
        func()
        self.assertEqual(counter.count, expected)
        prev = sys.monitoring.register_callback(TEST_TOOL, event, None)
        counter.count = 0
        func()
        self.assertEqual(counter.count, 0)
        self.assertEqual(prev, counter)
        sys.monitoring.set_events(TEST_TOOL, 0)

    def test_start_count(self):
        self.check_event_count(f1, E.PY_START, 1)

    def test_resume_count(self):
        self.check_event_count(g1, E.PY_RESUME, 2)

    def test_return_count(self):
        self.check_event_count(f1, E.PY_RETURN, 1)

    def test_call_count(self):
        self.check_event_count(f2, E.CALL, 3)

    def test_c_return_count(self):
        self.check_event_count(f2, E.C_RETURN, 2)


E = sys.monitoring.events

INSTRUMENTED_EVENTS = [
    (E.PY_START, "start"),
    (E.PY_RESUME, "resume"),
    (E.PY_RETURN, "return"),
    (E.PY_YIELD, "yield"),
    (E.JUMP, "jump"),
    (E.BRANCH, "branch"),
]

EXCEPT_EVENTS = [
    (E.RAISE, "raise"),
    (E.PY_UNWIND, "unwind"),
    (E.EXCEPTION_HANDLED, "exception_handled"),
]

SIMPLE_EVENTS = INSTRUMENTED_EVENTS + EXCEPT_EVENTS + [
    (E.C_RAISE, "c_raise"),
    (E.C_RETURN, "c_return"),
]


SIMPLE_EVENT_SET = functools.reduce(operator.or_, [ev for (ev, _) in SIMPLE_EVENTS], 0) | E.CALL


def just_pass():
    pass

just_pass.events = [
    "py_call",
    "start",
    "return",
]

def just_raise():
    raise Exception

just_raise.events = [
    'py_call',
    "start",
    "raise",
    "unwind",
]

def just_call():
    len([])

just_call.events = [
    'py_call',
    "start",
    "c_call",
    "c_return",
    "return",
]

def caught():
    try:
        1/0
    except Exception:
        pass

caught.events = [
    'py_call',
    "start",
    "raise",
    "exception_handled",
    "branch",
    "return",
]

def nested_call():
    just_pass()

nested_call.events = [
    "py_call",
    "start",
    "py_call",
    "start",
    "return",
    "return",
]

PY_CALLABLES = (types.FunctionType, types.MethodType)

class MonitoringEventsBase(MonitoringTestBase):

    def gather_events(self, func):
        events = []
        for event, event_name in SIMPLE_EVENTS:
            def record(*args, event_name=event_name):
                events.append(event_name)
            sys.monitoring.register_callback(TEST_TOOL, event, record)
        def record_call(code, offset, obj, arg):
            if isinstance(obj, PY_CALLABLES):
                events.append("py_call")
            else:
                events.append("c_call")
        sys.monitoring.register_callback(TEST_TOOL, E.CALL, record_call)
        sys.monitoring.set_events(TEST_TOOL, SIMPLE_EVENT_SET)
        events = []
        try:
            func()
        except:
            pass
        sys.monitoring.set_events(TEST_TOOL, 0)
        #Remove the final event, the call to `sys.monitoring.set_events`
        events = events[:-1]
        return events

    def check_events(self, func, expected=None):
        events = self.gather_events(func)
        if expected is None:
            expected = func.events
        self.assertEqual(events, expected)

class MonitoringEventsTest(MonitoringEventsBase, unittest.TestCase):

    def test_just_pass(self):
        self.check_events(just_pass)

    def test_just_raise(self):
        try:
            self.check_events(just_raise)
        except Exception:
            pass
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL), 0)

    def test_just_call(self):
        self.check_events(just_call)

    def test_caught(self):
        self.check_events(caught)

    def test_nested_call(self):
        self.check_events(nested_call)

UP_EVENTS = (E.C_RETURN, E.C_RAISE, E.PY_RETURN, E.PY_UNWIND, E.PY_YIELD)
DOWN_EVENTS = (E.PY_START, E.PY_RESUME)

from test.profilee import testfunc

class SimulateProfileTest(MonitoringEventsBase, unittest.TestCase):

    def test_balanced(self):
        events = self.gather_events(testfunc)
        c = collections.Counter(events)
        self.assertEqual(c["c_call"], c["c_return"])
        self.assertEqual(c["start"], c["return"] + c["unwind"])
        self.assertEqual(c["raise"], c["exception_handled"] + c["unwind"])

    def test_frame_stack(self):
        self.maxDiff = None
        stack = []
        errors = []
        seen = set()
        def up(*args):
            frame = sys._getframe(1)
            if not stack:
                errors.append("empty")
            else:
                expected = stack.pop()
                if frame != expected:
                    errors.append(f" Popping {frame} expected {expected}")
        def down(*args):
            frame = sys._getframe(1)
            stack.append(frame)
            seen.add(frame.f_code)
        def call(code, offset, callable, arg):
            if not isinstance(callable, PY_CALLABLES):
                stack.append(sys._getframe(1))
        for event in UP_EVENTS:
            sys.monitoring.register_callback(TEST_TOOL, event, up)
        for event in DOWN_EVENTS:
            sys.monitoring.register_callback(TEST_TOOL, event, down)
        sys.monitoring.register_callback(TEST_TOOL, E.CALL, call)
        sys.monitoring.set_events(TEST_TOOL, SIMPLE_EVENT_SET)
        testfunc()
        sys.monitoring.set_events(TEST_TOOL, 0)
        self.assertEqual(errors, [])
        self.assertEqual(stack, [sys._getframe()])
        self.assertEqual(len(seen), 9)


class CounterWithDisable:

    def __init__(self):
        self.disable = False
        self.count = 0

    def __call__(self, *args):
        self.count += 1
        if self.disable:
            return sys.monitoring.DISABLE


class RecorderWithDisable:

    def __init__(self, events):
        self.disable = False
        self.events = events

    def __call__(self, code, event):
        self.events.append(event)
        if self.disable:
            return sys.monitoring.DISABLE


class MontoringDisableAndRestartTest(MonitoringTestBase, unittest.TestCase):

    def test_disable(self):
        try:
            counter = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            self.assertEqual(counter.count, 0)
            counter.count = 0
            f1()
            self.assertEqual(counter.count, 1)
            counter.disable = True
            counter.count = 0
            f1()
            self.assertEqual(counter.count, 1)
            counter.count = 0
            f1()
            self.assertEqual(counter.count, 0)
            sys.monitoring.set_events(TEST_TOOL, 0)
        finally:
            sys.monitoring.restart_events()

    def test_restart(self):
        try:
            counter = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            counter.disable = True
            f1()
            counter.count = 0
            f1()
            self.assertEqual(counter.count, 0)
            sys.monitoring.restart_events()
            counter.count = 0
            f1()
            self.assertEqual(counter.count, 1)
            sys.monitoring.set_events(TEST_TOOL, 0)
        finally:
            sys.monitoring.restart_events()


class MultipleMonitorsTest(MonitoringTestBase, unittest.TestCase):

    def test_two_same(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            counter1 = CounterWithDisable()
            counter2 = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter1)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, counter2)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            sys.monitoring.set_events(TEST_TOOL2, E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL), E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), E.PY_START)
            self.assertEqual(sys.monitoring._all_events(), {'PY_START': (1 << TEST_TOOL) | (1 << TEST_TOOL2)})
            counter1.count = 0
            counter2.count = 0
            f1()
            count1 = counter1.count
            count2 = counter2.count
            self.assertEqual((count1, count2), (1, 1))
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, None)
            self.assertEqual(sys.monitoring._all_events(), {})

    def test_three_same(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            counter1 = CounterWithDisable()
            counter2 = CounterWithDisable()
            counter3 = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter1)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, counter2)
            sys.monitoring.register_callback(TEST_TOOL3, E.PY_START, counter3)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            sys.monitoring.set_events(TEST_TOOL2, E.PY_START)
            sys.monitoring.set_events(TEST_TOOL3, E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL), E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL3), E.PY_START)
            self.assertEqual(sys.monitoring._all_events(), {'PY_START': (1 << TEST_TOOL) | (1 << TEST_TOOL2) | (1 << TEST_TOOL3)})
            counter1.count = 0
            counter2.count = 0
            counter3.count = 0
            f1()
            count1 = counter1.count
            count2 = counter2.count
            count3 = counter3.count
            self.assertEqual((count1, count2, count3), (1, 1, 1))
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.set_events(TEST_TOOL3, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, None)
            sys.monitoring.register_callback(TEST_TOOL3, E.PY_START, None)
            self.assertEqual(sys.monitoring._all_events(), {})

    def test_two_different(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            counter1 = CounterWithDisable()
            counter2 = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter1)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_RETURN, counter2)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            sys.monitoring.set_events(TEST_TOOL2, E.PY_RETURN)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL), E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), E.PY_RETURN)
            self.assertEqual(sys.monitoring._all_events(), {'PY_START': 1 << TEST_TOOL, 'PY_RETURN': 1 << TEST_TOOL2})
            counter1.count = 0
            counter2.count = 0
            f1()
            count1 = counter1.count
            count2 = counter2.count
            self.assertEqual((count1, count2), (1, 1))
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_RETURN, None)
            self.assertEqual(sys.monitoring._all_events(), {})

    def test_two_with_disable(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            counter1 = CounterWithDisable()
            counter2 = CounterWithDisable()
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, counter1)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, counter2)
            sys.monitoring.set_events(TEST_TOOL, E.PY_START)
            sys.monitoring.set_events(TEST_TOOL2, E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL), E.PY_START)
            self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), E.PY_START)
            self.assertEqual(sys.monitoring._all_events(), {'PY_START': (1 << TEST_TOOL) | (1 << TEST_TOOL2)})
            counter1.count = 0
            counter2.count = 0
            counter1.disable = True
            f1()
            count1 = counter1.count
            count2 = counter2.count
            self.assertEqual((count1, count2), (1, 1))
            counter1.count = 0
            counter2.count = 0
            f1()
            count1 = counter1.count
            count2 = counter2.count
            self.assertEqual((count1, count2), (0, 1))
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.PY_START, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.PY_START, None)
            self.assertEqual(sys.monitoring._all_events(), {})
            sys.monitoring.restart_events()

    def test_with_instruction_event(self):
        """Test that the second tool can set events with instruction events set by the first tool."""
        def f():
            pass
        code = f.__code__

        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            sys.monitoring.set_local_events(TEST_TOOL, code, E.INSTRUCTION | E.LINE)
            sys.monitoring.set_local_events(TEST_TOOL2, code, E.LINE)
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            self.assertEqual(sys.monitoring._all_events(), {})


class LineMonitoringTest(MonitoringTestBase, unittest.TestCase):

    def test_lines_single(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            events = []
            recorder = RecorderWithDisable(events)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, recorder)
            sys.monitoring.set_events(TEST_TOOL, E.LINE)
            f1()
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            start = LineMonitoringTest.test_lines_single.__code__.co_firstlineno
            self.assertEqual(events, [start+7, 16, start+8])
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            self.assertEqual(sys.monitoring._all_events(), {})
            sys.monitoring.restart_events()

    def test_lines_loop(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            events = []
            recorder = RecorderWithDisable(events)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, recorder)
            sys.monitoring.set_events(TEST_TOOL, E.LINE)
            floop()
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            start = LineMonitoringTest.test_lines_loop.__code__.co_firstlineno
            self.assertEqual(events, [start+7, 23, 24, 23, 24, 23, start+8])
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            self.assertEqual(sys.monitoring._all_events(), {})
            sys.monitoring.restart_events()

    def test_lines_two(self):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            events = []
            recorder = RecorderWithDisable(events)
            events2 = []
            recorder2 = RecorderWithDisable(events2)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, recorder)
            sys.monitoring.register_callback(TEST_TOOL2, E.LINE, recorder2)
            sys.monitoring.set_events(TEST_TOOL, E.LINE); sys.monitoring.set_events(TEST_TOOL2, E.LINE)
            f1()
            sys.monitoring.set_events(TEST_TOOL, 0); sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.LINE, None)
            start = LineMonitoringTest.test_lines_two.__code__.co_firstlineno
            expected = [start+10, 16, start+11]
            self.assertEqual(events, expected)
            self.assertEqual(events2, expected)
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)
            sys.monitoring.set_events(TEST_TOOL2, 0)
            sys.monitoring.register_callback(TEST_TOOL, E.LINE, None)
            sys.monitoring.register_callback(TEST_TOOL2, E.LINE, None)
            self.assertEqual(sys.monitoring._all_events(), {})
            sys.monitoring.restart_events()

    def check_lines(self, func, expected, tool=TEST_TOOL):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            events = []
            recorder = RecorderWithDisable(events)
            sys.monitoring.register_callback(tool, E.LINE, recorder)
            sys.monitoring.set_events(tool, E.LINE)
            func()
            sys.monitoring.set_events(tool, 0)
            sys.monitoring.register_callback(tool, E.LINE, None)
            lines = [ line - func.__code__.co_firstlineno for line in events[1:-1] ]
            self.assertEqual(lines, expected)
        finally:
            sys.monitoring.set_events(tool, 0)


    def test_linear(self):

        def func():
            line = 1
            line = 2
            line = 3
            line = 4
            line = 5

        self.check_lines(func, [1,2,3,4,5])

    def test_branch(self):
        def func():
            if "true".startswith("t"):
                line = 2
                line = 3
            else:
                line = 5
            line = 6

        self.check_lines(func, [1,2,3,6])

    def test_try_except(self):

        def func1():
            try:
                line = 2
                line = 3
            except:
                line = 5
            line = 6

        self.check_lines(func1, [1,2,3,6])

        def func2():
            try:
                line = 2
                raise 3
            except:
                line = 5
            line = 6

        self.check_lines(func2, [1,2,3,4,5,6])

class TestDisable(MonitoringTestBase, unittest.TestCase):

    def gen(self, cond):
        for i in range(10):
            if cond:
                yield 1
            else:
                yield 2

    def raise_handle_reraise(self):
        try:
            1/0
        except:
            raise

    def test_disable_legal_events(self):
        for event, name in INSTRUMENTED_EVENTS:
            try:
                counter = CounterWithDisable()
                counter.disable = True
                sys.monitoring.register_callback(TEST_TOOL, event, counter)
                sys.monitoring.set_events(TEST_TOOL, event)
                for _ in self.gen(1):
                    pass
                self.assertLess(counter.count, 4)
            finally:
                sys.monitoring.set_events(TEST_TOOL, 0)
                sys.monitoring.register_callback(TEST_TOOL, event, None)


    def test_disable_illegal_events(self):
        for event, name in EXCEPT_EVENTS:
            try:
                counter = CounterWithDisable()
                counter.disable = True
                sys.monitoring.register_callback(TEST_TOOL, event, counter)
                sys.monitoring.set_events(TEST_TOOL, event)
                with self.assertRaises(ValueError):
                    self.raise_handle_reraise()
            finally:
                sys.monitoring.set_events(TEST_TOOL, 0)
                sys.monitoring.register_callback(TEST_TOOL, event, None)


class ExceptionRecorder:

    event_type = E.RAISE

    def __init__(self, events):
        self.events = events

    def __call__(self, code, offset, exc):
        self.events.append(("raise", type(exc)))

class CheckEvents(MonitoringTestBase, unittest.TestCase):

    def get_events(self, func, tool, recorders):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            event_list = []
            all_events = 0
            for recorder in recorders:
                ev = recorder.event_type
                sys.monitoring.register_callback(tool, ev, recorder(event_list))
                all_events |= ev
            sys.monitoring.set_events(tool, all_events)
            func()
            sys.monitoring.set_events(tool, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)
            return event_list
        finally:
            sys.monitoring.set_events(tool, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)

    def check_events(self, func, expected, tool=TEST_TOOL, recorders=(ExceptionRecorder,)):
        events = self.get_events(func, tool, recorders)
        if events != expected:
            print(events, file = sys.stderr)
        self.assertEqual(events, expected)

    def check_balanced(self, func, recorders):
        events = self.get_events(func, TEST_TOOL, recorders)
        self.assertEqual(len(events)%2, 0)
        for r, h in zip(events[::2],events[1::2]):
            r0 = r[0]
            self.assertIn(r0, ("raise", "reraise"))
            h0 = h[0]
            self.assertIn(h0, ("handled", "unwind"))
            self.assertEqual(r[1], h[1])


class StopiterationRecorder(ExceptionRecorder):

    event_type = E.STOP_ITERATION

class ReraiseRecorder(ExceptionRecorder):

    event_type = E.RERAISE

    def __call__(self, code, offset, exc):
        self.events.append(("reraise", type(exc)))

class UnwindRecorder(ExceptionRecorder):

    event_type = E.PY_UNWIND

    def __call__(self, code, offset, exc):
        self.events.append(("unwind", type(exc)))

class ExceptionHandledRecorder(ExceptionRecorder):

    event_type = E.EXCEPTION_HANDLED

    def __call__(self, code, offset, exc):
        self.events.append(("handled", type(exc)))

class ThrowRecorder(ExceptionRecorder):

    event_type = E.PY_THROW

    def __call__(self, code, offset, exc):
        self.events.append(("throw", type(exc)))

class ExceptionMonitoringTest(CheckEvents):


    exception_recorders = (
        ExceptionRecorder,
        ReraiseRecorder,
        ExceptionHandledRecorder,
        UnwindRecorder
    )

    def test_simple_try_except(self):

        def func1():
            try:
                line = 2
                raise KeyError
            except:
                line = 5
            line = 6

        self.check_events(func1, [("raise", KeyError)])

    def test_implicit_stop_iteration(self):

        def gen():
            yield 1
            return 2

        def implicit_stop_iteration():
            for _ in gen():
                pass

        self.check_events(implicit_stop_iteration, [("raise", StopIteration)], recorders=(StopiterationRecorder,))

    initial = [
        ("raise", ZeroDivisionError),
        ("handled", ZeroDivisionError)
    ]

    reraise = [
        ("reraise", ZeroDivisionError),
        ("handled", ZeroDivisionError)
    ]

    def test_explicit_reraise(self):

        def func():
            try:
                try:
                    1/0
                except:
                    raise
            except:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

    def test_explicit_reraise_named(self):

        def func():
            try:
                try:
                    1/0
                except Exception as ex:
                    raise
            except:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

    def test_implicit_reraise(self):

        def func():
            try:
                try:
                    1/0
                except ValueError:
                    pass
            except:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)


    def test_implicit_reraise_named(self):

        def func():
            try:
                try:
                    1/0
                except ValueError as ex:
                    pass
            except:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

    def test_try_finally(self):

        def func():
            try:
                try:
                    1/0
                finally:
                    pass
            except:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

    def test_async_for(self):

        def func():

            async def async_generator():
                for i in range(1):
                    raise ZeroDivisionError
                    yield i

            async def async_loop():
                try:
                    async for item in async_generator():
                        pass
                except Exception:
                    pass

            try:
                async_loop().send(None)
            except StopIteration:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

    def test_throw(self):

        def gen():
            yield 1
            yield 2

        def func():
            try:
                g = gen()
                next(g)
                g.throw(IndexError)
            except IndexError:
                pass

        self.check_balanced(
            func,
            recorders = self.exception_recorders)

        events = self.get_events(
            func,
            TEST_TOOL,
            self.exception_recorders + (ThrowRecorder,)
        )
        self.assertEqual(events[0], ("throw", IndexError))

class LineRecorder:

    event_type = E.LINE


    def __init__(self, events):
        self.events = events

    def __call__(self, code, line):
        self.events.append(("line", code.co_name, line - code.co_firstlineno))

class CallRecorder:

    event_type = E.CALL

    def __init__(self, events):
        self.events = events

    def __call__(self, code, offset, func, arg):
        self.events.append(("call", func.__name__, arg))

class CEventRecorder:

    def __init__(self, events):
        self.events = events

    def __call__(self, code, offset, func, arg):
        self.events.append((self.event_name, func.__name__, arg))

class CReturnRecorder(CEventRecorder):

    event_type = E.C_RETURN
    event_name = "C return"

class CRaiseRecorder(CEventRecorder):

    event_type = E.C_RAISE
    event_name = "C raise"

MANY_RECORDERS = ExceptionRecorder, CallRecorder, LineRecorder, CReturnRecorder, CRaiseRecorder

class TestManyEvents(CheckEvents):

    def test_simple(self):

        def func1():
            line1 = 1
            line2 = 2
            line3 = 3

        self.check_events(func1, recorders = MANY_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('call', 'func1', sys.monitoring.MISSING),
            ('line', 'func1', 1),
            ('line', 'func1', 2),
            ('line', 'func1', 3),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2)])

    def test_c_call(self):

        def func2():
            line1 = 1
            [].append(2)
            line3 = 3

        self.check_events(func2, recorders = MANY_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('call', 'func2', sys.monitoring.MISSING),
            ('line', 'func2', 1),
            ('line', 'func2', 2),
            ('call', 'append', [2]),
            ('C return', 'append', [2]),
            ('line', 'func2', 3),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2)])

    def test_try_except(self):

        def func3():
            try:
                line = 2
                raise KeyError
            except:
                line = 5
            line = 6

        self.check_events(func3, recorders = MANY_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('call', 'func3', sys.monitoring.MISSING),
            ('line', 'func3', 1),
            ('line', 'func3', 2),
            ('line', 'func3', 3),
            ('raise', KeyError),
            ('line', 'func3', 4),
            ('line', 'func3', 5),
            ('line', 'func3', 6),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2)])

class InstructionRecorder:

    event_type = E.INSTRUCTION

    def __init__(self, events):
        self.events = events

    def __call__(self, code, offset):
        # Filter out instructions in check_events to lower noise
        if code.co_name != "get_events":
            self.events.append(("instruction", code.co_name, offset))


LINE_AND_INSTRUCTION_RECORDERS = InstructionRecorder, LineRecorder

class TestLineAndInstructionEvents(CheckEvents):
    maxDiff = None

    def test_simple(self):

        def func1():
            line1 = 1
            line2 = 2
            line3 = 3

        self.check_events(func1, recorders = LINE_AND_INSTRUCTION_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func1', 1),
            ('instruction', 'func1', 2),
            ('instruction', 'func1', 4),
            ('line', 'func1', 2),
            ('instruction', 'func1', 6),
            ('instruction', 'func1', 8),
            ('line', 'func1', 3),
            ('instruction', 'func1', 10),
            ('instruction', 'func1', 12),
            ('instruction', 'func1', 14),
            ('line', 'get_events', 11)])

    def test_c_call(self):

        def func2():
            line1 = 1
            [].append(2)
            line3 = 3

        self.check_events(func2, recorders = LINE_AND_INSTRUCTION_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func2', 1),
            ('instruction', 'func2', 2),
            ('instruction', 'func2', 4),
            ('line', 'func2', 2),
            ('instruction', 'func2', 6),
            ('instruction', 'func2', 8),
            ('instruction', 'func2', 28),
            ('instruction', 'func2', 30),
            ('instruction', 'func2', 38),
            ('line', 'func2', 3),
            ('instruction', 'func2', 40),
            ('instruction', 'func2', 42),
            ('instruction', 'func2', 44),
            ('line', 'get_events', 11)])

    def test_try_except(self):

        def func3():
            try:
                line = 2
                raise KeyError
            except:
                line = 5
            line = 6

        self.check_events(func3, recorders = LINE_AND_INSTRUCTION_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func3', 1),
            ('instruction', 'func3', 2),
            ('line', 'func3', 2),
            ('instruction', 'func3', 4),
            ('instruction', 'func3', 6),
            ('line', 'func3', 3),
            ('instruction', 'func3', 8),
            ('instruction', 'func3', 18),
            ('instruction', 'func3', 20),
            ('line', 'func3', 4),
            ('instruction', 'func3', 22),
            ('line', 'func3', 5),
            ('instruction', 'func3', 24),
            ('instruction', 'func3', 26),
            ('instruction', 'func3', 28),
            ('line', 'func3', 6),
            ('instruction', 'func3', 30),
            ('instruction', 'func3', 32),
            ('instruction', 'func3', 34),
            ('line', 'get_events', 11)])

    def test_with_restart(self):
        def func1():
            line1 = 1
            line2 = 2
            line3 = 3

        self.check_events(func1, recorders = LINE_AND_INSTRUCTION_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func1', 1),
            ('instruction', 'func1', 2),
            ('instruction', 'func1', 4),
            ('line', 'func1', 2),
            ('instruction', 'func1', 6),
            ('instruction', 'func1', 8),
            ('line', 'func1', 3),
            ('instruction', 'func1', 10),
            ('instruction', 'func1', 12),
            ('instruction', 'func1', 14),
            ('line', 'get_events', 11)])

        sys.monitoring.restart_events()

        self.check_events(func1, recorders = LINE_AND_INSTRUCTION_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func1', 1),
            ('instruction', 'func1', 2),
            ('instruction', 'func1', 4),
            ('line', 'func1', 2),
            ('instruction', 'func1', 6),
            ('instruction', 'func1', 8),
            ('line', 'func1', 3),
            ('instruction', 'func1', 10),
            ('instruction', 'func1', 12),
            ('instruction', 'func1', 14),
            ('line', 'get_events', 11)])

    def test_turn_off_only_instruction(self):
        """
        LINE events should be recorded after INSTRUCTION event is turned off
        """
        events = []
        def line(*args):
            events.append("line")
        sys.monitoring.set_events(TEST_TOOL, 0)
        sys.monitoring.register_callback(TEST_TOOL, E.LINE, line)
        sys.monitoring.register_callback(TEST_TOOL, E.INSTRUCTION, lambda *args: None)
        sys.monitoring.set_events(TEST_TOOL, E.LINE | E.INSTRUCTION)
        sys.monitoring.set_events(TEST_TOOL, E.LINE)
        events = []
        a = 0
        sys.monitoring.set_events(TEST_TOOL, 0)
        self.assertGreater(len(events), 0)

class TestInstallIncrementally(MonitoringTestBase, unittest.TestCase):

    def check_events(self, func, must_include, tool=TEST_TOOL, recorders=(ExceptionRecorder,)):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            event_list = []
            all_events = 0
            for recorder in recorders:
                all_events |= recorder.event_type
                sys.monitoring.set_events(tool, all_events)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, recorder(event_list))
            func()
            sys.monitoring.set_events(tool, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)
            for line in must_include:
                self.assertIn(line, event_list)
        finally:
            sys.monitoring.set_events(tool, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)

    @staticmethod
    def func1():
        line1 = 1

    MUST_INCLUDE_LI = [
            ('instruction', 'func1', 2),
            ('line', 'func1', 2),
            ('instruction', 'func1', 4),
            ('instruction', 'func1', 6)]

    def test_line_then_instruction(self):
        recorders = [ LineRecorder, InstructionRecorder ]
        self.check_events(self.func1,
                          recorders = recorders, must_include = self.MUST_INCLUDE_LI)

    def test_instruction_then_line(self):
        recorders = [ InstructionRecorder, LineRecorder ]
        self.check_events(self.func1,
                          recorders = recorders, must_include = self.MUST_INCLUDE_LI)

    @staticmethod
    def func2():
        len(())

    MUST_INCLUDE_CI = [
            ('instruction', 'func2', 2),
            ('call', 'func2', sys.monitoring.MISSING),
            ('call', 'len', ()),
            ('instruction', 'func2', 12),
            ('instruction', 'func2', 14)]



    def test_call_then_instruction(self):
        recorders = [ CallRecorder, InstructionRecorder ]
        self.check_events(self.func2,
                          recorders = recorders, must_include = self.MUST_INCLUDE_CI)

    def test_instruction_then_call(self):
        recorders = [ InstructionRecorder, CallRecorder ]
        self.check_events(self.func2,
                          recorders = recorders, must_include = self.MUST_INCLUDE_CI)

LOCAL_RECORDERS = CallRecorder, LineRecorder, CReturnRecorder, CRaiseRecorder

class TestLocalEvents(MonitoringTestBase, unittest.TestCase):

    def check_events(self, func, expected, tool=TEST_TOOL, recorders=()):
        try:
            self.assertEqual(sys.monitoring._all_events(), {})
            event_list = []
            all_events = 0
            for recorder in recorders:
                ev = recorder.event_type
                sys.monitoring.register_callback(tool, ev, recorder(event_list))
                all_events |= ev
            sys.monitoring.set_local_events(tool, func.__code__, all_events)
            func()
            sys.monitoring.set_local_events(tool, func.__code__, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)
            self.assertEqual(event_list, expected)
        finally:
            sys.monitoring.set_local_events(tool, func.__code__, 0)
            for recorder in recorders:
                sys.monitoring.register_callback(tool, recorder.event_type, None)


    def test_simple(self):

        def func1():
            line1 = 1
            line2 = 2
            line3 = 3

        self.check_events(func1, recorders = LOCAL_RECORDERS, expected = [
            ('line', 'func1', 1),
            ('line', 'func1', 2),
            ('line', 'func1', 3)])

    def test_c_call(self):

        def func2():
            line1 = 1
            [].append(2)
            line3 = 3

        self.check_events(func2, recorders = LOCAL_RECORDERS, expected = [
            ('line', 'func2', 1),
            ('line', 'func2', 2),
            ('call', 'append', [2]),
            ('C return', 'append', [2]),
            ('line', 'func2', 3)])

    def test_try_except(self):

        def func3():
            try:
                line = 2
                raise KeyError
            except:
                line = 5
            line = 6

        self.check_events(func3, recorders = LOCAL_RECORDERS, expected = [
            ('line', 'func3', 1),
            ('line', 'func3', 2),
            ('line', 'func3', 3),
            ('line', 'func3', 4),
            ('line', 'func3', 5),
            ('line', 'func3', 6)])

    def test_set_non_local_event(self):
        with self.assertRaises(ValueError):
            sys.monitoring.set_local_events(TEST_TOOL, just_call.__code__, E.RAISE)

def line_from_offset(code, offset):
    for start, end, line in code.co_lines():
        if start <= offset < end:
            if line is None:
                return f"[offset={offset}]"
            return line - code.co_firstlineno
    return -1

class JumpRecorder:

    event_type = E.JUMP
    name = "jump"

    def __init__(self, events):
        self.events = events

    def __call__(self, code, from_, to):
        from_line = line_from_offset(code, from_)
        to_line = line_from_offset(code, to)
        self.events.append((self.name, code.co_name, from_line, to_line))


class BranchRecorder(JumpRecorder):

    event_type = E.BRANCH
    name = "branch"

class ReturnRecorder:

    event_type = E.PY_RETURN

    def __init__(self, events):
        self.events = events

    def __call__(self, code, offset, val):
        self.events.append(("return", val))


JUMP_AND_BRANCH_RECORDERS = JumpRecorder, BranchRecorder
JUMP_BRANCH_AND_LINE_RECORDERS = JumpRecorder, BranchRecorder, LineRecorder
FLOW_AND_LINE_RECORDERS = JumpRecorder, BranchRecorder, LineRecorder, ExceptionRecorder, ReturnRecorder

class TestBranchAndJumpEvents(CheckEvents):
    maxDiff = None

    def test_loop(self):

        def func():
            x = 1
            for a in range(2):
                if a:
                    x = 4
                else:
                    x = 6
            7

        self.check_events(func, recorders = JUMP_AND_BRANCH_RECORDERS, expected = [
            ('branch', 'func', 2, 2),
            ('branch', 'func', 3, 6),
            ('jump', 'func', 6, 2),
            ('branch', 'func', 2, 2),
            ('branch', 'func', 3, 4),
            ('jump', 'func', 4, 2),
            ('branch', 'func', 2, 7)])

        self.check_events(func, recorders = JUMP_BRANCH_AND_LINE_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func', 1),
            ('line', 'func', 2),
            ('branch', 'func', 2, 2),
            ('line', 'func', 3),
            ('branch', 'func', 3, 6),
            ('line', 'func', 6),
            ('jump', 'func', 6, 2),
            ('line', 'func', 2),
            ('branch', 'func', 2, 2),
            ('line', 'func', 3),
            ('branch', 'func', 3, 4),
            ('line', 'func', 4),
            ('jump', 'func', 4, 2),
            ('line', 'func', 2),
            ('branch', 'func', 2, 7),
            ('line', 'func', 7),
            ('line', 'get_events', 11)])

    def test_except_star(self):

        class Foo:
            def meth(self):
                pass

        def func():
            try:
                try:
                    raise KeyError
                except* Exception as e:
                    f = Foo(); f.meth()
            except KeyError:
                pass


        self.check_events(func, recorders = JUMP_BRANCH_AND_LINE_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func', 1),
            ('line', 'func', 2),
            ('line', 'func', 3),
            ('line', 'func', 4),
            ('branch', 'func', 4, 4),
            ('line', 'func', 5),
            ('line', 'meth', 1),
            ('jump', 'func', 5, 5),
            ('jump', 'func', 5, '[offset=114]'),
            ('branch', 'func', '[offset=120]', '[offset=124]'),
            ('line', 'get_events', 11)])

        self.check_events(func, recorders = FLOW_AND_LINE_RECORDERS, expected = [
            ('line', 'get_events', 10),
            ('line', 'func', 1),
            ('line', 'func', 2),
            ('line', 'func', 3),
            ('raise', KeyError),
            ('line', 'func', 4),
            ('branch', 'func', 4, 4),
            ('line', 'func', 5),
            ('line', 'meth', 1),
            ('return', None),
            ('jump', 'func', 5, 5),
            ('jump', 'func', 5, '[offset=114]'),
            ('branch', 'func', '[offset=120]', '[offset=124]'),
            ('return', None),
            ('line', 'get_events', 11)])

class TestLoadSuperAttr(CheckEvents):
    RECORDERS = CallRecorder, LineRecorder, CRaiseRecorder, CReturnRecorder

    def _exec(self, co):
        d = {}
        exec(co, d, d)
        return d

    def _exec_super(self, codestr, optimized=False):
        # The compiler checks for statically visible shadowing of the name
        # `super`, and declines to emit `LOAD_SUPER_ATTR` if shadowing is found.
        # So inserting `super = super` prevents the compiler from emitting
        # `LOAD_SUPER_ATTR`, and allows us to test that monitoring events for
        # `LOAD_SUPER_ATTR` are equivalent to those we'd get from the
        # un-optimized `LOAD_GLOBAL super; CALL; LOAD_ATTR` form.
        assignment = "x = 1" if optimized else "super = super"
        codestr = f"{assignment}\n{textwrap.dedent(codestr)}"
        co = compile(codestr, "<string>", "exec")
        # validate that we really do have a LOAD_SUPER_ATTR, only when optimized
        self.assertEqual(self._has_load_super_attr(co), optimized)
        return self._exec(co)

    def _has_load_super_attr(self, co):
        has = any(instr.opname == "LOAD_SUPER_ATTR" for instr in dis.get_instructions(co))
        if not has:
            has = any(
                isinstance(c, types.CodeType) and self._has_load_super_attr(c)
                for c in co.co_consts
            )
        return has

    def _super_method_call(self, optimized=False):
        codestr = """
            class A:
                def method(self, x):
                    return x

            class B(A):
                def method(self, x):
                    return super(
                    ).method(
                        x
                    )

            b = B()
            def f():
                return b.method(1)
        """
        d = self._exec_super(codestr, optimized)
        expected = [
            ('line', 'get_events', 10),
            ('call', 'f', sys.monitoring.MISSING),
            ('line', 'f', 1),
            ('call', 'method', d["b"]),
            ('line', 'method', 1),
            ('call', 'super', sys.monitoring.MISSING),
            ('C return', 'super', sys.monitoring.MISSING),
            ('line', 'method', 2),
            ('line', 'method', 3),
            ('line', 'method', 2),
            ('call', 'method', 1),
            ('line', 'method', 1),
            ('line', 'method', 1),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2),
        ]
        return d["f"], expected

    def test_method_call(self):
        nonopt_func, nonopt_expected = self._super_method_call(optimized=False)
        opt_func, opt_expected = self._super_method_call(optimized=True)

        self.check_events(nonopt_func, recorders=self.RECORDERS, expected=nonopt_expected)
        self.check_events(opt_func, recorders=self.RECORDERS, expected=opt_expected)

    def _super_method_call_error(self, optimized=False):
        codestr = """
            class A:
                def method(self, x):
                    return x

            class B(A):
                def method(self, x):
                    return super(
                        x,
                        self,
                    ).method(
                        x
                    )

            b = B()
            def f():
                try:
                    return b.method(1)
                except TypeError:
                    pass
                else:
                    assert False, "should have raised TypeError"
        """
        d = self._exec_super(codestr, optimized)
        expected = [
            ('line', 'get_events', 10),
            ('call', 'f', sys.monitoring.MISSING),
            ('line', 'f', 1),
            ('line', 'f', 2),
            ('call', 'method', d["b"]),
            ('line', 'method', 1),
            ('line', 'method', 2),
            ('line', 'method', 3),
            ('line', 'method', 1),
            ('call', 'super', 1),
            ('C raise', 'super', 1),
            ('line', 'f', 3),
            ('line', 'f', 4),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2),
        ]
        return d["f"], expected

    def test_method_call_error(self):
        nonopt_func, nonopt_expected = self._super_method_call_error(optimized=False)
        opt_func, opt_expected = self._super_method_call_error(optimized=True)

        self.check_events(nonopt_func, recorders=self.RECORDERS, expected=nonopt_expected)
        self.check_events(opt_func, recorders=self.RECORDERS, expected=opt_expected)

    def _super_attr(self, optimized=False):
        codestr = """
            class A:
                x = 1

            class B(A):
                def method(self):
                    return super(
                    ).x

            b = B()
            def f():
                return b.method()
        """
        d = self._exec_super(codestr, optimized)
        expected = [
            ('line', 'get_events', 10),
            ('call', 'f', sys.monitoring.MISSING),
            ('line', 'f', 1),
            ('call', 'method', d["b"]),
            ('line', 'method', 1),
            ('call', 'super', sys.monitoring.MISSING),
            ('C return', 'super', sys.monitoring.MISSING),
            ('line', 'method', 2),
            ('line', 'method', 1),
            ('line', 'get_events', 11),
            ('call', 'set_events', 2)
        ]
        return d["f"], expected

    def test_attr(self):
        nonopt_func, nonopt_expected = self._super_attr(optimized=False)
        opt_func, opt_expected = self._super_attr(optimized=True)

        self.check_events(nonopt_func, recorders=self.RECORDERS, expected=nonopt_expected)
        self.check_events(opt_func, recorders=self.RECORDERS, expected=opt_expected)

    def test_vs_other_type_call(self):
        code_template = textwrap.dedent("""
            class C:
                def method(self):
                    return {cls}().__repr__{call}
            c = C()
            def f():
                return c.method()
        """)

        def get_expected(name, call_method, ns):
            repr_arg = 0 if name == "int" else sys.monitoring.MISSING
            return [
                ('line', 'get_events', 10),
                ('call', 'f', sys.monitoring.MISSING),
                ('line', 'f', 1),
                ('call', 'method', ns["c"]),
                ('line', 'method', 1),
                ('call', name, sys.monitoring.MISSING),
                ('C return', name, sys.monitoring.MISSING),
                *(
                    [
                        ('call', '__repr__', repr_arg),
                        ('C return', '__repr__', repr_arg),
                    ] if call_method else []
                ),
                ('line', 'get_events', 11),
                ('call', 'set_events', 2),
            ]

        for call_method in [True, False]:
            with self.subTest(call_method=call_method):
                call_str = "()" if call_method else ""
                code_super = code_template.format(cls="super", call=call_str)
                code_int = code_template.format(cls="int", call=call_str)
                co_super = compile(code_super, '<string>', 'exec')
                self.assertTrue(self._has_load_super_attr(co_super))
                ns_super = self._exec(co_super)
                ns_int = self._exec(code_int)

                self.check_events(
                    ns_super["f"],
                    recorders=self.RECORDERS,
                    expected=get_expected("super", call_method, ns_super)
                )
                self.check_events(
                    ns_int["f"],
                    recorders=self.RECORDERS,
                    expected=get_expected("int", call_method, ns_int)
                )


class TestSetGetEvents(MonitoringTestBase, unittest.TestCase):

    def test_global(self):
        sys.monitoring.set_events(TEST_TOOL, E.PY_START)
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL), E.PY_START)
        sys.monitoring.set_events(TEST_TOOL2, E.PY_START)
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), E.PY_START)
        sys.monitoring.set_events(TEST_TOOL, 0)
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL), 0)
        sys.monitoring.set_events(TEST_TOOL2,0)
        self.assertEqual(sys.monitoring.get_events(TEST_TOOL2), 0)

    def test_local(self):
        code = f1.__code__
        sys.monitoring.set_local_events(TEST_TOOL, code, E.PY_START)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL, code), E.PY_START)
        sys.monitoring.set_local_events(TEST_TOOL2, code, E.PY_START)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL2, code), E.PY_START)
        sys.monitoring.set_local_events(TEST_TOOL, code, 0)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL, code), 0)
        sys.monitoring.set_local_events(TEST_TOOL2, code, 0)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL2, code), 0)

class TestUninitialized(unittest.TestCase, MonitoringTestBase):

    @staticmethod
    def f():
        pass

    def test_get_local_events_uninitialized(self):
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL, self.f.__code__), 0)

class TestRegressions(MonitoringTestBase, unittest.TestCase):

    def test_105162(self):
        caught = None

        def inner():
            nonlocal caught
            try:
                yield
            except Exception:
                caught = "inner"
                yield

        def outer():
            nonlocal caught
            try:
                yield from inner()
            except Exception:
                caught = "outer"
                yield

        def run():
            gen = outer()
            gen.send(None)
            gen.throw(Exception)
        run()
        self.assertEqual(caught, "inner")
        caught = None
        try:
            sys.monitoring.set_events(TEST_TOOL, E.PY_RESUME)
            run()
            self.assertEqual(caught, "inner")
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)

    def test_108390(self):

        class Foo:
            def __init__(self, set_event):
                if set_event:
                    sys.monitoring.set_events(TEST_TOOL, E.PY_RESUME)

        def make_foo_optimized_then_set_event():
            for i in range(100):
                Foo(i == 99)

        try:
            make_foo_optimized_then_set_event()
        finally:
            sys.monitoring.set_events(TEST_TOOL, 0)

    def test_gh108976(self):
        sys.monitoring.use_tool_id(0, "test")
        self.addCleanup(sys.monitoring.free_tool_id, 0)
        sys.monitoring.set_events(0, 0)
        sys.monitoring.register_callback(0, E.LINE, lambda *args: sys.monitoring.set_events(0, 0))
        sys.monitoring.register_callback(0, E.INSTRUCTION, lambda *args: 0)
        sys.monitoring.set_events(0, E.LINE | E.INSTRUCTION)
        sys.monitoring.set_events(0, 0)


class TestOptimizer(MonitoringTestBase, unittest.TestCase):

    def setUp(self):
        import _testinternalcapi
        self.old_opt = _testinternalcapi.get_optimizer()
        opt = _testinternalcapi.get_counter_optimizer()
        _testinternalcapi.set_optimizer(opt)
        super(TestOptimizer, self).setUp()

    def tearDown(self):
        import _testinternalcapi
        super(TestOptimizer, self).tearDown()
        _testinternalcapi.set_optimizer(self.old_opt)

    def test_for_loop(self):
        def test_func(x):
            i = 0
            while i < x:
                i += 1

        code = test_func.__code__
        sys.monitoring.set_local_events(TEST_TOOL, code, E.PY_START)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL, code), E.PY_START)
        test_func(1000)
        sys.monitoring.set_local_events(TEST_TOOL, code, 0)
        self.assertEqual(sys.monitoring.get_local_events(TEST_TOOL, code), 0)
