
#ifndef __UMO_PRESOLVED_MODEL_HPP__
#define __UMO_PRESOLVED_MODEL_HPP__

#include "model/model.hpp"

namespace umoi {
class PresolvedModel : public Model {
  public:
    PresolvedModel(const Model &);
    void load(Model &model);

  private:
    // Mapping from the original decisions to the new variables
    std::unordered_map<unsigned int, ExpressionId> decisionMapping_;
};
}


#endif

