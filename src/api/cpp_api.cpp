
#include <cassert>
#include <cstdlib>
#include <limits>
#include <ostream>
#include <stdexcept>

#include "api/umo.h"
#include "api/umo.hpp"

using namespace std;

// TODO: make it correct if runtime_error's constructor throws (possible if OOM)
#define UNWRAP_EXCEPTIONS(code)                                                \
    do {                                                                       \
        const char *err = NULL;                                                \
        code if (err != NULL) {                                                \
            std::runtime_error exc(err);                                       \
            free((char *)err);                                                 \
            throw exc;                                                         \
        }                                                                      \
    } while (0)

namespace umo {

long long makeConstant(umo_model *model, double val) {
    long long tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_create_constant(model, val, &err););
    return tmp;
}

long long makeNoaryOp(umo_model *model, umo_operator op) {
    long long tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_create_expression(model, op, 0, NULL, &err););
    return tmp;
}

long long makeUnaryOp(umo_model *model, umo_operator op, long long op1) {
    long long tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_create_expression(model, op, 1, &op1, &err););
    return tmp;
}

long long makeBinaryOp(umo_model *model, umo_operator op, long long op1,
                       long long op2) {
    long long operands[2] = {op1, op2};
    long long tmp;
    UNWRAP_EXCEPTIONS(tmp =
                          umo_create_expression(model, op, 2, operands, &err););
    return tmp;
}

long long makeTernaryOp(umo_model *model, umo_operator op, long long op1,
                        long long op2, long long op3) {
    long long operands[3] = {op1, op2, op3};
    long long tmp;
    UNWRAP_EXCEPTIONS(tmp =
                          umo_create_expression(model, op, 3, operands, &err););
    return tmp;
}

long long makeNaryOp(umo_model *model, umo_operator op, int nbOperands,
                     long long *operands) {
    long long tmp;
    UNWRAP_EXCEPTIONS(
        tmp = umo_create_expression(model, op, nbOperands, operands, &err););
    return tmp;
}

template <typename OperandExpression, typename ResultExpression>
ResultExpression unaryOp(umo_operator op, const OperandExpression &op1) {
    long long v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1,
                          const FloatExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, double op1Val,
                          const FloatExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1,
                          double op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1,
                          const IntExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, long long op1Val,
                          const IntExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double)op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1,
                          long long op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double)op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1,
                          const BoolExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, bool op1Val,
                          const BoolExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double)op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1,
                          bool op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double)op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

Model::Model() { UNWRAP_EXCEPTIONS(ptr_ = umo_create_model(&err);); }

Model::Model(Model &&o) { ptr_ = o.ptr_; }

Model &Model::operator=(Model &&o) {
    ptr_ = o.ptr_;
    return *this;
}

Model::~Model() {
    const char *err = NULL;
    umo_destroy_model(ptr_, &err);
    assert(err == NULL); // Destructor should never throw
}

FloatExpression Model::constant(double val) {
    long long v = makeConstant(ptr_, (double)val);
    return FloatExpression(ptr_, v);
}

IntExpression Model::constant(long long val) {
    long long v = makeConstant(ptr_, (double) val);
    return IntExpression(ptr_, v);
}

BoolExpression Model::constant(bool val) {
    long long v = makeConstant(ptr_, (double)val);
    return BoolExpression(ptr_, v);
}

FloatExpression Model::floatVar() {
    FloatExpression lbe = constant(-numeric_limits<double>::infinity());
    FloatExpression ube = constant(numeric_limits<double>::infinity());
    return binaryOp<FloatExpression>(UMO_OP_DEC_FLOAT, lbe, ube);
}

FloatExpression Model::floatVar(double lb, double ub) {
    FloatExpression lbe = constant(lb);
    FloatExpression ube = constant(ub);
    return binaryOp<FloatExpression>(UMO_OP_DEC_FLOAT, lbe, ube);
}

FloatExpression Model::floatVar(UnboundedT, double ub) {
    FloatExpression lbe = constant(-numeric_limits<double>::infinity());
    FloatExpression ube = constant(ub);
    return binaryOp<FloatExpression>(UMO_OP_DEC_FLOAT, lbe, ube);
}

FloatExpression Model::floatVar(double lb, UnboundedT) {
    FloatExpression lbe = constant(lb);
    FloatExpression ube = constant(numeric_limits<double>::infinity());
    return binaryOp<FloatExpression>(UMO_OP_DEC_FLOAT, lbe, ube);
}

FloatExpression Model::floatVar(UnboundedT, UnboundedT) {
    return floatVar();
}

IntExpression Model::intVar() {
    FloatExpression lbe = constant(-numeric_limits<double>::infinity());
    FloatExpression ube = constant(numeric_limits<double>::infinity());
    return binaryOp<IntExpression>(UMO_OP_DEC_INT, lbe, ube);
}

IntExpression Model::intVar(long long lb, long long ub) {
    IntExpression lbe = constant(lb);
    IntExpression ube = constant(ub);
    return binaryOp<IntExpression>(UMO_OP_DEC_INT, lbe, ube);
}

IntExpression Model::intVar(UnboundedT, long long ub) {
    FloatExpression lbe = constant(-numeric_limits<double>::infinity());
    IntExpression ube = constant(ub);
    return binaryOp<IntExpression>(UMO_OP_DEC_INT, lbe, ube);
}

IntExpression Model::intVar(long long lb, UnboundedT) {
    IntExpression lbe = constant(lb);
    FloatExpression ube = constant(numeric_limits<double>::infinity());
    return binaryOp<IntExpression>(UMO_OP_DEC_INT, lbe, ube);
}

IntExpression Model::intVar(UnboundedT, UnboundedT) {
    return intVar();
}

BoolExpression Model::boolVar() {
    long long v = makeNoaryOp(ptr_, UMO_OP_DEC_BOOL);
    return BoolExpression(ptr_, v);
}

Status Model::getStatus() {
    umo_solution_status ret;
    UNWRAP_EXCEPTIONS(ret = umo_get_solution_status(ptr_, &err););
    switch (ret) {
    case UMO_STATUS_INFEASIBLE:
        return Status::Infeasible;
    case UMO_STATUS_INVALID:
        return Status::Invalid;
    case UMO_STATUS_VALID:
        return Status::Valid;
    case UMO_STATUS_OPTIMAL:
        return Status::Optimal;
    case UMO_STATUS_UNBOUNDED:
        return Status::Unbounded;
    case UMO_STATUS_UNKNOWN:
        return Status::Unknown;
    default:
        throw std::runtime_error("Unknown status returned");
    }
}

void Model::solve() { UNWRAP_EXCEPTIONS(umo_solve(ptr_, &err);); }

void Model::check() { UNWRAP_EXCEPTIONS(umo_check(ptr_, &err);); }

double Model::getFloatParam(const std::string &param) {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp =
                          umo_get_float_parameter(ptr_, param.c_str(), &err););
    return tmp;
}

void Model::setFloatParam(const std::string &param, double val) {
    UNWRAP_EXCEPTIONS(umo_set_float_parameter(ptr_, param.c_str(), val, &err););
}

double Model::getTimeLimit() { return getFloatParam("time_limit"); }

void Model::setTimeLimit(double limit) { setFloatParam("time_limit", limit); }

std::string Model::getSolver() { return getStringParam("solver"); }

void Model::setSolver(const std::string &solver) {
    setStringParam("solver", solver);
}

std::string Model::getStringParam(const std::string &param) {
    const char *tmp;
    UNWRAP_EXCEPTIONS(tmp =
                          umo_get_string_parameter(ptr_, param.c_str(), &err););
    return tmp;
}

void Model::setStringParam(const std::string &param, const std::string &val) {
    UNWRAP_EXCEPTIONS(
        umo_set_string_parameter(ptr_, param.c_str(), val.c_str(), &err););
}

double FloatExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(model, v, &err););
    return tmp;
}

long long IntExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(model, v, &err););
    return (long long)tmp;
}

bool BoolExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(model, v, &err););
    return (bool)tmp;
}

void FloatExpression::setValue(double val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(model, v, val, &err););
}

void IntExpression::setValue(long long val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(model, v, (double)val, &err););
}

void BoolExpression::setValue(bool val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(model, v, (double)val, &err););
}

void constraint(const BoolExpression &c) {
    UNWRAP_EXCEPTIONS(umo_create_constraint(c.rawPtr(), c.rawId(), &err););
}

void minimize(const FloatExpression &o) {
    UNWRAP_EXCEPTIONS(
        umo_create_objective(o.rawPtr(), o.rawId(), UMO_OBJ_MINIMIZE, &err););
}

void maximize(const FloatExpression &o) {
    UNWRAP_EXCEPTIONS(
        umo_create_objective(o.rawPtr(), o.rawId(), UMO_OBJ_MAXIMIZE, &err););
}

FloatExpression operator+(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_MINUS_UNARY, op1);
}

FloatExpression operator*(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

FloatExpression &operator+=(FloatExpression &op1, const FloatExpression &op2) {
    op1 = op1 + op2;
    return op1;
}

FloatExpression &operator-=(FloatExpression &op1, const FloatExpression &op2) {
    op1 = op1 - op2;
    return op1;
}

FloatExpression &operator*=(FloatExpression &op1, const FloatExpression &op2) {
    op1 = op1 * op2;
    return op1;
}

FloatExpression &operator/=(FloatExpression &op1, const FloatExpression &op2) {
    op1 = op1 / op2;
    return op1;
}

FloatExpression &operator+=(FloatExpression &op1, double op2) {
    op1 = op1 + op2;
    return op1;
}

FloatExpression &operator-=(FloatExpression &op1, double op2) {
    op1 = op1 - op2;
    return op1;
}

FloatExpression &operator*=(FloatExpression &op1, double op2) {
    op1 = op1 * op2;
    return op1;
}

FloatExpression &operator/=(FloatExpression &op1, double op2) {
    op1 = op1 / op2;
    return op1;
}

BoolExpression operator==(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator==(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator==(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator!=(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator!=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator!=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator<=(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator<=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator<=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator>=(const FloatExpression &op1,
                          const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator>=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator>=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator<(const FloatExpression &op1,
                         const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator<(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator<(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator>(const FloatExpression &op1,
                         const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GT, op1, op2);
}

BoolExpression operator>(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GT, op1, op2);
}

BoolExpression operator>(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GT, op1, op2);
}

IntExpression operator+(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_SUM, op1, op2);
}

IntExpression operator+(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_SUM, op1, op2);
}

IntExpression operator+(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_SUM, op1, op2);
}

IntExpression operator-(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(const IntExpression &op1) {
    return unaryOp<IntExpression, IntExpression>(UMO_OP_MINUS_UNARY, op1);
}

IntExpression operator*(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_PROD, op1, op2);
}

IntExpression operator*(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_PROD, op1, op2);
}

IntExpression operator*(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_PROD, op1, op2);
}

IntExpression operator/(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_IDIV, op1, op2);
}

IntExpression operator/(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_IDIV, op1, op2);
}

IntExpression operator/(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_IDIV, op1, op2);
}

IntExpression operator%(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MOD, op1, op2);
}

IntExpression operator%(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MOD, op1, op2);
}

IntExpression operator%(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_MOD, op1, op2);
}

IntExpression &operator+=(IntExpression &op1, const IntExpression &op2) {
    op1 = op1 + op2;
    return op1;
}

IntExpression &operator-=(IntExpression &op1, const IntExpression &op2) {
    op1 = op1 - op2;
    return op1;
}

IntExpression &operator*=(IntExpression &op1, const IntExpression &op2) {
    op1 = op1 * op2;
    return op1;
}

IntExpression &operator/=(IntExpression &op1, const IntExpression &op2) {
    op1 = op1 / op2;
    return op1;
}

IntExpression &operator%=(IntExpression &op1, const IntExpression &op2) {
    op1 = op1 % op2;
    return op1;
}

IntExpression &operator+=(IntExpression &op1, long long op2) {
    op1 = op1 + op2;
    return op1;
}

IntExpression &operator-=(IntExpression &op1, long long op2) {
    op1 = op1 - op2;
    return op1;
}

IntExpression &operator*=(IntExpression &op1, long long op2) {
    op1 = op1 * op2;
    return op1;
}

IntExpression &operator/=(IntExpression &op1, long long op2) {
    op1 = op1 / op2;
    return op1;
}

IntExpression &operator%=(IntExpression &op1, long long op2) {
    op1 = op1 % op2;
    return op1;
}

BoolExpression operator&&(const BoolExpression &op1,
                          const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(bool op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(const BoolExpression &op1, bool op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator||(const BoolExpression &op1,
                          const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_OR, op1, op2);
}

BoolExpression operator||(bool op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_OR, op1, op2);
}

BoolExpression operator||(const BoolExpression &op1, bool op2) {
    return binaryOp<BoolExpression>(UMO_OP_OR, op1, op2);
}

BoolExpression operator^(const BoolExpression &op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_XOR, op1, op2);
}

BoolExpression operator^(bool op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_XOR, op1, op2);
}

BoolExpression operator^(const BoolExpression &op1, bool op2) {
    return binaryOp<BoolExpression>(UMO_OP_XOR, op1, op2);
}

BoolExpression operator!(const BoolExpression &op1) {
    return unaryOp<BoolExpression, BoolExpression>(UMO_OP_NOT, op1);
}

BoolExpression &operator&=(BoolExpression &op1, const BoolExpression &op2) {
    op1 = op1 && op2;
    return op1;
}

BoolExpression &operator|=(BoolExpression &op1, const BoolExpression &op2) {
    op1 = op1 || op2;
    return op1;
}

BoolExpression &operator^=(BoolExpression &op1, const BoolExpression &op2) {
    op1 = op1 ^ op2;
    return op1;
}

BoolExpression &operator&=(BoolExpression &op1, bool op2) {
    op1 = op1 && op2;
    return op1;
}

BoolExpression &operator|=(BoolExpression &op1, bool op2) {
    op1 = op1 || op2;
    return op1;
}

BoolExpression &operator^=(BoolExpression &op1, bool op2) {
    op1 = op1 ^ op2;
    return op1;
}

FloatExpression abs(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ABS, op1);
}

FloatExpression min(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MIN, op1, op2);
}

FloatExpression min(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MIN, op1, op2);
}

FloatExpression min(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_MIN, op1, op2);
}

FloatExpression max(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MAX, op1, op2);
}

FloatExpression max(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_MAX, op1, op2);
}

FloatExpression max(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_MAX, op1, op2);
}

IntExpression abs(const IntExpression &op1) {
    return unaryOp<IntExpression, IntExpression>(UMO_OP_ABS, op1);
}

IntExpression min(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MIN, op1, op2);
}

IntExpression min(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MIN, op1, op2);
}

IntExpression min(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_MIN, op1, op2);
}

IntExpression max(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MAX, op1, op2);
}

IntExpression max(long long op1, const IntExpression &op2) {
    return binaryOp<IntExpression>(UMO_OP_MAX, op1, op2);
}

IntExpression max(const IntExpression &op1, long long op2) {
    return binaryOp<IntExpression>(UMO_OP_MAX, op1, op2);
}

FloatExpression exp(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_EXP, op1);
}

FloatExpression log(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_LOG, op1);
}

FloatExpression log(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_LOGB, op1, op2);
}

FloatExpression log(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_LOGB, op1, op2);
}

FloatExpression log(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_LOGB, op1, op2);
}

FloatExpression pow(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_POW, op1, op2);
}

FloatExpression pow(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_POW, op1, op2);
}

FloatExpression pow(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_POW, op1, op2);
}

FloatExpression sqrt(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SQRT, op1);
}

FloatExpression square(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SQUARE, op1);
}

IntExpression square(const IntExpression &op1) {
    return unaryOp<IntExpression, IntExpression>(UMO_OP_SQUARE, op1);
}

IntExpression factorial(const IntExpression &op1) {
    return unaryOp<IntExpression, IntExpression>(UMO_OP_FACTORIAL, op1);
}

IntExpression round(const FloatExpression &op1) {
    return unaryOp<FloatExpression, IntExpression>(UMO_OP_ROUND, op1);
}

IntExpression floor(const FloatExpression &op1) {
    return unaryOp<FloatExpression, IntExpression>(UMO_OP_FLOOR, op1);
}

IntExpression ceil(const FloatExpression &op1) {
    return unaryOp<FloatExpression, IntExpression>(UMO_OP_CEIL, op1);
}

IntExpression sign(const FloatExpression &op1) {
    return unaryOp<FloatExpression, IntExpression>(UMO_OP_SIGN, op1);
}

FloatExpression frac(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_FRAC, op1);
}

FloatExpression cos(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_COS, op1);
}

FloatExpression sin(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SIN, op1);
}

FloatExpression tan(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_TAN, op1);
}

FloatExpression cosh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_COSH, op1);
}

FloatExpression sinh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SINH, op1);
}

FloatExpression tanh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_TANH, op1);
}

FloatExpression acos(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ACOS, op1);
}

FloatExpression asin(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ASIN, op1);
}

FloatExpression atan(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ATAN, op1);
}

FloatExpression acosh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ACOSH, op1);
}

FloatExpression asinh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ASINH, op1);
}

FloatExpression atanh(const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ATANH, op1);
}

template <typename ExpressionType>
ExpressionType naryOp(umo_operator op, const vector<ExpressionType> &operands) {
    if (operands.empty()) {
        // TODO: support constant expressions without an attached model
        throw std::runtime_error(
            "N-ary operations must receive at least one operand. To support "
            "empty operations, use the corresponding Model method instead.");
    }
    umo_model *model = operands[0].rawPtr();
    return naryOp(op, model, operands);
}

template <typename ExpressionType>
ExpressionType naryOp(umo_operator op, umo_model *model,
                      const vector<ExpressionType> &operands) {
    vector<long long> ops;
    ops.reserve(operands.size());
    for (ExpressionType expr : operands) {
        ops.push_back(expr.rawId());
    }
    long long v = makeNaryOp(model, op, ops.size(), ops.data());
    return ExpressionType(model, v);
}

FloatExpression Model::sum(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_SUM, rawPtr(), ops);
}

FloatExpression Model::prod(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_PROD, rawPtr(), ops);
}

FloatExpression Model::min(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_MIN, rawPtr(), ops);
}

FloatExpression Model::max(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_MAX, rawPtr(), ops);
}

IntExpression Model::sum(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_SUM, rawPtr(), ops);
}

IntExpression Model::prod(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_PROD, rawPtr(), ops);
}

IntExpression Model::min(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_MIN, rawPtr(), ops);
}

IntExpression Model::max(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_MAX, rawPtr(), ops);
}

IntExpression Model::sum(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_SUM, rawPtr(), ops);
}

BoolExpression Model::prod(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_PROD, rawPtr(), ops);
}

BoolExpression Model::min(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_MIN, rawPtr(), ops);
}

BoolExpression Model::max(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_MAX, rawPtr(), ops);
}

BoolExpression Model::logical_or(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_OR, rawPtr(), ops);
}

BoolExpression Model::logical_and(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_AND, rawPtr(), ops);
}

BoolExpression Model::logical_xor(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_XOR, rawPtr(), ops);
}

FloatExpression sum(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_SUM, ops);
}

FloatExpression prod(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_PROD, ops);
}

FloatExpression min(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_MIN, ops);
}

FloatExpression max(const std::vector<FloatExpression> &ops) {
    return naryOp(UMO_OP_MAX, ops);
}

IntExpression sum(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_SUM, ops);
}

IntExpression prod(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_PROD, ops);
}

IntExpression min(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_MIN, ops);
}

IntExpression max(const std::vector<IntExpression> &ops) {
    return naryOp(UMO_OP_MAX, ops);
}

IntExpression sum(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_SUM, ops);
}

BoolExpression prod(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_PROD, ops);
}

BoolExpression min(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_MIN, ops);
}

BoolExpression max(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_MAX, ops);
}

BoolExpression logical_or(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_OR, ops);
}

BoolExpression logical_and(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_AND, ops);
}

BoolExpression logical_xor(const std::vector<BoolExpression> &ops) {
    return naryOp(UMO_OP_XOR, ops);
}

double inf() {
    return numeric_limits<double>::infinity();
}

std::ostream &operator<<(std::ostream &os, const Status &status) {
    switch (status) {
    case Status::Infeasible:
        os << "Infeasible";
        break;
    case Status::Invalid:
        os << "Invalid";
        break;
    case Status::Valid:
        os << "Valid";
        break;
    case Status::Optimal:
        os << "Optimal";
        break;
    case Status::Unbounded:
        os << "Unbounded";
        break;
    case Status::Unknown:
        os << "Unknown";
        break;
    default:
        os << "UndefinedStatus";
        break;
    }
    return os;
}
} // namespace umo
