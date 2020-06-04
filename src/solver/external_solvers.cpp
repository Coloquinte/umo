
#include "solver/external_solvers.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>

#include "model/utils.hpp"
#include "presolve/presolve.hpp"
#include "presolve/to_sat.hpp"
#include "presolve/to_linear.hpp"

using namespace std;

namespace umoi {
bool CbcSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void CbcSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-cbc-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeLp(modf);
    modf.close();
    stringstream command;
    command << "cbc " << tmpModName << " ";
    command << "branch ";
    command << "solution " << tmpSolName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("CBC does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSolCbc(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

bool CplexSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void CplexSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-cplex-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeLp(modf);
    modf.close();
    stringstream command;
    command << "cplex -c ";
    command << "\"read " << tmpModName << "\" ";
    command << "optimize ";
    command << "\"write " << tmpSolName << "\" ";
    command << "quit ";
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("CPLEX does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSolCplex(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

bool GlpkSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void GlpkSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-glpk-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeLp(modf);
    modf.close();
    stringstream command;
    command << "glpsol";
    command << " --cpxlp " << tmpModName;
    command << " -o " << tmpSolName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0 && returnValue != 256) {
        if (returnValue == 127) {
            THROW_ERROR("GLPK does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSolGlpk(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

bool GurobiSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void GurobiSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-gurobi-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeLp(modf);
    modf.close();
    stringstream command;
    command << "gurobi";
    command << " ResultFile=" << tmpSolName;
    command << " " << tmpModName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("Gurobi does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSolGurobi(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

bool ScipSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void ScipSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-scip-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeLp(modf);
    modf.close();
    stringstream command;
    command << "scip ";
    command << "-c \"read " << tmpModName << "\" ";
    command << "-c optimize ";
    command << "-c \"write solution " << tmpSolName << "\" ";
    command << "-c quit ";
    command <<  endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("SCIP does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSolScip(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

bool MinisatSolver::valid(PresolvedModel &m) const {
    return presolve::ToSat().valid(m);
}

void MinisatSolver::run(PresolvedModel &m) const {
    presolve::ToSat().run(m);
    string tmpName = temporaryFilename("umo-minisat-", "");
    string tmpModName = tmpName + "-mod.cnf";
    string tmpSolName = tmpName + "-out.sol";
    ofstream modf(tmpModName);
    m.writeCnf(modf);
    modf.close();
    stringstream command;
    command << "minisat ";
    command << "-verb=0 ";
    command << tmpModName << " " << tmpSolName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue == 127) {
        // No other checks: minisat returns non-zero values on success
        THROW_ERROR("Minisat does not seem to be installed. The following command returned an error: " << command.str());
    }
    ifstream solf(tmpSolName);
    m.readCnfSolMinisat(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}
} // namespace umoi
