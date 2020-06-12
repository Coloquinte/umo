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

/*
BOOST_AUTO_TEST_CASE(SatifyBoolDec) {
    Model model;
    model.boolVar();
    model.boolVar();
    model.boolVar();
    BOOST_CHECK(model.getStatus() == Status::Valid);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}
*/

BOOST_AUTO_TEST_CASE(SatifyVarConstrainedPos) {
    Model model;
    BoolExpression x1 = model.boolVar();
    constraint(x1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x1.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyVarConstrainedNeg) {
    Model model;
    BoolExpression x1 = model.boolVar();
    constraint(!x1);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!x1.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedAnd) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(x1 && x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x1.getValue() && x2.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedOr) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(x1 || x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x1.getValue() || x2.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedXor) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(x1 ^ x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedAndNot) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(!x1 && !x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!x1.getValue() && !x2.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedOrNot) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(!x1 || !x2);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!x1.getValue() || !x2.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedNand) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(!(x1 && x2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!(x1.getValue() && x2.getValue()));
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedNor) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(!(x1 || x2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(!(x1.getValue() || x2.getValue()));
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyConstrainedXnor) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(!(x1 ^ x2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    BOOST_CHECK_THROW(model.solve(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(SatifyAnd) {
    Model model;
    model.boolVar() && model.boolVar();
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
}

BOOST_AUTO_TEST_CASE(SatifyAndNot) {
    Model model;
    !model.boolVar() && !model.boolVar();
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyOr) {
    Model model;
    model.boolVar() || model.boolVar();
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyOrNot) {
    Model model;
    !model.boolVar() || !model.boolVar();
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyAndOr) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    BoolExpression x3 = model.boolVar();
    BoolExpression x4 = model.boolVar();
    constraint((x1 || x2) && (x3 || x4));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK((x1.getValue() || x2.getValue()) && (x3.getValue() || x4.getValue()));
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyOrAnd) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    BoolExpression x3 = model.boolVar();
    BoolExpression x4 = model.boolVar();
    constraint((x1 && x2) || (x3 && x4));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK((x1.getValue() && x2.getValue()) || (x3.getValue() && x4.getValue()));
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

BOOST_AUTO_TEST_CASE(SatifyBigOr) {
    int nVars = 1000;
    Model model;
    std::vector<BoolExpression> vec;
    for (int i = 0; i < nVars; ++i) {
        vec.push_back(model.boolVar());
    }
    constraint(umo::logical_or(vec));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
    int cnt = 0;
    for (BoolExpression x : vec) {
        cnt += x.getValue();
    }
    BOOST_CHECK(cnt >= 1);
}

BOOST_AUTO_TEST_CASE(SatifyBigAnd) {
    int nVars = 1000;
    Model model;
    std::vector<BoolExpression> vec;
    for (int i = 0; i < nVars; ++i) {
        vec.push_back(model.boolVar());
    }
    constraint(umo::logical_and(vec));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
    int cnt = 0;
    for (BoolExpression x : vec) {
        cnt += x.getValue();
    }
    BOOST_CHECK_EQUAL(cnt, nVars);
}

BOOST_AUTO_TEST_CASE(SatifyBigNor) {
    int nVars = 1000;
    Model model;
    std::vector<BoolExpression> vec;
    for (int i = 0; i < nVars; ++i) {
        vec.push_back(model.boolVar());
    }
    constraint(!umo::logical_or(vec));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
    int cnt = 0;
    for (BoolExpression x : vec) {
        cnt += x.getValue();
    }
    BOOST_CHECK_EQUAL(cnt, 0);
}

BOOST_AUTO_TEST_CASE(SatifyBigNand) {
    int nVars = 1000;
    Model model;
    std::vector<BoolExpression> vec;
    for (int i = 0; i < nVars; ++i) {
        vec.push_back(model.boolVar());
    }
    constraint(!umo::logical_and(vec));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(model.getStatus() == Status::Optimal);
    int cnt = 0;
    for (BoolExpression x : vec) {
        cnt += x.getValue();
    }
    BOOST_CHECK(cnt < nVars);
}

BOOST_AUTO_TEST_CASE(SatifyUnsat) {
    Model model;
    BoolExpression x1 = model.boolVar();
    BoolExpression x2 = model.boolVar();
    constraint(x1 && x2);
    constraint(!(x1 && x2));
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK_EQUAL(model.getStatus(), Status::Infeasible);
}

BOOST_AUTO_TEST_CASE(SatifyWithTimeLimit) {
    Model model;
    BoolExpression x1 = model.boolVar();
    constraint(x1);
    model.setTimeLimit(2.0);
    model.setSolver(TOSTRING(SOLVER_PARAM));
    model.solve();
    BOOST_CHECK(x1.getValue());
    BOOST_CHECK(model.getStatus() == Status::Optimal);
}

