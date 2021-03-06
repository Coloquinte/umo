
#include "presolve/to_linear.hpp"
#include "model/operator.hpp"
#include "utils/utils.hpp"

#include <cassert>
#include <limits>

using namespace std;

namespace umoi {
namespace presolve {

class ToLinear::Transformer {
  public:
    Transformer(PresolvedModel &);
    void run();

    void createExpressions();
    void createObjectives();
    void linearizeExpressions();

    void linearize(uint32_t i);

    void linearizeSum(uint32_t i);
    void linearizeProd(uint32_t i);
    void linearizeCompare(uint32_t i);
    void linearizeAnd(uint32_t i);
    void linearizeOr(uint32_t i);
    void linearizeXor(uint32_t i);
    void copyExpression(uint32_t i);

    void linearizeConstrainedEq(ExpressionId op1, ExpressionId op2);
    void linearizeConstrainedLess(ExpressionId op1, ExpressionId op2,
                                  double margin);
    void linearizeConstrainedLeq(ExpressionId op1, ExpressionId op2);
    void linearizeConstrainedLt(ExpressionId op1, ExpressionId op2);

    // Helper function: get affine coefficients for a compressed (not/minus)
    // expression
    Element getElement(ExpressionId expr, bool useOriginalId=true) const;
    // Helper function: get ExpressionId for a compressed (not/minus) expression
    ExpressionId getExpressionId(ExpressionId expr);

    // Helper function: direct expression of a constraint
    void makeConstraint(const vector<double> &coefs,
                        const vector<ExpressionId> &operands, double lb,
                        double ub, bool useOriginalId=true);
    void makeConstraint(const vector<double> &coefs,
                        const vector<Element> &operands, double lb,
                        double ub);
    // Helper function: constrain variable i to be within certain bounds of the
    // summed operands
    void constrainToSum(uint32_t i, const vector<ExpressionId> &operands,
                        double lb, double ub);
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

    const double strictEqualityMargin = 1.0e-6;
};

struct ToLinear::Element {
    uint32_t var;
    double coef;
    double constant;
};

ToLinear::Transformer::Transformer(PresolvedModel &model) : model(model) {
    constantMInf =
        linearModel.createConstant(-numeric_limits<double>::infinity());
    constantPInf =
        linearModel.createConstant(numeric_limits<double>::infinity());
    constantZero = linearModel.createConstant(0.0);
    constantPOne = linearModel.createConstant(1.0);
    constantMOne = linearModel.createConstant(-1.0);
}

void ToLinear::Transformer::createExpressions() {
    // Copy expressions
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        const auto &expr = model.expression(i);
        if (expr.op == UMO_OP_INVALID)
            continue;
        if (expr.op == UMO_OP_CONSTANT)
            continue;
        if (model.isConstraint(i)) {
            if (!model.isConstraintPos(i)) {
                linearModel.mapping().emplace(i, constantZero);
            } else if (!model.isConstraintNeg(i)) {
                linearModel.mapping().emplace(i, constantPOne);
            } else {
                throw runtime_error(
                    "Inconsistent opposite constraints are not supported yet");
            }
        } else if (model.isDecision(i)) {
            ExpressionId newId;
            if (expr.op == UMO_OP_DEC_BOOL) {
                newId = linearModel.createExpression(UMO_OP_DEC_BOOL, {});
            } else if (expr.op == UMO_OP_DEC_INT ||
                       expr.op == UMO_OP_DEC_FLOAT) {
                ExpressionId lb = linearModel.createConstant(
                    model.getExpressionIdValue(expr.operands[0]));
                ExpressionId ub = linearModel.createConstant(
                    model.getExpressionIdValue(expr.operands[1]));
                newId = linearModel.createExpression(expr.op, {lb, ub});
            }
            linearModel.mapping().emplace(i, newId);
        } else {
            ExpressionId newId;
            switch (expr.type) {
            case UMO_TYPE_BOOL:
                newId = linearModel.createExpression(UMO_OP_DEC_BOOL, {});
                break;
            case UMO_TYPE_INT:
                newId = linearModel.createExpression(
                    UMO_OP_DEC_INT, {constantMInf, constantPInf});
                break;
            case UMO_TYPE_FLOAT:
                newId = linearModel.createExpression(
                    UMO_OP_DEC_FLOAT, {constantMInf, constantPInf});
                break;
            default:
                THROW_ERROR("Invalid type " << expr.type << " encountered");
            }
            linearModel.mapping().emplace(i, newId);
        }
    }
}

void ToLinear::Transformer::linearizeExpressions() {
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        linearize(i);
    }
}

void ToLinear::Transformer::createObjectives() {
    for (const auto obj : model.objectives()) {
        Element elt = getElement(obj.first);
        bool maxObj = (obj.second == UMO_OBJ_MAXIMIZE) ^ (elt.coef < 0.0);
        umo_objective_direction dir =
            maxObj ? UMO_OBJ_MAXIMIZE : UMO_OBJ_MINIMIZE;
        linearModel.createObjective(ExpressionId(elt.var, false, false), dir);
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
    case UMO_OP_LINEARCOMP:
        // Copy the expression as is
        copyExpression(i);
        break;
    default:
        THROW_ERROR("Operand type " << op << " not handled for linearization");
    }
}

void ToLinear::Transformer::run() {
    createExpressions();
    createObjectives();
    linearizeExpressions();
    model.apply(linearModel);
}

ToLinear::Element ToLinear::Transformer::getElement(ExpressionId id, bool useOriginalId) const {
    ExpressionId pid;
    if (useOriginalId) {
        if (model.isConstant(id.var())) {
            Element elt;
            elt.var = constantZero.var();
            elt.constant = model.getExpressionIdValue(id);
            elt.coef = 1.0;
            return elt;
        }
        else {
            pid = linearModel.mapping().at(id.var());
        }
    }
    else {
        pid = ExpressionId::fromVar(id.var());
    }
    assert(pid.isVar());
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

ExpressionId ToLinear::Transformer::getExpressionId(ExpressionId id) {
    if (model.isConstant(id.var())) {
        return linearModel.createConstant(model.getExpressionIdValue(id));
    }
    ExpressionId pid = linearModel.mapping().at(id.var());
    assert (!id.isNot() || !pid.isMinus() /*Inconsistent types (applying not to a general integer)*/);
    if (id.isNot())
        pid = pid.getNot();
    if (id.isMinus())
        pid = pid.getMinus();
    return pid;
}

void ToLinear::Transformer::makeConstraint(const vector<double> &coefs,
                                           const vector<Element> &ops,
                                           double lb, double ub) {
    if (lb > ub) {
        THROW_ERROR(
            "Lower bound ("
            << lb << ") bigger than upper bound (" << ub
            << ") during linearization: the model is obviously inconsistent");
    }
    assert(coefs.size() == ops.size());
    vector<ExpressionId> operands;
    // Reserve space for the bounds
    operands.emplace_back();
    operands.emplace_back();
    // Overall offset for the constraint's bounds
    double offset = 0.0;
    // Gather all operands; handle constants and compressed operands efficiently
    for (uint32_t i = 0; i < coefs.size(); ++i) {
        double coef = coefs[i];
        Element elt = ops[i];
        if (linearModel.isConstant(elt.var)) {
            // Constant in the linear model (happens for example when
            // replacing a constraint)
            offset += coef * (elt.constant +
                              elt.coef * linearModel.value(elt.var));
        } else {
            operands.push_back(linearModel.createConstant(coef * elt.coef));
            operands.push_back(ExpressionId(elt.var, false, false));
            offset += coef * elt.constant;
        }
    }
    // Lower/upper bound of the constraint
    lb -= offset;
    ub -= offset;
    if (operands.size() == 2) {
        // TODO: add some margin here
        if (lb > 0.0 || ub < 0.0) {
            // Obviously invalid
            THROW_ERROR("Constraint is obviously infeasible");
        } else {
            // Obviously valid
            return;
        }
    }
    ExpressionId lbId = linearModel.createConstant(lb);
    ExpressionId ubId = linearModel.createConstant(ub);
    operands[0] = lbId;
    operands[1] = ubId;
    // Constraint creation
    ExpressionId linearized =
        linearModel.createExpression(UMO_OP_LINEARCOMP, operands);
    linearModel.createConstraint(linearized);
}

void ToLinear::Transformer::makeConstraint(const vector<double> &coefs,
                                           const vector<ExpressionId> &ops,
                                           double lb, double ub, bool useOriginalId) {
    vector<Element> elements;
    elements.reserve(ops.size());
    for (ExpressionId op : ops) {
        elements.push_back(getElement(op, useOriginalId));
    }
    makeConstraint(coefs, elements, lb, ub);
}

void ToLinear::Transformer::constrainToSum(uint32_t i,
                                           const vector<ExpressionId> &ops,
                                           double lb, double ub) {
    vector<double> coefs(ops.size(), 1.0);
    coefs.push_back(-1.0);
    vector<ExpressionId> operands = ops;
    operands.push_back(ExpressionId(i, false, false));
    makeConstraint(coefs, operands, lb, ub);
}

void ToLinear::Transformer::constrainToProd(uint32_t i, ExpressionId op,
                                            double factor) {
    vector<double> coefs = {factor, -1.0};
    vector<ExpressionId> ops = {op, ExpressionId(i, false, false)};
    makeConstraint(coefs, ops, 0.0, 0.0);
}

void ToLinear::Transformer::linearizeConstrainedEq(ExpressionId op1,
                                                   ExpressionId op2) {
    vector<double> coefs = {1.0, -1.0};
    vector<ExpressionId> ops = {op1, op2};
    makeConstraint(coefs, ops, 0.0, 0.0);
}

void ToLinear::Transformer::linearizeConstrainedLess(ExpressionId op1,
                                                     ExpressionId op2,
                                                     double margin) {
    vector<double> coefs = {1.0, -1.0};
    vector<ExpressionId> ops = {op1, op2};
    makeConstraint(coefs, ops, -numeric_limits<double>::infinity(), -margin);
}

void ToLinear::Transformer::linearizeConstrainedLeq(ExpressionId op1,
                                                    ExpressionId op2) {
    linearizeConstrainedLess(op1, op2, 0.0);
}

void ToLinear::Transformer::linearizeConstrainedLt(ExpressionId op1,
                                                   ExpressionId op2) {
    linearizeConstrainedLess(op1, op2, strictEqualityMargin);
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
            constantProd *= model.getExpressionIdValue(id);
        } else if (!variableProd.valid()) {
            variableProd = id;
        } else {
            THROW_ERROR(
                "Impossible to linearize product with multiple variables");
        }
    }
    if (variableProd.valid()) {
        constrainToProd(i, variableProd, constantProd);
    } else {
        constrainToSum(i, {}, -constantProd, -constantProd);
    }
}

void ToLinear::Transformer::linearizeCompare(uint32_t i) {
    if (!model.isConstraint(i))
        THROW_ERROR("Comparisons that are not constraints are not handled yet");
    if (model.isConstraintPos(i) && model.isConstraintNeg(i))
        THROW_ERROR("The variable is constrained both ways. Model is "
                    "obviously inconsistent");

    const auto &expr = model.expression(i);
    ExpressionId op1 = expr.operands[0];
    ExpressionId op2 = expr.operands[1];

    if (model.isConstraintPos(i)) {
        switch (expr.op) {
        case UMO_OP_CMP_EQ:
            linearizeConstrainedEq(op1, op2);
            break;
        case UMO_OP_CMP_NEQ:
            THROW_ERROR("Impossible to constrain not equal");
            break;
        case UMO_OP_CMP_LEQ:
            linearizeConstrainedLeq(op1, op2);
            break;
        case UMO_OP_CMP_GEQ:
            linearizeConstrainedLeq(op2, op1);
            break;
        case UMO_OP_CMP_LT:
            linearizeConstrainedLt(op1, op2);
            break;
        case UMO_OP_CMP_GT:
            linearizeConstrainedLt(op2, op1);
            break;
        default:
            THROW_ERROR("Operator is not handled");
            break;
        }
    } else {
        switch (expr.op) {
        case UMO_OP_CMP_EQ:
            THROW_ERROR("Impossible to constrain not equal");
            break;
        case UMO_OP_CMP_NEQ:
            linearizeConstrainedEq(op1, op2);
            break;
        case UMO_OP_CMP_LEQ:
            linearizeConstrainedLt(op2, op1);
            break;
        case UMO_OP_CMP_GEQ:
            linearizeConstrainedLt(op1, op2);
            break;
        case UMO_OP_CMP_LT:
            linearizeConstrainedLeq(op2, op1);
            break;
        case UMO_OP_CMP_GT:
            linearizeConstrainedLeq(op1, op2);
            break;
        default:
            THROW_ERROR("Operator is not handled");
            break;
        }
    }
}

void ToLinear::Transformer::linearizeAnd(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_AND);
    // all operands true ==> y; sum xi - y <= n-1
    constrainToSum(i, expr.operands, -numeric_limits<double>::infinity(),
                   expr.operands.size() - 1);
    // y ==> Any operand; xi - y >= 0
    for (ExpressionId op : expr.operands) {
        constrainToSum(i, {op}, 0.0, numeric_limits<double>::infinity());
    }
}

void ToLinear::Transformer::linearizeOr(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_OR);
    // No operand true ==> !i; sum xi - y >= 0
    constrainToSum(i, expr.operands, 0.0, numeric_limits<double>::infinity());
    // Any operand ==> i; xi - y <= 0
    for (ExpressionId op : expr.operands) {
        constrainToSum(i, {op}, -numeric_limits<double>::infinity(), 0.0);
    }
}

void ToLinear::Transformer::linearizeXor(uint32_t i) {
    const auto &expr = model.expression(i);
    assert(expr.op == UMO_OP_XOR);
    vector<ExpressionId> operands = expr.operands;
    ExpressionId id = ExpressionId::fromVar(i).getNot();
    Element elt = getElement(id, true);
    for (ExpressionId op : operands) {
        ExpressionId xorId = linearModel.createExpression(UMO_OP_DEC_BOOL, {});
        Element elt1 = getElement(op, true);
        Element elt2 = getElement(xorId, false);
        makeConstraint({-1.0, 1.0, 1.0}, {elt1, elt2, elt}, 0.0, numeric_limits<double>::infinity());
        makeConstraint({1.0, -1.0, 1.0}, {elt1, elt2, elt}, 0.0, numeric_limits<double>::infinity());
        makeConstraint({1.0, 1.0, -1.0}, {elt1, elt2, elt}, 0.0, numeric_limits<double>::infinity());
        makeConstraint({-1.0, -1.0, -1.0}, {elt1, elt2, elt}, -2.0, numeric_limits<double>::infinity());
        elt = elt2;
    }
    makeConstraint({1.0}, {elt}, 1.0, numeric_limits<double>::infinity());
}

void ToLinear::Transformer::copyExpression(uint32_t i) {
    const auto &expr = model.expression(i);
    vector<ExpressionId> operands;
    operands.reserve(expr.operands.size());
    for (ExpressionId id : expr.operands) {
        operands.push_back(getExpressionId(id));
    }
    ExpressionId linearized = linearModel.createExpression(expr.op, operands);
    if (model.isConstraintPos(i))
        linearModel.createConstraint(linearized);
    if (model.isConstraintNeg(i))
        linearModel.createConstraint(linearized.getNot());
}

namespace {
int countNonConstantOperands(const PresolvedModel &model, const Model::ExpressionData &expr) {
    int nbNonConstant = 0;
    for (ExpressionId id : expr.operands) {
        if (!model.isConstant(id.var())) {
            ++nbNonConstant;
        }
    }
    return nbNonConstant;
}
}

bool ToLinear::valid(const PresolvedModel &model) const {
    if (model.nbObjectives() > 1)
        return false;
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        const auto &expr = model.expression(i);
        switch (expr.op) {
        case UMO_OP_INVALID:
        case UMO_OP_CONSTANT:
        case UMO_OP_DEC_BOOL:
        case UMO_OP_DEC_INT:
        case UMO_OP_DEC_FLOAT:
        case UMO_OP_AND:
        case UMO_OP_OR:
        case UMO_OP_SUM:
            continue;
        case UMO_OP_PROD:
            if (countNonConstantOperands(model, expr) >= 2) {
                return false;
            }
            continue;
        case UMO_OP_CMP_EQ:
        case UMO_OP_CMP_NEQ:
        case UMO_OP_CMP_LEQ:
        case UMO_OP_CMP_GEQ:
        case UMO_OP_CMP_LT:
        case UMO_OP_CMP_GT:
            if (!model.isConstraint(i)) {
                return false;
            }
            if (model.isConstraintPos(i) && model.isConstraintNeg(i)) {
                return false;
            }
            continue;
        case UMO_OP_LINEARCOMP:
            if (!model.isConstraintPos(i) || model.isConstraintNeg(i)) {
                return false;
            }
            continue;
        default:
            return false;
        }
    }
    return true;
}

void ToLinear::run(PresolvedModel &model) const {
    Transformer tf(model);
    tf.run();
}
} // namespace presolve
} // namespace umoi
