#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <stdexcept>

using namespace umo;

BOOST_AUTO_TEST_CASE(LinearizationFloatDec) {
    Model model;
    model.floatVar(-3.0, 10.2);
    model.floatVar(-50.9, 10.33);
    model.floatVar(-4.4, -2.0);
    model.floatVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationIntDec) {
    Model model;
    model.intVar(0, 10);
    model.intVar(-50, 10);
    model.intVar(-4, -2);
    model.intVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationBoolDec) {
    Model model;
    model.boolVar();
    model.boolVar();
    model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp1) {
    Model model;
    constraint(model.floatVar() > model.floatVar());
    constraint(model.floatVar() > 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp2) {
    Model model;
    constraint(model.floatVar() < model.floatVar());
    constraint(model.floatVar() < 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp3) {
    Model model;
    constraint(model.floatVar() >= model.floatVar());
    constraint(model.floatVar() >= 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp4) {
    Model model;
    constraint(model.floatVar() <= model.floatVar());
    constraint(model.floatVar() <= 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatEq1) {
    Model model;
    constraint(model.floatVar() == model.floatVar());
    constraint(model.floatVar() == 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatEq2) {
    Model model;
    constraint(!(model.floatVar() != model.floatVar()));
    constraint(!(model.floatVar() != 2.0));
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq1) {
    Model model;
    constraint(model.floatVar() != model.floatVar());
    constraint(model.floatVar() != 2.0);
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq2) {
    Model model;
    constraint(!(model.floatVar() == model.floatVar()));
    constraint(!(model.floatVar() == 2.0));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationSum1) {
    Model model;
    model.floatVar() + model.floatVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationMinus1) {
    Model model;
    model.floatVar() - model.floatVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationProd1) {
    Model model;
    5.0 * model.floatVar();
    4.0 * -model.floatVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationProd2) {
    Model model;
    5 * model.intVar();
    4 * -model.intVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationProd3) {
    Model model;
    5 * model.boolVar();
    4 * -model.boolVar();
    3 * !model.boolVar();
    2 * -!model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(LinearizationXor1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    maximize(dec1 ^ dec2);
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMax1) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(-50.0, 10.0);
    maximize(umo::max(dec1, dec2));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMin1) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(-50.0, 10.0);
    maximize(umo::min(dec1, dec2));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

