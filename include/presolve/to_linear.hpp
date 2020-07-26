
#ifndef __UMO_PRESOLVE_TO_LINEAR_HPP__
#define __UMO_PRESOLVE_TO_LINEAR_HPP__

#include "presolve/presolve.hpp"

namespace umoi {
namespace presolve {
class ToLinear final : public PresolverPass {
  public:
    ToLinear(bool keepNonlinearExpressions=false);
    std::string toString() const override { return "toLinear"; }

    bool valid(const PresolvedModel &model) const;
    void run(PresolvedModel &model) const override;

    class Transformer;
    struct Element;

  private:
    bool keepNonlinearExpressions_;
};
} // namespace presolve
} // namespace umoi

#endif
