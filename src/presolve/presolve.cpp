
#include "presolve/presolve.hpp"

#include "presolve/cleanup.hpp"
#include "presolve/flatten.hpp"
#include "presolve/propagate_constants.hpp"

#include "solver/external_solvers.hpp"

#include <iostream>

using namespace std;

namespace umoi {
namespace presolve {

PresolvedModel run(Model &m) {
    PresolvedModel model = m;
    Cleanup().run(model);
    Flatten().run(model);
    PropagateConstants().run(model);
    return model;
}

} // namespace presolve
} // namespace umoi
