
#ifndef UMO_OPERATORS_UNARY_HPP
#define UMO_OPERATORS_UNARY_HPP

#include "model/operator.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class UnaryOperator : public Operator {
    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 1;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        assert (nbOperands == 1);
        return operandTypes[0] == UMO_TYPE_FLOAT ? UMO_TYPE_FLOAT : UMO_TYPE_INT;
    }
};

class Abs : public UnaryOperator {
  public:
    std::string toString() const override {
        return "abs";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::abs(operands[0]);
    }

    static Abs instance;
};

class Square : public UnaryOperator {
  public:
    std::string toString() const override {
        return "square";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return operands[0] * operands[0];
    }

    static Square instance;
};

class UnaryMinus : public UnaryOperator {
  public:
    std::string toString() const override {
        return "uminus";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return -operands[0];
    }

    static UnaryMinus instance;
};

}
}

#endif

