
#ifndef UMO_OPERATORS_NARY_HPP
#define UMO_OPERATORS_NARY_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <limits>

namespace umoi {
namespace operators {

class Sum final : public NaryOp, public OutInferIntOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "sum";
    }

    double compute(int nbOperands, double *operands) const override {
        double result = 0.0;
        for (int i = 0; i < nbOperands; ++i) {
            result += operands[i];
        }
        return result;
    }

    static Sum instance;
};

class Product final : public NaryOp, public OutInferIntBoolOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "product";
    }

    double compute(int nbOperands, double *operands) const override {
        double result = 1.0;
        for (int i = 0; i < nbOperands; ++i) {
            result *= operands[i];
        }
        return result;
    }

    static Product instance;
};

class Min final : public NaryOp, public OutInferIntBoolOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "min";
    }

    double compute(int nbOperands, double *operands) const override {
        double result = std::numeric_limits<double>::infinity();
        for (int i = 0; i < nbOperands; ++i) {
            result = std::min(result, operands[i]);
        }
        return result;
    }

    static Min instance;
};

class Max final : public NaryOp, public OutInferIntBoolOp, public InFloatOp {
  public:
    std::string toString() const override {
        return "max";
    }

    double compute(int nbOperands, double *operands) const override {
        double result = -std::numeric_limits<double>::infinity();
        for (int i = 0; i < nbOperands; ++i) {
            result = std::max(result, operands[i]);
        }
        return result;
    }

    static Max instance;
};

}
}

#endif

