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

