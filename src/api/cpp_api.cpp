
#include "api/umo.hpp"
#include "api/umo.h"

#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace std;

namespace umo {

long long makeConstant(umo_model *model, double val) {
    const char *err = NULL;
    return umo_create_constant(model, val, &err);
}

long long makeNoaryOp(umo_model *model, umo_operator op) {
    const char *err = NULL;
    return umo_create_expression(model, op, 0, NULL, &err);
}

long long makeUnaryOp(umo_model *model, umo_operator op, long long op1) {
    const char *err = NULL;
    return umo_create_expression(model, op, 1, &op1, &err);
}

long long makeBinaryOp(umo_model *model, umo_operator op, long long op1, long long op2) {
    long long operands[2] = {op1, op2};
    const char *err = NULL;
    return umo_create_expression(model, op, 2, operands, &err);
}

long long makeTernaryOp(umo_model *model, umo_operator op, long long op1, long long op2, long long op3) {
    long long operands[3] = {op1, op2, op3};
    const char *err = NULL;
    return umo_create_expression(model, op, 3, operands, &err);
}

template<typename OperandExpression, typename ResultExpression>
ResultExpression unaryOp(umo_operator op, const OperandExpression &op1) {
    bool v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1, const FloatExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, double op1Val, const FloatExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const FloatExpression &op1, double op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1, const IntExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, long long op1Val, const IntExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double) op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const IntExpression &op1, long long op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double) op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1, const BoolExpression &op2) {
    bool v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return ResultExpression(op1.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, bool op1Val, const BoolExpression &op2) {
    bool op1 = makeConstant(op2.rawPtr(), (double) op1Val);
    bool v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return ResultExpression(op2.rawPtr(), v);
}

template<typename ResultExpression>
ResultExpression binaryOp(umo_operator op, const BoolExpression &op1, bool op2Val) {
    bool op2 = makeConstant(op1.rawPtr(), (double) op2Val);
    bool v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return ResultExpression(op1.rawPtr(), v);
}

Model::Model() {
    const char *err = NULL;
    ptr_ = umo_create_model(&err);
}

Model::Model(Model &&o) {
    ptr_ = o.ptr_;
}

Model &Model::operator=(Model &&o) {
    ptr_ = o.ptr_;
    return *this;
}

Model::~Model() {
    const char *err = NULL;
    umo_destroy_model(ptr_, &err);
}

FloatExpression Model::constant(double val) {
    long long v = makeConstant(ptr_, (double) val);
    return FloatExpression(ptr_, v);
}

IntExpression Model::constant(long long val) {
    long long v = makeConstant(ptr_, (double) val);
    return IntExpression(ptr_, v);
}

BoolExpression Model::constant(bool val) {
    long long v = makeConstant(ptr_, (double) val);
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
    long long v = makeNoaryOp(ptr_, UMO_OP_DEC_BOOL);
    return BoolExpression(ptr_, v);
}

void Model::solve() {
    const char *err = NULL;
    umo_solve(ptr_, &err);
}

double Model::getFloatParam(const std::string &param) {
    const char *err = NULL;
    return umo_get_float_parameter(ptr_, param.c_str(), &err);
}

void Model::setFloatParam(const std::string &param, double val) {
    const char *err = NULL;
    umo_set_float_parameter(ptr_, param.c_str(), val, &err);
}

std::string Model::getStringParam(const std::string &param) {
    const char *err = NULL;
    return umo_get_string_parameter(ptr_, param.c_str(), &err);
}

void Model::setStringParam(const std::string &param, const std::string &val) {
    const char *err = NULL;
    umo_set_string_parameter(ptr_, param.c_str(), val.c_str(), &err);
}

double FloatExpression::getValue() {
    const char *err = NULL;
    return umo_get_float_value(model, v, &err);
}

long long IntExpression::getValue() {
    const char *err = NULL;
    return (long long) umo_get_float_value(model, v, &err);
}

bool BoolExpression::getValue() {
    const char *err = NULL;
    return (bool) umo_get_float_value(model, v, &err);
}

void FloatExpression::setValue(double val) {
    const char *err = NULL;
    return umo_set_float_value(model, v, val, &err);
}

void IntExpression::setValue(long long val) {
    const char *err = NULL;
    return umo_set_float_value(model, v, (double) val, &err);
}

void BoolExpression::setValue(bool val) {
    const char *err = NULL;
    return umo_set_float_value(model, v, (double) val, &err);
}

void constraint(const BoolExpression &c) {
    const char *err = NULL;
    umo_create_constraint(c.rawPtr(), c.rawId(), &err);
}

void minimize(const FloatExpression &o) {
    const char *err = NULL;
    umo_create_objective(o.rawPtr(), o.rawId(), UMO_OBJ_MINIMIZE, &err);
}

void maximize(const FloatExpression &o) {
    const char *err = NULL;
    umo_create_objective(o.rawPtr(), o.rawId(), UMO_OBJ_MAXIMIZE, &err);
}

FloatExpression operator+(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_SUM, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1, const FloatExpression &op2) {
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

FloatExpression operator*(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_PROD, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(double op1, const FloatExpression &op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1, double op2) {
    return binaryOp<FloatExpression>(UMO_OP_DIV, op1, op2);
}

BoolExpression operator==(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator==(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator==(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_EQ, op1, op2);
}

BoolExpression operator!=(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator!=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator!=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_NEQ, op1, op2);
}

BoolExpression operator<=(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator<=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator<=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LEQ, op1, op2);
}

BoolExpression operator>=(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator>=(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator>=(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_GEQ, op1, op2);
}

BoolExpression operator<(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator<(double op1, const FloatExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator<(const FloatExpression &op1, double op2) {
    return binaryOp<BoolExpression>(UMO_OP_CMP_LT, op1, op2);
}

BoolExpression operator>(const FloatExpression &op1, const FloatExpression &op2) {
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

BoolExpression operator&&(const BoolExpression &op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(bool op1, const BoolExpression &op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(const BoolExpression &op1, bool op2) {
    return binaryOp<BoolExpression>(UMO_OP_AND, op1, op2);
}

BoolExpression operator||(const BoolExpression &op1, const BoolExpression &op2) {
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

FloatExpression cos   (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_COS,   op1);
}

FloatExpression sin   (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SIN,   op1);
}

FloatExpression tan   (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_TAN,   op1);
}

FloatExpression cosh  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_COSH,  op1);
}

FloatExpression sinh  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_SINH,  op1);
}

FloatExpression tanh  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_TANH,  op1);
}

FloatExpression acos  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ACOS,  op1);
}

FloatExpression asin  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ASIN,  op1);
}

FloatExpression atan  (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ATAN,  op1);
}

FloatExpression acosh (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ACOSH, op1);
}

FloatExpression asinh (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ASINH, op1);
}

FloatExpression atanh (const FloatExpression &op1) {
    return unaryOp<FloatExpression, FloatExpression>(UMO_OP_ATANH, op1);
}

}

