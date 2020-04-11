
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
    // Mimic command from Pulp: pulp/apis/coin_api.py
    command << "cbc " << tmpModName << " ";
    command << "branch ";
    command << "solution " << tmpSolName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("cbc does not seem to be installed. The following command returned an error: " << command.str());
        } else {
            THROW_ERROR("The following command finished with error " << returnValue << ": " << command.str());
        }
    }
    ifstream solf(tmpSolName);
    m.readLpSol(solf);
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
        THROW_ERROR("minisat does not seem to be installed. The following command returned an error: " << command.str());
    }
    ifstream solf(tmpSolName);
    m.readCnfSol(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}
} // namespace umoi
