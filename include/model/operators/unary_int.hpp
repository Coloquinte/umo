
#ifndef UMO_OPERATORS_UNARYINT_HPP
#define UMO_OPERATORS_UNARYINT_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class Round final : public UnaryOp, public OutIntOp {
  public:
    std::string toString() const override {
        return "round";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::round(operands[0]);
    }

    static Round instance;
};

class Floor final : public UnaryOp, public OutIntOp {
  public:
    std::string toString() const override {
        return "floor";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::floor(operands[0]);
    }

    static Floor instance;
};

class Ceil final : public UnaryOp, public OutIntOp {
  public:
    std::string toString() const override {
        return "ceil";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::ceil(operands[0]);
    }

    static Ceil instance;
};

class Sign final : public UnaryOp, public OutIntOp {
  public:
    std::string toString() const override {
        return "sign";
    }

    double compute(int nbOperands, double *operands) const override {
        return operands[0] >= 0.0 ? 1.0 : -1.0;
    }

    static Sign instance;
};

}
}

#endif

