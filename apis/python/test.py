
import unittest
import umo

class TestUmoApi(unittest.TestCase):

    def test_creation(self):
        m = umo.Model()

    def test_constants(self):
        m = umo.Model()
        b1 = m.constant(False)
        b2 = m.constant(True)
        self.assertEqual(b1.value, False)
        self.assertEqual(b2.value, True)
        self.assertTrue(isinstance(b1, umo.BoolExpression))
        self.assertTrue(isinstance(b2, umo.BoolExpression))
        i1 = m.constant(2)
        i2 = m.constant(-3.0)
        self.assertEqual(i1.value, 2)
        self.assertEqual(i2.value, -3)
        self.assertTrue(isinstance(i1, umo.IntExpression))
        self.assertTrue(isinstance(i2, umo.IntExpression))
        f1 = m.constant(2.4)
        f2 = m.constant(-3.9)
        self.assertEqual(f1.value, 2.4)
        self.assertEqual(f2.value, -3.9)
        self.assertTrue(isinstance(f1, umo.FloatExpression))
        self.assertTrue(isinstance(f2, umo.FloatExpression))

    def test_decisions(self):
        m = umo.Model()
        x1 = m.bool_var()
        x2 = m.int_var(-4, 2)
        x3 = m.float_var(0.5, 2)
        x1.value = False
        self.assertEqual(x1.value, False)
        x1.value = True
        self.assertEqual(x1.value, True)
        x2.value = 1
        self.assertEqual(x2.value, 1)
        x2.value = -2
        self.assertEqual(x2.value, -2)
        x3.value = 1.0
        self.assertEqual(x3.value, 1.0)
        x3.value = 1.83
        self.assertEqual(x3.value, 1.83)

    def test_float_compute(self):
        m = umo.Model()
        dec1 = m.float_var(0.0, 10.0)
        dec2 = m.float_var(0.0, 10.0)

    def test_int_compute(self):
        pass

    def test_bool_compute(self):
        pass

    def test_operations(self):
        pass
if __name__ == '__main__':
    unittest.main()

