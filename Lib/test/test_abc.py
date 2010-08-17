# Copyright 2007 Google, Inc. All Rights Reserved.
# Licensed to PSF under a Contributor Agreement.

"""Unit tests for abc.py."""

import unittest
from test import support

import abc
from inspect import isabstract


class TestABC(unittest.TestCase):

    def test_abstractmethod_basics(self):
        @abc.abstractmethod
        def foo(self): pass
        self.assertTrue(foo.__isabstractmethod__)
        def bar(self): pass
        self.assertFalse(hasattr(bar, "__isabstractmethod__"))

    def test_abstractproperty_basics(self):
        @abc.abstractproperty
        def foo(self): pass
        self.assertTrue(foo.__isabstractmethod__)
        def bar(self): pass
        self.assertFalse(hasattr(bar, "__isabstractmethod__"))

        class C(metaclass=abc.ABCMeta):
            @abc.abstractproperty
            def foo(self): return 3
        class D(C):
            @property
            def foo(self): return super().foo
        self.assertEqual(D().foo, 3)

    def test_abstractclassmethod_basics(self):
        @abc.abstractclassmethod
        def foo(cls): pass
        self.assertTrue(foo.__isabstractmethod__)
        @classmethod
        def bar(cls): pass
        self.assertFalse(hasattr(bar, "__isabstractmethod__"))

        class C(metaclass=abc.ABCMeta):
            @abc.abstractclassmethod
            def foo(cls): return cls.__name__
        self.assertRaises(TypeError, C)
        class D(C):
            @classmethod
            def foo(cls): return super().foo()
        self.assertEqual(D.foo(), 'D')
        self.assertEqual(D().foo(), 'D')

    def test_abstractstaticmethod_basics(self):
        @abc.abstractstaticmethod
        def foo(): pass
        self.assertTrue(foo.__isabstractmethod__)
        @staticmethod
        def bar(): pass
        self.assertFalse(hasattr(bar, "__isabstractmethod__"))

        class C(metaclass=abc.ABCMeta):
            @abc.abstractstaticmethod
            def foo(): return 3
        self.assertRaises(TypeError, C)
        class D(C):
            @staticmethod
            def foo(): return 4
        self.assertEqual(D.foo(), 4)
        self.assertEqual(D().foo(), 4)

    def test_abstractmethod_integration(self):
        for abstractthing in [abc.abstractmethod, abc.abstractproperty,
                              abc.abstractclassmethod,
                              abc.abstractstaticmethod]:
            class C(metaclass=abc.ABCMeta):
                @abstractthing
                def foo(self): pass  # abstract
                def bar(self): pass  # concrete
            self.assertEqual(C.__abstractmethods__, {"foo"})
            self.assertRaises(TypeError, C)  # because foo is abstract
            self.assertTrue(isabstract(C))
            class D(C):
                def bar(self): pass  # concrete override of concrete
            self.assertEqual(D.__abstractmethods__, {"foo"})
            self.assertRaises(TypeError, D)  # because foo is still abstract
            self.assertTrue(isabstract(D))
            class E(D):
                def foo(self): pass
            self.assertEqual(E.__abstractmethods__, set())
            E()  # now foo is concrete, too
            self.assertFalse(isabstract(E))
            class F(E):
                @abstractthing
                def bar(self): pass  # abstract override of concrete
            self.assertEqual(F.__abstractmethods__, {"bar"})
            self.assertRaises(TypeError, F)  # because bar is abstract now
            self.assertTrue(isabstract(F))

    def test_subclass_oldstyle_class(self):
        class A:
            __metaclass__ = abc.ABCMeta
        class OldstyleClass:
            pass
        self.assertFalse(issubclass(OldstyleClass, A))
        self.assertFalse(issubclass(A, OldstyleClass))

    def test_registration_basics(self):
        class A(metaclass=abc.ABCMeta):
            pass
        class B(object):
            pass
        b = B()
        self.assertFalse(issubclass(B, A))
        self.assertFalse(issubclass(B, (A,)))
        self.assertNotIsInstance(b, A)
        self.assertNotIsInstance(b, (A,))
        A.register(B)
        self.assertTrue(issubclass(B, A))
        self.assertTrue(issubclass(B, (A,)))
        self.assertIsInstance(b, A)
        self.assertIsInstance(b, (A,))
        class C(B):
            pass
        c = C()
        self.assertTrue(issubclass(C, A))
        self.assertTrue(issubclass(C, (A,)))
        self.assertIsInstance(c, A)
        self.assertIsInstance(c, (A,))

    def test_isinstance_invalidation(self):
        class A(metaclass=abc.ABCMeta):
            pass
        class B:
            pass
        b = B()
        self.assertFalse(isinstance(b, A))
        self.assertFalse(isinstance(b, (A,)))
        A.register(B)
        self.assertTrue(isinstance(b, A))
        self.assertTrue(isinstance(b, (A,)))

    def test_registration_builtins(self):
        class A(metaclass=abc.ABCMeta):
            pass
        A.register(int)
        self.assertIsInstance(42, A)
        self.assertIsInstance(42, (A,))
        self.assertTrue(issubclass(int, A))
        self.assertTrue(issubclass(int, (A,)))
        class B(A):
            pass
        B.register(str)
        class C(str): pass
        self.assertIsInstance("", A)
        self.assertIsInstance("", (A,))
        self.assertTrue(issubclass(str, A))
        self.assertTrue(issubclass(str, (A,)))
        self.assertTrue(issubclass(C, A))
        self.assertTrue(issubclass(C, (A,)))

    def test_registration_edge_cases(self):
        class A(metaclass=abc.ABCMeta):
            pass
        A.register(A)  # should pass silently
        class A1(A):
            pass
        self.assertRaises(RuntimeError, A1.register, A)  # cycles not allowed
        class B(object):
            pass
        A1.register(B)  # ok
        A1.register(B)  # should pass silently
        class C(A):
            pass
        A.register(C)  # should pass silently
        self.assertRaises(RuntimeError, C.register, A)  # cycles not allowed
        C.register(B)  # ok

    def test_register_non_class(self):
        class A(metaclass=abc.ABCMeta):
            pass
        self.assertRaisesRegexp(TypeError, "Can only register classes",
                                A.register, 4)

    def test_registration_transitiveness(self):
        class A(metaclass=abc.ABCMeta):
            pass
        self.assertTrue(issubclass(A, A))
        self.assertTrue(issubclass(A, (A,)))
        class B(metaclass=abc.ABCMeta):
            pass
        self.assertFalse(issubclass(A, B))
        self.assertFalse(issubclass(A, (B,)))
        self.assertFalse(issubclass(B, A))
        self.assertFalse(issubclass(B, (A,)))
        class C(metaclass=abc.ABCMeta):
            pass
        A.register(B)
        class B1(B):
            pass
        self.assertTrue(issubclass(B1, A))
        self.assertTrue(issubclass(B1, (A,)))
        class C1(C):
            pass
        B1.register(C1)
        self.assertFalse(issubclass(C, B))
        self.assertFalse(issubclass(C, (B,)))
        self.assertFalse(issubclass(C, B1))
        self.assertFalse(issubclass(C, (B1,)))
        self.assertTrue(issubclass(C1, A))
        self.assertTrue(issubclass(C1, (A,)))
        self.assertTrue(issubclass(C1, B))
        self.assertTrue(issubclass(C1, (B,)))
        self.assertTrue(issubclass(C1, B1))
        self.assertTrue(issubclass(C1, (B1,)))
        C1.register(int)
        class MyInt(int):
            pass
        self.assertTrue(issubclass(MyInt, A))
        self.assertTrue(issubclass(MyInt, (A,)))
        self.assertIsInstance(42, A)
        self.assertIsInstance(42, (A,))

    def test_all_new_methods_are_called(self):
        class A(metaclass=abc.ABCMeta):
            pass
        class B(object):
            counter = 0
            def __new__(cls):
                B.counter += 1
                return super().__new__(cls)
        class C(A, B):
            pass
        self.assertEqual(B.counter, 0)
        C()
        self.assertEqual(B.counter, 1)


def test_main():
    support.run_unittest(TestABC)


if __name__ == "__main__":
    unittest.main()
