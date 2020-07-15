#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CPP_API

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace umo;

BOOST_AUTO_TEST_CASE(SimpleModel) {
    Model model;
    FloatExpression fdec1 = model.floatVar(-10.0, 10.0);
    FloatExpression fdec2 = model.floatVar(5, 10.0);
    IntExpression idec = model.intVar(-2, 4);
    BoolExpression bdec = model.boolVar();
    maximize(fdec1 * fdec2);
    constraint(fdec1 <= idec);
    minimize(idec + bdec + fdec2);
    model.check();
}

BOOST_AUTO_TEST_CASE(UnboundedDecisions) {
    Model model;
    FloatExpression fdec = model.floatVar();
    IntExpression idec = model.intVar();
    maximize(fdec * idec);
    constraint(fdec <= idec);
    model.check();
}

BOOST_AUTO_TEST_CASE(HalfSpaceDecisions) {
    Model model;
    FloatExpression fdec1 = model.floatVar(umo::unbounded(), 0.0);
    FloatExpression fdec2 = model.floatVar(0.0, umo::unbounded());
    FloatExpression fdec3 = model.floatVar(umo::unbounded(), umo::unbounded());
    FloatExpression idec1 = model.intVar(umo::unbounded(), 0);
    FloatExpression idec2 = model.intVar(0, umo::unbounded());
    FloatExpression idec3 = model.intVar(umo::unbounded(), umo::unbounded());
    maximize(fdec1 * fdec2 * fdec3 * idec1 * idec2 * idec3);
    model.check();
}

BOOST_AUTO_TEST_CASE(DoublePromotions) {
    Model model;
    FloatExpression fdec = model.floatVar();
    IntExpression idec = model.intVar();
    BoolExpression bdec = model.boolVar();
    1.5+fdec;
    1.5-fdec;
    1.5*fdec;
    1.5/fdec;
    fdec+1.5;
    fdec-1.5;
    fdec*1.5;
    fdec/1.5;
    1.5+idec;
    1.5-idec;
    1.5*idec;
    1.5/idec;
    idec+1.5;
    idec-1.5;
    idec*1.5;
    idec/1.5;
    1.5+bdec;
    1.5-bdec;
    1.5*bdec;
    1.5/bdec;
    bdec+1.5;
    bdec-1.5;
    bdec*1.5;
    bdec/1.5;
    fdec += 1.5;
    fdec -= 1.5;
    fdec *= 1.5;
    fdec /= 1.5;
    model.check();
}

BOOST_AUTO_TEST_CASE(FloatPromotions) {
    Model model;
    FloatExpression fdec = model.floatVar();
    IntExpression idec = model.intVar();
    BoolExpression bdec = model.boolVar();
    1.5f+fdec;
    1.5f-fdec;
    1.5f*fdec;
    1.5f/fdec;
    fdec+1.5f;
    fdec-1.5f;
    fdec*1.5f;
    fdec/1.5f;
    1.5f+idec;
    1.5f-idec;
    1.5f*idec;
    1.5f/idec;
    idec+1.5f;
    idec-1.5f;
    idec*1.5f;
    idec/1.5f;
    1.5f+bdec;
    1.5f-bdec;
    1.5f*bdec;
    1.5f/bdec;
    bdec+1.5f;
    bdec-1.5f;
    bdec*1.5f;
    bdec/1.5f;
    fdec += 1.5f;
    fdec -= 1.5f;
    fdec *= 1.5f;
    fdec /= 1.5f;
    model.check();
}

BOOST_AUTO_TEST_CASE(IntPromotions) {
    Model model;
    FloatExpression fdec = model.floatVar();
    IntExpression idec = model.intVar();
    BoolExpression bdec = model.boolVar();
    1+fdec;
    1-fdec;
    1*fdec;
    1/fdec;
    fdec+1;
    fdec-1;
    fdec*1;
    fdec/1;
    1+idec;
    1-idec;
    1*idec;
    1/idec;
    idec+1;
    idec-1;
    idec*1;
    idec/1;
    1+bdec;
    1-bdec;
    1*bdec;
    1/bdec;
    1%bdec;
    bdec+1;
    bdec-1;
    bdec*1;
    bdec/1;
    bdec%1;
    fdec += 1;
    fdec -= 1;
    fdec *= 1;
    fdec /= 1;
    idec += 1;
    idec -= 1;
    idec *= 1;
    idec /= 1;
    model.check();
}

BOOST_AUTO_TEST_CASE(LongLongPromotions) {
    Model model;
    FloatExpression fdec = model.floatVar();
    IntExpression idec = model.intVar();
    BoolExpression bdec = model.boolVar();
    1ll+fdec;
    1ll-fdec;
    1ll*fdec;
    1ll/fdec;
    fdec+1ll;
    fdec-1ll;
    fdec*1ll;
    fdec/1ll;
    1ll+idec;
    1ll-idec;
    1ll*idec;
    1ll/idec;
    idec+1ll;
    idec-1ll;
    idec*1ll;
    idec/1ll;
    1ll+bdec;
    1ll-bdec;
    1ll*bdec;
    1ll/bdec;
    1ll%bdec;
    bdec+1ll;
    bdec-1ll;
    bdec*1ll;
    bdec/1ll;
    bdec%1ll;
    fdec += 1ll;
    fdec -= 1ll;
    fdec *= 1ll;
    fdec /= 1ll;
    idec += 1ll;
    idec -= 1ll;
    idec *= 1ll;
    idec /= 1ll;
    model.check();
}

BOOST_AUTO_TEST_CASE(FloatOps) {
    Model model;
    FloatExpression fdec1 = model.floatVar(-10.0, 10.0);
    FloatExpression fdec2 = model.floatVar(5, 10.0);
    FloatExpression f01 = fdec1 + fdec2;
    FloatExpression f02 = fdec1 + 1.0;
    FloatExpression f03 = 1.0 + fdec1;
    FloatExpression f04 = fdec1 * fdec2;
    FloatExpression f05 = fdec2 * 2.0;
    FloatExpression f06 = 2.0 * fdec2;
    FloatExpression f07 = fdec1 - fdec2;
    FloatExpression f08 = fdec2 - 1.0;
    FloatExpression f09 = 1.0 - fdec2;
    FloatExpression f10 = -fdec2;
    FloatExpression f11 = fdec1 / fdec2;
    FloatExpression f12 = fdec2 / 2.0;
    FloatExpression f13 = 2.0 / fdec2;
    f01.getValue();
    f02.getValue();
    f03.getValue();
    f04.getValue();
    f05.getValue();
    f06.getValue();
    f07.getValue();
    f08.getValue();
    f09.getValue();
    f10.getValue();
    f11.getValue();
    f12.getValue();
    f13.getValue();
    BoolExpression b01 = fdec1 < fdec2;
    BoolExpression b02 = fdec1 < 100.0;
    BoolExpression b03 = 100.0 < fdec2;
    BoolExpression b04 = fdec2 > fdec1;
    BoolExpression b05 = fdec1 > -100.0;
    BoolExpression b06 = 100.0 > fdec2;
    BoolExpression b07 = fdec1 <= fdec2;
    BoolExpression b08 = fdec1 <= 100.0;
    BoolExpression b09 = -100.0 <= fdec2;
    BoolExpression b10 = fdec2 >= fdec1;
    BoolExpression b11 = fdec1 >= -100.0;
    BoolExpression b12 = 100.0 >= fdec2;
    BoolExpression b13 = fdec1 == fdec2;
    BoolExpression b14 = fdec1 == 100.0;
    BoolExpression b15 = 100.0 == fdec2;
    BoolExpression b16 = fdec1 != fdec2;
    BoolExpression b17 = fdec1 != 100.0;
    BoolExpression b18 = 100.0 != fdec2;
    b01.getValue();
    b02.getValue();
    b03.getValue();
    b04.getValue();
    b05.getValue();
    b06.getValue();
    b07.getValue();
    b08.getValue();
    b09.getValue();
    b10.getValue();
    b11.getValue();
    b12.getValue();
    b13.getValue();
    b14.getValue();
    b15.getValue();
    b16.getValue();
    b17.getValue();
    b18.getValue();
    fdec1.setValue(-2.5);
    BOOST_CHECK_EQUAL(-2.5, fdec1.getValue());
    fdec1.setValue(4.8);
    BOOST_CHECK_EQUAL(4.8, fdec1.getValue());
    model.check();
}

BOOST_AUTO_TEST_CASE(IntOps) {
    Model model;
    IntExpression idec1 = model.intVar(-10, 10);
    IntExpression idec2 = model.intVar(5, 10);
    idec1.setValue(1);
    idec2.setValue(1);
    IntExpression i01 = idec1 + idec2;
    IntExpression i02 = idec1 + 1;
    IntExpression i03 = 1 + idec1;
    IntExpression i04 = idec1 * idec2;
    IntExpression i05 = idec2 * 2;
    IntExpression i06 = 2 * idec2;
    IntExpression i07 = idec1 - idec2;
    IntExpression i08 = idec2 - 1;
    IntExpression i09 = 1 - idec2;
    IntExpression i10 = -idec2;
    IntExpression i11 = idec1 / idec2;
    IntExpression i12 = idec2 / 2;
    IntExpression i13 = 2 / idec2;
    IntExpression i14 = idec1 % idec2;
    IntExpression i15 = idec2 % 2;
    IntExpression i16 = 2 % idec2;
    i01.getValue();
    i02.getValue();
    i03.getValue();
    i04.getValue();
    i05.getValue();
    i06.getValue();
    i07.getValue();
    i08.getValue();
    i09.getValue();
    i10.getValue();
    i11.getValue();
    i12.getValue();
    i13.getValue();
    i14.getValue();
    i15.getValue();
    i16.getValue();
    BoolExpression b01 = idec1 < idec2;
    BoolExpression b02 = idec1 < 100;
    BoolExpression b03 = 100 < idec2;
    BoolExpression b04 = idec2 > idec1;
    BoolExpression b05 = idec1 > -100;
    BoolExpression b06 = 100 > idec2;
    BoolExpression b07 = idec1 <= idec2;
    BoolExpression b08 = idec1 <= 100;
    BoolExpression b09 = -100 <= idec2;
    BoolExpression b10 = idec2 >= idec1;
    BoolExpression b11 = idec1 >= -100;
    BoolExpression b12 = 100 >= idec2;
    BoolExpression b13 = idec1 == idec2;
    BoolExpression b14 = idec1 == 100;
    BoolExpression b15 = 100 == idec2;
    BoolExpression b16 = idec1 != idec2;
    BoolExpression b17 = idec1 != 100;
    BoolExpression b18 = 100 != idec2;
    b01.getValue();
    b02.getValue();
    b03.getValue();
    b04.getValue();
    b05.getValue();
    b06.getValue();
    b07.getValue();
    b08.getValue();
    b09.getValue();
    b10.getValue();
    b11.getValue();
    b12.getValue();
    b13.getValue();
    b14.getValue();
    b15.getValue();
    b16.getValue();
    b17.getValue();
    b18.getValue();
    idec1.setValue(-2);
    BOOST_CHECK_EQUAL(-2, idec1.getValue());
    idec1.setValue(4);
    BOOST_CHECK_EQUAL(4, idec1.getValue());
    model.check();
}

BOOST_AUTO_TEST_CASE(BoolOps) {
    Model model;
    BoolExpression bdec1 = model.boolVar();
    BoolExpression bdec2 = model.boolVar();
    BoolExpression b01 = bdec1 && bdec2;
    BoolExpression b02 = bdec1 && true;
    BoolExpression b03 = false && bdec1;
    BoolExpression b04 = bdec1 || bdec2;
    BoolExpression b05 = bdec2 || false;
    BoolExpression b06 = true || bdec2;
    BoolExpression b07 = bdec1 ^ bdec2;
    BoolExpression b08 = bdec2 ^ false;
    BoolExpression b09 = true ^ bdec2;
    BoolExpression b10 = !bdec2;
    b01.getValue();
    b02.getValue();
    b03.getValue();
    b04.getValue();
    b05.getValue();
    b06.getValue();
    b07.getValue();
    b08.getValue();
    b09.getValue();
    b10.getValue();
    bdec1.setValue(true);
    BOOST_CHECK_EQUAL(true, bdec1.getValue());
    bdec1.setValue(false);
    BOOST_CHECK_EQUAL(false, bdec1.getValue());
    model.check();
}

BOOST_AUTO_TEST_CASE(Params) {
    Model model;
    model.setFloatParam("testFloatParam", -1.0);
    double resFloat1 = model.getFloatParam("testFloatParam");
    BOOST_CHECK_EQUAL(resFloat1, -1.0);
    model.setFloatParam("testFloatParam", 2.4);
    double resFloat2 = model.getFloatParam("testFloatParam");
    BOOST_CHECK_EQUAL(resFloat2, 2.4);
    model.setStringParam("testStringParam", "testValue");
    std::string resString = model.getStringParam("testStringParam");
    BOOST_CHECK_EQUAL(resString, "testValue");
    BOOST_CHECK_EQUAL(model.getTimeLimit(), std::numeric_limits<double>::infinity());
    model.setTimeLimit(10);
    BOOST_CHECK_EQUAL(model.getTimeLimit(), 10.0);
    model.setTimeLimit(0.125);
    BOOST_CHECK_EQUAL(model.getTimeLimit(), 0.125);
    model.check();
}

BOOST_AUTO_TEST_CASE(Exception) {
    Model model;
    BOOST_CHECK_THROW(model.constant(NAN), std::runtime_error);
    model.check();
}

BOOST_AUTO_TEST_CASE(CompoundFloat) {
    Model model;
    FloatExpression dec1 = model.floatVar(-10.0, 10.0);
    FloatExpression dec2 = model.floatVar(5.0, 10.0);
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
    model.check();
}

BOOST_AUTO_TEST_CASE(CompoundInt) {
    Model model;
    IntExpression dec1 = model.intVar(-10, 10);
    IntExpression dec2 = model.intVar(5, 10);
    dec1 += 3;
    dec1 += dec2;
    dec2 -= 3;
    dec2 -= dec1;
    dec1 *= 3;
    dec1 *= dec2;
    dec2 /= 3;
    dec2 /= dec1;
    dec2 %= 3;
    dec2 %= dec1;
    model.check();
}

BOOST_AUTO_TEST_CASE(CompoundBool) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    dec1 &= dec2;
    dec1 &= true;
    dec2 |= dec1;
    dec2 |= false;
    dec1 ^= dec2;
    dec1 ^= true;
    model.check();
}

BOOST_AUTO_TEST_CASE(NaryFloat) {
    Model model;
    FloatExpression dec1 = model.floatVar();
    FloatExpression dec2 = model.floatVar();
    FloatExpression dec3 = model.floatVar();
    sum(std::vector<FloatExpression>{dec1, dec2, dec3});
    prod(std::vector<FloatExpression>{dec1, dec2, dec3});
    min(std::vector<FloatExpression>{dec1, dec2, dec3});
    max(std::vector<FloatExpression>{dec1, dec2, dec3});
    BOOST_CHECK_THROW(sum(std::vector<FloatExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(prod(std::vector<FloatExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(max(std::vector<FloatExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(min(std::vector<FloatExpression>()), std::runtime_error);
    model.sum(std::vector<FloatExpression>());
    model.prod(std::vector<FloatExpression>());
    model.max(std::vector<FloatExpression>());
    model.min(std::vector<FloatExpression>());
    model.check();
}

BOOST_AUTO_TEST_CASE(NaryInt) {
    Model model;
    IntExpression dec1 = model.intVar();
    IntExpression dec2 = model.intVar();
    IntExpression dec3 = model.intVar();
    sum(std::vector<IntExpression>{dec1, dec2, dec3});
    prod(std::vector<IntExpression>{dec1, dec2, dec3});
    min(std::vector<IntExpression>{dec1, dec2, dec3});
    max(std::vector<IntExpression>{dec1, dec2, dec3});
    BOOST_CHECK_THROW(sum(std::vector<IntExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(prod(std::vector<IntExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(max(std::vector<IntExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(min(std::vector<IntExpression>()), std::runtime_error);
    model.sum(std::vector<IntExpression>());
    model.prod(std::vector<IntExpression>());
    model.max(std::vector<IntExpression>());
    model.min(std::vector<IntExpression>());
    model.check();
}

BOOST_AUTO_TEST_CASE(NaryBool) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    logical_or(std::vector<BoolExpression>{dec1, dec2, dec3});
    logical_and(std::vector<BoolExpression>{dec1, dec2, dec3});
    logical_xor(std::vector<BoolExpression>{dec1, dec2, dec3});
    sum(std::vector<BoolExpression>{dec1, dec2, dec3});
    prod(std::vector<BoolExpression>{dec1, dec2, dec3});
    min(std::vector<BoolExpression>{dec1, dec2, dec3});
    max(std::vector<BoolExpression>{dec1, dec2, dec3});
    BOOST_CHECK_THROW(sum(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(prod(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(max(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(min(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(logical_or(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(logical_and(std::vector<BoolExpression>()), std::runtime_error);
    BOOST_CHECK_THROW(logical_xor(std::vector<BoolExpression>()), std::runtime_error);
    model.sum(std::vector<BoolExpression>());
    model.prod(std::vector<BoolExpression>());
    model.max(std::vector<BoolExpression>());
    model.min(std::vector<BoolExpression>());
    model.logical_or(std::vector<BoolExpression>());
    model.logical_and(std::vector<BoolExpression>());
    model.logical_xor(std::vector<BoolExpression>());
    model.check();
}

