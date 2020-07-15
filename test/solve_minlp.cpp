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

const double eps = 0.1;

BOOST_AUTO_TEST_CASE(Square1) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize(x*x + (-2)*x + 1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Square2) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize( (x + (-1)) * (x + (-1)) );
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Square3) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize(x*x + 2*x + 1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), -1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Square4) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize( (x + 1) * (x + 1) );
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), -1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LowerBound) {
    Model model;
    FloatExpression x = model.floatVar(2.0, umo::inf());
    minimize(x*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(UpperBound) {
    Model model;
    FloatExpression x = model.floatVar(-umo::inf(), -2.0);
    minimize(x*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), -2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(FixedVar) {
    Model model;
    FloatExpression x = model.floatVar(2.0, 2.0);
    minimize(x*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Exp) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize(umo::exp(x) + (-1)*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Log) {
    Model model;
    FloatExpression x = model.floatVar(1.0e-5, 1.0e5);
    maximize(umo::log(x) + (-1)*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Cos1) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    minimize(umo::cos(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Cos2) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    maximize(umo::cos(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Sin1) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    minimize(umo::sin(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Sin2) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    maximize(umo::sin(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Tan1) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    minimize(umo::tan(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 0, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Tan2) {
    Model model;
    FloatExpression x = model.floatVar(0, 1);
    maximize(umo::tan(x));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Bool1) {
    Model model;
    BoolExpression x = model.boolVar();
    minimize(x*x + (-1.6)*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x.getValue());
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Int1) {
    Model model;
    IntExpression x = model.intVar();
    minimize(x*x + (-3.8)*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(x.getValue(), 2);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Bounds1) {
    Model model;
    BoolExpression b1 = model.boolVar();
    BoolExpression b2 = model.boolVar();
    IntExpression i1 = model.intVar(-9, 9);
    IntExpression i2 = model.intVar(-8, 8);
    IntExpression i3 = model.intVar(-7, 7);
    IntExpression i4 = model.intVar(-6, 6);
    FloatExpression f1 = model.floatVar(-5, 5);
    FloatExpression f2 = model.floatVar(-4, 4);
    FloatExpression f3 = model.floatVar(-3, 3);
    FloatExpression f4 = model.floatVar(-2, 2);
    maximize(i1 + i2 + i3 + i4 + f1 + f2 + f3 + f4 + b1 + b2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(i1.getValue(), 9);
    BOOST_CHECK_EQUAL(i2.getValue(), 8);
    BOOST_CHECK_EQUAL(i3.getValue(), 7);
    BOOST_CHECK_EQUAL(i4.getValue(), 6);
    BOOST_CHECK_CLOSE(f1.getValue(), 5.0, eps);
    BOOST_CHECK_CLOSE(f2.getValue(), 4.0, eps);
    BOOST_CHECK_CLOSE(f3.getValue(), 3.0, eps);
    BOOST_CHECK_CLOSE(f4.getValue(), 2.0, eps);
    BOOST_CHECK(b1.getValue());
    BOOST_CHECK(b2.getValue());
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}
