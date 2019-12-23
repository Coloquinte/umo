
#ifndef UMO_OPERATORS_MISC_HPP
#define UMO_OPERATORS_MISC_HPP

#include "model/operator.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class Pow : public Operator {
  public:
    std::string toString() const override {
        return "pow";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_FLOAT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        return std::pow(operands[0], operands[1]);
    }

    static Pow instance;
};

class Logb : public Operator {
  public:
    std::string toString() const override {
        return "logb";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_FLOAT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        return std::log(operands[0]) / std::log(operands[1]);
    }

    static Logb instance;
};

class BinaryMinus : public Operator {
  public:
    std::string toString() const override {
        return "minus";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        if (operandTypes[0] == UMO_TYPE_FLOAT || operandTypes[1] == UMO_TYPE_FLOAT)
            return UMO_TYPE_FLOAT;
        return UMO_TYPE_INT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        return operands[0] - operands[1];
    }

    static BinaryMinus instance;
};

class Div : public Operator {
  public:
    std::string toString() const override {
        return "div";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_FLOAT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        return operands[0] / operands[1];
    }

    static Div instance;
};

class Idiv : public Operator {
  public:
    std::string toString() const override {
        return "idiv";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_INT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        long long n = operands[0];
        long long d = operands[1];
        return n / d;
    }

    static Idiv instance;
};

class Mod : public Operator {
  public:
    std::string toString() const override {
        return "mod";
    }

    bool validOperandCount(int nbOperands) const override {
        return nbOperands == 2;
    }

    umo_type resultType(int nbOperands, umo_type *operandTypes, umo_operator *operandOps) const override {
        return UMO_TYPE_INT;
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 2);
        long long n = operands[0];
        long long d = operands[1];
        return n % d;
    }

    static Mod instance;
};

}
}

#endif

