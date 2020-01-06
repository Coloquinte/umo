
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

    double getFloatValue(ExpressionId expr);
    void setFloatValue(ExpressionId expr, double value);
    umo_solution_status getStatus();

    void solve();
    void check() const;

    double getFloatParameter(const std::string &param) const;
    void setFloatParameter(const std::string &param, double value);
    const std::string &getStringParameter(const std::string &param) const;
    void setStringParameter(const std::string &param, const std::string &value);

    std::uint32_t nbExpressions() const { return expressions_.size(); }
    std::uint32_t nbConstraints() const { return constraints_.size(); }
    std::uint32_t nbObjectives() const { return objectives_.size(); }

  private:
    void checkExpressionId(ExpressionId expr) const;
    umo_type getExpressionIdType(ExpressionId expr) const;
    umo_operator getExpressionIdOp(ExpressionId expr) const;
    double getExpressionIdValue(ExpressionId expr) const;

    umo_type checkAndInferType(const ExpressionData &expr) const;
    std::vector<umo_type> getOperandTypes(const ExpressionData &expr) const;
    std::vector<umo_operator> getOperandOps(const ExpressionData &expr) const;

    void compute();

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
    ExpressionData(umo_operator op)
        : op(op) {}
    ExpressionData(umo_operator op, umo_type type)
        : op(op), type(type) {}
};
}


#endif

