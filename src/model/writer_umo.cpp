
#include "model/model.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

namespace umoi {

class ModelWriterUmo {
  public:
    ModelWriterUmo(const Model &m, ostream &s);

    void write();

  protected:
    void initVarToId();

    string varName(uint32_t i) const;
    string exprName(ExpressionId id) const;

  private:
    const Model &m_;
    ostream &s_;
    vector<uint32_t> varToId_;
};

ModelWriterUmo::ModelWriterUmo(const Model &m, ostream &s) : m_(m), s_(s) {}

void ModelWriterUmo::write() {
    initVarToId();
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_INVALID)
            continue;
        if (expr.op == UMO_OP_CONSTANT)
            continue;
        s_ << varName(i) << " <- " << Operator::get(expr.op).toString();
        for (ExpressionId operand : expr.operands) {
            s_ << " " << exprName(operand);
        }
        s_ << endl;
    }
    vector<ExpressionId> constraints(m_.constraints().begin(),
                                     m_.constraints().end());
    sort(constraints.begin(), constraints.end());
    for (ExpressionId constraint : constraints) {
        s_ << "constraint " << exprName(constraint) << endl;
    }
    for (const auto obj : m_.objectives()) {
        if (obj.second == UMO_OBJ_MAXIMIZE) {
            s_ << "maximize ";
        } else {
            s_ << "minimize ";
        }
        s_ << exprName(obj.first) << endl;
    }
    s_ << endl;
}

void ModelWriterUmo::initVarToId() {
    varToId_.assign(m_.nbExpressions(), -1);
    uint32_t id = 0;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_INVALID)
            continue;
        if (expr.op == UMO_OP_CONSTANT)
            continue;
        varToId_[i] = id++;
    }
}

string ModelWriterUmo::varName(uint32_t i) const {
    if (varToId_[i] == (uint32_t)-1) {
        THROW_ERROR("Expression " << i << " (operator " << m_.expression(i).op
                                  << ") hasn't been assigned a name");
    }
    stringstream s;
    s << "x" << varToId_[i];
    return s.str();
}

string ModelWriterUmo::exprName(ExpressionId id) const {
    stringstream s;
    s << (id.isMinus() ? "-" : "");
    s << (id.isNot() ? "!" : "");
    umo_operator operandOp = m_.expression(id.var()).op;
    if (operandOp == UMO_OP_CONSTANT) {
        s << m_.value(id.var());
    } else {
        s << "x" << varToId_[id.var()];
    }
    return s.str();
}

void Model::writeUmo(ostream &os) const {
    /*
     * Write the model in a textual format for debug purposes
     */
    ModelWriterUmo(*this, os).write();
}
} // namespace umoi
