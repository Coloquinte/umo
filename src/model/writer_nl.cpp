
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
 * NL operators:
 *   Unary:
 *     13  floor
 *     14  ceil
 *     15  abs
 *     16  neg
 *     34  not
 *     37  tanh
 *     38  tan
 *     39  sqrt
 *     40  sinh
 *     41  sin
 *     42  log10
 *     43  log
 *     44  exp
 *     45  cosh
 *     46  cos
 *     47  atanh
 *     49  atan
 *     50  asinh
 *     51  asin
 *     52  acosh
 *     53  acos
 *   Binary:
 *     0   plus
 *     1   minus
 *     2   mult
 *     3   div
 *     4   rem
 *     5   pow
 *     6   less
 *     20  or
 *     21  and
 *     22  lt
 *     23  le
 *     24  eq
 *     28  ge
 *     29  gt
 *     30  ne
 *     48  atan
 *     255  intdiv
 *     56  precision
 *     57  round
 *     58  trunc
 *     73  iff
 *   Nary:
 *     11  min
 *     12  max
 *     54  sum
 *     59  count
 *     60  numberof
 *     61  numberofs
 *     70  and
 *     71  or
 *     74  alldiff
 *   Selection:
 *     35  if
 *     65  ifs
 *     72  implies
 */

class ModelWriterNl {
  public:
    static constexpr int InvalidOp = -1;
    static constexpr int32_t InvalidId = -1;

  public:
    ModelWriterNl(const Model &m, ostream &s);

    void write();
    static vector<int32_t> getVarToId(const Model &m);

    int countVariables() const;
    int countConstraints() const;
    int countObjectives() const;

    void writeHeader();
    void writeObjectives();
    void writeConstraints();
    void writeBounds();

    // TODO: write linear constraint
    void writeExpressionGraph(ExpressionId id);

  private:
    void initUmoToNl();
    void initVarToId();

  private:
    const Model &m_;
    ostream &s_;
    vector<int> umoToNlOp_;
    vector<int32_t> varToId_;
};

constexpr int ModelWriterNl::InvalidOp;
constexpr int32_t ModelWriterNl::InvalidId;

ModelWriterNl::ModelWriterNl(const Model &m, ostream &s) : m_(m), s_(s) {}

int ModelWriterNl::countVariables() const {
    int cnt = 0;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (varToId_[i] != InvalidId) {
            ++cnt;
        }
    }
    return cnt;
}

int ModelWriterNl::countConstraints() const {
    return m_.nbConstraints();
}

int ModelWriterNl::countObjectives() const {
    return m_.nbObjectives();
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
            if (op == UMO_OP_MINUS_UNARY || op == UMO_OP_NOT) {
                // TODO: get expression data for operators with compact representation (NOT/MINUS)
                THROW_ERROR("NL writer does not handle operator " << op);
            }
            const Model::ExpressionData &expr = m_.expression(id.var());
            if (op == UMO_OP_SUM) {
                if (expr.operands.size() < 2) {
                    THROW_ERROR("NL writer only supports sums with two or more operands");
                }
                else if (expr.operands.size() == 2) {
                    // Binary case; NL file format is ****ed up
                    s_ << "o0" << endl;
                }
                else {
                    // Nary case
                    s_ << "o" << umoToNlOp_.at(op) << endl;
                    s_ << expr.operands.size() << endl;
                }
            }
            else if (op == UMO_OP_PROD) {
                if (expr.operands.size() != 2) {
                    THROW_ERROR("NL writer only supports products with two operands");
                }
                s_ << "o" << umoToNlOp_.at(op) << endl;
            }
            else if (Operator::get(op).isNary()) {
                s_ << "o" << umoToNlOp_.at(op) << endl;
                s_ << expr.operands.size() << endl;
            }
            else {
                s_ << "o" << umoToNlOp_.at(op) << endl;
            }
            for (uint32_t j = 0; j < expr.operands.size(); ++j) {
                ExpressionId opExpr = expr.operands[expr.operands.size() - 1 - j];
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

vector<int32_t> ModelWriterNl::getVarToId(const Model &m) {
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

void ModelWriterNl::initVarToId() {
    varToId_ = getVarToId(m_);
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
    s_ << "0 0 0 0 0 "
       << "# discrete variables: binary, integer, nonlinear (b,c,o)" << endl;
    s_ << "0 0 "
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
}

void ModelWriterNl::writeConstraints() {
}

void ModelWriterNl::writeBounds() {
    s_ << "b" << endl;
    for (uint32_t i = 0; i < m_.nbExpressions(); ++i) {
        if (varToId_[i] == InvalidId) {
            continue;
        }
        const Model::ExpressionData &expr = m_.expression(i);
        if (expr.op == UMO_OP_DEC_INT || expr.op == UMO_OP_DEC_FLOAT) {
            double lb = m_.value(expr.operands[0].var());
            double ub = m_.value(expr.operands[1].var());
            if (std::isfinite(lb) && std::isfinite(ub)) {
                s_ << "0 " << lb << " " << ub << endl;
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
    }
}

void ModelWriterNl::write() {
    initUmoToNl();
    initVarToId();
    writeHeader();
    writeObjectives();
    writeConstraints();
    writeBounds();
}

void Model::writeNl(ostream &os) const {
    ModelWriterNl(*this, os).write();
}

void Model::readNlSol(istream &is) {
    vector<int32_t> varToId = ModelWriterNl::getVarToId(*this);
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
