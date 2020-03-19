
#ifndef __UMO_PRESOLVE_TO_LINEAR_HPP__
#define __UMO_PRESOLVE_TO_LINEAR_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class ToLinear final : public PresolverPass {
  public:
    std::string toString() const override { return "toLinear"; }

    bool valid(const PresolvedModel &model) const;
    void run(PresolvedModel &model) const override;

    class Transformer;
    struct Element;
};
} // namespace presolve
} // namespace umoi

#endif
