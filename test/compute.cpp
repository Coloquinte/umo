#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>

using namespace umo;

BOOST_AUTO_TEST_CASE(FloatCompute) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(0.0, 10.0);
    FloatExpression x01 = dec1 + dec2;
    FloatExpression x02 = dec1 - dec2;
    FloatExpression x03 = dec1 * dec2;
    FloatExpression x04 = dec1 / dec2;
    for (double val1 : {5.0, -4.0}) {
        for (double val2 : {-7.0, 2.0}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(), val1 + val2);
            BOOST_CHECK_EQUAL(x02.getValue(), val1 - val2);
            BOOST_CHECK_EQUAL(x03.getValue(), val1 * val2);
            BOOST_CHECK_EQUAL(x04.getValue(), val1 / val2);
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(IntCompute) {
    Model model;
    IntExpression dec1 = model.intVar(0, 10);
    IntExpression dec2 = model.intVar(0, 10);
    IntExpression x01 = dec1 + dec2;
    IntExpression x02 = dec1 - dec2;
    IntExpression x03 = dec1 * dec2;
    IntExpression x04 = dec1 / dec2;
    IntExpression x05 = dec1 % dec2;
    for (long long val1 : {5, -4}) {
        for (long long val2 : {-7, 2}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(), val1 + val2);
            BOOST_CHECK_EQUAL(x02.getValue(), val1 - val2);
            BOOST_CHECK_EQUAL(x03.getValue(), val1 * val2);
            BOOST_CHECK_EQUAL(x04.getValue(), val1 / val2);
            BOOST_CHECK_EQUAL(x05.getValue(), val1 % val2);
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(BoolCompute) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression x01 =  dec1 &&  dec2;
    BoolExpression x02 =  dec1 ||  dec2;
    BoolExpression x03 =  dec1 && !dec2;
    BoolExpression x04 = !dec1 ||  dec2;
    BoolExpression x05 = !(dec1 || dec2);
    BoolExpression x06 = !(dec1 && dec2);
    for (bool val1 : {false, true}) {
        for (bool val2 : {false, true}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(),  val1 &&  val2);
            BOOST_CHECK_EQUAL(x02.getValue(),  val1 ||  val2);
            BOOST_CHECK_EQUAL(x03.getValue(),  val1 && !val2);
            BOOST_CHECK_EQUAL(x04.getValue(), !val1 ||  val2);
            BOOST_CHECK_EQUAL(x05.getValue(), !(val1 || val2));
            BOOST_CHECK_EQUAL(x06.getValue(), !(val1 && val2));
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(Operations) {
    Model model;
    FloatExpression dec = model.floatVar(-10.0, 10.0);
    FloatExpression x01 = umo::exp(dec);
    FloatExpression x02 = umo::cos(dec);
    FloatExpression x03 = umo::atan(dec);
    for (double val : {5.0, -4.0, 2.5}) {
        dec.setValue(val);
        BOOST_CHECK_EQUAL(x01.getValue(), std::exp(val));
        BOOST_CHECK_EQUAL(x02.getValue(), std::cos(val));
        BOOST_CHECK_EQUAL(x03.getValue(), std::atan(val));
    }
    model.check();
}


