
#ifndef UMO_OPERATORS_CONCEPTS_HPP
#define UMO_OPERATORS_CONCEPTS_HPP

#include "model/operator.hpp"

#include <cassert>

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
};

class BinaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
        return nbOperands == 2;
    }
};

class NaryOp : virtual public Operator {
    bool validOperandCount(int nbOperands) const final override {
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
        return operandOps[0] == UMO_OP_CONSTANT
            && operandOps[1] == UMO_OP_CONSTANT;
    }
};

class LeafOp : virtual public Operator {
    bool isLeaf() const final override {
        return true;
    }
};

}
}

#endif

