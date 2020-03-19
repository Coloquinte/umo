
#include "presolve/to_sat.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <cassert>

using namespace std;

namespace umoi {
namespace presolve {

class ToSat::Transformer {
  public:
    Transformer(PresolvedModel &);
    void run();

    void createExpressions();
    void satify(uint32_t i);

    void constrainPos(uint32_t i);
    void constrainNeg(uint32_t i);
    void satifyAnd(uint32_t i);
    void satifyOr(uint32_t i);
    void satifyXor(uint32_t i);
    void satifyConstrainedAnd(uint32_t i);
    void satifyConstrainedOr(uint32_t i);
    void satifyConstrainedXor(uint32_t i);
    void satifyConstrainedNAnd(uint32_t i);
    void satifyConstrainedNOr(uint32_t i);
    void satifyConstrainedNXor(uint32_t i);

    // Helper function: direct expression of a generalized and
    void satifyConstrainedGeneralized(uint32_t i, bool inInv, bool outInv);
    void satifyGeneralized(uint32_t i, bool inInv, bool outInv);

    // Helper function: direct expression of a clause
    void makeClause(const vector<ExpressionId> &operands);
    ExpressionId getMapping(ExpressionId id) const;

  private:
    PresolvedModel &model;
    PresolvedModel satModel;

    ExpressionId constantZero;
    ExpressionId constantOne;
};

ToSat::Transformer::Transformer(PresolvedModel &model) : model(model) {
    constantZero = satModel.createConstant(0.0);
    constantOne = satModel.createConstant(1.0);
}

void ToSat::Transformer::createExpressions() {
    // Copy expressions
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        const auto &expr = model.expression(i);
        if (expr.op == UMO_OP_INVALID)
            continue;
        if (expr.op == UMO_OP_CONSTANT)
            continue;
        if (model.isDecision(i)) {
            if (expr.op != UMO_OP_DEC_BOOL) {
                throw runtime_error("Only boolean expressions are supported");
            }
            ExpressionId newId = satModel.createExpression(UMO_OP_DEC_BOOL, {});
            satModel.mapping().emplace(i, newId);
        } else {
            if (expr.type != UMO_TYPE_BOOL) {
                throw runtime_error("Only boolean expressions are supported");
            }
            if (model.isConstraint(i)) {
                // All constraints are converted without additional variables
                continue;
            }
            ExpressionId newId = satModel.createExpression(UMO_OP_DEC_BOOL, {});
            satModel.mapping().emplace(i, newId);
        }
    }
}

void ToSat::Transformer::satify(uint32_t i) {
    if (model.isConstant(i)) {
        if (model.isConstraint(i)) {
            throw runtime_error("Constraints on constants are not supported");
        }
        return;
    }
    umo_operator op = model.expression(i).op;
    if (model.isConstraint(i)) {
        if (model.isConstraintPos(i)) {
            switch (op) {
            case UMO_OP_DEC_BOOL:
                constrainPos(i);
                break;
            case UMO_OP_AND:
                satifyConstrainedAnd(i);
                break;
            case UMO_OP_OR:
                satifyConstrainedOr(i);
                break;
            case UMO_OP_XOR:
                satifyConstrainedXor(i);
                break;
            default:
                THROW_ERROR("Operand type " << op << " not handled by SAT");
            }
        }
        if (model.isConstraintNeg(i)) {
            switch (op) {
            case UMO_OP_DEC_BOOL:
                constrainNeg(i);
                break;
            case UMO_OP_AND:
                satifyConstrainedNAnd(i);
                break;
            case UMO_OP_OR:
                satifyConstrainedNOr(i);
                break;
            case UMO_OP_XOR:
                satifyConstrainedNXor(i);
                break;
            default:
                THROW_ERROR("Operand type " << op << " not handled by SAT");
            }
        }
    }
    else {
        switch (op) {
        case UMO_OP_DEC_BOOL:
            break;
        case UMO_OP_AND:
            satifyAnd(i);
            break;
        case UMO_OP_OR:
            satifyOr(i);
            break;
        case UMO_OP_XOR:
            satifyXor(i);
            break;
        default:
            THROW_ERROR("Operand type " << op << " not handled by SAT");
        }
    }
}

void ToSat::Transformer::constrainPos(uint32_t i) {
    makeClause({ExpressionId::fromVar(i)});
}

void ToSat::Transformer::constrainNeg(uint32_t i) {
    makeClause({ExpressionId::fromVar(i).getNot()});
}

void ToSat::Transformer::satifyAnd(uint32_t i) {
    satifyGeneralized(i, false, false);
}

void ToSat::Transformer::satifyOr(uint32_t i) {
    satifyGeneralized(i, true, true);
}

void ToSat::Transformer::satifyXor(uint32_t i) {
    THROW_ERROR("Xor is not handled yet");
}

void ToSat::Transformer::satifyConstrainedAnd(uint32_t i) {
    satifyConstrainedGeneralized(i, false, false);
}

void ToSat::Transformer::satifyConstrainedOr(uint32_t i) {
    satifyConstrainedGeneralized(i, true, true);
}

void ToSat::Transformer::satifyConstrainedXor(uint32_t i) {
    THROW_ERROR("Xor is not handled yet");
}

void ToSat::Transformer::satifyConstrainedNAnd(uint32_t i) {
    satifyConstrainedGeneralized(i, false, true);
}

void ToSat::Transformer::satifyConstrainedNOr(uint32_t i) {
    satifyConstrainedGeneralized(i, true, false);
}

void ToSat::Transformer::satifyConstrainedNXor(uint32_t i) {
    THROW_ERROR("Xor is not handled yet");
}

void ToSat::Transformer::satifyConstrainedGeneralized(uint32_t i, bool inInv, bool outInv) {
    const Model::ExpressionData &expr = model.expression(i);
    if (outInv) {
        // Case of a or (single clause)
        vector<ExpressionId> operands;
        operands.reserve(expr.operands.size());
        for (ExpressionId id : expr.operands) {
            ExpressionId op = inInv ? id : id.getNot();
            operands.push_back(op);
        }
        makeClause(operands);
    }
    else {
        // Case of a and (all constrained)
        for (ExpressionId id : expr.operands) {
            ExpressionId op = inInv ? id.getNot() : id;
            makeClause({op});
        }
    }
}

void ToSat::Transformer::satifyGeneralized(uint32_t i, bool inInv, bool outInv) {
    const Model::ExpressionData &expr = model.expression(i);
    ExpressionId idOut = ExpressionId::fromVar(i);
    // Output --> Input implication
    vector<ExpressionId> operands;
    operands.reserve(expr.operands.size() + 1);
    for (ExpressionId id : expr.operands) {
        ExpressionId op = inInv ? id : id.getNot();
        operands.push_back(op);
    }
    operands.push_back(outInv ? idOut.getNot() : idOut);
    makeClause(operands);
    // Input --> Output implications
    for (ExpressionId id : expr.operands) {
        ExpressionId op = inInv ? id.getNot() : id;
        ExpressionId opOut = outInv ? idOut : idOut.getNot();
        makeClause({op, opOut});
    }
}

void ToSat::Transformer::makeClause(const vector<ExpressionId> &operands) {
    bool forceOne = false;
    vector<ExpressionId> nonConstantOperands;
    nonConstantOperands.reserve(operands.size());
    for (ExpressionId id : operands) {
        if (model.isConstant(id.var())) {
            forceOne |= (model.getExpressionIdValue(id) != 0.0);
        }
        else {
            nonConstantOperands.push_back(getMapping(id));
        }
    }
    if (!forceOne && !nonConstantOperands.empty()) {
        ExpressionId expr = satModel.createExpression(UMO_OP_OR, nonConstantOperands);
        satModel.createConstraint(expr);
    }
}

ExpressionId ToSat::Transformer::getMapping(ExpressionId id) const {
    ExpressionId pid = satModel.mapping().at(id.var());
    assert (pid.isVar());
    return id.isNot() ? pid.getNot() : pid;
}

void ToSat::Transformer::run() {
    createExpressions();
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        satify(i);
    }
    model.apply(satModel);
}

bool ToSat::valid(const PresolvedModel &model) const {
    for (uint32_t i = 0; i < model.nbExpressions(); ++i) {
        const auto &expr = model.expression(i);
        switch (expr.op) {
            case UMO_OP_INVALID:
            case UMO_OP_CONSTANT:
            case UMO_OP_DEC_BOOL:
            case UMO_OP_AND:
            case UMO_OP_OR:
                continue;
            default:
                return false;
        }
    }
    return true;
}

void ToSat::run(PresolvedModel &model) const {
    Transformer tf(model);
    tf.run();
}
} // namespace presolve
} // namespace umoi
