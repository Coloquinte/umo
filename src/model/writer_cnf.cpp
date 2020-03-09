
#include "model/model.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace umoi;
using namespace std;

namespace umoi {

class ModelWriterCnf {
  public:
    ModelWriterCnf(const Model &m, ostream &s);

    void write();

  protected:
    void initVarToId();

    uint32_t countClauses() const;
    uint32_t countVars() const;

    void check() const;

  private:
    const Model &m_;
    ostream &s_;
    vector<uint32_t> varToId_;
};

ModelWriterCnf::ModelWriterCnf(const Model &m, ostream &s)
: m_(m)
, s_(s) {
}

void ModelWriterCnf::write() {
    check();
    initVarToId();

    s_ << "p cnf " << countClauses() << " " << countVars() << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_OR) {
            for (ExpressionId id : expr.operands) {
                s_ << (id.isNot() ? "-" : "") << id.var() << " ";
            }
            s_ << "0" << endl;
        }
    }
}

void ModelWriterCnf::initVarToId() {
    varToId_.assign(m_.nbExpressions(), -1);
    // Start at one to make it simpler
    uint32_t id = 1;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_DEC_BOOL)
            varToId_[i] = id++;
    }
}

uint32_t ModelWriterCnf::countClauses() const {
    uint32_t cnt = 0;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (m_.expression(i).op == UMO_OP_OR) {
            ++cnt;
        }
    }
    return cnt;
}

uint32_t ModelWriterCnf::countVars() const {
    uint32_t cnt = 0;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (m_.expression(i).op == UMO_OP_DEC_BOOL) {
            ++cnt;
        }
    }
    return cnt;
}

void ModelWriterCnf::check() const {
    if (m_.nbObjectives() > 0) {
        THROW_ERROR("Objectives are not supported by the CNF file writer");
    }
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        umo_operator op = m_.expression(i).op;
        if (op == UMO_OP_INVALID)
            continue;
        if (op == UMO_OP_CONSTANT)
            continue;
        if (op == UMO_OP_DEC_BOOL) {
            if (m_.isConstraint(i)) {
                THROW_ERROR("Variables cannot be constraints for the CNF file writer");
            }
            continue;
        }
        if (op == UMO_OP_OR) {
            if (m_.isConstraintNeg(i) || !m_.isConstraintPos(i)) {
                THROW_ERROR("All clauses must be constraints for the CNF file writer");
            }
            continue;
        }
        THROW_ERROR("Operator " << op << " is not handled by the CNF file writer");
    }
}

void Model::writeCnf(ostream &os) const {
    /*
     * Write the model in CNF format for compatibility with SAT solvers
     */
    ModelWriterCnf(*this, os).write();
}

} // namespace umoi
