
#ifndef UMO_OPERATORS_DECISION_HPP
#define UMO_OPERATORS_DECISION_HPP

#include "model/operator.hpp"

#include <stdexcept>

namespace umoi {
namespace operators {

class DecBool : public Operator {
  public:
    std::string toString() const override {
        return "bool";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 0;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_BOOL;
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (bool) is not possible.");
    }

    static DecBool instance;
};

class DecInt : public Operator {
  public:
    std::string toString() const override {
        return "int";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    bool validOperandTypes(int nbOperands, umo_type *operandTypes) const override {
        return operandTypes[0] != UMO_TYPE_FLOAT
            && operandTypes[1] != UMO_TYPE_FLOAT;
    }

    bool validOperandOps(int nbOperands, umo_operator *operandOps) const override {
        return operandOps[0] == UMO_OP_CONSTANT
            && operandOps[1] == UMO_OP_CONSTANT;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_INT;
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (int) is not possible.");
    }

    static DecInt instance;
};

class DecFloat : public Operator {
  public:
    std::string toString() const override {
        return "float";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    bool validOperandOps(int nbOperands, umo_operator *operandOps) const override {
        return operandOps[0] == UMO_OP_CONSTANT
            && operandOps[1] == UMO_OP_CONSTANT;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_FLOAT;
    }

    double compute(int nbOperands, double *operands) const override {
        throw std::runtime_error("Computing a decision operator (float) is not possible.");
    }

    static DecFloat instance;
};

}
}

#endif

