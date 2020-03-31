
#ifndef __UMO_SOLVER_HPP__
#define __UMO_SOLVER_HPP__

#include "model/model.hpp"

namespace umoi {
class Solver {
  public:
    virtual void run(Model &) const = 0;

    static std::string temporaryFilename(const std::string &prefix,
                                         const std::string &suffix);
};
} // namespace umoi

#endif
