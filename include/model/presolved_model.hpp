
#ifndef __UMO_PRESOLVED_MODEL_HPP__
#define __UMO_PRESOLVED_MODEL_HPP__

#include "model/model.hpp"

namespace umoi {
class PresolvedModel : public Model {
  public:
    PresolvedModel();
    PresolvedModel(const Model &);
    void push(Model &model);
    void pull(Model &model);
    void apply(const PresolvedModel &model);

    const std::unordered_map<unsigned int, ExpressionId> &mapping() const {
        return variableMapping_;
    }
    std::unordered_map<unsigned int, ExpressionId> &mapping() {
        return variableMapping_;
    }

  private:
    // Mapping from the original decisions to the new variables
    std::unordered_map<unsigned int, ExpressionId> variableMapping_;
};
} // namespace umoi

#endif
