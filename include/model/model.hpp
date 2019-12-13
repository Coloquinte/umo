
#ifndef __UMO_MODEL_HPP__
#define __UMO_MODEL_HPP__

#include "api/umo_enums.h"
#include "model/expression_id.hpp"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace umoi {
class Model {
  public:
    struct ExpressionData;

  public:
    ExpressionId createConstant(double value);
    ExpressionId createExpression(umo_operator op, long long *beginOp, long long *endOp);

    void createConstraint(ExpressionId expr);
    void createObjective(ExpressionId expr, umo_objective_direction dir);

    double getFloatValue(ExpressionId expr) const;
    void setFloatValue(ExpressionId expr, double value);
    umo_solution_status getStatus() const;

    void solve();

    double getFloatParameter(const std::string &param) const;
    void setFloatParameter(const std::string &param, double value);
    const std::string &getStringParameter(const std::string &param) const;
    void setStringParameter(const std::string &param, const std::string &value);

    std::uint32_t nbExpressions() const { return expressions_.size(); }
    std::uint32_t nbConstraints() const { return constraints_.size(); }
    std::uint32_t nbObjectives() const { return objectives_.size(); }

  private:
    void checkExpression(ExpressionId expr) const;

  private:
    std::vector<ExpressionData> expressions_;

    // Constraints
    std::unordered_set<ExpressionId> constraints_;

    // Objectives
    std::vector<std::pair<ExpressionId, umo_objective_direction> > objectives_;

    // Constant values to variable
    std::unordered_map<double, std::uint32_t> constants_;

    std::vector<double> values_;
    bool computed_;
    umo_solution_status status_;

    std::unordered_map<std::string, std::string> stringParams_;
    std::unordered_map<std::string, double> floatParams_;
};

struct Model::ExpressionData {
    umo_operator op;
    umo_type type;
    std::vector<ExpressionId> operands;
    ExpressionData(umo_operator op, umo_type type)
        : op(op), type(type) {}
};
}


#endif

