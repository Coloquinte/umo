
#include "model/presolved_model.hpp"

#include "model/operator.hpp"

using namespace std;

namespace umoi {
PresolvedModel::PresolvedModel() {}

PresolvedModel::PresolvedModel(const Model &model) : Model(model) {
    for (size_t i = 0; i < expressions_.size(); ++i) {
        if (Operator::get(expressions_[i].op).isDecision()) {
            variableMapping_.emplace(i, ExpressionId(i, false, false));
        }
    }
}

void PresolvedModel::push(Model &model) {
    for (auto p : variableMapping_) {
        double val = getFloatValue(p.second);
        model.setFloatValue(ExpressionId(p.first, false, false), val);
    }
    // Push status for a model declared optimal or unfeasible
    model.setStatus(getStatus());
}

void PresolvedModel::pull(Model &model) {
    for (auto p : variableMapping_) {
        double val = model.getFloatValue(ExpressionId(p.first, false, false));
        if (Operator::get(getExpressionIdOp(p.second)).isDecision()) {
            // Only set decisions in the presolved model
            setFloatValue(p.second, val);
        }
    }
}

namespace {
ExpressionId composeExpressions(ExpressionId expr1, ExpressionId expr2) {
    if (expr1.isNot() && expr2.isMinus())
        throw runtime_error(
            "Attempting to compose NOT and MINUS when applying a new presolve");
    bool isNot = expr1.isNot() ^ expr2.isNot();
    bool isMinus = expr1.isMinus() ^ expr2.isMinus();
    return ExpressionId(expr2.var(), isNot, isMinus);
}
} // namespace

void PresolvedModel::apply(const PresolvedModel &next) {
    // Update the decision mapping
    unordered_map<unsigned int, ExpressionId> newMapping;
    const auto &map1 = variableMapping_;
    const auto &map2 = next.variableMapping_;
    for (auto p : map1) {
        // Find its definition in map2
        ExpressionId expr1 = p.second;
        auto it = map2.find(expr1.var());
        if (it == map2.end())
            throw runtime_error(
                "A decision variable was lost when applying a new presolve");
        ExpressionId expr2 = it->second;
        ExpressionId composed = composeExpressions(expr1, expr2);
        // Now apply the NOT/MINUS operations and push the new definition
        newMapping.emplace(p.first, composed);
    }

    *this = next;
    variableMapping_ = newMapping;
}
} // namespace umoi
