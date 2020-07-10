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

BOOST_AUTO_TEST_CASE(Small) {
    Model model;
    FloatExpression x = model.floatVar();
    minimize(x*x + (-2)*x + 1);
    model.solve();
    BOOST_CHECK_CLOSE(x.getValue(), 1, eps);
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Optimal);
}

