
#ifndef __UMO_PRESOLVE_CLEANUP_HPP__
#define __UMO_PRESOLVE_CLEANUP_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class Cleanup final : public PresolverPass {
  public:
    std::string toString() const override { return "cleanup"; }

    void run(PresolvedModel &model) override;
};
} // namespace presolve
} // namespace umoi

#endif
