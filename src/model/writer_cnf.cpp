
#include "model/model.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;

namespace umoi {

class ModelWriterCnf {
  public:
    static constexpr uint32_t InvalidId = 0;

  public:
    ModelWriterCnf(const Model &m, ostream &s);

    void write();
    static vector<uint32_t> getVarToId(const Model &m);

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

constexpr uint32_t ModelWriterCnf::InvalidId;

ModelWriterCnf::ModelWriterCnf(const Model &m, ostream &s) : m_(m), s_(s) {}

void ModelWriterCnf::write() {
    check();
    initVarToId();

    s_ << "p cnf " << countClauses() << " " << countVars() << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_OR) {
            for (ExpressionId id : expr.operands) {
                uint32_t cnfId = varToId_[id.var()];
                if (cnfId == InvalidId)
                    THROW_ERROR("Attempting to write a variable without id (constant?) in CNF file writer");
                s_ << (id.isNot() ? "-" : "") << cnfId << " ";
            }
            s_ << "0" << endl;
        }
    }
}

vector<uint32_t> ModelWriterCnf::getVarToId(const Model &m) {
    vector<uint32_t> varToId(m.nbExpressions(), InvalidId);
    // Start at one to make it simpler
    uint32_t id = 1;
    for (uint32_t i = 0; i < m.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m.expression(i);
        if (expr.op == UMO_OP_DEC_BOOL)
            varToId[i] = id++;
    }
    return varToId;
}

void ModelWriterCnf::initVarToId() { varToId_ = getVarToId(m_); }

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
                THROW_ERROR(
                    "Variables cannot be constraints for the CNF file writer");
            }
            continue;
        }
        if (op == UMO_OP_OR) {
            if (m_.isConstraintNeg(i) || !m_.isConstraintPos(i)) {
                THROW_ERROR(
                    "All clauses must be constraints for the CNF file writer");
            }
            continue;
        }
        THROW_ERROR("Operator " << op
                                << " is not handled by the CNF file writer");
    }
}

void Model::writeCnf(ostream &os) const {
    /*
     * Write the model in Dimacs CNF format for compatibility with SAT solvers
     */
    ModelWriterCnf(*this, os).write();
}

void Model::readCnfSolMinisat(istream &is) {
    /*
     * Read the solution in Dimacs CNF solution format
     */
    vector<uint32_t> varToId = ModelWriterCnf::getVarToId(*this);
    uint32_t maxId = *max_element(varToId.begin(), varToId.end());
    vector<char> polarities(maxId + 1);
    // Read the status
    string firstLine;
    getline(is, firstLine);
    if (firstLine == "UNSAT") {
        setStatus(UMO_STATUS_INFEASIBLE);
        return;
    }
    // Read the variable values
    while (is.good()) {
        long long v;
        is >> v;
        if (v == 0)
            break;
        if (v > 0) {
            polarities.at(v) = 1;
        } else {
            polarities.at(-v) = -1;
        }
    }
    // Update the model
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterCnf::InvalidId) {
            char polarity = polarities.at(id);
            if (polarity < 0) {
                setFloatValue(ExpressionId::fromVar(i), 0.0);
            } else if (polarity > 0) {
                setFloatValue(ExpressionId::fromVar(i), 1.0);
            }
        }
    }
    setStatus(UMO_STATUS_OPTIMAL);
}
} // namespace umoi
