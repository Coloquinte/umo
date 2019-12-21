
#include "api/umo.hpp"
#include "api/umo.h"

#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace std;
using namespace umo;

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

FloatExpression unaryOp(umo_operator op, const FloatExpression &op1) {
    bool v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return FloatExpression(op1.rawPtr(), v);
}

FloatExpression binaryOp(umo_operator op, const FloatExpression &op1, const FloatExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return FloatExpression(op1.rawPtr(), v);
}

FloatExpression binaryOp(umo_operator op, double op1Val, const FloatExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return FloatExpression(op2.rawPtr(), v);
}

FloatExpression binaryOp(umo_operator op, const FloatExpression &op1, double op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return FloatExpression(op1.rawPtr(), v);
}

IntExpression unaryOp(umo_operator op, const IntExpression &op1) {
    bool v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return IntExpression(op1.rawPtr(), v);
}

IntExpression binaryOp(umo_operator op, const IntExpression &op1, const IntExpression &op2) {
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return IntExpression(op1.rawPtr(), v);
}

IntExpression binaryOp(umo_operator op, long long op1Val, const IntExpression &op2) {
    long long op1 = makeConstant(op2.rawPtr(), (double) op1Val);
    long long v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return IntExpression(op2.rawPtr(), v);
}

IntExpression binaryOp(umo_operator op, const IntExpression &op1, long long op2Val) {
    long long op2 = makeConstant(op1.rawPtr(), (double) op2Val);
    long long v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return IntExpression(op1.rawPtr(), v);
}

BoolExpression unaryOp(umo_operator op, const BoolExpression &op1) {
    bool v = makeUnaryOp(op1.rawPtr(), op, op1.rawId());
    return BoolExpression(op1.rawPtr(), v);
}

BoolExpression binaryOp(umo_operator op, const BoolExpression &op1, const BoolExpression &op2) {
    bool v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2.rawId());
    return BoolExpression(op1.rawPtr(), v);
}

BoolExpression binaryOp(umo_operator op, bool op1Val, const BoolExpression &op2) {
    bool op1 = makeConstant(op2.rawPtr(), (double) op1Val);
    bool v = makeBinaryOp(op2.rawPtr(), op, op1, op2.rawId());
    return BoolExpression(op2.rawPtr(), v);
}

BoolExpression binaryOp(umo_operator op, const BoolExpression &op1, bool op2Val) {
    bool op2 = makeConstant(op1.rawPtr(), (double) op2Val);
    bool v = makeBinaryOp(op1.rawPtr(), op, op1.rawId(), op2);
    return BoolExpression(op1.rawPtr(), v);
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
    return binaryOp(UMO_OP_DEC_FLOAT, lbe, ube);
}

IntExpression Model::intVar(long long lb, long long ub) {
    IntExpression lbe = constant(lb);
    IntExpression ube = constant(ub);
    return binaryOp(UMO_OP_DEC_INT, lbe, ube);
}

BoolExpression Model::boolVar() {
    long long v = makeNoaryOp(ptr_, UMO_OP_DEC_BOOL);
    return BoolExpression(ptr_, v);
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
    return binaryOp(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(double op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_SUM, op1, op2);
}

FloatExpression operator+(const FloatExpression &op1, double op2) {
    return binaryOp(UMO_OP_SUM, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(double op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1, double op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

FloatExpression operator-(const FloatExpression &op1) {
    return unaryOp(UMO_OP_MINUS_UNARY, op1);
}

FloatExpression operator*(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(double op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

FloatExpression operator*(const FloatExpression &op1, double op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(double op1, const FloatExpression &op2) {
    return binaryOp(UMO_OP_DIV, op1, op2);
}

FloatExpression operator/(const FloatExpression &op1, double op2) {
    return binaryOp(UMO_OP_DIV, op1, op2);
}

IntExpression operator+(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_SUM, op1, op2);
}

IntExpression operator+(long long op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_SUM, op1, op2);
}

IntExpression operator+(const IntExpression &op1, long long op2) {
    return binaryOp(UMO_OP_SUM, op1, op2);
}

IntExpression operator-(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(long long op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(const IntExpression &op1, long long op2) {
    return binaryOp(UMO_OP_MINUS_BINARY, op1, op2);
}

IntExpression operator-(const IntExpression &op1) {
    return unaryOp(UMO_OP_MINUS_UNARY, op1);
}

IntExpression operator*(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

IntExpression operator*(long long op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

IntExpression operator*(const IntExpression &op1, long long op2) {
    return binaryOp(UMO_OP_PROD, op1, op2);
}

IntExpression operator/(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_IDIV, op1, op2);
}

IntExpression operator/(long long op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_IDIV, op1, op2);
}

IntExpression operator/(const IntExpression &op1, long long op2) {
    return binaryOp(UMO_OP_IDIV, op1, op2);
}

IntExpression operator%(const IntExpression &op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_MOD, op1, op2);
}

IntExpression operator%(long long op1, const IntExpression &op2) {
    return binaryOp(UMO_OP_MOD, op1, op2);
}

IntExpression operator%(const IntExpression &op1, long long op2) {
    return binaryOp(UMO_OP_MOD, op1, op2);
}

BoolExpression operator&&(const BoolExpression &op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(bool op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_AND, op1, op2);
}

BoolExpression operator&&(const BoolExpression &op1, bool op2) {
    return binaryOp(UMO_OP_AND, op1, op2);
}

BoolExpression operator||(const BoolExpression &op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_OR, op1, op2);
}

BoolExpression operator||(bool op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_OR, op1, op2);
}

BoolExpression operator||(const BoolExpression &op1, bool op2) {
    return binaryOp(UMO_OP_OR, op1, op2);
}

BoolExpression operator^(const BoolExpression &op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_XOR, op1, op2);
}

BoolExpression operator^(bool op1, const BoolExpression &op2) {
    return binaryOp(UMO_OP_XOR, op1, op2);
}

BoolExpression operator^(const BoolExpression &op1, bool op2) {
    return binaryOp(UMO_OP_XOR, op1, op2);
}

BoolExpression operator!(const BoolExpression &op1) {
    return unaryOp(UMO_OP_NOT, op1);
}







