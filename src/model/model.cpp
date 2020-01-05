
#include "model/model.hpp"
#include "model/operator.hpp"

#include <cmath>
#include <stdexcept>

using namespace umoi;
using namespace std;

namespace {
umo_type computeType(double value) {
    if (value == 0.0 || value == 1.0) {
        return UMO_TYPE_BOOL;
    }
    if (value == std::round(value)) {
        return UMO_TYPE_INT;
    }
    return UMO_TYPE_FLOAT;
}

bool isTypeCompatible(umo_type varType, double value) {
    umo_type valueType = computeType(value);
    if (varType == UMO_TYPE_BOOL && valueType != UMO_TYPE_BOOL) {
        return false;
    }
    if (varType == UMO_TYPE_INT && valueType == UMO_TYPE_FLOAT) {
        return false;
    }
    return true;
}
}

ExpressionId Model::createConstant(double value) {
    if (isnan(value)) {
        throw runtime_error("Constants with Not-a-Number value are forbidden");
    }
    auto itp = constants_.emplace(value, expressions_.size());
    if (itp.second) {
        // New constant inserted
        expressions_.emplace_back(UMO_OP_CONSTANT, computeType(value));
        values_.push_back(value);
    }
    return ExpressionId(itp.first->second, false, false);
}

ExpressionId Model::createExpression(umo_operator op, long long *beginOp, long long *endOp) {
    computed_ = false;
    std::uint32_t var = expressions_.size();
    // Gathe operands with type info
    ExpressionData expr(op);
    for (long long *it = beginOp; it != endOp; ++it) {
        expr.operands.push_back(ExpressionId::fromRaw(*it));
    }
    expr.type = checkAndInferType(expr);
    expressions_.push_back(expr);
    values_.push_back(0.0);
    return ExpressionId(var, false, false);
}

void Model::createConstraint(ExpressionId expr) {
    constraints_.insert(expr);
}

void Model::createObjective(ExpressionId expr, umo_objective_direction dir) {
    objectives_.emplace_back(expr, dir);
}

double Model::getFloatValue(ExpressionId expr) const {
    double val = values_[expr.var()];
    if (expr.isNot()) val = 1.0 - val;
    if (expr.isMinus()) val = -val;
    return val;
}

void Model::setFloatValue(ExpressionId expr, double value) {
    checkExpressionId(expr);
    umo_operator varOp = expressions_[expr.var()].op;
    if (varOp != UMO_OP_DEC_BOOL && varOp != UMO_OP_DEC_INT && varOp != UMO_OP_DEC_FLOAT)
        throw runtime_error("Only decisions can be set");
    umo_type varType = expressions_[expr.var()].type;
    if (!isTypeCompatible(varType, value))
        throw runtime_error("Setting a value of the wrong type");
    values_[expr.var()] = value;
}

umo_solution_status Model::getStatus() const {
    return status_;
}

void Model::solve() {
}

double Model::getFloatParameter(const std::string &param) const {
    return floatParams_.at(param);
}

void Model::setFloatParameter(const std::string &param, double value) {
    floatParams_.emplace(param, value);
}

const std::string &Model::getStringParameter(const std::string &param) const {
    return stringParams_.at(param);
}

void Model::setStringParameter(const std::string &param, const std::string &value) {
    stringParams_.emplace(param, value);
}

void Model::checkExpressionId(ExpressionId expr) const {
    if (expr.var() >= expressions_.size())
        throw runtime_error("Expression is out of bounds");
    umo_type type = expressions_[expr.var()].type;
    if (expr.isNot() && type != UMO_TYPE_BOOL)
        throw runtime_error("The NOT bit is set but the variable is not of boolean type");
}

umo_type Model::getExpressionIdType(ExpressionId expr) const {
    umo_type type = expressions_[expr.var()].type;
    if (expr.isMinus() && type == UMO_TYPE_BOOL)
        return UMO_TYPE_INT;
    return type;
}

umo_type Model::checkAndInferType(const ExpressionData &expr) const {
    for (ExpressionId id : expr.operands) {
        checkExpressionId(id);
    }
    std::vector<umo_type> operandTypes = getOperandTypes(expr);
    std::vector<umo_operator> operandOps = getOperandOps(expr);
    const Operator &op = Operator::get(expr.op);
    if (!op.validOperands(expr.operands.size(), operandTypes.data(), operandOps.data())) {
        if (!op.validOperandCount(expr.operands.size()))
            throw std::runtime_error("Invalid number of operands.");
        if (!op.validOperandTypes(expr.operands.size(), operandTypes.data()))
            throw std::runtime_error("Invalid operand types.");
        if (!op.validOperandOps(expr.operands.size(), operandOps.data()))
            throw std::runtime_error("Invalid operand operations.");
        throw std::runtime_error("Invalid operands (unknown).");
    }
    return op.resultType(expr.operands.size(), operandTypes.data(), operandOps.data());
}

std::vector<umo_type> Model::getOperandTypes(const ExpressionData &expr) const {
    std::vector<umo_type> operandTypes;
    for (ExpressionId id : expr.operands) {
        operandTypes.push_back(getExpressionIdType(id));
    }
    return operandTypes;
}

std::vector<umo_operator> Model::getOperandOps(const ExpressionData &expr) const {
    std::vector<umo_operator> operandOps;
    for (ExpressionId id : expr.operands) {
        operandOps.push_back(expressions_[id.var()].op);
    }
    return operandOps;
}

void Model::checkConsistency() const {
    for (size_t i = 0; i < expressions_.size(); ++i) {
        const ExpressionData &expr = expressions_[i];
        umo_type type = checkAndInferType(expr);
        for (ExpressionId id : expr.operands) {
            if (id.var() >= i) throw std::runtime_error("The expressions are not in sorted order");
        }
    }
}

void Model::compute() {

}

