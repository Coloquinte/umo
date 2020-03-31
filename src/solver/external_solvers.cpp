
#include "solver/external_solvers.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>

using namespace std;

namespace umoi {
void CbcSolver::run(Model &m) const {
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
    system(command.str().c_str());
    ifstream solf(tmpSolName);
    m.readLpSol(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}

void MinisatSolver::run(Model &m) const {
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
    system(command.str().c_str());
    ifstream solf(tmpSolName);
    m.readCnfSol(solf);
    solf.close();
    remove(tmpModName.c_str());
    remove(tmpSolName.c_str());
}
} // namespace umoi
