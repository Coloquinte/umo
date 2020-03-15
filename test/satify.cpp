#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

using namespace umo;

BOOST_AUTO_TEST_CASE(SatifyBoolDec) {
    Model model;
    model.boolVar();
    model.boolVar();
    model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedAnd) {
    Model model;
    constraint(model.boolVar() && model.boolVar());
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedOr) {
    Model model;
    constraint(model.boolVar() || model.boolVar());
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedAndNot) {
    Model model;
    constraint(!model.boolVar() && !model.boolVar());
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedOrNot) {
    Model model;
    constraint(!model.boolVar() || !model.boolVar());
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedNand) {
    Model model;
    constraint(!(model.boolVar() && model.boolVar()));
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedNor) {
    Model model;
    constraint(!(model.boolVar() || model.boolVar()));
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyAnd) {
    Model model;
    model.boolVar() && model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyAndNot) {
    Model model;
    !model.boolVar() && !model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyOr) {
    Model model;
    model.boolVar() || model.boolVar();
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyOrNot) {
    Model model;
    !model.boolVar() || !model.boolVar();
    model.solve();
}

