
#ifndef UMO_OPERATORS_COMPARE_HPP
#define UMO_OPERATORS_COMPARE_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <cassert>

namespace umoi {
namespace operators {

class CompareOp : public BinaryOp, public ComparisonOp, public InFloatOp {
};

class Eq final : public CompareOp {
  public:
    std::string toString() const override {
        return "eq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareEq(operands[0], operands[1]);
    }

    static Eq instance;
};

class Neq final : public CompareOp {
  public:
    std::string toString() const override {
        return "neq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareNeq(operands[0], operands[1]);
    }

    static Neq instance;
};

class Leq final : public CompareOp {
  public:
    std::string toString() const override {
        return "leq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareLeq(operands[0], operands[1]);
    }

    static Leq instance;
};

class Geq final : public CompareOp {
  public:
    std::string toString() const override {
        return "geq";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareGeq(operands[0], operands[1]);
    }

    static Geq instance;
};

class Lt final : public CompareOp {
  public:
    std::string toString() const override {
        return "lt";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareLt(operands[0], operands[1]);
    }

    static Lt instance;
};

class Gt final : public CompareOp {
  public:
    std::string toString() const override {
        return "gt";
    }

    double compute(int nbOperands, double *operands) const override {
        assert(nbOperands == 2);
        return compareGt(operands[0], operands[1]);
    }

    static Gt instance;
};

}
}

#endif

