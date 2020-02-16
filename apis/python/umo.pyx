
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
from libc.stdlib cimport free


cdef unwrap_error(const char **err):
    if (err[0]) != NULL:
        msg = err[0].decode('UTF-8')
        free(<char*> err[0])
        err[0] = NULL
        raise RuntimeError(msg)


cdef is_bool(value):
    return isinstance(value, bool) or value == bool(value)


cdef is_int(value):
    return isinstance(value, int) or value == int(value)


cdef is_float(value):
    return isinstance(value, float) or value == float(value)


cdef is_intexpr(value):
    if isinstance(value, Expression):
        return isinstance(value, IntExpression)
    else:
        return is_int(value)


class SolutionStatus:
    INFEASIBLE = 0
    VALID = 1
    INVALID = 2
    OPTIMAL = 3


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
        if not is_float(value):
            raise TypeError("constant() argument must be a number")
        cdef const char *err = NULL
        cdef long long v = umo_create_constant(self.ptr, <double> value, &err)
        unwrap_error(&err)
        cdef Expression expr = Expression.create(self, v)
        if is_bool(value):
            return expr._asbool()
        elif is_int(value):
            return expr._asint()
        else:
            return expr._asfloat()

    def bool_var(self):
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(self.ptr, UMO_OP_DEC_BOOL, 0, NULL, &err)
        unwrap_error(&err)
        return Expression.create(self, v)._asbool()

    def int_var(self, lb, ub):
        if not is_int(lb) or not is_int(ub):
            raise TypeError("int_var() arguments must be integer numbers")
        cdef Expression clb = self.constant(lb)
        cdef Expression cub = self.constant(ub)
        return Expression._binary_method_typed(clb, cub, UMO_OP_DEC_INT)._asint()

    def float_var(self, lb, ub):
        if not is_float(lb) or not is_float(ub):
            raise TypeError("float_var() arguments must be numbers")
        cdef Expression clb = self.constant(lb)
        cdef Expression cub = self.constant(ub)
        return Expression._binary_method_typed(clb, cub, UMO_OP_DEC_FLOAT)._asfloat()

    @property
    def status(self):
        cdef const char *err = NULL
        cdef umo_solution_status status = umo_get_solution_status(self.ptr, &err)
        unwrap_error(&err)
        if status == UMO_STATUS_INFEASIBLE:
            return SolutionStatus.INFEASIBLE
        elif status == UMO_STATUS_INVALID:
            return SolutionStatus.INVALID
        elif status == UMO_STATUS_VALID:
            return SolutionStatus.VALID
        elif status == UMO_STATUS_OPTIMAL:
            return SolutionStatus.OPTIMAL
        else:
            raise RuntimeError("Unknown solution status")

    @property
    def time_limit(self):
        return self.get_param("umo_time_limit")

    @time_limit.setter
    def time_limit(self, value):
        self.set_param("umo_time_limit", value)

    def solve(self):
        cdef const char *err = NULL
        umo_solve(self.ptr, &err)
        unwrap_error(&err)

    def check(self):
        cdef const char *err = NULL
        umo_check(self.ptr, &err)
        unwrap_error(&err)

    def get_param(self, param):
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        cdef double val = umo_get_float_parameter(self.ptr, cparam, &err)
        unwrap_error(&err)
        return val

    def set_param(self, param, value):
        if not is_float(value):
            raise TypeError("set_param() argument must be a number")
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        umo_set_float_parameter(self.ptr, cparam, float(value), &err)
        unwrap_error(&err)

cdef class Expression:
    cdef readonly object model
    cdef long long v

    cdef umo_model *get_ptr(self):
        assert isinstance(self.model, Model)
        return (<Model> self.model).ptr

    @staticmethod
    cdef Expression create(object model, long long v):
        expr = Expression()
        expr.model = model
        expr.v = v
        return expr

    def __bool__(self):
        raise NotImplementedError("Expressions are not convertible to booleans; use Expression.value to query its value in a given solution.")

    @staticmethod
    cdef Expression _unary_method(expr, umo_operator op):
        if not isinstance(expr, Expression):
            raise TypeError("Argument must be an expression")
        return Expression._unary_method_typed(<Expression> expr, op)

    @staticmethod
    def _binary_method(expr1, expr2, op):
        if not isinstance(expr1, Expression) and not isinstance(expr2, Expression):
            raise TypeError("One of the arguments must be an expression")
        if not isinstance(expr1, Expression):
            if not is_float(expr1):
                raise TypeError("Argument must be an expression or a number")
            assert isinstance(expr2, Expression)
            expr1 = (<Expression> expr2).model.constant(expr1)
        if not isinstance(expr2, Expression):
            assert isinstance(expr1, Expression)
            if not is_float(expr2):
                raise TypeError("Argument must be an expression or a number")
            expr2 = (<Expression> expr1).model.constant(expr2)
        assert isinstance(expr1, Expression) and isinstance(expr2, Expression)
        return Expression._binary_method_typed(<Expression> expr1, <Expression> expr2, <umo_operator> op)

    @staticmethod
    cdef Expression _unary_method_typed(Expression expr, umo_operator op):
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(expr.get_ptr(), op, 1, &expr.v, &err)
        unwrap_error(&err)
        return Expression.create(expr.model, v)

    @staticmethod
    cdef Expression _binary_method_typed(Expression o1, Expression o2, umo_operator op):
        cdef long long operands[2]
        operands[0] = o1.v
        operands[1] = o2.v
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(o1.get_ptr(), op, 2, operands, &err)
        unwrap_error(&err)
        return Expression.create(o1.model, v)

    def _asfloat(self):
        expr = FloatExpression()
        expr.model = self.model
        expr.v = self.v
        return expr

    def _asint(self):
        expr = IntExpression()
        expr.model = self.model
        expr.v = self.v
        return expr

    def _asbool(self):
        expr = BoolExpression()
        expr.model = self.model
        expr.v = self.v
        return expr

    def _asrettype(self, o1, o2=None):
        if is_intexpr(o1) and (o2 is None or is_intexpr(o2)):
            return self._asint()
        else:
            return self._asfloat()


cdef class FloatExpression(Expression):
    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.get_ptr(), self.v, &err)
        unwrap_error(&err)
        return float(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.get_ptr(), self.v, value, &err)
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
        return Expression._binary_method(o1, o2, UMO_OP_SUM)._asrettype(o1, o2)

    def __sub__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_MINUS_BINARY)._asrettype(o1, o2)

    def __mul__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_PROD)._asrettype(o1, o2)

    def __div__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_DIV)._asfloat()

    def __truediv__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_DIV)._asfloat()

    def __neg__(self):
        return Expression._unary_method(self, UMO_OP_MINUS_UNARY)._asrettype(self)

    def __pos__(self):
        return self

    def __abs__(self):
        return Expression._unary_method(self, UMO_OP_ABS)._asrettype(self)

    def __round__(self):
        return Expression._unary_method(self, UMO_OP_ROUND)._asint()

    def __pow__(o1, o2, o3):
        return Expression._binary_method(o1, o2, UMO_OP_POW)._asfloat()


cdef class IntExpression(FloatExpression):
    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.get_ptr(), self.v, &err)
        unwrap_error(&err)
        return int(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.get_ptr(), self.v, value, &err)
        unwrap_error(&err)

    def __floordiv__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_IDIV)._asint()

    def __mod__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_MOD)._asint()


cdef class BoolExpression(IntExpression):
    @property
    def value(self):
        cdef const char *err = NULL
        cdef double val = umo_get_float_value(self.get_ptr(), self.v, &err)
        unwrap_error(&err)
        return bool(val)

    @value.setter
    def value(self, value):
        cdef const char *err = NULL
        umo_set_float_value(self.get_ptr(), self.v, value, &err)
        unwrap_error(&err)

    def __and__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_AND)._asbool()

    def __or__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_OR)._asbool()

    def __xor__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_XOR)._asbool()

    def __invert__(self):
        return Expression._unary_method(self, UMO_OP_NOT)._asbool()


def constraint(expr):
    if not isinstance(expr, BoolExpression):
        raise RuntimeError("constraint() argument must be a BoolExpression")
    cdef BoolExpression o = <BoolExpression> expr
    cdef const char* err = NULL
    umo_create_constraint(o.get_ptr(), o.v, &err)
    unwrap_error(&err)


def minimize(expr):
    if not isinstance(expr, FloatExpression):
        raise RuntimeError("minimize() argument must be a FloatExpression")
    cdef FloatExpression o = <FloatExpression> expr
    cdef const char* err = NULL
    umo_create_objective(o.get_ptr(), o.v, UMO_OBJ_MINIMIZE, &err)
    unwrap_error(&err)


def maximize(expr):
    if not isinstance(expr, FloatExpression):
        raise RuntimeError("maximize() argument must be a FloatExpression")
    cdef FloatExpression o = <FloatExpression> expr
    cdef const char* err = NULL
    umo_create_objective(o.get_ptr(), o.v, UMO_OBJ_MAXIMIZE, &err)
    unwrap_error(&err)


def exp(expr):
    return Expression._unary_method(expr, UMO_OP_EXP)._asfloat()


def log(expr):
    return Expression._unary_method(expr, UMO_OP_LOG)._asfloat()


def sqrt(expr):
    return Expression._unary_method(expr, UMO_OP_SQRT)._asfloat()


def square(expr):
    return Expression._unary_method(expr, UMO_OP_SQUARE)._asrettype(expr)


def round(expr):
    return Expression._unary_method(expr, UMO_OP_ROUND)._asint()


def floor(expr):
    return Expression._unary_method(expr, UMO_OP_FLOOR)._asint()


def ceil(expr):
    return Expression._unary_method(expr, UMO_OP_CEIL)._asint()


def sign(expr):
    return Expression._unary_method(expr, UMO_OP_SIGN)._asint()


def frac(expr):
    return Expression._unary_method(expr, UMO_OP_FRAC)._asfloat()


def cos(expr):
    return Expression._unary_method(expr, UMO_OP_COS)._asfloat()


def sin(expr):
    return Expression._unary_method(expr, UMO_OP_SIN)._asfloat()


def tan(expr):
    return Expression._unary_method(expr, UMO_OP_TAN)._asfloat()


def cosh(expr):
    return Expression._unary_method(expr, UMO_OP_COSH)._asfloat()


def sinh(expr):
    return Expression._unary_method(expr, UMO_OP_SINH)._asfloat()


def tanh(expr):
    return Expression._unary_method(expr, UMO_OP_TANH)._asfloat()


def acos(expr):
    return Expression._unary_method(expr, UMO_OP_ACOS)._asfloat()


def asin(expr):
    return Expression._unary_method(expr, UMO_OP_ASIN)._asfloat()


def atan(expr):
    return Expression._unary_method(expr, UMO_OP_ATAN)._asfloat()


def acosh(expr):
    return Expression._unary_method(expr, UMO_OP_ACOSH)._asfloat()


def asinh(expr):
    return Expression._unary_method(expr, UMO_OP_ASINH)._asfloat()


def atanh(expr):
    return Expression._unary_method(expr, UMO_OP_ATANH)._asfloat()

# TODO: n-ary operations, lpsum
