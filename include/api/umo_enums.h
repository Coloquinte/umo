
#ifndef __UMO_ENUMS_H__
#define __UMO_ENUMS_H__

#ifdef __cplusplus
extern "C" {
#endif

enum umo_type {
    UMO_TYPE_INVALID,
    UMO_TYPE_BOOL,
    UMO_TYPE_INT,
    UMO_TYPE_FLOAT
};

enum umo_objective_direction {
    UMO_OBJ_MINIMIZE,
    UMO_OBJ_MAXIMIZE
};

enum umo_solution_status {
    UMO_STATUS_INFEASIBLE,
    UMO_STATUS_INVALID,
    UMO_STATUS_VALID,
    UMO_STATUS_OPTIMAL
};

enum umo_operator {
    UMO_OP_INVALID,
    // Decisions and constants
    UMO_OP_CONSTANT,
    UMO_OP_DEC_BOOL,
    UMO_OP_DEC_INT,
    UMO_OP_DEC_FLOAT,
    // Logical operators
    UMO_OP_NOT,
    UMO_OP_AND,
    UMO_OP_OR,
    UMO_OP_XOR,
    // Arithmetic operators
    UMO_OP_SUM,
    UMO_OP_PROD,
    UMO_OP_MINUS_UNARY,
    UMO_OP_MINUS_BINARY,
    UMO_OP_INV,
    UMO_OP_DIV,
    UMO_OP_IDIV,
    UMO_OP_MOD,
    UMO_OP_MIN,
    UMO_OP_MAX,
    // Comparison operators
    UMO_OP_CMP_EQ,
    UMO_OP_CMP_NEQ,
    UMO_OP_CMP_LEQ,
    UMO_OP_CMP_GEQ,
    UMO_OP_CMP_LT,
    UMO_OP_CMP_GT,
    // Math operators
    UMO_OP_SQRT,
    UMO_OP_SQUARE,
    UMO_OP_ABS,
    UMO_OP_POW,
    UMO_OP_LOGB,
    UMO_OP_EXP,
    UMO_OP_LOG,
    UMO_OP_FACTORIAL,
    // Rounding operators
    UMO_OP_ROUND,
    UMO_OP_FLOOR,
    UMO_OP_CEIL,
    UMO_OP_SIGN,
    UMO_OP_FRAC,
    // Trigonometric operators
    UMO_OP_COS,
    UMO_OP_SIN,
    UMO_OP_TAN,
    UMO_OP_COSH,
    UMO_OP_SINH,
    UMO_OP_TANH,
    UMO_OP_ACOS,
    UMO_OP_ASIN,
    UMO_OP_ATAN,
    UMO_OP_ACOSH,
    UMO_OP_ASINH,
    UMO_OP_ATANH,
    // Direct support for linear models
    UMO_OP_LINEAR,
    UMO_OP_LINEARCOMP,


    // End indicator
    UMO_OP_END
};

typedef enum umo_type umo_type;
typedef enum umo_objective_direction umo_objective_direction;
typedef enum umo_solution_status umo_solution_status;
typedef enum umo_operator umo_operator;

#ifdef __cplusplus
}
#endif

#endif

