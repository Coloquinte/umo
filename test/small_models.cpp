#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PRESOLVE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <vector>

using namespace umo;
using namespace std;

BOOST_AUTO_TEST_CASE(Model1) {
    Model model;
    FloatExpression fdec1 = model.floatVar(-10.0, 10.0);
    FloatExpression fdec2 = model.floatVar(0.0, 10.0);
    FloatExpression fdec3 = model.floatVar(0.0, 10.0);
    maximize(fdec1 + 3.0 * fdec2 + 2 * fdec3);
    constraint(fdec1 + fdec2 <= 4.0 && fdec1 - fdec3 >= fdec2 - 1);
    model.solve();
}

BOOST_AUTO_TEST_CASE(Model2) {
    Model model;
    FloatExpression fdec1 = model.floatVar(-10.0, 10.0);
    FloatExpression fdec2 = model.floatVar(0.0, 10.0);
    FloatExpression fdec3 = model.floatVar(1.0, 10.0);
    maximize(fdec1 + 3.0 * fdec2 + 2 / fdec3);
    constraint(fdec1 + fdec2 <= 4.0 && fdec1 / fdec3 >= 1.5 * fdec2);
    model.solve();
}

BOOST_AUTO_TEST_CASE(Model3) {
    Model model;
    FloatExpression fdec1 = model.floatVar(-10.0, 10.0);
    FloatExpression fdec2 = model.floatVar(0.0, 10.0);
    FloatExpression fdec3 = model.floatVar(1.0, 10.0);
    maximize(fdec1 - 3.0 * fdec2 + 2 / exp(fdec3));
    constraint(fdec1 + fdec2 <= 4.0 || fdec1 / fdec3 >= 1.5 * fdec2);
    constraint(cos(fdec1) >= -1.2);
    constraint(sin(fdec2) <= 2.0);
    model.solve();
}

BOOST_AUTO_TEST_CASE(Knapsack1) {
    Model model;
    FloatExpression weight = model.constant(0.0);
    FloatExpression value = model.constant(0.0);
    vector<BoolExpression> expressions;
    for (int i = 0; i < 100; ++i) {
        BoolExpression expr = model.boolVar();
        weight += i * expr;
        value += i * i * expr;
    }
    maximize(value);
    constraint(weight <= 1000.0);
}
