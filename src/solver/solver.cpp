
#include "solver/solver.hpp"

#include <iomanip>
#include <random>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#endif

using namespace std;

namespace umoi {
string Solver::temporaryFilename(const string &prefix, const string &suffix) {
    // TODO: hack because std::filesystem is painful to link with
 #ifdef _WIN32
    string tmpDir;
    char buf[_MAX_PATH];
    if (GetTempPathA(_MAX_PATH, buf))
        tmpDir = buf;
    else
        throw std::runtime_error("Could not locate the directory for temporary files.");
 #else
    string tmpDir = "/tmp/";
 #endif
    random_device rd;
    uniform_int_distribution<uint64_t> dist;
    uint64_t randInt = dist(rd);
    stringstream fname;
    fname << tmpDir;
    fname << prefix;
    fname << hex << randInt;
    fname << suffix;
    return fname.str();
}
} // namespace umoi
