
#ifndef UMO_OPERATORS_UNARYFLOAT_HPP
#define UMO_OPERATORS_UNARYFLOAT_HPP

#include "model/operator.hpp"
#include "model/operators/concepts.hpp"

#include <cmath>

namespace umoi {
namespace operators {

class Abs final : public UnaryOp, public OutInferIntOp {
  public:
    std::string toString() const override {
        return "abs";
    }

    double compute(int nbOperands, double *operands) const override {
        return std::abs(operands[0]);
    }

    static Abs instance;
};

class Square final : public UnaryOp, public OutInferIntOp {
  public:
    std::string toString() const override {
        return "square";
    }

    double compute(int nbOperands, double *operands) const override {
        return operands[0] * operands[0];
    }

    static Square instance;
};

class UnaryMinus final : public UnaryOp, public OutInferIntOp {
  public:
    std::string toString() const override {
        return "uminus";
    }

    double compute(int nbOperands, double *operands) const override {
        return -operands[0];
    }

    static UnaryMinus instance;
};

class Exp final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "exp";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::exp(operands[0]);
    }

    static Exp instance;
};

class Log final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "log";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::log(operands[0]);
    }

    static Log instance;
};

class Sqrt final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "sqrt";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::sqrt(operands[0]);
    }

    static Sqrt instance;
};

class Inv final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "inv";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return 1.0 / operands[0];
    }

    static Inv instance;
};

class Frac final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "frac";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return operands[0] - std::floor(operands[0]);
    }

    static Frac instance;
};

class Cos final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "cos";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::cos(operands[0]);
    }

    static Cos instance;
};


class Sin final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "sin";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::sin(operands[0]);
    }

    static Sin instance;
};


class Tan final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "tan";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::tan(operands[0]);
    }

    static Tan instance;
};


class Acos final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "acos";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::acos(operands[0]);
    }

    static Acos instance;
};


class Asin final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "asin";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::asin(operands[0]);
    }

    static Asin instance;
};


class Atan final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "atan";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::atan(operands[0]);
    }

    static Atan instance;
};

class Cosh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "cosh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::cosh(operands[0]);
    }

    static Cosh instance;
};


class Sinh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "sinh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::sinh(operands[0]);
    }

    static Sinh instance;
};


class Tanh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "tanh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::tanh(operands[0]);
    }

    static Tanh instance;
};


class Acosh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "acosh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::acosh(operands[0]);
    }

    static Acosh instance;
};


class Asinh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "asinh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::asinh(operands[0]);
    }

    static Asinh instance;
};


class Atanh final : public UnaryOp, public OutFloatOp {
  public:
    std::string toString() const override {
        return "atanh";
    }

    double compute(int nbOperands, double *operands) const override {
        assert (nbOperands == 1);
        return std::atanh(operands[0]);
    }

    static Atanh instance;
};

}
}

#endif

