
import unittest
import umo
import math

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
        x01 = dec1 + dec2
        x02 = dec1 - dec2
        x03 = dec1 * dec2
        x04 = dec1 / dec2
        x05 = -dec1
        for val1 in [5.0, -4.0, 2.5]:
            for val2 in [-7.0, 2.0, 1.3]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 + val2)
                self.assertEqual(x02.value, val1 - val2)
                self.assertEqual(x03.value, val1 * val2)
                self.assertEqual(x04.value, val1 / val2)
                self.assertEqual(x05.value, -val1)

    def test_int_compute(self):
        m = umo.Model()
        dec1 = m.int_var(0.0, 10.0)
        dec2 = m.int_var(0.0, 10.0)
        x01 = dec1 + dec2
        x02 = dec1 - dec2
        x03 = dec1 * dec2
        x04 = dec1 // dec2
        x05 = dec1 % dec2
        x06 = -dec1
        x07 = +dec1
        for val1 in [5, 4, 0]:
            for val2 in [1, 2, 3]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 + val2)
                self.assertEqual(x02.value, val1 - val2)
                self.assertEqual(x03.value, val1 * val2)
                self.assertEqual(x04.value, val1 // val2)
                self.assertEqual(x05.value, val1 % val2)
                self.assertEqual(x06.value, -val1)
                self.assertEqual(x07.value, +val1)

    def test_bool_compute(self):
        m = umo.Model()
        dec1 = m.bool_var()
        dec2 = m.bool_var()
        x01 = dec1 & dec2
        x02 = dec1 | dec2
        x03 = dec1 ^ dec2
        x04 = ~dec1
        for val1 in [False, True]:
            for val2 in [False, True]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 & val2)
                self.assertEqual(x02.value, val1 | val2)
                self.assertEqual(x03.value, val1 ^ val2)
                self.assertEqual(x04.value, not val1)

    def test_operations(self):
        m = umo.Model()
        dec = m.float_var(-10.0, 10.0)
        x01 = umo.exp(dec)
        x02 = umo.cos(dec)
        x03 = umo.atan(dec)
        for val in [5.0, -4.0, 2.5]:
            dec.value = val
            self.assertEqual(x01.value, math.exp(val))
            self.assertEqual(x02.value, math.cos(val))
            self.assertEqual(x03.value, math.atan(val))

if __name__ == '__main__':
    unittest.main()

