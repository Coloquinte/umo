
#include "presolve/presolve.hpp"

#include "presolve/cleanup.hpp"
#include "presolve/flatten.hpp"
#include "presolve/propagate_constants.hpp"
#include "presolve/to_linear.hpp"
#include "presolve/to_sat.hpp"

#include <iostream>

using namespace std;

namespace umoi {
namespace presolve {

PresolvedModel run(const Model &m) {
    //m.writeUmo(cout);
    PresolvedModel model = m;
    Cleanup().run(model);
    Flatten().run(model);
    PropagateConstants().run(model);
    if (ToSat().valid(model)) {
        ToSat().run(model);
        //model.writeCnf(cout);
    }
    else if (ToLinear().valid(model)) {
        ToLinear().run(model);
        //model.writeLp(cout);
    }
    else {
        throw std::runtime_error("The model cannot be converted to SAT or MIP");
    }
    //model.writeUmo(cout);
    return model;
}

} // namespace presolve
} // namespace umoi
