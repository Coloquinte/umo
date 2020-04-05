
#ifndef __UMO_EXTERNAL_SOLVERS_HPP__
#define __UMO_EXTERNAL_SOLVERS_HPP__

#include "solver/solver.hpp"

/*
 * Those solvers are given the problem as a temporary file
 * and called as external programs.
 *
 * Steps:
 *    allocate temporary name for input file
 *    allocate temporary name for output file
 *    write input file
 *    launch command and wait
 *    read output file
 */

namespace umoi {
class CbcSolver : public Solver {
  public:
    bool valid(PresolvedModel &) const override;
    void run(PresolvedModel &) const override;
};

class MinisatSolver : public Solver {
  public:
    bool valid(PresolvedModel &) const override;
    void run(PresolvedModel &) const override;
};
} // namespace umoi

#endif
