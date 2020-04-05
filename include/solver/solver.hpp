
#ifndef __UMO_SOLVER_HPP__
#define __UMO_SOLVER_HPP__

#include <string>

namespace umoi {
class PresolvedModel;
class Solver {
  public:
    virtual bool valid(PresolvedModel &) const = 0;
    virtual void run(PresolvedModel &) const = 0;

    static std::string temporaryFilename(const std::string &prefix,
                                         const std::string &suffix);
};
} // namespace umoi

#endif
