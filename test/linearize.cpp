#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

using namespace umo;

const double eps = 0.001;

BOOST_AUTO_TEST_CASE(LinearizationFloatDec) {
    Model model;
    FloatExpression x1 = model.floatVar(-3.0, 10.2);
    FloatExpression x2 = model.floatVar(-50.9, 10.33);
    FloatExpression x3 = model.floatVar(-4.4, -2.0);
    FloatExpression x4 = model.floatVar(10.0, 21.2);
    maximize(x1 + x2 + x3 + x4);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x1.getValue(), 10.2, eps);
    BOOST_CHECK_CLOSE(x2.getValue(), 10.33, eps);
    BOOST_CHECK_CLOSE(x3.getValue(), -2.0, eps);
    BOOST_CHECK_CLOSE(x4.getValue(), 21.2, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationIntDec) {
    Model model;
    IntExpression x1 = model.intVar(0, 10);
    IntExpression x2 = model.intVar(-50, 10);
    IntExpression x3 = model.intVar(-4, -2);
    IntExpression x4 = model.intVar(10, 21);
    maximize(x1 + x2 + x3 + x4);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(x1.getValue(), 10);
    BOOST_CHECK_EQUAL(x2.getValue(), 10);
    BOOST_CHECK_EQUAL(x3.getValue(), -2);
    BOOST_CHECK_EQUAL(x4.getValue(), 21);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationBoolDec) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    BoolExpression x3 = model.boolVar();
    BoolExpression x4 = model.boolVar();
    maximize(x1 + x2 + x3 + x4);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x1.getValue());
    BOOST_CHECK(x2.getValue());
    BOOST_CHECK(x3.getValue());
    BOOST_CHECK(x4.getValue());
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp1) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(x > 2.0);
    minimize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp2) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(x < 2.0);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp3) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(x >= 2.0);
    minimize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp4) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(x <= 2.0);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp5) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(5.0, 10.0);
    constraint(x >= y);
    minimize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 5.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatCmp6) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(5.0, 10.0);
    constraint(x <= y);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 10.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}


BOOST_AUTO_TEST_CASE(LinearizationFloatEq1) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(x == 2.0);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatEq2) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(0.0, 2.0);
    constraint(x == y);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatEq3) {
    Model model;
    FloatExpression x = model.floatVar();
    constraint(!(x != 2.0));
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatEq4) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(0.0, 2.0);
    constraint(!(x != y));
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq1) {
    Model model;
    FloatExpression x = model.floatVar(0.0, 5.0);
    constraint(x != 2.0);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq2) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(0.0, 2.0);
    constraint(x != y);
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq3) {
    Model model;
    FloatExpression x = model.floatVar(0.0, 5.0);
    constraint(!(x == 2.0));
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationFloatNeq4) {
    Model model;
    FloatExpression x = model.floatVar();
    FloatExpression y = model.floatVar(0.0, 2.0);
    constraint(!(x == y));
    maximize(x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationAnd1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    std::vector<BoolExpression> vec{dec1, dec2, dec3};
    BoolExpression obj = logical_and(vec);
    maximize(obj);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(dec1.getValue() && dec2.getValue() && dec3.getValue());
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationAnd2) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    std::vector<BoolExpression> vec{dec1, dec2, dec3};
    BoolExpression obj = logical_and(vec);
    minimize(obj);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!(dec1.getValue() && dec2.getValue() && dec3.getValue()));
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationOr1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    std::vector<BoolExpression> vec{dec1, dec2, dec3};
    BoolExpression obj = logical_or(vec);
    maximize(obj);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(dec1.getValue() || dec2.getValue() || dec3.getValue());
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationOr2) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    std::vector<BoolExpression> vec{dec1, dec2, dec3};
    BoolExpression obj = logical_or(vec);
    minimize(obj);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!(dec1.getValue() || dec2.getValue() || dec3.getValue()));
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationXor1) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    maximize(dec1 ^ dec2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMax1) {
    Model model;
    FloatExpression dec1 = model.floatVar();
    FloatExpression dec2 = model.floatVar();
    maximize(umo::max(dec1, dec2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMin1) {
    Model model;
    FloatExpression dec1 = model.floatVar();
    FloatExpression dec2 = model.floatVar();
    maximize(umo::min(dec1, dec2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationMultiObjective) {
    Model model;
    FloatExpression dec1 = model.floatVar();
    FloatExpression dec2 = model.floatVar();
    maximize(dec1);
    minimize(dec2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(LinearizationUnfeasible) {
    Model model;
    FloatExpression dec1 = model.floatVar(0, 10);
    FloatExpression dec2 = model.floatVar(0, 10);
    constraint(dec1 + 1 <= dec2);
    constraint(dec2 + 1 <= dec1);
    maximize(dec1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Infeasible);
}

BOOST_AUTO_TEST_CASE(LinearizationUnbounded) {
    Model model;
    FloatExpression dec1 = model.floatVar();
    FloatExpression dec2 = model.floatVar();
    constraint(dec1 <= dec2);
    maximize(dec1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Unbounded);
}

BOOST_AUTO_TEST_CASE(LinearizationSimple1) {
    Model model;
    FloatExpression x1 = model.floatVar();
    FloatExpression x2 = model.floatVar();
    constraint(3 * x1 + x2 <= 4);
    constraint(2 * x1 + 3 * x2 <= 5);
    constraint(2 * x1 + 9 * x2 <= 8);
    maximize(x1 + 2 * x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x1.getValue(), 1.12, eps);
    BOOST_CHECK_CLOSE(x2.getValue(), 0.64, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationSimple2) {
    Model model;
    FloatExpression x1 = model.floatVar();
    FloatExpression x2 = model.floatVar();
    FloatExpression x3 = model.floatVar();
    FloatExpression x4 = model.floatVar();
    constraint(x1 <= 5);
    constraint(x2 <= 3);
    constraint(x3 <= 10);
    constraint(x4 < 50);
    constraint(x1 + 3 * x2 <= 3);
    constraint(x1 + 2 * x2 <= 4);
    constraint(x2 + x1 <= 3);
    constraint(x3 + x4 <= 2);
    constraint(x1 + x3 <= 5);
    constraint(x1 + x4 + 3 * x2 <= 4);
    maximize(x1 + x2 + 2 * x3 + x4);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x1.getValue(), -5, eps);
    BOOST_CHECK_CLOSE(x2.getValue(), 2.6666666, eps);
    BOOST_CHECK_CLOSE(x3.getValue(), 10, eps);
    BOOST_CHECK_CLOSE(x4.getValue(), -8, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LinearizationWithTimeLimit) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 5.0);
    maximize(dec1);
    model.setTimeLimit(2.0);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}
