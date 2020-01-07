
#ifndef UMO_OPERATORS_LINEAR_HPP
#define UMO_OPERATORS_LINEAR_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

namespace umoi {
namespace operators {

class Linear final : public OutFloatOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "linear";
    }

    bool validOperandCount(int nbOperands) const final override {
        return (nbOperands % 2) == 0;
    }

    bool validOperandOps(int nbOperands, umo_operator *operandOps) const override {
        for (int i = 0; 2*i < nbOperands; ++i) {
            if (operandOps[2*i] != UMO_OP_CONSTANT) return false;
        }
        return true;
    }

    double compute(int nbOperands, double *operands) const override {
        double ret = 0.0;
        for (int i = 0; 2*i < nbOperands; ++i) {
            ret += operands[2*i] * operands[2*i+1];
        }
        return ret;
    }

    static Linear instance;
};

class LinearComp final : public ComparisonOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "linComp";
    }

    bool validOperandCount(int nbOperands) const final override {
        return (nbOperands % 2) == 0 && nbOperands >= 4;
    }

    bool validOperandOps(int nbOperands, umo_operator *operandOps) const override {
        for (int i = 1; 2*i < nbOperands; ++i) {
            if (operandOps[2*i] != UMO_OP_CONSTANT) return false;
        }
        return true;
    }

    double compute(int nbOperands, double *operands) const override {
        if (nbOperands == 2) return compareLeq(operands[0], operands[1]);
        double val = 0.0;
        for (int i = 1; 2*i < nbOperands; ++i) {
            val += operands[2*i] * operands[2*i+1];
        }
        return compareLeq(operands[0], val) && compareLeq(val, operands[1]);
    }

    static LinearComp instance;
};

}
}

#endif

