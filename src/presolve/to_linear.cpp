
#include "presolve/to_linear.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <limits>
#include <cassert>

using namespace std;

namespace umoi {
namespace presolve {

class ToLinear::Transformer {
  public:
    Transformer(PresolvedModel &);
    void run();

    void createExpressions();
    void linearize(uint32_t i);

    void linearizeSum(uint32_t i);
    void linearizeProd(uint32_t i);
    void linearizeCompare(uint32_t i);
    void linearizeAnd(uint32_t i);
    void linearizeOr(uint32_t i);
    void linearizeXor(uint32_t i);

    // Helper function: get affine coefficients for a compressed (not/minus) expression
    Element getElement(ExpressionId expr) const;
    // Helper function: constrain variable i to be within certain bounds of the summed operands
    void constrainToSum(uint32_t i, const vector<ExpressionId> &operands, double lb, double ub);
    // Helper function: constrain variable i to be equal to factor * op
    void constrainToProd(uint32_t i, ExpressionId op, double factor);

  private:
    PresolvedModel &model;
    PresolvedModel linearModel;

    ExpressionId constantMInf;
    ExpressionId constantPInf;
    ExpressionId constantZero;
    ExpressionId constantPOne;
    ExpressionId constantMOne;
};

struct ToLinear::Element {
    uint32_t var;
    double coef;
    double constant;
};

ToLinear::Transformer::Transformer(PresolvedModel &model) : model(model) {
    constantMInf = linearModel.createConstant(-numeric_limits<double>::infinity());
    constantPInf = linearModel.createConstant(numeric_limits<double>::infinity());
    constantZero = linearModel.createConstant(0.0);
    constantPOne = linearModel.createConstant(1.0);
    constantMOne = linearModel.createConstant(-1.0);
}

void ToLinear::Transformer::createExpressions() {
    // Copy expressions
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        const auto &expr = model.expression(i);
        if (model.isLeaf(i)) {
            ExpressionId newId;
            if (model.isConstraint(i)) {
                throw runtime_error("Constraints on leaf expressions are not supported yet");
            } else if (expr.op == UMO_OP_DEC_BOOL) {
                newId = linearModel.createExpression(UMO_OP_DEC_BOOL, {});
            } else if (expr.op == UMO_OP_DEC_INT
                    || expr.op == UMO_OP_DEC_FLOAT) {
                ExpressionId lb = linearModel.createConstant(model.getFloatValue(expr.operands[0]));
                ExpressionId ub = linearModel.createConstant(model.getFloatValue(expr.operands[1]));
                newId = linearModel.createExpression(expr.op, {lb, ub});
            }
            else {
                newId = linearModel.createConstant(model.value(i));
            }
            linearModel.mapping().emplace(i, newId);
        } else {
            // Other type of variable; convert it to a decision
            ExpressionId newId;
            switch (expr.type) {
            case UMO_TYPE_BOOL:
                newId = linearModel.createExpression(UMO_OP_DEC_BOOL, {});
                break;
            case UMO_TYPE_INT:
                newId = linearModel.createExpression(UMO_OP_DEC_INT, {constantMInf, constantPInf});
                break;
            case UMO_TYPE_FLOAT:
                newId = linearModel.createExpression(UMO_OP_DEC_FLOAT, {constantMInf, constantPInf});
                break;
            default:
                THROW_ERROR("Invalid type " << expr.type << " encountered");
            }
            linearModel.mapping().emplace(i, newId);
        }
    }
}

void ToLinear::Transformer::linearize(uint32_t i) {
    if (model.isLeaf(i))
        return;
    umo_operator op = model.expression(i).op;
    switch (op) {
        case UMO_OP_SUM:
            linearizeSum(i);
            break;
        case UMO_OP_PROD:
            linearizeProd(i);
            break;
        case UMO_OP_CMP_EQ:
        case UMO_OP_CMP_NEQ:
        case UMO_OP_CMP_LEQ:
        case UMO_OP_CMP_GEQ:
        case UMO_OP_CMP_LT:
        case UMO_OP_CMP_GT:
            linearizeCompare(i);
            break;
        case UMO_OP_AND:
            linearizeAnd(i);
            break;
        case UMO_OP_OR:
            linearizeOr(i);
            break;
        case UMO_OP_XOR:
            linearizeXor(i);
            break;
        default:
            THROW_ERROR("Operand type " << op << " not handled for linearization");
    }
}

void ToLinear::Transformer::run() {
    createExpressions();
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        linearize(i);
    }
    model.apply(linearModel);
}

ToLinear::Element ToLinear::Transformer::getElement(ExpressionId id) const {
    ExpressionId pid = linearModel.mapping().at(id.var());
    assert (!pid.isNot() && !pid.isMinus());
    Element elt;
    elt.var = pid.var();
    elt.constant = 0.0;
    elt.coef = 1.0;
    // Apply compressed operators
    if (id.isNot()) {
        elt.constant = 1.0;
        elt.coef = -1.0;
    }
    if (id.isMinus()) {
        elt.constant = -elt.constant;
        elt.coef = -elt.coef;
    }
    return elt;
}

void ToLinear::Transformer::constrainToSum(uint32_t i, const vector<ExpressionId> &ops, double lb, double ub) {
    vector<ExpressionId> operands;
    // Reserve space for the bounds
    operands.emplace_back();
    operands.emplace_back();
    // Overall offset for the constraint's bounds
    double offset = 0.0;
    // Expressions for the operands
    for (ExpressionId id : ops) {
        Element elt = getElement(id);
        operands.push_back(linearModel.createConstant(elt.coef));
        operands.push_back(ExpressionId(elt.var, false, false));
        offset += elt.constant;
    }
    // Expression for the defined variable
    operands.push_back(linearModel.createConstant(-1.0));
    ExpressionId pi = linearModel.mapping().at(i);
    assert (!pi.isNot() && !pi.isMinus());
    operands.push_back(ExpressionId(pi.var(), false, false));
    // Lower/upper bound of the constraint
    ExpressionId lbId = linearModel.createConstant(offset + lb);
    ExpressionId ubId = linearModel.createConstant(offset + ub);
    operands[0] = lbId;
    operands[1] = ubId;
    // Constraint creation
    ExpressionId linearized = linearModel.createExpression(UMO_OP_LINEARCOMP, operands);
    linearModel.createConstraint(linearized);
}

void ToLinear::Transformer::constrainToProd(uint32_t i, ExpressionId op, double factor) {
    vector<ExpressionId> operands;
    // Reserve space for the bounds
    operands.emplace_back();
    operands.emplace_back();
    Element elt = getElement(op);
    operands.push_back(linearModel.createConstant(factor * elt.coef));
    operands.push_back(ExpressionId(elt.var, false, false));
    // Expression for the defined variable
    operands.push_back(linearModel.createConstant(-1.0));
    ExpressionId pi = linearModel.mapping().at(i);
    assert (!pi.isNot() && !pi.isMinus());
    operands.push_back(ExpressionId(pi.var(), false, false));
    // Lower/upper bound of the constraint
    ExpressionId constantId = linearModel.createConstant(factor * elt.constant);
    operands[0] = constantId;
    operands[1] = constantId;
    // Constraint creation
    ExpressionId linearized = linearModel.createExpression(UMO_OP_LINEARCOMP, operands);
    linearModel.createConstraint(linearized);
}

void ToLinear::Transformer::linearizeSum(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_SUM);
    constrainToSum(i, expr.operands, 0.0, 0.0);
}

void ToLinear::Transformer::linearizeProd(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_PROD);
    double constantProd = 1.0;
    ExpressionId variableProd;
    for (ExpressionId id : expr.operands) {
        if (model.isConstant(id.var())) {
            constantProd *= model.getFloatValue(id);
        }
        else if (!variableProd.valid()) {
            variableProd = id;
        }
        else {
            THROW_ERROR("Impossible to linearize product with multiple variables");
        }
    }
    if (variableProd.valid()) {
        constrainToProd(i, variableProd, constantProd);
    }
    else {
        constrainToSum(i, {}, -constantProd, -constantProd);
    }
}

void ToLinear::Transformer::linearizeCompare(uint32_t i) {
    const auto &expr = model.expression(i);
    if (!model.isConstraint(i))
        THROW_ERROR("Comparisons that are not constraints are not handled yet");
    // TODO
}

void ToLinear::Transformer::linearizeAnd(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_AND);
    // all operands true ==> y; sum xi - y <= n-1
    constrainToSum(i, expr.operands, -numeric_limits<double>::infinity(), expr.operands.size() - 1);
    // y ==> Any operand; xi - y >= 0
    for (ExpressionId op : expr.operands) {
        constrainToSum(i, {op}, 0.0, numeric_limits<double>::infinity());
    }
}

void ToLinear::Transformer::linearizeOr(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_OR);
    // No operand true ==> !i; sum xi - y >= 0
    constrainToSum(i, expr.operands, 0.0, -numeric_limits<double>::infinity());
    // Any operand ==> i; xi - y <= 0
    for (ExpressionId op : expr.operands) {
        constrainToSum(i, {op}, numeric_limits<double>::infinity(), 0.0);
    }
}

void ToLinear::Transformer::linearizeXor(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_XOR);
    if (!model.isConstraint(i))
        THROW_ERROR("Xor is not handled yet");
    // TODO: Split the Xor into multiple intermediate xor, then linearize them
}

void ToLinear::run(PresolvedModel &model) {
    Transformer tf(model);
    tf.run();
}
} // namespace presolve
} // namespace umoi
