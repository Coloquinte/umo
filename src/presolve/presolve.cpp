
#include "presolve/presolve.hpp"

#include "presolve/cleanup.hpp"
#include "presolve/flatten.hpp"
#include "presolve/propagate_constants.hpp"
#include "presolve/to_linear.hpp"

using namespace std;

namespace umoi {
namespace presolve {

PresolvedModel run(const Model &m) {
    PresolvedModel model = m;
    Cleanup().run(model);
    Flatten().run(model);
    PropagateConstants().run(model);
    ToLinear().run(model);
    return model;
}

} // namespace presolve
} // namespace umoi
