
#include "presolve/presolve.hpp"
#include "presolve/normalization_pass.hpp"

using namespace std;

namespace umoi {
PresolvedModel presolve(const Model &m) {
    PresolvedModel model = m;
    NormalizationPass().run(model);
    return model;
}
}


