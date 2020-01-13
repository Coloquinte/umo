
#ifndef UMO_OPERATOR_HPP
#define UMO_OPERATOR_HPP

#include "api/umo_enums.h"

#include <iosfwd>

namespace umoi {

class Operator {
  public:
    static const Operator &get(umo_operator op);

    virtual std::string toString() const =0;

    // Check validity of operand types
    virtual bool validOperands(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const;
    virtual bool validOperandCount(int nbOperands) const { return true; }
    virtual bool validOperandTypes(int nbOperands, umo_type *operandTypes) const { return true; }
    virtual bool validOperandOps(int nbOperands, umo_operator *operandOps) const { return true; }

    // Infer result type
    virtual umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const;

    // Perform the computation
    virtual double compute(int nbOperands, double *operands) const =0;

    // Additional information
    virtual bool isLeaf() const { return false; }
    virtual bool isDecision() const { return false; }
};

std::ostream& operator<<(std::ostream &os, umo_operator op);

}

#endif

