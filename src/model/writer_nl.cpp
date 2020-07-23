
#include "model/model.hpp"
#include "model/operator.hpp"
#include "model/utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

namespace umoi {

/*
 * See https://ampl.github.io/nlwrite.pdf for a description of the NL format
 */

class ModelWriterNl {
  public:
    static constexpr int InvalidOp = -1;
    static constexpr int32_t InvalidId = -1;

  public:
    ModelWriterNl(const Model &m, ostream &s);

    void write();
    static vector<int32_t> getUmoToNlId(const Model &m);

    int countVariables() const;
    int countConstraints() const;
    int countObjectives() const;
    int countJacobianNonZeros() const;
    int countGradientNonZeros() const;

    void writeHeader(); // Header part
    void writeObjectives(); // "O" lines
    void writeNonLinearConstraints(); // "C" lines
    void writeConstraintBounds(); // "r" lines
    void writeVariableBounds(); // "b" lines
    void writeJacobianSize(); // "k" lines
    void writeLinearConstraints(); // "J" lines

    // TODO: write linear constraint
    void writeExpressionGraph(ExpressionId id);
    void writeLinearExpression(ExpressionId id);
    void writeBounds(double lb, double ub);

  private:
    void initUmoToNl();
    void initUmoToNlId();
    void initBoolVariables();
    void initIntVariables();
    void initFloatVariables();
    void initJacobianSize();

  private:
    const Model &m_;
    ostream &s_;
    vector<int> umoToNlOp_;
    vector<int32_t> varToId_;

    vector<uint32_t> boolVariables_;
    vector<uint32_t> intVariables_;
    vector<uint32_t> floatVariables_;
    vector<uint32_t> jacobianSize_;
};

constexpr int ModelWriterNl::InvalidOp;
constexpr int32_t ModelWriterNl::InvalidId;

ModelWriterNl::ModelWriterNl(const Model &m, ostream &s) : m_(m), s_(s) {}

int ModelWriterNl::countVariables() const {
    return boolVariables_.size() + intVariables_.size() + floatVariables_.size();
}

int ModelWriterNl::countConstraints() const {
    return m_.nbConstraints();
}

int ModelWriterNl::countObjectives() const {
    int ret = m_.nbObjectives();
    return ret >= 1 ? ret : 1; // Dummy objective
}

int ModelWriterNl::countJacobianNonZeros() const {
    return jacobianSize_.back();
}

int ModelWriterNl::countGradientNonZeros() const {
    return 0;
}

void ModelWriterNl::initBoolVariables() {
    boolVariables_.clear();
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        umo_operator op = m_.expression(i).op;
        if (op == UMO_OP_DEC_BOOL) {
            boolVariables_.push_back(i);
        }
    }
}

void ModelWriterNl::initIntVariables() {
    intVariables_.clear();
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        umo_operator op = m_.expression(i).op;
        if (op == UMO_OP_DEC_INT) {
            intVariables_.push_back(i);
        }
    }
}

void ModelWriterNl::initFloatVariables() {
    floatVariables_.clear();
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        umo_operator op = m_.expression(i).op;
        if (op == UMO_OP_DEC_FLOAT) {
            floatVariables_.push_back(i);
        }
    }
}

void ModelWriterNl::writeExpressionGraph(ExpressionId exprId) {
    std::vector<ExpressionId> toWrite;
    toWrite.push_back(exprId);
    while (!toWrite.empty()) {
        ExpressionId id = toWrite.back();
        toWrite.pop_back();
        umo_operator op = m_.getExpressionIdOp(id);
        if (op == UMO_OP_CONSTANT) {
            assert (varToId_[id.var()] == InvalidId);
            s_ << "n" << m_.value(id.var()) << endl;
        }
        else if (op == UMO_OP_DEC_BOOL || op == UMO_OP_DEC_INT ||
            op == UMO_OP_DEC_FLOAT) {
            assert (varToId_[id.var()] != InvalidId);
            s_ << "v" << varToId_[id.var()] << endl;
        }
        else {
            if (umoToNlOp_.at(op) == InvalidOp) {
                THROW_ERROR("Operator " << op << " has no NL counterpart");
            }
            vector<ExpressionId> operands = m_.getExpressionIdOperands(id);
            if (op == UMO_OP_SUM) {
                if (operands.size() < 2) {
                    THROW_ERROR("NL writer only supports sums with two or more operands");
                }
                else if (operands.size() == 2) {
                    // Binary case; NL file format is ****ed up
                    s_ << "o0" << endl;
                }
                else {
                    // Nary case
                    s_ << "o" << umoToNlOp_.at(op) << endl;
                    s_ << operands.size() << endl;
                }
            }
            else if (op == UMO_OP_PROD) {
                if (operands.size() != 2) {
                    THROW_ERROR("NL writer only supports products with two operands");
                }
                s_ << "o" << umoToNlOp_.at(op) << endl;
            }
            else if (Operator::get(op).isNary()) {
                s_ << "o" << umoToNlOp_.at(op) << endl;
                s_ << operands.size() << endl;
            }
            else {
                s_ << "o" << umoToNlOp_.at(op) << endl;
            }
            for (uint32_t j = 0; j < operands.size(); ++j) {
                ExpressionId opExpr = operands[operands.size() - 1 - j];
                toWrite.push_back(opExpr);
            }
        }
    }
}

void ModelWriterNl::initUmoToNl() {
    umoToNlOp_ = std::vector<int>(UMO_OP_END, -1);
    // Unary AMPL operators
    umoToNlOp_[UMO_OP_FLOOR] = 13;
    umoToNlOp_[UMO_OP_CEIL] = 14;
    umoToNlOp_[UMO_OP_ABS] = 15;
    umoToNlOp_[UMO_OP_MINUS_UNARY] = 16;
    umoToNlOp_[UMO_OP_NOT] = 34;
    umoToNlOp_[UMO_OP_TANH] = 37;
    umoToNlOp_[UMO_OP_TAN] = 38;
    umoToNlOp_[UMO_OP_SQRT] = 39;
    umoToNlOp_[UMO_OP_SINH] = 40;
    umoToNlOp_[UMO_OP_SIN] = 41;
    umoToNlOp_[UMO_OP_LOG] = 43;
    umoToNlOp_[UMO_OP_EXP] = 44;
    umoToNlOp_[UMO_OP_COSH] = 45;
    umoToNlOp_[UMO_OP_COS] = 46;
    umoToNlOp_[UMO_OP_ATANH] = 47;
    umoToNlOp_[UMO_OP_ATAN] = 49;
    umoToNlOp_[UMO_OP_ASINH] = 50;
    umoToNlOp_[UMO_OP_ASIN] = 51;
    umoToNlOp_[UMO_OP_ACOSH] = 52;
    umoToNlOp_[UMO_OP_ACOS] = 53;
    // Binary AMPL operators
    umoToNlOp_[UMO_OP_MINUS_BINARY] = 1;
    umoToNlOp_[UMO_OP_PROD] = 2;
    umoToNlOp_[UMO_OP_DIV] = 3;
    umoToNlOp_[UMO_OP_MOD] = 4;
    umoToNlOp_[UMO_OP_POW] = 5;
    umoToNlOp_[UMO_OP_CMP_LT] = 22;
    umoToNlOp_[UMO_OP_CMP_LEQ] = 23;
    umoToNlOp_[UMO_OP_CMP_EQ] = 24;
    umoToNlOp_[UMO_OP_CMP_GEQ] = 28;
    umoToNlOp_[UMO_OP_CMP_GT] = 29;
    umoToNlOp_[UMO_OP_CMP_NEQ] = 30;
    umoToNlOp_[UMO_OP_IDIV] = 255;
    // Nary AMPL operators
    umoToNlOp_[UMO_OP_MIN] = 11;
    umoToNlOp_[UMO_OP_MAX] = 12;
    umoToNlOp_[UMO_OP_SUM] = 54;
    umoToNlOp_[UMO_OP_AND] = 70;
    umoToNlOp_[UMO_OP_OR] = 71;
}

vector<int32_t> ModelWriterNl::getUmoToNlId(const Model &m) {
    vector<int32_t> varToId(m.nbExpressions(), InvalidId);
    int32_t id = 0;
    for (uint32_t i = 0; i < m.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m.expression(i);
        if (expr.op == UMO_OP_DEC_FLOAT)
            varToId[i] = id++;
    }
    for (uint32_t i = 0; i < m.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m.expression(i);
        if (expr.op == UMO_OP_DEC_BOOL)
            varToId[i] = id++;
    }
    for (uint32_t i = 0; i < m.nbExpressions(); ++i) {
        const Model::ExpressionData &expr = m.expression(i);
        if (expr.op == UMO_OP_DEC_INT)
            varToId[i] = id++;
    }
    return varToId;
}

void ModelWriterNl::initUmoToNlId() {
    varToId_ = getUmoToNlId(m_);
}

void ModelWriterNl::initJacobianSize() {
    jacobianSize_.assign(countVariables(), 0);
    for (ExpressionId id : m_.constraints()) {
        umo_operator op = m_.getExpressionIdOp(id);
        if (op != UMO_OP_LINEARCOMP) {
            THROW_ERROR("Cannot export nonlinear constraints yet in NL file writer");
        }
        const Model::ExpressionData &expr = m_.expression(id.var());
        for (uint32_t j = 1; 2 * j + 1 < expr.operands.size(); ++j) {
            ExpressionId id = expr.operands[2 * j + 1];
            uint32_t ind = varToId_.at(id.var());
            ++jacobianSize_[ind];
        }
    }
    int totSize = 0;
    for (size_t i = 0; i < jacobianSize_.size(); ++i) {
        totSize += jacobianSize_[i];
        jacobianSize_[i] = totSize;
    }
}

void ModelWriterNl::writeHeader() {
    s_ << "g 1 1 0 # problem umo" << endl;
    s_ << countVariables() << " "
       << countConstraints() << " "
       << countObjectives() << " "
       << "0 0 0 "
       << "# vars, constraints, objectives, ranges, eqns, lcons" << endl;
    s_ << "0 0 "
       << "# nonlinear constraints, objectives" << endl;
    s_ << "0 0 "
       << "# network constraints: nonlinear, linear" << endl;
    s_ << "0 0 0 "
       << "# nonlinear vars in constraints, objectives, both" << endl;
    s_ << "0 1 0 1 "
       << "# linear network variables; functions; arith, flags" << endl;
    s_ << boolVariables_.size() << " "
       << intVariables_.size() << " "
       << "0 0 0 "
       << "# discrete variables: binary, integer, nonlinear (b,c,o)" << endl;
    s_ << countJacobianNonZeros() << " "
       << countGradientNonZeros() << " "
       << "# nonzeros in Jacobian, gradients" << endl;
    s_ << "0 0 "
       << "# max name lengths: constraints, variables" << endl;
    s_ << "0 0 0 0 0 "
       << "# common exprs: b,c,o,c1,o1" << endl;
}

void ModelWriterNl::writeObjectives() {
    for (uint32_t i = 0; i < m_.nbObjectives(); ++i) {
        ExpressionId obj = m_.objective(i).first;
        bool maximize = m_.objective(0).second == UMO_OBJ_MAXIMIZE;
        s_ << "O" << i << " " << (maximize ? "1" : "0") << endl;
        writeExpressionGraph(obj);
    }
    if (m_.nbObjectives() == 0) {
        // Dummy objective
        s_ << "O0 1\nn0" << endl;
    }
}

void ModelWriterNl::writeLinearExpression(ExpressionId id) {
    umo_operator op = m_.getExpressionIdOp(id);
    assert (op == UMO_OP_LINEARCOMP);
    const Model::ExpressionData &expr = m_.expression(id.var());
    s_ << (expr.operands.size() - 2) / 2 << endl;
    for (uint32_t j = 1; 2 * j + 1 < expr.operands.size(); ++j) {
        double val = m_.value(expr.operands[2 * j].var());
        ExpressionId id = expr.operands[2 * j + 1];
        uint32_t ind = varToId_.at(id.var());
        if (ind == InvalidId) {
            THROW_ERROR("Cannot export linear constraints on non-leaf expressions in NL file writer");
        }
        s_ << ind << " " << val << endl;
    }
}

void ModelWriterNl::writeNonLinearConstraints() {
    if (m_.constraints().empty()) return;
    uint32_t i = 0;
    for (ExpressionId id : m_.constraints()) {
        umo_operator op = m_.getExpressionIdOp(id);
        if (op != UMO_OP_LINEARCOMP) {
            THROW_ERROR("Cannot export nonlinear constraints yet in NL file writer");
        }
        s_ << "C" << i << endl;
        s_ << "n0" << endl;
        ++i;
    }
}

void ModelWriterNl::writeConstraintBounds() {
    if (countConstraints() == 0) return;
    s_ << "r" << endl;
    for (ExpressionId id : m_.constraints()) {
        umo_operator op = m_.getExpressionIdOp(id);
        if (op != UMO_OP_LINEARCOMP) {
            THROW_ERROR("Cannot export nonlinear constraints yet in NL file writer");
        }
        const Model::ExpressionData &expr = m_.expression(id.var());
        double lb = m_.value(expr.operands[0].var());
        double ub = m_.value(expr.operands[1].var());
        writeBounds(lb, ub);
    }
}

void ModelWriterNl::writeLinearConstraints() {
    if (countConstraints() == 0) return;
    uint32_t i = 0;
    for (ExpressionId id : m_.constraints()) {
        umo_operator op = m_.getExpressionIdOp(id);
        if (op != UMO_OP_LINEARCOMP) {
            THROW_ERROR("Cannot export nonlinear constraints yet in NL file writer");
        }
        s_ << "J" << i << " ";
        writeLinearExpression(id);
        ++i;
    }
}

void ModelWriterNl::writeJacobianSize() {
    if (countJacobianNonZeros() == 0) return;
    s_ << "k " << countVariables() - 1 << endl;
    for (size_t i = 0; i+1 < jacobianSize_.size(); ++i) {
        s_ << jacobianSize_[i] << endl;
    }
}

void ModelWriterNl::writeBounds(double lb, double ub) {
    if (std::isfinite(lb) && std::isfinite(ub)) {
        if (lb == ub) {
            s_ << "4 " << lb << endl;
        }
        else {
            s_ << "0 " << lb << " " << ub << endl;
        }
    }
    else if (std::isfinite(lb)) {
        s_ << "2 " << lb << endl;
    }
    else if (std::isfinite(ub)) {
        s_ << "1 " << ub << endl;
    }
    else {
        s_ << "3" << endl;
    }
}

void ModelWriterNl::writeVariableBounds() {
    s_ << "b" << endl;
    for (uint32_t i : floatVariables_) {
        const Model::ExpressionData &expr = m_.expression(i);
        assert(expr.op == UMO_OP_DEC_FLOAT);
        double lb = m_.value(expr.operands[0].var());
        double ub = m_.value(expr.operands[1].var());
        writeBounds(lb, ub);
    }
    for (uint32_t i : boolVariables_) {
        writeBounds(0, 1);
    }
    for (uint32_t i : intVariables_) {
        const Model::ExpressionData &expr = m_.expression(i);
        assert(expr.op == UMO_OP_DEC_INT);
        double lb = m_.value(expr.operands[0].var());
        double ub = m_.value(expr.operands[1].var());
        writeBounds(lb, ub);
    }
}

void ModelWriterNl::write() {
    initUmoToNl();
    initUmoToNlId();
    initBoolVariables();
    initIntVariables();
    initFloatVariables();
    initJacobianSize();
    writeHeader(); // Header part
    writeObjectives(); // "O" lines
    writeNonLinearConstraints(); // "C" lines
    writeConstraintBounds(); // "r" lines
    writeVariableBounds(); // "b" lines
    writeJacobianSize(); // "k" lines
    writeLinearConstraints(); // "J" lines
}

void Model::writeNl(ostream &os) const {
    ModelWriterNl(*this, os).write();
}

void Model::readNlSol(istream &is) {
    vector<int32_t> varToId = ModelWriterNl::getUmoToNlId(*this);
    string line;
    stringstream ss;
    getline(is, line);
    getline(is, line);
    getline(is, line);
    getline(is, line);
    std::vector<int> z;
    int nopts = 0;
    if (line.size() >= 7 && line.substr(0, 7) == "Options") {
        getline(is, line);
        ss = stringstream(line);
        ss >> nopts;
        if (nopts > 4) {
            nopts -= 2;
        }
        for (int i = 0; i < nopts+4; ++i) {
            getline(is, line);
            ss = stringstream(line);
            int tmp;
            ss >> tmp;
            z.push_back(tmp);
        }
    }
    else {
        THROW_ERROR("\"Options\" line not found by NL file reader");
    }
    std::vector<double> constraints;
    std::vector<double> variables;
    int nconstraints = z.at(nopts + 1);
    int nvariables = z.at(nopts + 3);
    for (int i = 0; i < nconstraints; ++i) {
        getline(is, line);
        ss = stringstream(line);
        double tmp;
        ss >> tmp;
        constraints.push_back(tmp);
    }
    for (int i = 0; i < nvariables; ++i) {
        getline(is, line);
        ss = stringstream(line);
        double tmp;
        ss >> tmp;
        variables.push_back(tmp);
    }

    umo_solution_status status = UMO_STATUS_UNKNOWN;
    getline(is, line);
    if (line.size() >= 5 && line.substr(0, 5) == "objno") {
        int objno0 = -1;
        int objno1 = -1;
        ss = stringstream(line.substr(5));
        ss >> objno0 >> objno1;
        if (objno1 < 0) {
            // ???
            status = UMO_STATUS_UNKNOWN;
        }
        else if (objno1 < 100) {
            // Optimal
            status = UMO_STATUS_OPTIMAL;
        }
        else if (objno1 < 200) {
            // Optimal, but issues were detected
            status = UMO_STATUS_OPTIMAL;
        }
        else if (objno1 < 300) {
            // Infeasible
            status = UMO_STATUS_INFEASIBLE;
        }
        else if (objno1 < 400) {
            // Unbounded
            status = UMO_STATUS_UNBOUNDED;
        }
        else if (objno1 < 500) {
            // Stopped on limit
            status = UMO_STATUS_UNKNOWN;
        }
        else if (objno1 < 600) {
            // Stopped on error
            status = UMO_STATUS_UNKNOWN;
        }
        else {
            // ???
            status = UMO_STATUS_UNKNOWN;
        }
    }
    else {
        THROW_ERROR("\"objno\" line not found by NL file reader");
    }

    for (uint32_t i = 0; i < nbExpressions(); ++i) {
        uint32_t id = varToId.at(i);
        if (id != ModelWriterNl::InvalidId) {
            double val = variables.at(id);
            setFloatValue(ExpressionId::fromVar(i), val);
        }
    }
    if (status != UMO_STATUS_UNKNOWN) {
        setStatus(status);
    }
}
} // namespace umoi
