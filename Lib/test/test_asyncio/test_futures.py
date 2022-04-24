"""Tests for futures.py."""

import concurrent.futures
import gc
import re
import sys
import threading
import unittest
from unittest import mock
from types import GenericAlias
import asyncio
from asyncio import futures
from test.test_asyncio import utils as test_utils
from test import support


def tearDownModule():
    asyncio.set_event_loop_policy(None)


def _fakefunc(f):
    return f


def first_cb():
    pass


def last_cb():
    pass


class DuckFuture:
    # Class that does not inherit from Future but aims to be duck-type
    # compatible with it.

    _asyncio_future_blocking = False
    __cancelled = False
    __result = None
    __exception = None

    def cancel(self):
        if self.done():
            return False
        self.__cancelled = True
        return True

    def cancelled(self):
        return self.__cancelled

    def done(self):
        return (self.__cancelled
                or self.__result is not None
                or self.__exception is not None)

    def result(self):
        self.assertFalse(self.cancelled())
        if self.__exception is not None:
            raise self.__exception
        return self.__result

    def exception(self):
        self.assertFalse(self.cancelled())
        return self.__exception

    def set_result(self, result):
        self.assertFalse(self.done())
        self.assertIsNotNone(result)
        self.__result = result

    def set_exception(self, exception):
        self.assertFalse(self.done())
        self.assertIsNotNone(exception)
        self.__exception = exception

    def __iter__(self):
        if not self.done():
            self._asyncio_future_blocking = True
            yield self
        self.assertTrue(self.done())
        return self.result()


class DuckTests(test_utils.TestCase):

    def setUp(self):
        super().setUp()
        self.loop = self.new_test_loop()
        self.addCleanup(self.loop.close)

    def test_wrap_future(self):
        f = DuckFuture()
        g = asyncio.wrap_future(f)
        self.assertIs(g, f)

    def test_ensure_future(self):
        f = DuckFuture()
        g = asyncio.ensure_future(f)
        self.assertIs(g, f)


class BaseFutureTests:

    def _new_future(self,  *args, **kwargs):
        return self.cls(*args, **kwargs)

    def setUp(self):
        super().setUp()
        self.loop = self.new_test_loop()
        self.addCleanup(self.loop.close)

    def test_generic_alias(self):
        future = self.cls[str]
        self.assertEqual(future.__args__, (str,))
        self.assertIsInstance(future, GenericAlias)

    def test_isfuture(self):
        class MyFuture:
            _asyncio_future_blocking = None

            def __init__(self):
                self._asyncio_future_blocking = False

        self.assertFalse(asyncio.isfuture(MyFuture))
        self.assertTrue(asyncio.isfuture(MyFuture()))
        self.assertFalse(asyncio.isfuture(1))

        # As `isinstance(Mock(), Future)` returns `False`
        self.assertFalse(asyncio.isfuture(mock.Mock()))

        f = self._new_future(loop=self.loop)
        self.assertTrue(asyncio.isfuture(f))
        self.assertFalse(asyncio.isfuture(type(f)))

        # As `isinstance(Mock(Future), Future)` returns `True`
        self.assertTrue(asyncio.isfuture(mock.Mock(type(f))))

        f.cancel()

    def test_initial_state(self):
        f = self._new_future(loop=self.loop)
        self.assertFalse(f.cancelled())
        self.assertFalse(f.done())
        f.cancel()
        self.assertTrue(f.cancelled())

    def test_constructor_without_loop(self):
        with self.assertWarns(DeprecationWarning) as cm:
            with self.assertRaisesRegex(RuntimeError, 'There is no current event loop'):
                self._new_future()
        self.assertEqual(cm.filename, __file__)

    def test_constructor_use_running_loop(self):
        async def test():
            return self._new_future()
        f = self.loop.run_until_complete(test())
        self.assertIs(f._loop, self.loop)
        self.assertIs(f.get_loop(), self.loop)

    def test_constructor_use_global_loop(self):
        # Deprecated in 3.10
        asyncio.set_event_loop(self.loop)
        self.addCleanup(asyncio.set_event_loop, None)
        with self.assertWarns(DeprecationWarning) as cm:
            f = self._new_future()
        self.assertEqual(cm.filename, __file__)
        self.assertIs(f._loop, self.loop)
        self.assertIs(f.get_loop(), self.loop)

    def test_constructor_positional(self):
        # Make sure Future doesn't accept a positional argument
        self.assertRaises(TypeError, self._new_future, 42)

    def test_uninitialized(self):
        # Test that C Future doesn't crash when Future.__init__()
        # call was skipped.

        fut = self.cls.__new__(self.cls, loop=self.loop)
        self.assertRaises(asyncio.InvalidStateError, fut.result)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        self.assertRaises(asyncio.InvalidStateError, fut.exception)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        with self.assertRaises((RuntimeError, AttributeError)):
            fut.set_result(None)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        with self.assertRaises((RuntimeError, AttributeError)):
            fut.set_exception(Exception)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        with self.assertRaises((RuntimeError, AttributeError)):
            fut.cancel()

        fut = self.cls.__new__(self.cls, loop=self.loop)
        with self.assertRaises((RuntimeError, AttributeError)):
            fut.add_done_callback(lambda f: None)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        with self.assertRaises((RuntimeError, AttributeError)):
            fut.remove_done_callback(lambda f: None)

        fut = self.cls.__new__(self.cls, loop=self.loop)
        try:
            repr(fut)
        except (RuntimeError, AttributeError):
            pass

        fut = self.cls.__new__(self.cls, loop=self.loop)
        try:
            fut.__await__()
        except RuntimeError:
            pass

        fut = self.cls.__new__(self.cls, loop=self.loop)
        try:
            iter(fut)
        except RuntimeError:
            pass

        fut = self.cls.__new__(self.cls, loop=self.loop)
        self.assertFalse(fut.cancelled())
        self.assertFalse(fut.done())

    def test_future_cancel_message_getter(self):
        f = self._new_future(loop=self.loop)
        self.assertTrue(hasattr(f, '_cancel_message'))
        self.assertEqual(f._cancel_message, None)

        with self.assertWarnsRegex(
            DeprecationWarning,
            "Passing 'msg' argument"
        ):
            f.cancel('my message')
        with self.assertRaises(asyncio.CancelledError):
            self.loop.run_until_complete(f)
        self.assertEqual(f._cancel_message, 'my message')

    def test_future_cancel_message_setter(self):
        f = self._new_future(loop=self.loop)
        with self.assertWarnsRegex(
            DeprecationWarning,
            "Passing 'msg' argument"
        ):
            f.cancel('my message')
        f._cancel_message = 'my new message'
        self.assertEqual(f._cancel_message, 'my new message')

        # Also check that the value is used for cancel().
        with self.assertRaises(asyncio.CancelledError):
            self.loop.run_until_complete(f)
        self.assertEqual(f._cancel_message, 'my new message')

    def test_cancel(self):
        f = self._new_future(loop=self.loop)
        self.assertTrue(f.cancel())
        self.assertTrue(f.cancelled())
        self.assertTrue(f.done())
        self.assertRaises(asyncio.CancelledError, f.result)
        self.assertRaises(asyncio.CancelledError, f.exception)
        self.assertRaises(asyncio.InvalidStateError, f.set_result, None)
        self.assertRaises(asyncio.InvalidStateError, f.set_exception, None)
        self.assertFalse(f.cancel())

    def test_result(self):
        f = self._new_future(loop=self.loop)
        self.assertRaises(asyncio.InvalidStateError, f.result)

        f.set_result(42)
        self.assertFalse(f.cancelled())
        self.assertTrue(f.done())
        self.assertEqual(f.result(), 42)
        self.assertEqual(f.exception(), None)
        self.assertRaises(asyncio.InvalidStateError, f.set_result, None)
        self.assertRaises(asyncio.InvalidStateError, f.set_exception, None)
        self.assertFalse(f.cancel())

    def test_exception(self):
        exc = RuntimeError()
        f = self._new_future(loop=self.loop)
        self.assertRaises(asyncio.InvalidStateError, f.exception)

        # StopIteration cannot be raised into a Future - CPython issue26221
        self.assertRaisesRegex(TypeError, "StopIteration .* cannot be raised",
                               f.set_exception, StopIteration)

        f.set_exception(exc)
        self.assertFalse(f.cancelled())
        self.assertTrue(f.done())
        self.assertRaises(RuntimeError, f.result)
        self.assertEqual(f.exception(), exc)
        self.assertRaises(asyncio.InvalidStateError, f.set_result, None)
        self.assertRaises(asyncio.InvalidStateError, f.set_exception, None)
        self.assertFalse(f.cancel())

    def test_exception_class(self):
        f = self._new_future(loop=self.loop)
        f.set_exception(RuntimeError)
        self.assertIsInstance(f.exception(), RuntimeError)

    def test_yield_from_twice(self):
        f = self._new_future(loop=self.loop)

        def fixture():
            yield 'A'
            x = yield from f
            yield 'B', x
            y = yield from f
            yield 'C', y

        g = fixture()
        self.assertEqual(next(g), 'A')  # yield 'A'.
        self.assertEqual(next(g), f)  # First yield from f.
        f.set_result(42)
        self.assertEqual(next(g), ('B', 42))  # yield 'B', x.
        # The second "yield from f" does not yield f.
        self.assertEqual(next(g), ('C', 42))  # yield 'C', y.

    def test_future_repr(self):
        self.loop.set_debug(True)
        f_pending_debug = self._new_future(loop=self.loop)
        frame = f_pending_debug._source_traceback[-1]
        self.assertEqual(
            repr(f_pending_debug),
            f'<{self.cls.__name__} pending created at {frame[0]}:{frame[1]}>')
        f_pending_debug.cancel()

        self.loop.set_debug(False)
        f_pending = self._new_future(loop=self.loop)
        self.assertEqual(repr(f_pending), f'<{self.cls.__name__} pending>')
        f_pending.cancel()

        f_cancelled = self._new_future(loop=self.loop)
        f_cancelled.cancel()
        self.assertEqual(repr(f_cancelled), f'<{self.cls.__name__} cancelled>')

        f_result = self._new_future(loop=self.loop)
        f_result.set_result(4)
        self.assertEqual(
            repr(f_result), f'<{self.cls.__name__} finished result=4>')
        self.assertEqual(f_result.result(), 4)

        exc = RuntimeError()
        f_exception = self._new_future(loop=self.loop)
        f_exception.set_exception(exc)
        self.assertEqual(
            repr(f_exception),
            f'<{self.cls.__name__} finished exception=RuntimeError()>')
        self.assertIs(f_exception.exception(), exc)

        def func_repr(func):
            filename, lineno = test_utils.get_function_source(func)
            text = '%s() at %s:%s' % (func.__qualname__, filename, lineno)
            return re.escape(text)

        f_one_callbacks = self._new_future(loop=self.loop)
        f_one_callbacks.add_done_callback(_fakefunc)
        fake_repr = func_repr(_fakefunc)
        self.assertRegex(
            repr(f_one_callbacks),
            r'<' + self.cls.__name__ + r' pending cb=\[%s\]>' % fake_repr)
        f_one_callbacks.cancel()
        self.assertEqual(repr(f_one_callbacks),
                         f'<{self.cls.__name__} cancelled>')

        f_two_callbacks = self._new_future(loop=self.loop)
        f_two_callbacks.add_done_callback(first_cb)
        f_two_callbacks.add_done_callback(last_cb)
        first_repr = func_repr(first_cb)
        last_repr = func_repr(last_cb)
        self.assertRegex(repr(f_two_callbacks),
                         r'<' + self.cls.__name__ + r' pending cb=\[%s, %s\]>'
                         % (first_repr, last_repr))

        f_many_callbacks = self._new_future(loop=self.loop)
        f_many_callbacks.add_done_callback(first_cb)
        for i in range(8):
            f_many_callbacks.add_done_callback(_fakefunc)
        f_many_callbacks.add_done_callback(last_cb)
        cb_regex = r'%s, <8 more>, %s' % (first_repr, last_repr)
        self.assertRegex(
            repr(f_many_callbacks),
            r'<' + self.cls.__name__ + r' pending cb=\[%s\]>' % cb_regex)
        f_many_callbacks.cancel()
        self.assertEqual(repr(f_many_callbacks),
                         f'<{self.cls.__name__} cancelled>')

    def test_copy_state(self):
        from asyncio.futures import _copy_future_state

        f = self._new_future(loop=self.loop)
        f.set_result(10)

        newf = self._new_future(loop=self.loop)
        _copy_future_state(f, newf)
        self.assertTrue(newf.done())
        self.assertEqual(newf.result(), 10)

        f_exception = self._new_future(loop=self.loop)
        f_exception.set_exception(RuntimeError())

        newf_exception = self._new_future(loop=self.loop)
        _copy_future_state(f_exception, newf_exception)
        self.assertTrue(newf_exception.done())
        self.assertRaises(RuntimeError, newf_exception.result)

        f_cancelled = self._new_future(loop=self.loop)
        f_cancelled.cancel()

        newf_cancelled = self._new_future(loop=self.loop)
        _copy_future_state(f_cancelled, newf_cancelled)
        self.assertTrue(newf_cancelled.cancelled())

    def test_iter(self):
        fut = self._new_future(loop=self.loop)

        def coro():
            yield from fut

        def test():
            arg1, arg2 = coro()

        with self.assertRaisesRegex(RuntimeError, "await wasn't used"):
            test()
        fut.cancel()

    def test_log_traceback(self):
        fut = self._new_future(loop=self.loop)
        with self.assertRaisesRegex(ValueError, 'can only be set to False'):
            fut._log_traceback = True

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_abandoned(self, m_log):
        fut = self._new_future(loop=self.loop)
        del fut
        self.assertFalse(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_not_called_after_cancel(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_exception(Exception())
        fut.cancel()
        del fut
        self.assertFalse(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_result_unretrieved(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_result(42)
        del fut
        self.assertFalse(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_result_retrieved(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_result(42)
        fut.result()
        del fut
        self.assertFalse(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_exception_unretrieved(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_exception(RuntimeError('boom'))
        del fut
        test_utils.run_briefly(self.loop)
        support.gc_collect()
        self.assertTrue(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_exception_retrieved(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_exception(RuntimeError('boom'))
        fut.exception()
        del fut
        self.assertFalse(m_log.error.called)

    @mock.patch('asyncio.base_events.logger')
    def test_tb_logger_exception_result_retrieved(self, m_log):
        fut = self._new_future(loop=self.loop)
        fut.set_exception(RuntimeError('boom'))
        self.assertRaises(RuntimeError, fut.result)
        del fut
        self.assertFalse(m_log.error.called)

    def test_wrap_future(self):

        def run(arg):
            return (arg, threading.get_ident())
        ex = concurrent.futures.ThreadPoolExecutor(1)
        f1 = ex.submit(run, 'oi')
        f2 = asyncio.wrap_future(f1, loop=self.loop)
        res, ident = self.loop.run_until_complete(f2)
        self.assertTrue(asyncio.isfuture(f2))
        self.assertEqual(res, 'oi')
        self.assertNotEqual(ident, threading.get_ident())
        ex.shutdown(wait=True)

    def test_wrap_future_future(self):
        f1 = self._new_future(loop=self.loop)
        f2 = asyncio.wrap_future(f1)
        self.assertIs(f1, f2)

    def test_wrap_future_without_loop(self):
        def run(arg):
            return (arg, threading.get_ident())
        ex = concurrent.futures.ThreadPoolExecutor(1)
        f1 = ex.submit(run, 'oi')
        with self.assertWarns(DeprecationWarning) as cm:
            with self.assertRaises(RuntimeError):
                asyncio.wrap_future(f1)
        self.assertEqual(cm.filename, __file__)
        ex.shutdown(wait=True)

    def test_wrap_future_use_running_loop(self):
        def run(arg):
            return (arg, threading.get_ident())
        ex = concurrent.futures.ThreadPoolExecutor(1)
        f1 = ex.submit(run, 'oi')
        async def test():
            return asyncio.wrap_future(f1)
        f2 = self.loop.run_until_complete(test())
        self.assertIs(self.loop, f2._loop)
        ex.shutdown(wait=True)

    def test_wrap_future_use_global_loop(self):
        # Deprecated in 3.10
        asyncio.set_event_loop(self.loop)
        self.addCleanup(asyncio.set_event_loop, None)
        def run(arg):
            return (arg, threading.get_ident())
        ex = concurrent.futures.ThreadPoolExecutor(1)
        f1 = ex.submit(run, 'oi')
        with self.assertWarns(DeprecationWarning) as cm:
            f2 = asyncio.wrap_future(f1)
        self.assertEqual(cm.filename, __file__)
        self.assertIs(self.loop, f2._loop)
        ex.shutdown(wait=True)

    def test_wrap_future_cancel(self):
        f1 = concurrent.futures.Future()
        f2 = asyncio.wrap_future(f1, loop=self.loop)
        f2.cancel()
        test_utils.run_briefly(self.loop)
        self.assertTrue(f1.cancelled())
        self.assertTrue(f2.cancelled())

    def test_wrap_future_cancel2(self):
        f1 = concurrent.futures.Future()
        f2 = asyncio.wrap_future(f1, loop=self.loop)
        f1.set_result(42)
        f2.cancel()
        test_utils.run_briefly(self.loop)
        self.assertFalse(f1.cancelled())
        self.assertEqual(f1.result(), 42)
        self.assertTrue(f2.cancelled())

    def test_future_source_traceback(self):
        self.loop.set_debug(True)

        future = self._new_future(loop=self.loop)
        lineno = sys._getframe().f_lineno - 1
        self.assertIsInstance(future._source_traceback, list)
        self.assertEqual(future._source_traceback[-2][:3],
                         (__file__,
                          lineno,
                          'test_future_source_traceback'))

    @mock.patch('asyncio.base_events.logger')
    def check_future_exception_never_retrieved(self, debug, m_log):
        self.loop.set_debug(debug)

        def memory_error():
            try:
                raise MemoryError()
            except BaseException as exc:
                return exc
        exc = memory_error()

        future = self._new_future(loop=self.loop)
        future.set_exception(exc)
        future = None
        test_utils.run_briefly(self.loop)
        support.gc_collect()

        regex = f'^{self.cls.__name__} exception was never retrieved\n'
        exc_info = (type(exc), exc, exc.__traceback__)
        m_log.error.assert_called_once_with(mock.ANY, exc_info=exc_info)

        message = m_log.error.call_args[0][0]
        self.assertRegex(message, re.compile(regex, re.DOTALL))

    def test_future_exception_never_retrieved(self):
        self.check_future_exception_never_retrieved(False)

    def test_future_exception_never_retrieved_debug(self):
        self.check_future_exception_never_retrieved(True)

    def test_set_result_unless_cancelled(self):
        fut = self._new_future(loop=self.loop)
        fut.cancel()
        futures._set_result_unless_cancelled(fut, 2)
        self.assertTrue(fut.cancelled())

    def test_future_stop_iteration_args(self):
        fut = self._new_future(loop=self.loop)
        fut.set_result((1, 2))
        fi = fut.__iter__()
        result = None
        try:
            fi.send(None)
        except StopIteration as ex:
            result = ex.args[0]
        else:
            self.fail('StopIteration was expected')
        self.assertEqual(result, (1, 2))

    def test_future_iter_throw(self):
        fut = self._new_future(loop=self.loop)
        fi = iter(fut)
        self.assertRaises(TypeError, fi.throw,
                          Exception, Exception("elephant"), 32)
        self.assertRaises(TypeError, fi.throw,
                          Exception("elephant"), Exception("elephant"))
        self.assertRaises(TypeError, fi.throw, list)

    def test_future_del_collect(self):
        class Evil:
            def __del__(self):
                gc.collect()

        for i in range(100):
            fut = self._new_future(loop=self.loop)
            fut.set_result(Evil())


@unittest.skipUnless(hasattr(futures, '_CFuture'),
                     'requires the C _asyncio module')
class CFutureTests(BaseFutureTests, test_utils.TestCase):
    try:
        cls = futures._CFuture
    except AttributeError:
        cls = None

    def test_future_del_segfault(self):
        fut = self._new_future(loop=self.loop)
        with self.assertRaises(AttributeError):
            del fut._asyncio_future_blocking
        with self.assertRaises(AttributeError):
            del fut._log_traceback


@unittest.skipUnless(hasattr(futures, '_CFuture'),
                     'requires the C _asyncio module')
class CSubFutureTests(BaseFutureTests, test_utils.TestCase):
    try:
        class CSubFuture(futures._CFuture):
            pass

        cls = CSubFuture
    except AttributeError:
        cls = None


class PyFutureTests(BaseFutureTests, test_utils.TestCase):
    cls = futures._PyFuture


class BaseFutureDoneCallbackTests():

    def setUp(self):
        super().setUp()
        self.loop = self.new_test_loop()

    def run_briefly(self):
        test_utils.run_briefly(self.loop)

    def _make_callback(self, bag, thing):
        # Create a callback function that appends thing to bag.
        def bag_appender(future):
            bag.append(thing)
        return bag_appender

    def _new_future(self):
        raise NotImplementedError

    def test_callbacks_remove_first_callback(self):
        bag = []
        f = self._new_future()

        cb1 = self._make_callback(bag, 42)
        cb2 = self._make_callback(bag, 17)
        cb3 = self._make_callback(bag, 100)

        f.add_done_callback(cb1)
        f.add_done_callback(cb2)
        f.add_done_callback(cb3)

        f.remove_done_callback(cb1)
        f.remove_done_callback(cb1)

        self.assertEqual(bag, [])
        f.set_result('foo')

        self.run_briefly()

        self.assertEqual(bag, [17, 100])
        self.assertEqual(f.result(), 'foo')

    def test_callbacks_remove_first_and_second_callback(self):
        bag = []
        f = self._new_future()

        cb1 = self._make_callback(bag, 42)
        cb2 = self._make_callback(bag, 17)
        cb3 = self._make_callback(bag, 100)

        f.add_done_callback(cb1)
        f.add_done_callback(cb2)
        f.add_done_callback(cb3)

        f.remove_done_callback(cb1)
        f.remove_done_callback(cb2)
        f.remove_done_callback(cb1)

        self.assertEqual(bag, [])
        f.set_result('foo')

        self.run_briefly()

        self.assertEqual(bag, [100])
        self.assertEqual(f.result(), 'foo')

    def test_callbacks_remove_third_callback(self):
        bag = []
        f = self._new_future()

        cb1 = self._make_callback(bag, 42)
        cb2 = self._make_callback(bag, 17)
        cb3 = self._make_callback(bag, 100)

        f.add_done_callback(cb1)
        f.add_done_callback(cb2)
        f.add_done_callback(cb3)

        f.remove_done_callback(cb3)
        f.remove_done_callback(cb3)

        self.assertEqual(bag, [])
        f.set_result('foo')

        self.run_briefly()

        self.assertEqual(bag, [42, 17])
        self.assertEqual(f.result(), 'foo')

    def test_callbacks_invoked_on_set_result(self):
        bag = []
        f = self._new_future()
        f.add_done_callback(self._make_callback(bag, 42))
        f.add_done_callback(self._make_callback(bag, 17))

        self.assertEqual(bag, [])
        f.set_result('foo')

        self.run_briefly()

        self.assertEqual(bag, [42, 17])
        self.assertEqual(f.result(), 'foo')

    def test_callbacks_invoked_on_set_exception(self):
        bag = []
        f = self._new_future()
        f.add_done_callback(self._make_callback(bag, 100))

        self.assertEqual(bag, [])
        exc = RuntimeError()
        f.set_exception(exc)

        self.run_briefly()

        self.assertEqual(bag, [100])
        self.assertEqual(f.exception(), exc)

    def test_remove_done_callback(self):
        bag = []
        f = self._new_future()
        cb1 = self._make_callback(bag, 1)
        cb2 = self._make_callback(bag, 2)
        cb3 = self._make_callback(bag, 3)

        # Add one cb1 and one cb2.
        f.add_done_callback(cb1)
        f.add_done_callback(cb2)

        # One instance of cb2 removed. Now there's only one cb1.
        self.assertEqual(f.remove_done_callback(cb2), 1)

        # Never had any cb3 in there.
        self.assertEqual(f.remove_done_callback(cb3), 0)

        # After this there will be 6 instances of cb1 and one of cb2.
        f.add_done_callback(cb2)
        for i in range(5):
            f.add_done_callback(cb1)

        # Remove all instances of cb1. One cb2 remains.
        self.assertEqual(f.remove_done_callback(cb1), 6)

        self.assertEqual(bag, [])
        f.set_result('foo')

        self.run_briefly()

        self.assertEqual(bag, [2])
        self.assertEqual(f.result(), 'foo')

    def test_remove_done_callbacks_list_mutation(self):
        # see http://bugs.python.org/issue28963 for details

        fut = self._new_future()
        fut.add_done_callback(str)

        for _ in range(63):
            fut.add_done_callback(id)

        class evil:
            def __eq__(self, other):
                fut.remove_done_callback(id)
                return False

        fut.remove_done_callback(evil())

    def test_schedule_callbacks_list_mutation_1(self):
        # see http://bugs.python.org/issue28963 for details

        def mut(f):
            f.remove_done_callback(str)

        fut = self._new_future()
        fut.add_done_callback(mut)
        fut.add_done_callback(str)
        fut.add_done_callback(str)
        fut.set_result(1)
        test_utils.run_briefly(self.loop)

    def test_schedule_callbacks_list_mutation_2(self):
        # see http://bugs.python.org/issue30828 for details

        fut = self._new_future()
        fut.add_done_callback(str)

        for _ in range(63):
            fut.add_done_callback(id)

        max_extra_cbs = 100
        extra_cbs = 0

        class evil:
            def __eq__(self, other):
                nonlocal extra_cbs
                extra_cbs += 1
                if extra_cbs < max_extra_cbs:
                    fut.add_done_callback(id)
                return False

        fut.remove_done_callback(evil())


@unittest.skipUnless(hasattr(futures, '_CFuture'),
                     'requires the C _asyncio module')
class CFutureDoneCallbackTests(BaseFutureDoneCallbackTests,
                               test_utils.TestCase):

    def _new_future(self):
        return futures._CFuture(loop=self.loop)


@unittest.skipUnless(hasattr(futures, '_CFuture'),
                     'requires the C _asyncio module')
class CSubFutureDoneCallbackTests(BaseFutureDoneCallbackTests,
                                  test_utils.TestCase):

    def _new_future(self):
        class CSubFuture(futures._CFuture):
            pass
        return CSubFuture(loop=self.loop)


class PyFutureDoneCallbackTests(BaseFutureDoneCallbackTests,
                                test_utils.TestCase):

    def _new_future(self):
        return futures._PyFuture(loop=self.loop)


class BaseFutureInheritanceTests:

    def _get_future_cls(self):
        raise NotImplementedError

    def setUp(self):
        super().setUp()
        self.loop = self.new_test_loop()
        self.addCleanup(self.loop.close)

    def test_inherit_without_calling_super_init(self):
        # See https://bugs.python.org/issue38785 for the context
        cls = self._get_future_cls()

        class MyFut(cls):
            def __init__(self, *args, **kwargs):
                # don't call super().__init__()
                pass

        fut = MyFut(loop=self.loop)
        with self.assertRaisesRegex(
            RuntimeError,
            "Future object is not initialized."
        ):
            fut.get_loop()


class PyFutureInheritanceTests(BaseFutureInheritanceTests,
                               test_utils.TestCase):
    def _get_future_cls(self):
        return futures._PyFuture


@unittest.skipUnless(hasattr(futures, '_CFuture'),
                     'requires the C _asyncio module')
class CFutureInheritanceTests(BaseFutureInheritanceTests,
                              test_utils.TestCase):
    def _get_future_cls(self):
        return futures._CFuture


if __name__ == '__main__':
    unittest.main()
