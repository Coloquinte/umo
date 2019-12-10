
#ifndef __UMO_MODEL_HPP__
#define __UMO_MODEL_HPP__

#include "api/umo_enums.h"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace umoi {
class Model {
  private:
    struct ExpressionData {
        std::uint32_t beginOperands;
        std::uint32_t endOperands;
        umo_operator op;
        umo_type type;
        bool constraintPos;
        bool constraintNeg;
        bool objectivePos;
        bool objectiveNeg;
    };

    class VarData {
      public:
        VarData(std::uint32_t var, bool isNot, bool isOpp) {
            v = var << 2;
            if (isNot) v |= 0x01;
            if (isOpp) v |= 0x02;
        }
        bool isNot() const {
            return v & 0x01;
        }
        bool isOpp() const {
            return v & 0x02;
        }
        std::uint32_t ind() const {
            return v >> 2;
        }
        std::uint32_t raw() const {
            return v;
        }
        static VarData fromRaw(std::uint32_t raw) {
            return VarData(raw);
        }

      private:
        VarData(std::uint32_t raw) {
            v = raw;
        }

      private:
        std::uint32_t v;
    };

  public:
    std::uint32_t createConstant(double value);
    std::uint32_t createExpression(umo_operator op, long long *beginOp, long long *endOp);

    void createConstraint(std::uint32_t expr);
    void createObjective(std::uint32_t expr, umo_objective_direction dir);

    double getFloatValue(std::uint32_t expr) const;
    void setFloatValue(std::uint32_t expr, double value);
    umo_solution_status getStatus() const;

    void solve();

    double getFloatParameter(const std::string &param) const;
    void setFloatParameter(const std::string &param, double value);
    const std::string &getStringParameter(const std::string &param) const;
    void setStringParameter(const std::string &param, const std::string &value);

    std::uint32_t nbExpressions() const { return expressions_.size(); }
    std::uint32_t nbObjectives() const { return objectives_.size(); }

  private:
    std::vector<ExpressionData> expressions_;
    std::vector<VarData> operands_;
    std::vector<VarData> objectives_;
    std::unordered_map<double, VarData> constants_;

    std::vector<double> values_;
    bool computed_;
    umo_solution_status status_;

    std::unordered_map<std::string, std::string> stringParams_;
    std::unordered_map<std::string, double> floatParams_;
};
}

#endif

