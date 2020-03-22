
#ifndef __UMO_PRESOLVE_HPP__
#define __UMO_PRESOLVE_HPP__

#include <string>

#include "model/presolved_model.hpp"

namespace umoi {
namespace presolve {

class PresolverPass {
  public:
    virtual std::string toString() const = 0;

    virtual void run(PresolvedModel &model) const = 0;
};

PresolvedModel run(Model &model);

} // namespace presolve
} // namespace umoi

#endif
