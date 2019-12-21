#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE C_API

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

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
}

