
#ifndef __UMO_PRESOLVE_TO_SAT_HPP__
#define __UMO_PRESOLVE_TO_SAT_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class ToSat final : public PresolverPass {
  public:
    std::string toString() const override { return "toSat"; }

    bool valid(const PresolvedModel &model) const;
    void run(PresolvedModel &model) const override;

    class Transformer;
};
} // namespace presolve
} // namespace umoi

#endif
