
#include "solver/external_solvers.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "utils/utils.hpp"
#include "presolve/presolve.hpp"
#include "presolve/to_linear.hpp"
#include "presolve/to_sat.hpp"

using namespace std;

namespace umoi {

class ITmpFile : public ifstream {
    // Use RAII to free temporary files when an exception is thrown
  public:
    ITmpFile(const std::string &filename) : ifstream(filename), filename_(filename) {}
    ~ITmpFile() {
        close();
        remove(filename_.c_str());
    }
    
  private:
    std::string filename_;
};

class OTmpFile : public ofstream {
    // Use RAII to free temporary files when an exception is thrown
  public:
    OTmpFile(const std::string &filename) : ofstream(filename), filename_(filename) {}
    ~OTmpFile() {
        close();
        remove(filename_.c_str());
    }
    
  private:
    std::string filename_;
};

bool CbcSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void CbcSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-cbc-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeLp(modf);
    stringstream command;
    command << "cbc " << tmpModName << " ";
    double time = m.getFloatParameter("time_limit");
    if (std::isfinite(time)) {
        command << "\"sec " << time << "\" ";
    }
    command << "branch ";
    command << "solution " << tmpSolName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("CBC does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readLpSolCbc(solf);
}

bool CplexSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void CplexSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-cplex-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeLp(modf);
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
            THROW_ERROR("CPLEX does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readLpSolCplex(solf);
}

bool GlpkSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void GlpkSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-glpk-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeLp(modf);
    stringstream command;
    command << "glpsol";
    command << " --cpxlp " << tmpModName;
    command << " -o " << tmpSolName;
    double time = m.getFloatParameter("time_limit");
    if (std::isfinite(time)) {
        command << " --tmlim " << (long long)std::ceil(time);
    }
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0 && returnValue != 256) {
        if (returnValue == 127) {
            THROW_ERROR("GLPK does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readLpSolGlpk(solf);
}

bool GurobiSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void GurobiSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-gurobi-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeLp(modf);
    stringstream command;
    command << "gurobi";
    command << " ResultFile=" << tmpSolName;
    command << " " << tmpModName;
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("Gurobi does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readLpSolGurobi(solf);
}

bool ScipSolver::valid(PresolvedModel &m) const {
    return presolve::ToLinear().valid(m);
}

void ScipSolver::run(PresolvedModel &m) const {
    presolve::ToLinear().run(m);
    string tmpName = temporaryFilename("umo-scip-", "");
    string tmpModName = tmpName + "-mod.lp";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeLp(modf);
    stringstream command;
    command << "scip ";
    double time = m.getFloatParameter("time_limit");
    if (std::isfinite(time)) {
        command << "-c \"set limits time " << time << "\" ";
    }
    command << "-c \"read " << tmpModName << "\" ";
    command << "-c optimize ";
    command << "-c \"write solution " << tmpSolName << "\" ";
    command << "-c quit ";
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("SCIP does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readLpSolScip(solf);
}

bool MinisatSolver::valid(PresolvedModel &m) const {
    return presolve::ToSat().valid(m);
}

void MinisatSolver::run(PresolvedModel &m) const {
    presolve::ToSat().run(m);
    string tmpName = temporaryFilename("umo-minisat-", "");
    string tmpModName = tmpName + "-mod.cnf";
    string tmpSolName = tmpName + "-out.sol";
    OTmpFile modf(tmpModName);
    m.writeCnf(modf);
    stringstream command;
    command << "minisat ";
    command << "-verb=0 ";
    command << tmpModName << " " << tmpSolName << " ";
    double time = m.getFloatParameter("time_limit");
    if (std::isfinite(time)) {
        command << "-cpu-lim " << (long long)std::ceil(time);
    }
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue == 127) {
        // No other checks: minisat returns non-zero values on success
        THROW_ERROR("Minisat does not seem to be installed. The following "
                    "command returned an error: "
                    << command.str());
    }
    ITmpFile solf(tmpSolName);
    m.readCnfSolMinisat(solf);
}

bool CouenneSolver::valid(PresolvedModel &m) const {
    return true;
}

void CouenneSolver::run(PresolvedModel &m) const {
    presolve::ToLinear(true).run(m);
    string tmpName = temporaryFilename("umo-couenne-", "");
    // There is a bug in Couenne: only the extension must differ
    string tmpModName = tmpName + "-mod.nl";
    string tmpSolName = tmpName + "-mod.sol";
    OTmpFile modf(tmpModName);
    m.writeNl(modf);
    stringstream command;
    command << "couenne ";
    command << "-o " << tmpSolName << " ";
    // TODO: handle the options and time limit for Couenne; as Couenne makes use of a local couenne.opt file, this requires a temporary directory
    command << tmpModName << " ";
    command << endl;
    int returnValue = system(command.str().c_str());
    if (returnValue != 0) {
        if (returnValue == 127) {
            THROW_ERROR("Couenne does not seem to be installed. The following "
                        "command returned an error: "
                        << command.str());
        } else {
            THROW_ERROR("The following command finished with error "
                        << returnValue << ": " << command.str());
        }
    }
    ITmpFile solf(tmpSolName);
    m.readNlSol(solf);
}

} // namespace umoi
