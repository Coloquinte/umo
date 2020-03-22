
#include "presolve/presolve.hpp"

#include "presolve/cleanup.hpp"
#include "presolve/flatten.hpp"
#include "presolve/propagate_constants.hpp"
#include "presolve/to_linear.hpp"
#include "presolve/to_sat.hpp"

#include "solver/external_solvers.hpp"

#include <iostream>

using namespace std;

namespace umoi {
namespace presolve {

PresolvedModel run(Model &m) {
    // m.writeUmo(cout);
    PresolvedModel model = m;
    Cleanup().run(model);
    Flatten().run(model);
    PropagateConstants().run(model);
    if (ToSat().valid(model)) {
        ToSat().run(model);
        // model.writeCnf(cout);
        MinisatSolver().run(model);
        model.push(m);
    } else if (ToLinear().valid(model)) {
        ToLinear().run(model);
        // model.writeLp(cout);
        CbcSolver().run(model);
        model.push(m);
    } else {
        throw std::runtime_error("The model cannot be converted to SAT or MIP");
    }
    // model.writeUmo(cout);
    return model;
}

} // namespace presolve
} // namespace umoi
