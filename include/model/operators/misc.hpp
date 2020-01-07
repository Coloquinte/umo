
#ifndef UMO_OPERATORS_MISC_HPP
#define UMO_OPERATORS_MISC_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class Pow final : public BinaryOp, public OutFloatOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "pow";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::pow(operands[0], operands[1]);
    }

    static Pow instance;
};

class Logb final : public BinaryOp, public OutFloatOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "logb";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::log(operands[0]) / std::log(operands[1]);
    }

    static Logb instance;
};

class BinaryMinus final : public BinaryOp, public OutInferIntOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "minus";
    }

    double compute(int nbOperands, double *operands) const override {
        return operands[0] - operands[1];
    }

    static BinaryMinus instance;
};

class Div final : public BinaryOp, public OutFloatOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "div";
    }

    double compute(int nbOperands, double *operands) const override {
        return operands[0] / operands[1];
    }

    static Div instance;
};

class Idiv final : public BinaryOp, public OutIntOp, public InIntOp {
  public:
    std::string toString() const override {
        return "idiv";
    }

    double compute(int nbOperands, double *operands) const override {
        long long n = operands[0];
        long long d = operands[1];
        if (d == 0) return NAN;
        return n / d;
    }

    static Idiv instance;
};

class Mod final : public BinaryOp, public OutIntOp, public InIntOp {
  public:
    std::string toString() const override {
        return "mod";
    }

    double compute(int nbOperands, double *operands) const override {
        long long n = operands[0];
        long long d = operands[1];
        if (d == 0) return NAN;
        return n % d;
    }

    static Mod instance;
};

}
}

#endif

