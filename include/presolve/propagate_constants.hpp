
#ifndef __UMO_PRESOLVE_PROPAGATE_CONSTANTS_HPP__
#define __UMO_PRESOLVE_PROPAGATE_CONSTANTS_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class PropagateConstants final : public PresolverPass {
  public:
    std::string toString() const override {
        return "propagateConstants";
    }

    void run(PresolvedModel &model) override;
};
}
}

#endif




