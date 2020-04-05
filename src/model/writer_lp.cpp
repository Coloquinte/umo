
#include "model/model.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

namespace umoi {

class ModelWriterLp {
  public:
    static constexpr int32_t InvalidId = -1;

  public:
    ModelWriterLp(const Model &m, ostream &s);

    void write();
    static vector<int32_t> getVarToId(const Model &m);

  protected:
    void initVarToId();

    string varName(uint32_t i) const;
    string exprName(ExpressionId id) const;
    void writeLpLinearExpression(uint32_t i);

    void check() const;

  private:
    const Model &m_;
    ostream &s_;
    vector<int32_t> varToId_;
};

constexpr int32_t ModelWriterLp::InvalidId;

ModelWriterLp::ModelWriterLp(const Model &m, ostream &s) : m_(m), s_(s) {}

void ModelWriterLp::write() {
    check();
    initVarToId();
    // Write the objectives
    if (m_.nbObjectives() == 1) {
        ExpressionId obj = m_.objective(0).first;
        bool dir = (m_.objective(0).second == UMO_OBJ_MAXIMIZE);
        if (obj.isMinus())
            dir = !dir;
        if (obj.isNot())
            dir = !dir;
        s_ << (dir ? "Maximize" : "Minimize") << endl;
        s_ << "\t" << varName(obj.var()) << endl;
    } else {
        s_ << "Maximize" << endl;
        s_ << "\t0" << endl;
    }
    // Write the constraints
    s_ << "Subject To" << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op != UMO_OP_LINEARCOMP)
            continue;
        if (!m_.isConstraintPos(i))
            continue;
        double lb = m_.value(expr.operands[0].var());
        double ub = m_.value(expr.operands[1].var());
        if (lb == ub && isfinite(lb)) {
            s_ << "\t";
            writeLpLinearExpression(i);
            s_ << " = " << lb << endl;
        } else {
            if (isfinite(lb)) {
                s_ << "\t";
                writeLpLinearExpression(i);
                s_ << " >= " << lb << endl;
            }
            if (isfinite(ub)) {
                s_ << "\t";
                writeLpLinearExpression(i);
                s_ << " <= " << ub << endl;
            }
        }
    }
    // Write the bounds
    s_ << "Bounds" << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_DEC_INT || expr.op == UMO_OP_DEC_FLOAT) {
            double lb = m_.value(expr.operands[0].var());
            double ub = m_.value(expr.operands[1].var());
            s_ << "\t" << lb << " <= " << varName(i) << " <= " << ub << endl;
        }
    }
    // Write the types
    bool binaryFound = false;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (m_.expression(i).op == UMO_OP_DEC_BOOL) {
            if (!binaryFound) {
                binaryFound = true;
                s_ << "Binary" << endl;
            }
            s_ << "\t" << varName(i) << endl;
        }
    }
    bool integerFound = false;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (m_.expression(i).op == UMO_OP_DEC_INT) {
            if (!integerFound) {
                integerFound = true;
                s_ << "General" << endl;
            }
            s_ << "\t" << varName(i) << endl;
        }
    }
    s_ << "End" << endl;
}

vector<int32_t> ModelWriterLp::getVarToId(const Model &m) {
    vector<int32_t> varToId(m.nbExpressions(), InvalidId);
    int32_t id = 0;
    for (uint32_t i = 0; i < m.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m.expression(i);
        if (expr.op == UMO_OP_DEC_BOOL)
            varToId[i] = id++;
        if (expr.op == UMO_OP_DEC_INT)
            varToId[i] = id++;
        if (expr.op == UMO_OP_DEC_FLOAT)
            varToId[i] = id++;
    }
    return varToId;
}

void ModelWriterLp::initVarToId() { varToId_ = getVarToId(m_); }

string ModelWriterLp::varName(uint32_t i) const {
    if (varToId_[i] == InvalidId) {
        THROW_ERROR("Expression " << i << " (operator " << m_.expression(i).op
                                  << ") hasn't been assigned a name");
    }
    stringstream s;
    s << "x" << varToId_[i];
    return s.str();
}

string ModelWriterLp::exprName(ExpressionId id) const {
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

void ModelWriterLp::check() const {
    if (m_.nbObjectives() > 1) {
        THROW_ERROR(
            "Multiple objectives are not supported by the LP file writer");
    }
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        umo_operator op = expr.op;
        if (op == UMO_OP_INVALID)
            continue;
        if (op == UMO_OP_CONSTANT)
            continue;
        if (op == UMO_OP_DEC_BOOL || op == UMO_OP_DEC_INT ||
            op == UMO_OP_DEC_FLOAT) {
            if (m_.isConstraint(i)) {
                THROW_ERROR(
                    "Variables cannot be constraints for the LP file writer");
            }
            continue;
        }
        if (op == UMO_OP_LINEARCOMP) {
            if (m_.isConstraintNeg(i) || !m_.isConstraintPos(i)) {
                THROW_ERROR("All comparisons must be constraints for the LP "
                            "file writer");
            }
            for (uint32_t j = 1; 2 * j + 1 < expr.operands.size(); ++j) {
                umo_operator operandOp =
                    m_.getExpressionIdOp(expr.operands[2 * j + 1]);
                if (!Operator::get(operandOp).isDecision()) {
                    THROW_ERROR("All operands of linear constraints must be "
                                "decision variables for the LP file writer");
                }
            }
            continue;
        }
        THROW_ERROR("Operator " << op
                                << " is not handled by the LP file writer");
    }
}

void ModelWriterLp::writeLpLinearExpression(uint32_t i) {
    const Model::ExpressionData &expr = m_.expression(i);
    for (uint32_t j = 1; 2 * j + 1 < expr.operands.size(); ++j) {
        double val = m_.value(expr.operands[2 * j].var());
        ExpressionId id = expr.operands[2 * j + 1];
        if (j > 1) {
            s_ << (val >= 0.0 ? " + " : " - ");
        } else {
            s_ << (val >= 0.0 ? " " : "- ");
        }
        s_ << abs(val) << " " << varName(id.var());
    }
}

void Model::writeLp(ostream &os) const {
    /*
     * Write the model in LP format for compatibility with MILP solvers
     */
    ModelWriterLp(*this, os).write();
}

void Model::readLpSol(istream &is) {
    /*
     * Read the solution in LP solution format
     */
    vector<int32_t> varToId = ModelWriterLp::getVarToId(*this);
    int32_t maxId = *max_element(varToId.begin(), varToId.end());
    unordered_map<string, double> values;
    // Read the status
    string firstLine;
    getline(is, firstLine);
    bool statusOptimal = false;
    if (firstLine.find("Optimal") != string::npos) {
        statusOptimal = true;
    }
    else if (firstLine.find("Infeasible") != string::npos) {
        setStatus(UMO_STATUS_INFEASIBLE);
        return;
    }
    else {
        THROW_ERROR("Cannot parse status in line \"" << firstLine <<"\"");
    }
    // Read the variable values
    while (is.good()) {
        int32_t varId;
        string varName;
        double value;
        double dualValue;
        is >> varId >> varName >> value >> dualValue;
        if (varId > maxId || varId < 0) THROW_ERROR("Out of bound variable index " << varId << " after LP solution for variable " << varName);
        values[varName] = value;
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterLp::InvalidId) {
            stringstream ss;
            ss << "x" << id;
            setFloatValue(ExpressionId::fromVar(i), values.at(ss.str()));
        }
    }
    if (statusOptimal) {
        setStatus(UMO_STATUS_OPTIMAL);
    }
}
} // namespace umoi
