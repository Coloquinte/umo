
#ifndef UMO_OPERATORS_DECISION_HPP
#define UMO_OPERATORS_DECISION_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <stdexcept>

namespace umoi {
namespace operators {

class DecBool : public NoaryOp, public OutBoolOp {
  public:
    std::string toString() const override {
        return "bool";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (bool) is not possible.");
    }

    static DecBool instance;
};

class DecInt : public BinaryOp, public OutIntOp, public InIntOp, public ConstantInputOp {
  public:
    std::string toString() const override {
        return "int";
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (int) is not possible.");
    }

    static DecInt instance;
};

class DecFloat : public BinaryOp, public OutFloatOp, public InFloatOp, public ConstantInputOp {
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

