
#ifndef UMO_OPERATORS_NARY_HPP
#define UMO_OPERATORS_NARY_HPP

#include "model/operator.hpp"

#include <limits>

namespace umoi {
namespace operators {

class NaryOperator : public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] == UMO_TYPE_FLOAT) return UMO_TYPE_FLOAT;
        }
        // TODO: min/max should have different behaviour for the zero operand case
        return UMO_TYPE_INT;
    }
};

class Sum : public NaryOperator {
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

class Product : public NaryOperator {
  public:
    std::string toString() const override {
        return "product";
    }

    double compute(int nbOperands, double *operands) const override {
        double result = 0.0;
        for (int i = 0; i < nbOperands; ++i) {
            result *= operands[i];
        }
        return result;
    }

    static Product instance;
};

class Min : public NaryOperator {
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

class Max : public NaryOperator {
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

