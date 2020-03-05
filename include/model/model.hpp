
#ifndef __UMO_MODEL_HPP__
#define __UMO_MODEL_HPP__

#include <cstdint>
#include <iosfwd>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api/umo_enums.h"
#include "model/expression_id.hpp"

namespace umoi {
class Model {
  public:
    struct ExpressionData;
    typedef std::pair<ExpressionId, umo_objective_direction> ObjectiveData;

  public:
    Model();

    ExpressionId createConstant(double value);
    ExpressionId createExpression(umo_operator op, long long *beginOp,
                                  long long *endOp);
    ExpressionId createExpression(umo_operator op,
                                  const std::vector<ExpressionId> &operands);

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

    const std::vector<ExpressionData> &expressions() const {
        return expressions_;
    }
    const std::unordered_set<ExpressionId> &constraints() const {
        return constraints_;
    }
    const std::vector<ObjectiveData> &objectives() const { return objectives_; }

    const ExpressionData &expression(std::uint32_t id) const {
        return expressions_[id];
    }
    const ObjectiveData &objective(std::uint32_t id) const {
        return objectives_[id];
    }
    const double &value(std::uint32_t id) const { return values_[id]; }

    bool isConstant(std::uint32_t id) const;
    bool isLeaf(std::uint32_t id) const;
    bool isDecision(std::uint32_t id) const;
    bool isConstraint(std::uint32_t id) const;
    bool isConstraintNeg(std::uint32_t id) const;
    bool isConstraintPos(std::uint32_t id) const;
    bool isObjective(std::uint32_t id) const;

    umo_type getExpressionIdType(ExpressionId expr) const;
    umo_operator getExpressionIdOp(ExpressionId expr) const;
    double getExpressionIdValue(ExpressionId expr) const;

    void write(std::ostream &) const;
    void writeLp(std::ostream &) const;
    void writeCnf(std::ostream &) const;
    void writeAmpl(std::ostream &) const;

  protected:
    void checkTypes() const;
    void checkTopologicalOrder() const;
    void checkCompressedOperands() const;

    void checkExpressionId(ExpressionId expr) const;

    umo_type checkAndInferType(const ExpressionData &expr) const;
    std::vector<umo_type> getOperandTypes(const ExpressionData &expr) const;
    std::vector<umo_operator> getOperandOps(const ExpressionData &expr) const;

    void compute();

    void initDefaultParameters();

  protected:
    std::vector<ExpressionData> expressions_;

    // Constraints
    std::unordered_set<ExpressionId> constraints_;

    // Objectives (ordered)
    std::vector<ObjectiveData> objectives_;

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
    ExpressionData(umo_operator op) : op(op) {}
    ExpressionData(umo_operator op, umo_type type) : op(op), type(type) {}
};

inline std::ostream &operator<<(std::ostream &os, const Model &model) {
    model.write(os);
    return os;
}
} // namespace umoi

#endif
