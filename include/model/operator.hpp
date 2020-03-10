
#ifndef UMO_OPERATOR_HPP
#define UMO_OPERATOR_HPP

#include <iosfwd>

#include "api/umo_enums.h"

namespace umoi {

class Operator {
  public:
    static const Operator &get(umo_operator op);

    virtual std::string toString() const = 0;

    // Check validity of operand types
    virtual bool validOperands(int nbOperands, umo_type *operandTypes,
                               umo_operator *operandOps) const;
    virtual bool validOperandCount(int nbOperands) const { return true; }
    virtual bool validOperandTypes(int nbOperands,
                                   umo_type *operandTypes) const {
        return true;
    }
    virtual bool validOperandOps(int nbOperands,
                                 umo_operator *operandOps) const {
        return true;
    }

    // Infer result type
    virtual umo_type resultType(int nbOperands, umo_type *operandTypes,
                                umo_operator *operandOps) const;

    // Perform the computation
    virtual double compute(int nbOperands, double *operands) const = 0;

    // Is a leaf of the expression graph (constant/decision)
    virtual bool isLeaf() const { return false; }
    // Is a decision variable
    virtual bool isDecision() const { return false; }
    // Is a constant
    virtual bool isConstant() const { return false; }
    // Is an associative operation: can be flattened
    virtual bool isAssociative() const { return false; }
    // Is an idempotent operation: can remove duplicates
    virtual bool isIdempotent() const { return false; }
    // Is a comparison
    virtual bool isComparison() const { return false; }

    // TODO:
    // Bound computation
    // Differenciation
    // Direction information (relies on bounds)
    // Convexity information (relies on bounds)
};

std::ostream &operator<<(std::ostream &os, umo_operator op);

} // namespace umoi

#endif
