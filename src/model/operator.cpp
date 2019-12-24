
#include "model/operator.hpp"
#include "model/operators/decision.hpp"
#include "model/operators/logical.hpp"
#include "model/operators/nary.hpp"
#include "model/operators/compare.hpp"
#include "model/operators/unary_float.hpp"
#include "model/operators/unary_int.hpp"
#include "model/operators/misc.hpp"

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
        case UMO_OP_ROUND:
            return Round::instance;
        case UMO_OP_FLOOR:
            return Floor::instance;
        case UMO_OP_CEIL:
            return Ceil::instance;
        case UMO_OP_SIGN:
            return Sign::instance;
        case UMO_OP_ABS:
            return Abs::instance;
        case UMO_OP_SQUARE:
            return Square::instance;
        case UMO_OP_MINUS_UNARY:
            return UnaryMinus::instance;
        case UMO_OP_POW:
            return Pow::instance;
        case UMO_OP_LOGB:
            return Logb::instance;
        case UMO_OP_MINUS_BINARY:
            return BinaryMinus::instance;
        case UMO_OP_DIV:
            return Div::instance;
        case UMO_OP_IDIV:
            return Idiv::instance;
        case UMO_OP_MOD:
            return Mod::instance;
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

DecBool DecBool::instance;
DecInt DecInt::instance;
DecFloat DecFloat::instance;
Not Not::instance;
Or Or::instance;
And And::instance;
Xor Xor::instance;
Sum Sum::instance;
Product Product::instance;
Min Min::instance;
Max Max::instance;
Eq Eq::instance;
Neq Neq::instance;
Leq Leq::instance;
Geq Geq::instance;
Lt Lt::instance;
Gt Gt::instance;
Exp Exp::instance;
Log Log::instance;
Sqrt Sqrt::instance;
Inv Inv::instance;
Frac Frac::instance;
Cos Cos::instance;
Sin Sin::instance;
Tan Tan::instance;
Acos Acos::instance;
Asin Asin::instance;
Atan Atan::instance;
Cosh Cosh::instance;
Sinh Sinh::instance;
Tanh Tanh::instance;
Acosh Acosh::instance;
Asinh Asinh::instance;
Atanh Atanh::instance;
Round Round::instance;
Floor Floor::instance;
Ceil Ceil::instance;
Sign Sign::instance;
Abs Abs::instance;
Square Square::instance;
UnaryMinus UnaryMinus::instance;
Pow Pow::instance;
Logb Logb::instance;
BinaryMinus BinaryMinus::instance;
Div Div::instance;
Idiv Idiv::instance;
Mod Mod::instance;

}

