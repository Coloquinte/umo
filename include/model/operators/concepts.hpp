
#ifndef UMO_OPERATORS_CONCEPTS_HPP
#define UMO_OPERATORS_CONCEPTS_HPP

#include "model/operator.hpp"

#include <cassert>
#include <algorithm>

namespace umoi {
namespace operators {

class NoaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
        return nbOperands == 0;
    }
};

class UnaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
        return nbOperands == 1;
    }

    bool isUnary() const final override {
        return true;
    }
};

class BinaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
        return nbOperands == 2;
    }

    bool isBinary() const final override {
        return true;
    }
};

class NaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
        return true;
    }

    bool isNary() const final override {
        return true;
    }
};

class OutFloatOp : virtual public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const final override {
        return UMO_TYPE_FLOAT;
    }
};

class OutIntOp : virtual public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const final override {
        return UMO_TYPE_INT;
    }
};

class OutBoolOp : virtual public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const final override {
        return UMO_TYPE_BOOL;
    }
};

class OutInferIntOp : virtual public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const final override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] == UMO_TYPE_FLOAT) {
                return UMO_TYPE_FLOAT;
            }
        }
        return UMO_TYPE_INT;
    }
};

class OutInferIntBoolOp : virtual public Operator {
    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const final override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] == UMO_TYPE_FLOAT) {
                return UMO_TYPE_FLOAT;
            }
        }
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] == UMO_TYPE_INT) {
                return UMO_TYPE_INT;
            }
        }
        return UMO_TYPE_BOOL;
    }
};

class InFloatOp : virtual public Operator {
    bool validOperandTypes(int nbOperands, umo_type *operandTypes) const final override {
        return true;
    }
};

class InIntOp : virtual public Operator {
    bool validOperandTypes(int nbOperands, umo_type *operandTypes) const final override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] == UMO_TYPE_FLOAT) {
                return false;
            }
        }
        return true;
    }
};

class InBoolOp : virtual public Operator {
    bool validOperandTypes(int nbOperands, umo_type *operandTypes) const final override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandTypes[i] != UMO_TYPE_BOOL) {
                return false;
            }
        }
        return true;
    }
};

class ConstantInputOp : virtual public Operator {
    bool validOperandOps(int nbOperands, umo_operator *operandOps) const override {
        for (int i = 0; i < nbOperands; ++i) {
            if (operandOps[i] != UMO_OP_CONSTANT) return false;
        }
        return true;
    }
};

class LeafOp : virtual public Operator {
    bool isLeaf() const final override {
        return true;
    }
};

class DecisionOp : public LeafOp {
    bool isDecision() const final override {
        return true;
    }
};

class ComparisonOp : public OutBoolOp {
  public:
    bool compareEq(double op1, double op2) const {
        double diff = std::abs(op1 - op2);
        if (diff <= absTol)
            return true;
        double magnitude = std::max(std::abs(op1), std::abs(op2));
        return diff <= relTol * magnitude;
    }

    bool compareNeq(double op1, double op2) const {
        return !compareEq(op1, op2);
    }

    bool compareLt(double op1, double op2) const {
        return op1 <= op2 && !compareEq(op1, op2);
    }

    bool compareGt(double op1, double op2) const {
        return op1 >= op2 && !compareEq(op1, op2);
    }

    bool compareLeq(double op1, double op2) const {
        return op1 <= op2 || compareEq(op1, op2);
    }

    bool compareGeq(double op1, double op2) const {
        return op1 >= op2 || compareEq(op1, op2);
    }

    bool isComparison() const final override { return true; }

    // TODO: make tolerance a runtime parameter
    const double absTol = 1.0e-8;
    const double relTol = 1.0e-6;
};

class AssociativeOp : virtual public Operator {
    bool isAssociative() const final override { return true; }
};

class IdempotentOp : virtual public Operator {
    bool isIdempotent() const final override { return true; }
};
}
}

#endif

