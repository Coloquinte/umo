
#ifndef __UMO_API_H__
#define __UMO_API_H__

#include "umo_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

struct umo_model;

typedef struct umo_model umo_model;

// Create and destroy model
umo_model *umo_create_model(char **err);
void umo_destroy_model(umo_model *, char **err);

// Create expressions
long long umo_create_constant(umo_model *, double value, char **err);
long long umo_create_expression(umo_model *, umo_operator op, int nb_operands, long long *operands, char **err);

// Assign constraints and objectives
void umo_create_constraint(umo_model *, long long expr, char **err);
void umo_create_objective(umo_model *, long long expr, char **err);

// Model parameters
double umo_get_float_parameter(umo_model *, const char *param, char **err);
void umo_set_float_parameter(umo_model *, const char *param, double value, char **err);
const char *umo_get_string_parameter(umo_model *, const char *param, char **err);
void umo_set_string_parameter(umo_model *, const char *param, const char *value, char **err);

// Set and retrieve solution
double umo_get_float_value(umo_model *, long long expr, char **err);
void umo_set_float_value(umo_model *, long long expr, double value, char **err);
umo_solution_status umo_get_solution_status(umo_model *, char **err);

void umo_solve(umo_model *, char **err);

// TODO: basic introspection
// * access expression tree
// * access objective bounds

#ifdef __cplusplus
}
#endif

#endif

