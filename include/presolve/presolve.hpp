
#ifndef __UMO_PRESOLVE_HPP__
#define __UMO_PRESOLVE_HPP__

#include <string>

#include "model/presolved_model.hpp"

namespace umoi {

class PresolverPass {
  public:
    virtual std::string toString() const =0;

    virtual void run(PresolvedModel &model) =0;
};

PresolvedModel presolve(const Model &model);

}

#endif



