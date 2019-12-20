
#include "api/umo.h"

#include "model/model.hpp"

#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace std;
using namespace umoi;

#define UNWRAP_EXCEPTIONS(code) \
    do {\
        try {\
            code\
        } catch(exception &e) {\
            if(err != NULL) {\
                char *msg = (char *) malloc(strlen(e.what())*sizeof(char));\
                if (msg != NULL) {\
                    strcpy(msg, e.what());\
                    *err = msg;\
                }\
                else {\
                    *err = "Failed to allocate memory for the error message";\
                }\
            }\
        }\
    } while(0)
    

umo_model *umo_create_model(const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = new Model();
    return (umo_model *) model;
  );
  return nullptr;
}

void umo_destroy_model(umo_model *m, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    delete model;
  );
}

long long umo_create_constant(umo_model *m, double value, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    return model->createConstant(value).raw();
  );
  return -1ll;
}

long long umo_create_expression(umo_model *m, umo_operator op, int nb_operands, long long *operands, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    return model->createExpression(op, operands, operands + nb_operands).raw();
  );
  return -1ll;
}

void umo_create_constraint(umo_model *m, long long expr, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    model->createConstraint(ExpressionId::fromRaw(expr));
  );
}

void umo_create_objective(umo_model *m, long long expr, umo_objective_direction dir, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    model->createObjective(ExpressionId::fromRaw(expr), dir);
  );
}

double umo_get_float_value(umo_model *m, long long expr, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    return model->getFloatValue(ExpressionId::fromRaw(expr));
  );
  return NAN;
}

void umo_set_float_value(umo_model *m, long long expr, double value, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    model->setFloatValue(ExpressionId::fromRaw(expr), value);
  );
}

umo_solution_status umo_get_solution_status(umo_model *m, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    return model->getStatus();
  );
  return UMO_STATUS_INVALID;
}

void umo_solve(umo_model *m, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    model->solve();
  );
}

double umo_get_float_parameter(umo_model *m, const char *param, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    string p(param);
    return model->getFloatParameter(p);
  );
  return 0.0;
}

void umo_set_float_parameter(umo_model *m, const char *param, double value, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    string p(param);
    model->setFloatParameter(p, value);
  );
}

const char *umo_get_string_parameter(umo_model *m, const char *param, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    string p(param);
    return model->getStringParameter(p).c_str();
  );
  return NULL;
}

void umo_set_string_parameter(umo_model *m, const char *param, const char *value, const char **err) {
  UNWRAP_EXCEPTIONS(
    Model *model = (Model *) m;
    string p(param);
    string v(value);
    model->setStringParameter(p, value);
  );
}

