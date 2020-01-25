
cdef extern from "api/umo.h":
    enum umo_operator:
        UMO_OP_INVALID,
        UMO_OP_CONSTANT,
        UMO_OP_DEC_BOOL,
        UMO_OP_DEC_INT,
        UMO_OP_DEC_FLOAT,
        UMO_OP_NOT,
        UMO_OP_AND,
        UMO_OP_OR,
        UMO_OP_XOR,
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
        UMO_OP_CMP_EQ,
        UMO_OP_CMP_NEQ,
        UMO_OP_CMP_LEQ,
        UMO_OP_CMP_GEQ,
        UMO_OP_CMP_LT,
        UMO_OP_CMP_GT,
        UMO_OP_SQRT,
        UMO_OP_SQUARE,
        UMO_OP_ABS,
        UMO_OP_POW,
        UMO_OP_LOGB,
        UMO_OP_EXP,
        UMO_OP_LOG,
        UMO_OP_FACTORIAL,
        UMO_OP_ROUND,
        UMO_OP_FLOOR,
        UMO_OP_CEIL,
        UMO_OP_SIGN,
        UMO_OP_FRAC,
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
        UMO_OP_LINEAR,
        UMO_OP_LINEARCOMP,
        UMO_OP_END

    enum umo_solution_status:
        UMO_STATUS_INFEASIBLE,
        UMO_STATUS_INVALID,
        UMO_STATUS_VALID,
        UMO_STATUS_OPTIMAL

    enum umo_objective_direction:
        UMO_OBJ_MINIMIZE,
        UMO_OBJ_MAXIMIZE

    struct umo_model:
        pass

    umo_model *umo_create_model(const char **err)
    void umo_destroy_model(umo_model *, const char **err)
    long long umo_create_constant(umo_model *, double value, const char **err)
    long long umo_create_expression(umo_model *, umo_operator op, int nb_operands, long long *operands, const char **err)
    void umo_create_constraint(umo_model *, long long expr, const char **err)
    void umo_create_objective(umo_model *, long long expr, umo_objective_direction dir, const char **err)
    double umo_get_float_parameter(umo_model *, const char *param, const char **err)
    void umo_set_float_parameter(umo_model *, const char *param, double value, const char **err)
    const char *umo_get_string_parameter(umo_model *, const char *param, const char **err)
    void umo_set_string_parameter(umo_model *, const char *param, const char *value, const char **err)
    double umo_get_float_value(umo_model *, long long expr, const char **err)
    void umo_set_float_value(umo_model *, long long expr, double value, const char **err)
    umo_solution_status umo_get_solution_status(umo_model *, const char **err)
    void umo_solve(umo_model *, const char **err)
    void umo_check(umo_model *, const char **err)


from cpython.object cimport Py_EQ, Py_NE, Py_LT, Py_GT, Py_LE, Py_GE

cdef unwrap_error(const char **err):
    if (err[0]) != NULL:
        raise RuntimeError("An error occured")
        # TODO: free err[0]

cdef is_bool(value):
    return value == bool(value)

cdef is_int(value):
    return value == int(value)

cdef class Model:
    cdef umo_model *ptr

    def __cinit__(self):
        cdef const char* err = NULL
        self.ptr = umo_create_model(&err)
        unwrap_error(&err)

    def __dealloc__(self):
        cdef const char* err = NULL
        umo_destroy_model(self.ptr, &err)
        unwrap_error(&err)

    def constant(self, value):
        cdef const char *err = NULL
        cdef long long v = umo_create_constant(self.ptr, <double> value, &err)
        unwrap_error(&err)
        if is_bool(value):
            return BoolExpression.create(self.ptr, v)
        elif is_int(value):
            return IntExpression.create(self.ptr, v)
        else:
            return FloatExpression.create(self.ptr, v)

    def bool_var(self):
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(self.ptr, UMO_OP_DEC_BOOL, 0, NULL, &err)
        unwrap_error(&err)
        return BoolExpression.create(self.ptr, v)

    def int_var(self, lb, ub):
        # TODO: check types
        clb = self.constant(lb)
        cub = self.constant(ub)
        return Expression._binary_method(clb, cub, UMO_OP_DEC_INT)._asint()

    def float_var(self, lb, ub):
        # TODO: check types
        clb = self.constant(lb)
        cub = self.constant(ub)
        return Expression._binary_method(clb, cub, UMO_OP_DEC_FLOAT)._asfloat()

    def __repr__(self):
        return "Model"


cdef class Expression:
    cdef umo_model *ptr
    cdef long long v

    @staticmethod
    cdef create(umo_model *ptr, long long v):
        expr = Expression()
        expr.ptr = ptr
        expr.v = v
        return expr

    def __bool__(self):
        raise NotImplementedError("Pyumo expressions are not convertible to booleans; use Expression.value to query its value in a given solution.")

    @staticmethod
    cdef Expression _unary_method(Expression expr, umo_operator op):
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(expr.ptr, op, 1, &expr.v, &err)
        unwrap_error(&err)
        return Expression.create(expr.ptr, v)

    @staticmethod
    cdef Expression _binary_method(Expression o1, Expression o2, umo_operator op):
        # TODO: handle numeric types
        cdef long long operands[2]
        operands[0] = o1.v
        operands[1] = o2.v
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(o1.ptr, op, 2, operands, &err)
        unwrap_error(&err)
        return Expression.create(o1.ptr, v)

    cdef FloatExpression _asfloat(self):
        expr = FloatExpression()
        expr.ptr = self.ptr
        expr.v = self.v
        return expr

    cdef IntExpression _asint(self):
        expr = IntExpression()
        expr.ptr = self.ptr
        expr.v = self.v
        return expr

    cdef BoolExpression _asbool(self):
        expr = BoolExpression()
        expr.ptr = self.ptr
        expr.v = self.v
        return expr

    def __repr__(self):
        return "Expression#{}".format(self.v)


cdef class FloatExpression(Expression):
    @staticmethod
    cdef create(umo_model *ptr, long long v):
        expr = FloatExpression()
        expr.ptr = ptr
        expr.v = v
        return expr

    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.ptr, self.v, &err)
        unwrap_error(&err)
        return float(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.ptr, self.v, value, &err)
        unwrap_error(&err)

    def __richcmp__(o1, o2, int op):
        umo_op = None
        if op == Py_LT:
            umo_op = UMO_OP_CMP_LT
        elif op == Py_GT:
            umo_op = UMO_OP_CMP_GT
        elif op == Py_GE:
            umo_op = UMO_OP_CMP_GEQ
        elif op == Py_LE:
            umo_op = UMO_OP_CMP_LEQ
        elif op == Py_EQ:
            umo_op = UMO_OP_CMP_EQ
        elif op == Py_NE:
            umo_op = UMO_OP_CMP_NEQ
        return Expression._binary_method(o1, o2, umo_op)._asbool()

    def __add__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_SUM)._asfloat()

    def __sub__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_MINUS_BINARY)._asfloat()

    def __mul__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_PROD)._asfloat()

    def __truediv__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_DIV)._asfloat()

    def __neg__(self):
        return Expression._unary_method(self, UMO_OP_MINUS_UNARY)._asfloat()

    def __pos__(self):
        return self

    def __abs__(self):
        return Expression._unary_method(self, UMO_OP_ABS)._asfloat()

    def __pow__(o1, o2, o3):
        return Expression._binary_method(o1, o2, UMO_OP_POW)._asfloat()

    def __repr__(self):
        return "Float#{}".format(self.v)


cdef class IntExpression(FloatExpression):
    @staticmethod
    cdef create(umo_model *ptr, long long v):
        expr = IntExpression()
        expr.ptr = ptr
        expr.v = v
        return expr

    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.ptr, self.v, &err)
        unwrap_error(&err)
        return int(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.ptr, self.v, value, &err)
        unwrap_error(&err)

    def __floordiv__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_IDIV)._asint()

    def __mod__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_MOD)._asint()

    def __repr__(self):
        return "Int#{}".format(self.v)


cdef class BoolExpression(IntExpression):
    @staticmethod
    cdef create(umo_model *ptr, long long v):
        expr = BoolExpression()
        expr.ptr = ptr
        expr.v = v
        return expr

    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.ptr, self.v, &err)
        unwrap_error(&err)
        return bool(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.ptr, self.v, value, &err)
        unwrap_error(&err)

    def __and__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_AND)._asbool()

    def __or__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_OR)._asbool()

    def __xor__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_XOR)._asbool()

    def __invert__(self):
        return Expression._unary_method(self, UMO_OP_NOT)._asbool()

    def __repr__(self):
        return "Bool#{}".format(self.v)



