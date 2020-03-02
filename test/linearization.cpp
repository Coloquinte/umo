#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <stdexcept>

using namespace umo;

BOOST_AUTO_TEST_CASE(LinearizationFloat1) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(-50.0, 10.0);
    FloatExpression dec3 = model.floatVar(3.0, 6.0);
    constraint(dec1 + dec2 + dec3 <= 1.0);
    constraint(dec1 + dec2 - dec3 >= -20.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationBool1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    constraint(dec1 + dec2 + dec3 <= 2.0);
    constraint(dec1 + !dec2 + (-dec3) >= 1.0);
    constraint(dec1 + !dec2 + (-!dec3) >= 1.0);
    constraint(!(dec1 && dec2 && dec3));
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationCompare1) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(-50.0, 10.0);
    maximize(dec1 < dec2);
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationXor1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    maximize(dec1 ^ dec2 ^ dec3);
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMax1) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(-50.0, 10.0);
    maximize(umo::max(dec1, dec2));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}


