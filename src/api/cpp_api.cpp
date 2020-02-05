
#include <cassert>
#include <cstdlib>
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

template <typename OperandExpression, typename ResultExpression>
ResultExpression unaryOp(umo_operator op, const OperandExpression &op1) {
    long long v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1,
                          const FloatExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, double op1Val,
                          const FloatExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1,
                          double op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1,
                          const IntExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, long long op1Val,
                          const IntExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double)op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1,
                          long long op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double)op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1,
                          const BoolExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, bool op1Val,
                          const BoolExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double)op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.ptr(), v);
}

template <typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1,
                          bool op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double)op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.ptr(), v);
}

void destroyModel(umo_model *m) {}

Model::Model() {
    UNWRAP_EXCEPTIONS(ptr_ = std::shared_ptr<umo_model>(
                          umo_create_model(&err), [](umo_model *m) {
                              const char *err = NULL;
                              umo_destroy_model(m, &err);
                              // Destructor should never throw
                              assert(err == NULL);
                          }););
}

Model::Model(Model &&o) { ptr_ = std::move(o.ptr_); }

Model &Model::operator=(Model &&o) {
    ptr_ = std::move(o.ptr_);
    return *this;
}

FloatExpression Model::constant(double val) {
    long long v = makeConstant(rawPtr(), (double)val);
    return FloatExpression(ptr_, v);
}

IntExpression Model::constant(long long val) {
    long long v = makeConstant(rawPtr(), (double)val);
    return IntExpression(ptr_, v);
}

BoolExpression Model::constant(bool val) {
    long long v = makeConstant(rawPtr(), (double)val);
    return BoolExpression(ptr_, v);
}

FloatExpression Model::floatVar(double lb, double ub) {
    FloatExpression lbe = constant(lb);
    FloatExpression ube = constant(ub);
    return binaryOp<FloatExpression>(UMO_OP_DEC_FLOAT, lbe, ube);
}

IntExpression Model::intVar(long long lb, long long ub) {
    IntExpression lbe = constant(lb);
    IntExpression ube = constant(ub);
    return binaryOp<IntExpression>(UMO_OP_DEC_INT, lbe, ube);
}

BoolExpression Model::boolVar() {
    long long v = makeNoaryOp(rawPtr(), UMO_OP_DEC_BOOL);
    return BoolExpression(ptr_, v);
}

Status Model::getStatus() {
    umo_solution_status ret;
    UNWRAP_EXCEPTIONS(ret = umo_get_solution_status(rawPtr(), &err););
    switch (ret) {
    case UMO_STATUS_INFEASIBLE:
        return Status::Infeasible;
    case UMO_STATUS_INVALID:
        return Status::Invalid;
    case UMO_STATUS_VALID:
        return Status::Valid;
    case UMO_STATUS_OPTIMAL:
        return Status::Optimal;
    default:
        throw std::runtime_error("Unknown status returned");
    }
}

void Model::solve() { UNWRAP_EXCEPTIONS(umo_solve(rawPtr(), &err);); }

void Model::check() { UNWRAP_EXCEPTIONS(umo_check(rawPtr(), &err);); }

double Model::getFloatParam(const std::string &param) {
    double tmp;
    UNWRAP_EXCEPTIONS(
        tmp = umo_get_float_parameter(rawPtr(), param.c_str(), &err););
    return tmp;
}

void Model::setFloatParam(const std::string &param, double val) {
    UNWRAP_EXCEPTIONS(
        umo_set_float_parameter(rawPtr(), param.c_str(), val, &err););
}

double Model::getTimeLimit() { return getFloatParam("umo_time_limit"); }

void Model::setTimeLimit(double limit) {
    setFloatParam("umo_time_limit", limit);
}

std::string Model::getStringParam(const std::string &param) {
    const char *tmp;
    UNWRAP_EXCEPTIONS(
        tmp = umo_get_string_parameter(rawPtr(), param.c_str(), &err););
    return tmp;
}

void Model::setStringParam(const std::string &param, const std::string &val) {
    UNWRAP_EXCEPTIONS(
        umo_set_string_parameter(rawPtr(), param.c_str(), val.c_str(), &err););
}

double FloatExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(rawPtr(), v, &err););
    return tmp;
}

long long IntExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(rawPtr(), v, &err););
    return (long long)tmp;
}

bool BoolExpression::getValue() {
    double tmp;
    UNWRAP_EXCEPTIONS(tmp = umo_get_float_value(rawPtr(), v, &err););
    return (bool)tmp;
}

void FloatExpression::setValue(double val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(rawPtr(), v, val, &err););
}

void IntExpression::setValue(long long val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(rawPtr(), v, (double)val, &err););
}

void BoolExpression::setValue(bool val) {
    UNWRAP_EXCEPTIONS(umo_set_float_value(rawPtr(), v, (double)val, &err););
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

} // namespace umo
