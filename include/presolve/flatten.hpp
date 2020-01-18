
#ifndef __UMO_PRESOLVE_FLATTEN_HPP__
#define __UMO_PRESOLVE_FLATTEN_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class Flatten final : public PresolverPass {
  public:
    std::string toString() const override {
        return "flatten";
    }

    void run(PresolvedModel &model) override;
};
}
}

#endif




