
#ifndef UMO_HPP
#define UMO_HPP

#include <string>
#include <vector>

extern "C" {
struct umo_model;
}

namespace umo {

class Model;
class FloatExpression;
class IntExpression;
class BoolExpression;

enum class Status { Infeasible, Invalid, Valid, Optimal };

class Model {
  public:
    Model();
    Model(Model &&);
    Model &operator=(Model &&);
    ~Model();

    FloatExpression constant(double);
    IntExpression constant(long long);
    BoolExpression constant(bool);

    FloatExpression floatVar();
    FloatExpression floatVar(double lb, double ub);
    IntExpression intVar();
    IntExpression intVar(long long lb, long long ub);
    BoolExpression boolVar();

    Status getStatus();

    void solve();
    void check();

    double getFloatParam(const std::string &param);
    void setFloatParam(const std::string &param, double val);
    std::string getStringParam(const std::string &param);
    void setStringParam(const std::string &param, const std::string &val);

    double getTimeLimit();
    void setTimeLimit(double limit);

    // N-ary operations
    FloatExpression sum(const std::vector<FloatExpression> &);
    FloatExpression prod(const std::vector<FloatExpression> &);
    FloatExpression min(const std::vector<FloatExpression> &);
    FloatExpression max(const std::vector<FloatExpression> &);

    IntExpression sum(const std::vector<IntExpression> &);
    IntExpression prod(const std::vector<IntExpression> &);
    IntExpression min(const std::vector<IntExpression> &);
    IntExpression max(const std::vector<IntExpression> &);

    IntExpression sum(const std::vector<BoolExpression> &);
    BoolExpression prod(const std::vector<BoolExpression> &);
    BoolExpression min(const std::vector<BoolExpression> &);
    BoolExpression max(const std::vector<BoolExpression> &);
    BoolExpression logical_or(const std::vector<BoolExpression> &);
    BoolExpression logical_and(const std::vector<BoolExpression> &);
    BoolExpression logical_xor(const std::vector<BoolExpression> &);

    umo_model *rawPtr() const { return ptr_; }

  private:
    // TODO: switch everything to a shared pointer
    struct umo_model *ptr_;
};

class Expression {
  public:
    umo_model *rawPtr() const { return model; }
    long long rawId() const { return v; }

    Expression(umo_model *model, long long v) : model(model), v(v) {}

  protected:
    umo_model *model;
    long long v;
};

class FloatExpression : public Expression {
  public:
    double getValue();
    void setValue(double val);

    FloatExpression(umo_model *model, long long v) : Expression(model, v) {}
};

class IntExpression : public FloatExpression {
  public:
    long long getValue();
    void setValue(long long val);

    IntExpression(umo_model *model, long long v) : FloatExpression(model, v) {}
};

class BoolExpression : public IntExpression {
  public:
    bool getValue();
    void setValue(bool val);

    BoolExpression(umo_model *model, long long v) : IntExpression(model, v) {}
};

void constraint(const BoolExpression &);
void minimize(const FloatExpression &);
void maximize(const FloatExpression &);

FloatExpression operator+(const FloatExpression &, const FloatExpression &);
FloatExpression operator+(double, const FloatExpression &);
FloatExpression operator+(const FloatExpression &, double);
FloatExpression operator-(const FloatExpression &, const FloatExpression &);
FloatExpression operator-(double, const FloatExpression &);
FloatExpression operator-(const FloatExpression &, double);
FloatExpression operator-(const FloatExpression &);
FloatExpression operator*(const FloatExpression &, const FloatExpression &);
FloatExpression operator*(double, const FloatExpression &);
FloatExpression operator*(const FloatExpression &, double);
FloatExpression operator/(const FloatExpression &, const FloatExpression &);
FloatExpression operator/(double, const FloatExpression &);
FloatExpression operator/(const FloatExpression &, double);
FloatExpression &operator+=(FloatExpression &, const FloatExpression &);
FloatExpression &operator-=(FloatExpression &, const FloatExpression &);
FloatExpression &operator*=(FloatExpression &, const FloatExpression &);
FloatExpression &operator/=(FloatExpression &, const FloatExpression &);
FloatExpression &operator+=(FloatExpression &, double);
FloatExpression &operator-=(FloatExpression &, double);
FloatExpression &operator*=(FloatExpression &, double);
FloatExpression &operator/=(FloatExpression &, double);

BoolExpression operator==(const FloatExpression &, const FloatExpression &);
BoolExpression operator==(double, const FloatExpression &);
BoolExpression operator==(const FloatExpression &, double);
BoolExpression operator!=(const FloatExpression &, const FloatExpression &);
BoolExpression operator!=(double, const FloatExpression &);
BoolExpression operator!=(const FloatExpression &, double);
BoolExpression operator<=(const FloatExpression &, const FloatExpression &);
BoolExpression operator<=(double, const FloatExpression &);
BoolExpression operator<=(const FloatExpression &, double);
BoolExpression operator>=(const FloatExpression &, const FloatExpression &);
BoolExpression operator>=(double, const FloatExpression &);
BoolExpression operator>=(const FloatExpression &, double);
BoolExpression operator<(const FloatExpression &, const FloatExpression &);
BoolExpression operator<(double, const FloatExpression &);
BoolExpression operator<(const FloatExpression &, double);
BoolExpression operator>(const FloatExpression &, const FloatExpression &);
BoolExpression operator>(double, const FloatExpression &);
BoolExpression operator>(const FloatExpression &, double);

IntExpression operator+(const IntExpression &, const IntExpression &);
IntExpression operator+(long long, const IntExpression &);
IntExpression operator+(const IntExpression &, long long);
IntExpression operator-(const IntExpression &, const IntExpression &);
IntExpression operator-(long long, const IntExpression &);
IntExpression operator-(const IntExpression &, long long);
IntExpression operator-(const IntExpression &);
IntExpression operator*(const IntExpression &, const IntExpression &);
IntExpression operator*(long long, const IntExpression &);
IntExpression operator*(const IntExpression &, long long);
IntExpression operator/(const IntExpression &, const IntExpression &);
IntExpression operator/(long long, const IntExpression &);
IntExpression operator/(const IntExpression &, long long);
IntExpression operator%(const IntExpression &, const IntExpression &);
IntExpression operator%(long long, const IntExpression &);
IntExpression operator%(const IntExpression &, long long);
IntExpression &operator+=(IntExpression &, const IntExpression &);
IntExpression &operator-=(IntExpression &, const IntExpression &);
IntExpression &operator*=(IntExpression &, const IntExpression &);
IntExpression &operator/=(IntExpression &, const IntExpression &);
IntExpression &operator%=(IntExpression &, const IntExpression &);
IntExpression &operator+=(IntExpression &, long long);
IntExpression &operator-=(IntExpression &, long long);
IntExpression &operator*=(IntExpression &, long long);
IntExpression &operator/=(IntExpression &, long long);
IntExpression &operator%=(IntExpression &, long long);

BoolExpression operator&&(const BoolExpression &, const BoolExpression &);
BoolExpression operator&&(bool, const BoolExpression &);
BoolExpression operator&&(const BoolExpression &, bool);
BoolExpression operator||(const BoolExpression &, const BoolExpression &);
BoolExpression operator||(bool, const BoolExpression &);
BoolExpression operator||(const BoolExpression &, bool);
BoolExpression operator^(const BoolExpression &, const BoolExpression &);
BoolExpression operator^(bool, const BoolExpression &);
BoolExpression operator^(const BoolExpression &, bool);
BoolExpression operator!(const BoolExpression &);
BoolExpression &operator&=(BoolExpression &, const BoolExpression &);
BoolExpression &operator|=(BoolExpression &, const BoolExpression &);
BoolExpression &operator^=(BoolExpression &, const BoolExpression &);
BoolExpression &operator&=(BoolExpression &, bool);
BoolExpression &operator|=(BoolExpression &, bool);
BoolExpression &operator^=(BoolExpression &, bool);

FloatExpression abs(const FloatExpression &);
FloatExpression min(const FloatExpression &, const FloatExpression &);
FloatExpression min(double, const FloatExpression &);
FloatExpression min(const FloatExpression &, double);
FloatExpression max(const FloatExpression &, const FloatExpression &);
FloatExpression max(double, const FloatExpression &);
FloatExpression max(const FloatExpression &, double);

IntExpression abs(const IntExpression &);
IntExpression min(const IntExpression &, const IntExpression &);
IntExpression min(long long, const IntExpression &);
IntExpression min(const IntExpression &, long long);
IntExpression max(const IntExpression &, const IntExpression &);
IntExpression max(long long, const IntExpression &);
IntExpression max(const IntExpression &, long long);

FloatExpression exp(const FloatExpression &);
FloatExpression log(const FloatExpression &);
FloatExpression log(const FloatExpression &, const FloatExpression &);
FloatExpression log(double, const FloatExpression &);
FloatExpression log(const FloatExpression &, double);
FloatExpression pow(const FloatExpression &, const FloatExpression &);
FloatExpression pow(double, const FloatExpression &);
FloatExpression pow(const FloatExpression &, double);
FloatExpression sqrt(const FloatExpression &);
FloatExpression square(const FloatExpression &);

IntExpression square(const IntExpression &);

IntExpression round(const FloatExpression &);
IntExpression floor(const FloatExpression &);
IntExpression ceil(const FloatExpression &);
IntExpression sign(const FloatExpression &);
FloatExpression frac(const FloatExpression &);

FloatExpression cos(const FloatExpression &);
FloatExpression sin(const FloatExpression &);
FloatExpression tan(const FloatExpression &);
FloatExpression cosh(const FloatExpression &);
FloatExpression sinh(const FloatExpression &);
FloatExpression tanh(const FloatExpression &);
FloatExpression acos(const FloatExpression &);
FloatExpression asin(const FloatExpression &);
FloatExpression atan(const FloatExpression &);
FloatExpression acosh(const FloatExpression &);
FloatExpression asinh(const FloatExpression &);
FloatExpression atanh(const FloatExpression &);

// N-ary operations
FloatExpression sum(const std::vector<FloatExpression> &);
FloatExpression prod(const std::vector<FloatExpression> &);
FloatExpression min(const std::vector<FloatExpression> &);
FloatExpression max(const std::vector<FloatExpression> &);

IntExpression sum(const std::vector<IntExpression> &);
IntExpression prod(const std::vector<IntExpression> &);
IntExpression min(const std::vector<IntExpression> &);
IntExpression max(const std::vector<IntExpression> &);

IntExpression sum(const std::vector<BoolExpression> &);
BoolExpression prod(const std::vector<BoolExpression> &);
BoolExpression min(const std::vector<BoolExpression> &);
BoolExpression max(const std::vector<BoolExpression> &);
BoolExpression logical_or(const std::vector<BoolExpression> &);
BoolExpression logical_and(const std::vector<BoolExpression> &);
BoolExpression logical_xor(const std::vector<BoolExpression> &);

} // namespace umo

#endif
