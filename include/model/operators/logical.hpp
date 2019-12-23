
#ifndef UMO_OPERATORS_LOGICAL_HPP
#define UMO_OPERATORS_LOGICAL_HPP

#include "model/operator.hpp"

#include <stdexcept>
#include <cassert>

namespace umoi {
namespace operators {

class LogicalOperator : public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_BOOL;
    }
};

class Not : public LogicalOperator {
  public:
    std::string toString() const override {
        return "not";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 1;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return !operands[0];
    }

    static Not instance;
};

class And : public LogicalOperator {
  public:
    std::string toString() const override {
        return "and";
    }

    double compute(int nbOperands, double *operands) const override {
        bool result = true;
        for (int i = 0; i < nbOperands; ++i) {
            result &= (bool) operands[i];
        }
        return result;
    }

    static And instance;
};

class Or : public LogicalOperator {
  public:
    std::string toString() const override {
        return "or";
    }

    double compute(int nbOperands, double *operands) const override {
        bool result = false;
        for (int i = 0; i < nbOperands; ++i) {
            result |= (bool) operands[i];
        }
        return result;
    }

    static Or instance;
};

class Xor : public LogicalOperator {
  public:
    std::string toString() const override {
        return "xor";
    }

    double compute(int nbOperands, double *operands) const override {
        bool result = false;
        for (int i = 0; i < nbOperands; ++i) {
            result ^= (bool) operands[i];
        }
        return result;
    }

    static Xor instance;
};


}
}

#endif

