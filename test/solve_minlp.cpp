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
    //BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(Square2) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize( (x + (-1)) * (x + (-1)) );
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    //BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(LowerBound) {
    Model model;
    FloatExpression x = model.floatVar(2.0, std::numeric_limits<double>::infinity());
    minimize(x*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 2.0, eps);
    //BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

BOOST_AUTO_TEST_CASE(UpperBound) {
    Model model;
    FloatExpression x = model.floatVar(-std::numeric_limits<double>::infinity(), -2.0);
    minimize(x*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), -2.0, eps);
    //BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

/*
BOOST_AUTO_TEST_CASE(Exp) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize(exp(x) + (-1)*x);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    //BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}
*/

