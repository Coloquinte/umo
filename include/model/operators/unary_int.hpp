
#ifndef UMO_OPERATORS_UNARYINT_HPP
#define UMO_OPERATORS_UNARYINT_HPP

#include "model/operator.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class UnaryIntOperator : public Operator {
    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 1;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_INT;
    }
};

class Round : public UnaryFloatOperator {
  public:
    std::string toString() const override {
        return "round";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::round(operands[0]);
    }

    static Round instance;
};

class Floor : public UnaryFloatOperator {
  public:
    std::string toString() const override {
        return "floor";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::floor(operands[0]);
    }

    static Floor instance;
};

class Ceil : public UnaryFloatOperator {
  public:
    std::string toString() const override {
        return "ceil";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::ceil(operands[0]);
    }

    static Ceil instance;
};

class Sign : public UnaryFloatOperator {
  public:
    std::string toString() const override {
        return "sign";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return operands[0] >= 0.0 ? 1.0 : -1.0;
    }

    static Sign instance;
};

}
}

#endif

