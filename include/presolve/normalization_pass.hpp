
#ifndef __UMO_NORMALIZATION_PASS_HPP__
#define __UMO_NORMALIZATION_PASS_HPP__

#include "presolve/presolve.hpp"

namespace umoi {

class NormalizationPass final : public PresolverPass {
  public:
    std::string toString() const override {
        return "normalization";
    }

    void run(PresolvedModel &model) override;
};

}

#endif




