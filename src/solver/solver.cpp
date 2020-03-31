
#include "solver/solver.hpp"

#include <filesystem>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

using namespace std;

namespace umoi {
string Solver::temporaryFilename(const string &prefix, const string &suffix) {
    // TODO: temporary hack because std::filesystem is painful to link with
    string tmpDir = "/tmp/";
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
