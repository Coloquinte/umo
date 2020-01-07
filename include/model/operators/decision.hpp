
#ifndef UMO_OPERATORS_DECISION_HPP
#define UMO_OPERATORS_DECISION_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <stdexcept>

namespace umoi {
namespace operators {

class Constant final : public NoaryOp, public LeafOp {
  public:
    std::string toString() const override {
        return "constant";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a constant is not possible.");
    }

    static Constant instance;
};

class DecBool final : public NoaryOp, public LeafOp, public OutBoolOp {
  public:
    std::string toString() const override {
        return "bool";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (bool) is not possible.");
    }

    static DecBool instance;
};

class DecInt final : public BinaryOp, public LeafOp, public OutIntOp, public InIntOp, public ConstantInputOp {
  public:
    std::string toString() const override {
        return "int";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (int) is not possible.");
    }

    static DecInt instance;
};

class DecFloat final : public BinaryOp, public LeafOp, public OutFloatOp, public InFloatOp, public ConstantInputOp {
  public:
    std::string toString() const override {
        return "float";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (float) is not possible.");
    }

    static DecFloat instance;
};

}
}

#endif

