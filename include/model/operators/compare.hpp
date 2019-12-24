
#ifndef UMO_OPERATORS_COMPARE_HPP
#define UMO_OPERATORS_COMPARE_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <cassert>

namespace umoi {
namespace operators {

class CompareOperator : public BinaryOp, public OutBoolOp, public InFloatOp {
  public:
    bool compareEqual(double op1, double op2) const {
        double diff = std::abs(op1 - op2);
        if (diff <= absTol)
            return true;
        double magnitude = std::max(std::abs(op1), std::abs(op2));
        return diff <= relTol * magnitude;
    }

    bool compareLessThan(double op1, double op2) const {
        return op1 <= op2 && !compareEqual(op1, op2);
    }

    // TODO: make tolerance a runtime parameter
    const double absTol = 1.0e-6;
    const double relTol = 1.0e-8;
};

class Eq : public CompareOperator {
  public:
    std::string toString() const override {
        return "eq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareEqual(operands[0], operands[1]);
    }

    static Eq instance;
};

class Neq : public CompareOperator {
  public:
    std::string toString() const override {
        return "neq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return !compareEqual(operands[0], operands[1]);
    }

    static Neq instance;
};

class Leq : public CompareOperator {
  public:
    std::string toString() const override {
        return "leq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return !compareLessThan(operands[1], operands[0]);
    }

    static Leq instance;
};

class Geq : public CompareOperator {
  public:
    std::string toString() const override {
        return "geq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return !compareLessThan(operands[0], operands[1]);
    }

    static Geq instance;
};

class Lt : public CompareOperator {
  public:
    std::string toString() const override {
        return "lt";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareLessThan(operands[0], operands[1]);
    }

    static Lt instance;
};

class Gt : public CompareOperator {
  public:
    std::string toString() const override {
        return "gt";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareLessThan(operands[1], operands[0]);
    }

    static Gt instance;
};

}
}

#endif

