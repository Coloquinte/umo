
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
        m.check()

    def test_decisions(self):
        m = umo.Model()
        x1 = m.bool_var()
        x2 = m.int_var(-4, 2)
        x3 = m.float_var(0.5, 2)
        self.assertTrue(isinstance(x1, umo.BoolExpression))
        self.assertTrue(isinstance(x2, umo.IntExpression))
        self.assertTrue(isinstance(x3, umo.FloatExpression))
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
        m.check()

    def test_float_compute(self):
        m = umo.Model()
        dec1 = m.float_var(0.0, 10.0)
        dec2 = m.float_var(0.0, 10.0)
        x01 = dec1 + dec2
        x02 = dec1 - dec2
        x03 = dec1 * dec2
        x04 = dec1 / dec2
        x05 = -dec1
        for val1 in [5.2, -4.0, 2.5]:
            for val2 in [-7.97, 2.0, 1.3]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 + val2)
                self.assertEqual(x02.value, val1 - val2)
                self.assertEqual(x03.value, val1 * val2)
                self.assertEqual(x04.value, val1 / val2)
                self.assertEqual(x05.value, -val1)
        m.check()

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
        self.assertTrue(isinstance(x01, umo.IntExpression))
        self.assertTrue(isinstance(x02, umo.IntExpression))
        self.assertTrue(isinstance(x03, umo.IntExpression))
        self.assertTrue(isinstance(x04, umo.IntExpression))
        self.assertTrue(isinstance(x05, umo.IntExpression))
        self.assertTrue(isinstance(x06, umo.IntExpression))
        self.assertTrue(isinstance(x07, umo.IntExpression))
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
        m.check()

    def test_bool_compute(self):
        m = umo.Model()
        dec1 = m.bool_var()
        dec2 = m.bool_var()
        x01 = dec1 & dec2
        x02 = dec1 | dec2
        x03 = dec1 ^ dec2
        x04 = ~dec1
        self.assertTrue(isinstance(x01, umo.BoolExpression))
        self.assertTrue(isinstance(x02, umo.BoolExpression))
        self.assertTrue(isinstance(x03, umo.BoolExpression))
        self.assertTrue(isinstance(x04, umo.BoolExpression))
        for val1 in [False, True]:
            for val2 in [False, True]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 & val2)
                self.assertEqual(x02.value, val1 | val2)
                self.assertEqual(x03.value, val1 ^ val2)
                self.assertEqual(x04.value, not val1)
        m.check()

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
        m.check()

    def test_comparisons(self):
        m = umo.Model()
        dec1 = m.float_var(0.0, 10.0)
        dec2 = m.float_var(0.0, 10.0)
        x01 = dec1 == dec2
        x02 = dec1 != dec2
        x03 = dec1 <= dec2
        x04 = dec1 >= dec2
        x05 = dec1 < dec2
        x06 = dec1 > dec2
        self.assertTrue(isinstance(x01, umo.BoolExpression))
        self.assertTrue(isinstance(x02, umo.BoolExpression))
        self.assertTrue(isinstance(x03, umo.BoolExpression))
        self.assertTrue(isinstance(x04, umo.BoolExpression))
        self.assertTrue(isinstance(x05, umo.BoolExpression))
        self.assertTrue(isinstance(x06, umo.BoolExpression))
        for val1 in [5.0, -4.0, 2.5]:
            for val2 in [5.0, -4.0, 2.5]:
                dec1.value = val1
                dec2.value = val2
                self.assertEqual(x01.value, val1 == val2)
                self.assertEqual(x02.value, val1 != val2)
                self.assertEqual(x03.value, val1 <= val2)
                self.assertEqual(x04.value, val1 >= val2)
                self.assertEqual(x05.value, val1 < val2)
                self.assertEqual(x06.value, val1 > val2)
        m.check()

    def test_status(self):
        m = umo.Model()
        dec = m.bool_var()
        umo.constraint(dec)
        dec.value = True
        self.assertEqual(m.status, umo.SolutionStatus.VALID)
        dec.value = False
        self.assertEqual(m.status, umo.SolutionStatus.INVALID)
        m.check()

    def test_errors(self):
        m = umo.Model()
        self.assertRaises(RuntimeError, m.constant, float("nan"))

    def test_float_ops(self):
        m = umo.Model()
        fdec1 = m.float_var(-10.0, 10.0)
        fdec2 = m.float_var(5, 10.0)
        f01 = fdec1 + fdec2
        f02 = fdec1 + 1.0
        f03 = 1.0 + fdec1
        f04 = fdec1 * fdec2
        f05 = fdec2 * 2.0
        f06 = 2.0 * fdec2
        f07 = fdec1 - fdec2
        f08 = fdec2 - 1.0
        f09 = 1.0 - fdec2
        f10 = -fdec2
        f11 = fdec1 / fdec2
        f12 = fdec2 / 2.0
        f13 = 2.0 / fdec2
        f01.value
        f02.value
        f03.value
        f04.value
        f05.value
        f06.value
        f07.value
        f08.value
        f09.value
        f10.value
        f11.value
        f12.value
        f13.value
        b01 = fdec1 < fdec2
        b02 = fdec1 < 100.0
        b03 = 100.0 < fdec2
        b04 = fdec2 > fdec1
        b05 = fdec1 > -100.0
        b06 = 100.0 > fdec2
        b07 = fdec1 <= fdec2
        b08 = fdec1 <= 100.0
        b09 = -100.0 <= fdec2
        b10 = fdec2 >= fdec1
        b11 = fdec1 >= -100.0
        b12 = 100.0 >= fdec2
        b13 = fdec1 == fdec2
        b14 = fdec1 == 100.0
        b15 = 100.0 == fdec2
        b16 = fdec1 != fdec2
        b17 = fdec1 != 100.0
        b18 = 100.0 != fdec2
        b01.value
        b02.value
        b03.value
        b04.value
        b05.value
        b06.value
        b07.value
        b08.value
        b09.value
        b10.value
        b11.value
        b12.value
        b13.value
        b14.value
        b15.value
        b16.value
        b17.value
        b18.value
        m.check()

    def test_int_ops(self):
        m = umo.Model()
        idec1 = m.int_var(-10, 10)
        idec2 = m.int_var(5, 10)
        idec1.value = 1
        idec2.value = 1
        i01 = idec1 + idec2
        i02 = idec1 + 1
        i03 = 1 + idec1
        i04 = idec1 * idec2
        i05 = idec2 * 2
        i06 = 2 * idec2
        i07 = idec1 - idec2
        i08 = idec2 - 1
        i09 = 1 - idec2
        i10 = -idec2
        i11 = idec1 / idec2
        i12 = idec2 / 2
        i13 = 2 / idec2
        i14 = idec1 % idec2
        i15 = idec2 % 2
        i16 = 2 % idec2
        i01.value
        i02.value
        i03.value
        i04.value
        i05.value
        i06.value
        i07.value
        i08.value
        i09.value
        i10.value
        i11.value
        i12.value
        i13.value
        i14.value
        i15.value
        i16.value
        b01 = idec1 < idec2
        b02 = idec1 < 100
        b03 = 100 < idec2
        b04 = idec2 > idec1
        b05 = idec1 > -100
        b06 = 100 > idec2
        b07 = idec1 <= idec2
        b08 = idec1 <= 100
        b09 = -100 <= idec2
        b10 = idec2 >= idec1
        b11 = idec1 >= -100
        b12 = 100 >= idec2
        b13 = idec1 == idec2
        b14 = idec1 == 100
        b15 = 100 == idec2
        b16 = idec1 != idec2
        b17 = idec1 != 100
        b18 = 100 != idec2
        b01.value
        b02.value
        b03.value
        b04.value
        b05.value
        b06.value
        b07.value
        b08.value
        b09.value
        b10.value
        b11.value
        b12.value
        b13.value
        b14.value
        b15.value
        b16.value
        b17.value
        b18.value
        m.check()

    def test_bool_ops(self):
        m = umo.Model()
        bdec1 = m.bool_var()
        bdec2 = m.bool_var()
        b01 = bdec1 & bdec2
        b02 = bdec1 & True
        b03 = False & bdec1
        b04 = bdec1 | bdec2
        b05 = bdec2 | False
        b06 = True | bdec2
        b07 = bdec1 ^ bdec2
        b08 = bdec2 ^ False
        b09 = True ^ bdec2
        b10 = ~bdec2
        b01.value
        b02.value
        b03.value
        b04.value
        b05.value
        b06.value
        b07.value
        b08.value
        b09.value
        b10.value
        m.check()

    def test_compount_float(self):
        m = umo.Model()
        dec1 = m.float_var(-10.0, 10.0);
        dec2 = m.float_var(5.0, 10.0);
        dec1 += 3.0;
        dec1 += 3;
        dec1 += dec2;
        dec2 -= 3.0;
        dec2 -= 3;
        dec2 -= dec1;
        dec1 *= 3.0;
        dec1 *= 3;
        dec1 *= dec2;
        dec2 /= 3.0;
        dec2 /= 3;
        dec2 /= dec1;
        self.assertTrue(isinstance(dec1, umo.FloatExpression))
        self.assertTrue(isinstance(dec2, umo.FloatExpression))
        m.check()

    def test_compount_int(self):
        m = umo.Model()
        dec1 = m.int_var(-10, 10);
        dec2 = m.int_var(5, 10);
        dec1 += 3;
        dec1 += dec2;
        dec2 -= 3;
        dec2 -= dec1;
        dec1 *= 3;
        dec1 *= dec2;
        dec2 //= 3;
        dec2 //= dec1;
        dec2 %= 3;
        dec2 %= dec1;
        self.assertTrue(isinstance(dec1, umo.IntExpression))
        self.assertTrue(isinstance(dec2, umo.IntExpression))
        m.check()

    def test_compount_bool(self):
        m = umo.Model()
        dec1 = m.bool_var();
        dec2 = m.bool_var();
        dec1 &= dec2;
        dec1 &= True;
        dec2 |= dec1;
        dec2 |= False;
        dec1 ^= dec2;
        dec1 ^= True;
        self.assertTrue(isinstance(dec1, umo.BoolExpression))
        self.assertTrue(isinstance(dec2, umo.BoolExpression))
        m.check()

    def test_intrinsics(self):
        m = umo.Model()
        dec1 = m.float_var(-10.0, 10.0)
        x01 = abs(dec1)
        x03 = pow(dec1, -3)
        x04 = pow(dec1, 4)
        for val1 in [5.2, -4.0, 2.51]:
            dec1.value = val1
            self.assertEqual(x01.value, abs(val1))
            self.assertEqual(x03.value, pow(val1, -3))
            self.assertEqual(x04.value, pow(val1, 4))
        m.check()

    def test_int_intrinsics(self):
        m = umo.Model()
        dec1 = m.int_var(-10, 10)
        x01 = abs(dec1)
        x02 = pow(dec1, 4)
        self.assertTrue(isinstance(x01, umo.IntExpression))
        for val1 in [5, -4, 2, 0]:
            dec1.value = val1
            self.assertEqual(x01.value, abs(val1))
            self.assertEqual(x02.value, pow(val1, 4))
        m.check()

    def test_deletion(self):
        m = umo.Model()
        dec1 = m.bool_var()
        m = None
        dec2 = dec1.model.int_var(-10, 10)
        dec1.model.check()

    def test_params(self):
        m = umo.Model()
        m.set_param("test_param", 5.0)
        self.assertEqual(m.get_param("test_param"), 5.0)
        m.set_param("test_param", 6.4)
        self.assertEqual(m.get_param("test_param"), 6.4)
        self.assertEqual(m.time_limit, float("inf"))
        m.time_limit = 10.0
        self.assertEqual(m.time_limit, 10.0)

if __name__ == '__main__':
    unittest.main()

