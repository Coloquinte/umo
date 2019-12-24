#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE OPERATORS

#include <boost/test/unit_test.hpp>

#include "api/umo_enums.h"
#include "model/operator.hpp"

#include <cmath>

using namespace umoi;

BOOST_AUTO_TEST_CASE(Basic) {
    const Operator &op = Operator::get(UMO_OP_DIV);
    umo_type operandTypes[2] = {UMO_TYPE_INT, UMO_TYPE_INT};
    umo_operator operandOps[2] = {UMO_OP_SUM, UMO_OP_PROD};
    BOOST_CHECK(op.validOperands(2, operandTypes, operandOps));
    BOOST_CHECK(!op.validOperands(1, operandTypes, operandOps));
}
