
#include "model/model.hpp"
#include "model/operator.hpp"
#include "utils/utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

namespace umoi {

class ModelWriterLp {
  public:
    static constexpr int32_t InvalidId = -1;
    static constexpr size_t maxLineLength = 256;

  public:
    ModelWriterLp(const Model &m, ostream &s);

    void write();
    static vector<int32_t> getVarToId(const Model &m);

  protected:
    void initVarToId();

    void writeObjective();
    void writeConstraints();
    void writeBounds();
    void writeIntegers();

    string varName(uint32_t i) const;
    string exprName(ExpressionId id) const;
    void writeLpLinearExpression(uint32_t i);

    void check() const;

  private:
    const Model &m_;
    ostream &s_;
    vector<int32_t> varToId_;
    vector<char> variableSeen_;
};

constexpr int32_t ModelWriterLp::InvalidId;

ModelWriterLp::ModelWriterLp(const Model &m, ostream &s) : m_(m), s_(s) {}

void ModelWriterLp::writeObjective() {
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
        assert (m_.nbObjectives() == 0);
        s_ << "Maximize" << endl;
        s_ << "\tdummy" << endl;
    }
}

void ModelWriterLp::writeConstraints() {
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
    // GLPK crashes if the constraint section is empty or if there is no objective
    s_ << "\tdummy = 0" << endl;
}

void ModelWriterLp::writeBounds() {
    s_ << "Bounds" << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_DEC_BOOL && !variableSeen_[i]) {
            // Scip crashes if a binary variable is not referenced before the Binary section
            s_ << "\t0 <= " << varName(i) << " <= 1" << endl;
        }
        if (expr.op == UMO_OP_DEC_INT || expr.op == UMO_OP_DEC_FLOAT) {
            double lb = m_.value(expr.operands[0].var());
            double ub = m_.value(expr.operands[1].var());
            s_ << "\t";
            if (std::isfinite(lb)) {
                s_ << lb;
            }
            else {
                if (!(lb < 0)) {
                    throw std::runtime_error("Infinite lower bound should be negative");
                }
                s_ << "-inf";
            }
            s_ << " <= " << varName(i) << " <= ";
            if (std::isfinite(ub)) {
                s_ << ub << endl;
            }
            else {
                if (!(ub > 0)) {
                    throw std::runtime_error("Infinite upper bound should be positive");
                }
                s_ << "+inf" << endl;
            }
        }
    }
}

void ModelWriterLp::writeIntegers() {
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
}

void ModelWriterLp::write() {
    check();
    initVarToId();
    writeObjective();
    writeConstraints();
    writeBounds();
    writeIntegers();
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

void ModelWriterLp::initVarToId() {
    varToId_ = getVarToId(m_);
    variableSeen_ = vector<char>(m_.nbExpressions(), 0);
}

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
                    "Variables cannot be direct constraints for the LP file writer");
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
    stringstream s;
    for (uint32_t j = 1; 2 * j + 1 < expr.operands.size(); ++j) {
        double val = m_.value(expr.operands[2 * j].var());
        ExpressionId id = expr.operands[2 * j + 1];
        variableSeen_[id.var()] = true;
        if (j > 1) {
            s << (val >= 0.0 ? " + " : " - ");
        } else {
            s << (val >= 0.0 ? " " : "- ");
        }
        s << abs(val) << " " << varName(id.var());
        if (s.str().size() > maxLineLength - 20) {
            // Enforce a small-enough line length
            s_ << s.str() << endl;
            s.str("");
        }
    }
    s_ << s.str();
}

void Model::writeLp(ostream &os) const { ModelWriterLp(*this, os).write(); }

namespace {
umo_solution_status readStatusCbc(istream &is) {
    string firstLine;
    getline(is, firstLine);
    if (firstLine.find("Optimal") != string::npos) {
        return UMO_STATUS_OPTIMAL;
    } else if (firstLine.find("Unbounded") != string::npos) {
        return UMO_STATUS_UNBOUNDED;
    } else if (firstLine.find("Integer") != string::npos) {
        return UMO_STATUS_UNKNOWN;
    } else if (firstLine.find("Infeasible") != string::npos) {
        return UMO_STATUS_INFEASIBLE;
    } else {
        THROW_ERROR("Cannot parse status in line \"" << firstLine << "\"");
    }
}

umo_solution_status readStatusScip(istream &is) {
    string firstLine;
    getline(is, firstLine);
    if (firstLine.find("optimal") != string::npos) {
        return UMO_STATUS_OPTIMAL;
    } else if (firstLine.find("infeasible or unbounded") != string::npos) {
        return UMO_STATUS_UNKNOWN;
    } else if (firstLine.find("limit reached") != string::npos) {
        return UMO_STATUS_UNKNOWN;
    } else if (firstLine.find("interrupt") != string::npos) {
        return UMO_STATUS_UNKNOWN;
    } else if (firstLine.find("unbounded") != string::npos) {
        return UMO_STATUS_UNBOUNDED;
    } else if (firstLine.find("infeasible") != string::npos) {
        return UMO_STATUS_INFEASIBLE;
    } else {
        THROW_ERROR("Cannot parse status in line \"" << firstLine << "\"");
    }
}

umo_solution_status readStatusGlpk(istream &is) {
    string line;
    getline(is, line);
    getline(is, line);
    getline(is, line);
    getline(is, line);
    getline(is, line);
    if (line.size() > 12) {
        string statusLine = line.substr(12);
        if (statusLine == "INTEGER OPTIMAL") {
            return UMO_STATUS_OPTIMAL;
        } else if (statusLine == "OPTIMAL") {
            return UMO_STATUS_OPTIMAL;
        } else if (statusLine == "INTEGER NON-OPTIMAL") {
            return UMO_STATUS_UNKNOWN;
        } else if (statusLine == "UNDEFINED") {
            return UMO_STATUS_UNKNOWN;
        } else if (statusLine == "UNBOUNDED") {
            return UMO_STATUS_UNBOUNDED;
        } else if (statusLine == "INFEASIBLE (FINAL)") {
            return UMO_STATUS_INFEASIBLE;
        } else if (statusLine == "INTEGER EMPTY") {
            return UMO_STATUS_INFEASIBLE;
        }
    }
    THROW_ERROR("Cannot parse status in line \"" << line << "\"");
}
} // namespace

void Model::readLpSolCbc(istream &is) {
    vector<int32_t> varToId = ModelWriterLp::getVarToId(*this);
    int32_t maxId = *max_element(varToId.begin(), varToId.end());
    umo_solution_status status = readStatusCbc(is);
    if (status == UMO_STATUS_INFEASIBLE) {
        setStatus(UMO_STATUS_INFEASIBLE);
        return;
    }
    unordered_map<string, double> values;
    while (is.good()) {
        int32_t varId;
        string varName;
        double value;
        double dualValue;
        is >> varId >> varName >> value >> dualValue;
        if (is.fail())
            break;
        if (varId > maxId+1 /* +1 for dummy */ || varId < 0)
            THROW_ERROR("Out of bound variable index "
                        << varId << " after LP solution for variable "
                        << varName);
        values[varName] = value;
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterLp::InvalidId) {
            stringstream ss;
            ss << "x" << id;
            auto it = values.find(ss.str());
            double val = it != values.end() ? it->second : 0.0;
            setFloatValue(ExpressionId::fromVar(i), val);
        }
    }
    if (status != UMO_STATUS_UNKNOWN) {
        setStatus(status);
    }
}

void Model::readLpSolScip(istream &is) {
    vector<int32_t> varToId = ModelWriterLp::getVarToId(*this);
    int32_t maxId = *max_element(varToId.begin(), varToId.end());
    umo_solution_status status = readStatusScip(is);
    if (status == UMO_STATUS_INFEASIBLE) {
        setStatus(UMO_STATUS_INFEASIBLE);
        return;
    }
    unordered_map<string, double> values;
    while (is.good()) {
        string line;
        getline(is, line);
        stringstream ss(line);
        string varName;
        double value;
        ss >> varName >> value;
        if (is.fail())
            break;
        values[varName] = value;
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterLp::InvalidId) {
            stringstream ss;
            ss << "x" << id;
            auto it = values.find(ss.str());
            double val = it != values.end() ? it->second : 0.0;
            setFloatValue(ExpressionId::fromVar(i), val);
        }
    }
    if (status != UMO_STATUS_UNKNOWN) {
        setStatus(status);
    }
}

void Model::readLpSolGlpk(istream &is) {
    vector<int32_t> varToId = ModelWriterLp::getVarToId(*this);
    int32_t maxId = *max_element(varToId.begin(), varToId.end());
    umo_solution_status status = readStatusGlpk(is);
    if (status == UMO_STATUS_INFEASIBLE) {
        setStatus(UMO_STATUS_INFEASIBLE);
        return;
    }
    unordered_map<string, double> values;
    while (is.good()) {
        string line;
        getline(is, line);
        if (line.find("Column name") != string::npos) {
            getline(is, line);
            break;
        }
    }
    for (int i = 0; i <= maxId+1 /* +1 for dummy */; ++i) {
        if (!is.good())
            break;
        string line;
        getline(is, line);
        stringstream ss(line);
        int varId;
        string varName;
        string filler;
        double value;
        ss >> varId >> varName >> filler >> value;
        if (is.fail())
            break;
        values[varName] = value;
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterLp::InvalidId) {
            stringstream ss;
            ss << "x" << id;
            auto it = values.find(ss.str());
            double val = it != values.end() ? it->second : 0.0;
            setFloatValue(ExpressionId::fromVar(i), val);
        }
    }
    if (status != UMO_STATUS_UNKNOWN) {
        setStatus(status);
    }
    //THROW_ERROR("GLPK reader is not implemented yet");
}

void Model::readLpSolGurobi(istream &is) {
    vector<int32_t> varToId = ModelWriterLp::getVarToId(*this);
    int32_t maxId = *max_element(varToId.begin(), varToId.end());
    unordered_map<string, double> values;
    // WARNING: Gurobi does not give status information!!!!
    string firstLine;
    getline(is, firstLine);
    string secondLine;
    getline(is, secondLine);
    // Read the variable values
    while (is.good()) {
        string line;
        getline(is, line);
        stringstream ss(line);
        string varName;
        double value;
        ss >> varName >> value;
        if (is.fail())
            break;
        values[varName] = value;
    }
    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterLp::InvalidId) {
            stringstream ss;
            ss << "x" << id;
            auto it = values.find(ss.str());
            double val = it != values.end() ? it->second : 0.0;
            setFloatValue(ExpressionId::fromVar(i), val);
        }
    }
    // WARNING: We assume optimality like Pulp, but it may not be the case
    if (getStatus() == UMO_STATUS_VALID) {
        setStatus(UMO_STATUS_OPTIMAL);
    }
}

void Model::readLpSolCplex(istream &is) {
    THROW_ERROR("Cplex reader is not implemented yet");
}
} // namespace umoi
