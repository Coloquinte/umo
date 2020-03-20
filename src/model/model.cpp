
#include "model/model.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "model/operator.hpp"
#include "presolve/presolve.hpp"

using namespace std;

namespace umoi {
namespace {
umo_type computeType(double value) {
    if (value == 0.0 || value == 1.0) {
        return UMO_TYPE_BOOL;
    }
    if (value == round(value)) {
        // Includes infinities
        return UMO_TYPE_INT;
    }
    return UMO_TYPE_FLOAT;
}

bool isSubtype(umo_type subtype, umo_type type) {
    if (type == UMO_TYPE_BOOL && subtype != UMO_TYPE_BOOL) {
        return false;
    }
    if (type == UMO_TYPE_INT && subtype == UMO_TYPE_FLOAT) {
        return false;
    }
    return true;
}

bool isTypeCompatible(umo_type varType, double value) {
    umo_type valueType = computeType(value);
    return isSubtype(valueType, varType);
}
} // namespace

Model::Model() {
    computed_ = false;
    initDefaultParameters();
}

ExpressionId Model::createConstant(double value) {
    if (isnan(value)) {
        throw runtime_error("Constants with Not-a-Number value are forbidden");
    }
    auto itp = constants_.emplace(value, nbExpressions());
    if (itp.second) {
        // New constant inserted
        expressions_.emplace_back(UMO_OP_CONSTANT, computeType(value));
        values_.push_back(value);
    }
    return ExpressionId(itp.first->second, false, false);
}

ExpressionId Model::createExpression(umo_operator op, long long *beginOp,
                                     long long *endOp) {
    vector<ExpressionId> operands;
    for (long long *it = beginOp; it != endOp; ++it) {
        operands.push_back(ExpressionId::fromRaw(*it));
    }
    return createExpression(op, operands);
}

ExpressionId Model::createExpression(umo_operator op,
                                     const vector<ExpressionId> &operands) {
    if (op == UMO_OP_CONSTANT || op == UMO_OP_INVALID || op >= UMO_OP_END) {
        throw runtime_error("Invalid expression type");
    }

    computed_ = false;
    // Gather operands with type info
    ExpressionData expr(op);
    expr.operands = operands;
    expr.type = checkAndInferType(expr);

    // Handle compressed representations
    if (op == UMO_OP_NOT) {
        assert(operands.size() == 1);
        return operands[0].getNot();
    }
    if (op == UMO_OP_MINUS_UNARY) {
        assert(operands.size() == 1);
        return operands[0].getMinus();
    }
    if (op == UMO_OP_MINUS_BINARY) {
        assert(operands.size() == 2);
        expr.op = UMO_OP_SUM;
        expr.operands[1] = expr.operands[1].getMinus();
    }

    // Add the expression
    uint32_t var = nbExpressions();
    expressions_.push_back(expr);
    values_.push_back(0.0);
    return ExpressionId(var, false, false);
}

void Model::createConstraint(ExpressionId expr) {
    computed_ = false;
    constraints_.insert(expr);
}

void Model::createObjective(ExpressionId expr, umo_objective_direction dir) {
    computed_ = false;
    objectives_.emplace_back(expr, dir);
}

double Model::getFloatValue(ExpressionId expr) {
    if (!computed_)
        compute();
    return getExpressionIdValue(expr);
}

void Model::setFloatValue(ExpressionId expr, double value) {
    computed_ = false;
    checkExpressionId(expr);
    umo_operator varOp = getExpressionIdOp(expr);
    if (varOp != UMO_OP_DEC_BOOL && varOp != UMO_OP_DEC_INT &&
        varOp != UMO_OP_DEC_FLOAT)
        throw runtime_error("Only decisions can be set");
    umo_type varType = expressions_[expr.var()].type;
    if (!isTypeCompatible(varType, value))
        throw runtime_error("Setting a value of the wrong type");
    values_[expr.var()] = value;
}

umo_solution_status Model::getStatus() {
    if (!computed_)
        compute();
    return status_;
}

void Model::solve() {
    check();
    PresolvedModel presolved = presolve::run(*this);
    presolved.check();
}

double Model::getFloatParameter(const string &param) const {
    return floatParams_.at(param);
}

void Model::setFloatParameter(const string &param, double value) {
    floatParams_[param] = value;
}

const string &Model::getStringParameter(const string &param) const {
    return stringParams_.at(param);
}

void Model::setStringParameter(const string &param, const string &value) {
    stringParams_[param] = value;
}

void Model::checkExpressionId(ExpressionId expr) const {
    if (expr.var() >= nbExpressions())
        throw runtime_error("Expression is out of bounds");
    umo_type type = expressions_[expr.var()].type;
    if (expr.isNot() && type != UMO_TYPE_BOOL)
        throw runtime_error(
            "The NOT bit is set but the variable is not of boolean type");
}

umo_type Model::getExpressionIdType(ExpressionId expr) const {
    umo_type type = expressions_[expr.var()].type;
    if (expr.isMinus() && type == UMO_TYPE_BOOL)
        return UMO_TYPE_INT;
    return type;
}

umo_operator Model::getExpressionIdOp(ExpressionId expr) const {
    if (expr.isMinus())
        return UMO_OP_MINUS_UNARY;
    if (expr.isNot())
        return UMO_OP_NOT;
    return expressions_[expr.var()].op;
}

double Model::getExpressionIdValue(ExpressionId expr) const {
    double val = values_[expr.var()];
    if (expr.isNot())
        val = 1.0 - val;
    if (expr.isMinus())
        val = -val;
    return val;
}

umo_type Model::checkAndInferType(const ExpressionData &expr) const {
    for (ExpressionId id : expr.operands) {
        checkExpressionId(id);
    }
    vector<umo_type> operandTypes = getOperandTypes(expr);
    vector<umo_operator> operandOps = getOperandOps(expr);
    const Operator &op = Operator::get(expr.op);
    if (!op.validOperands(expr.operands.size(), operandTypes.data(),
                          operandOps.data())) {
        if (!op.validOperandCount(expr.operands.size()))
            throw runtime_error("Invalid number of operands.");
        if (!op.validOperandTypes(expr.operands.size(), operandTypes.data()))
            throw runtime_error("Invalid operand types.");
        if (!op.validOperandOps(expr.operands.size(), operandOps.data()))
            throw runtime_error("Invalid operand operations.");
        throw runtime_error("Invalid operands (unknown).");
    }
    return op.resultType(expr.operands.size(), operandTypes.data(),
                         operandOps.data());
}

vector<umo_type> Model::getOperandTypes(const ExpressionData &expr) const {
    vector<umo_type> operandTypes;
    for (ExpressionId id : expr.operands) {
        operandTypes.push_back(getExpressionIdType(id));
    }
    return operandTypes;
}

vector<umo_operator> Model::getOperandOps(const ExpressionData &expr) const {
    vector<umo_operator> operandOps;
    for (ExpressionId id : expr.operands) {
        operandOps.push_back(expressions_[id.var()].op);
    }
    return operandOps;
}

void Model::check() const {
    checkTypes();
    checkTopologicalOrder();
    checkCompressedOperands();
}

bool Model::isConstant(uint32_t i) const {
    return Operator::get(expressions_[i].op).isConstant();
}

bool Model::isLeaf(uint32_t i) const {
    return Operator::get(expressions_[i].op).isLeaf();
}

bool Model::isDecision(uint32_t i) const {
    return Operator::get(expressions_[i].op).isDecision();
}

bool Model::isConstraint(uint32_t i) const {
    return isConstraintNeg(i) || isConstraintPos(i);
}

bool Model::isConstraintNeg(uint32_t i) const {
    if (expressions_[i].type != UMO_TYPE_BOOL)
        return false;
    return constraints_.count(ExpressionId(i, true, false));
}

bool Model::isConstraintPos(uint32_t i) const {
    if (expressions_[i].type != UMO_TYPE_BOOL)
        return false;
    return constraints_.count(ExpressionId(i, false, false));
}

bool Model::isObjective(uint32_t i) const {
    for (auto o : objectives_) {
        if (o.first.var() == i)
            return true;
    }
    return false;
}

void Model::checkTypes() const {
    if (expressions_.size() != values_.size()) {
        throw runtime_error("Different number of expressions and values");
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        const ExpressionData &expr = expressions_[i];
        umo_type type = expr.op == UMO_OP_CONSTANT ? computeType(values_[i])
                                                   : checkAndInferType(expr);
        if (!isSubtype(type, expr.type))
            throw runtime_error(
                "The computed type is not compatible with the inferred type");
    }
}

void Model::checkTopologicalOrder() const {
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        const ExpressionData &expr = expressions_[i];
        for (ExpressionId id : expr.operands) {
            if (id.var() >= i)
                throw runtime_error("The expressions are not in sorted order");
        }
    }
}

void Model::checkCompressedOperands() const {
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        const ExpressionData &expr = expressions_[i];
        if (expr.op == UMO_OP_NOT)
            throw runtime_error("NOT expressions should be compressed");
        if (expr.op == UMO_OP_MINUS_UNARY)
            throw runtime_error("Unary MINUS expressions should be compressed");
        if (expr.op == UMO_OP_MINUS_BINARY)
            throw runtime_error(
                "Binary MINUS expressions should be compressed");
    }
}

void Model::compute() {
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        const ExpressionData &expr = expressions_[i];
        const Operator &op = Operator::get(expr.op);
        if (op.isLeaf())
            continue;
        vector<double> operands;
        for (ExpressionId id : expr.operands) {
            operands.push_back(getExpressionIdValue(id));
        }
        values_[i] = op.compute(operands.size(), operands.data());
    }
    bool constraintViolated = false;
    for (ExpressionId constraint : constraints_) {
        double val = getExpressionIdValue(constraint);
        if (val == 0.0)
            constraintViolated = true;
    }
    status_ = constraintViolated ? UMO_STATUS_INVALID : UMO_STATUS_VALID;
    computed_ = true;
}

void Model::initDefaultParameters() {
    setFloatParameter("umo_time_limit", numeric_limits<double>::infinity());
}
} // namespace umoi
