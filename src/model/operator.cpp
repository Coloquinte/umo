
#include "model/operator.hpp"
#include "model/operators/decision.hpp"
#include "model/operators/logical.hpp"
#include "model/operators/nary.hpp"
#include "model/operators/compare.hpp"
#include "model/operators/unary_float.hpp"

#include <stdexcept>
#include <ostream>

namespace umoi {

using namespace operators;

std::ostream& operator<<(std::ostream &os, umo_operator op) {
    return os << Operator::get(op).toString();
}

const Operator &Operator::get(umo_operator op) {
    if (op >= UMO_OP_END)
        throw std::runtime_error("Operator identifier is out of range");
    switch(op) {
        case UMO_OP_INVALID:
            throw std::runtime_error("Attempting to use the invalid operator");
        case UMO_OP_DEC_BOOL:
            return DecBool::instance;
        case UMO_OP_DEC_INT:
            return DecInt::instance;
        case UMO_OP_DEC_FLOAT:
            return DecFloat::instance;
        case UMO_OP_NOT:
            return Not::instance;
        case UMO_OP_OR:
            return Or::instance;
        case UMO_OP_AND:
            return And::instance;
        case UMO_OP_XOR:
            return Xor::instance;
        case UMO_OP_SUM:
            return Sum::instance;
        case UMO_OP_PROD:
            return Product::instance;
        case UMO_OP_MIN:
            return Min::instance;
        case UMO_OP_MAX:
            return Max::instance;
        case UMO_OP_CMP_EQ:
            return Eq::instance;
        case UMO_OP_CMP_NEQ:
            return Neq::instance;
        case UMO_OP_CMP_LEQ:
            return Leq::instance;
        case UMO_OP_CMP_GEQ:
            return Geq::instance;
        case UMO_OP_CMP_LT:
            return Lt::instance;
        case UMO_OP_CMP_GT:
            return Gt::instance;
        case UMO_OP_EXP:
            return Exp::instance;
        case UMO_OP_LOG:
            return Log::instance;
        case UMO_OP_SQRT:
            return Sqrt::instance;
        case UMO_OP_INV:
            return Inv::instance;
        case UMO_OP_FRAC:
            return Frac::instance;
        case UMO_OP_COS:
            return Cos::instance;
        case UMO_OP_SIN:
            return Sin::instance;
        case UMO_OP_TAN:
            return Tan::instance;
        case UMO_OP_ACOS:
            return Acos::instance;
        case UMO_OP_ASIN:
            return Asin::instance;
        case UMO_OP_ATAN:
            return Atan::instance;
        case UMO_OP_COSH:
            return Cosh::instance;
        case UMO_OP_SINH:
            return Sinh::instance;
        case UMO_OP_TANH:
            return Tanh::instance;
        case UMO_OP_ACOSH:
            return Acosh::instance;
        case UMO_OP_ASINH:
            return Asinh::instance;
        case UMO_OP_ATANH:
            return Atanh::instance;
        default:
            throw std::runtime_error("Operator is not implemented.");
    }
}

bool Operator::validOperands(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const {
    return validOperandCount(nbOperands)
        && validOperandTypes(nbOperands, operandTypes)
        && validOperandOps(nbOperands, operandOps);
}

umo_type Operator::resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const {
    return UMO_TYPE_FLOAT;
}

}

