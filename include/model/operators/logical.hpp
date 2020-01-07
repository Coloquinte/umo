
#ifndef UMO_OPERATORS_LOGICAL_HPP
#define UMO_OPERATORS_LOGICAL_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <stdexcept>
#include <cassert>

namespace umoi {
namespace operators {

class Not final : public UnaryOp, public OutBoolOp, public InBoolOp {
  public:
    std::string toString() const override {
        return "not";
    }

    double compute(int nbOperands, double *operands) const override {
        return !operands[0];
    }

    static Not instance;
};

class And final : public NaryOp, public OutBoolOp, public InBoolOp {
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

class Or final : public NaryOp, public OutBoolOp, public InBoolOp {
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

class Xor final : public NaryOp, public OutBoolOp, public InBoolOp {
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

