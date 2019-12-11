
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
    std::uint32_t createConstant(double value);
    std::uint32_t createExpression(umo_operator op, long long *beginOp, long long *endOp);

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
    std::vector<ExpressionData> expressions_;
    std::vector<ExpressionId> operands_;
    std::unordered_set<ExpressionId> constraints_;
    std::vector<ExpressionId> objectives_;
    std::unordered_map<double, std::uint32_t> constants_;

    std::vector<double> values_;
    bool computed_;
    umo_solution_status status_;

    std::unordered_map<std::string, std::string> stringParams_;
    std::unordered_map<std::string, double> floatParams_;
};

struct Model::ExpressionData {
    std::uint32_t beginOperands;
    std::uint32_t endOperands;
    umo_operator op;
    umo_type type;
};
}


#endif

