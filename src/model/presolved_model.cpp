
#include "model/presolved_model.hpp"
#include "model/operator.hpp"

using namespace umoi;
using namespace std;

void PresolvedModel::load(Model &model) {
    for (auto p : decisionMapping_) {
        double val = getFloatValue(p.second);
        model.setFloatValue(ExpressionId(p.first, false, false), val);
    }
}

PresolvedModel::PresolvedModel(const Model &model)
: Model(model) {
    for (size_t i = 0; i < expressions_.size(); ++i) {
        if (Operator::get(expressions_[i].op).isDecision()) {
            decisionMapping_.emplace(i, ExpressionId(i, false, false));
        }
    }
}

