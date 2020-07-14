
"""
UMO is an optimization modeler
"""

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
        UMO_STATUS_OPTIMAL,
        UMO_STATUS_UNBOUNDED,
        UMO_STATUS_UNKNOWN

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
    try:
        return isinstance(value, bool) or value == bool(value)
    except:
        return False


cdef is_int(value):
    try:
        return isinstance(value, int) or value == int(value)
    except:
        return False


cdef is_float(value):
    try:
        return isinstance(value, float) or value == float(value)
    except:
        return False


cdef is_boolexpr(value):
    if isinstance(value, Expression):
        return isinstance(value, BoolExpression)
    else:
        return is_bool(value)


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
    UNBOUNDED = 4
    UNKNOWN = 5


cdef class UnboundedT:
    DUMMY = 0


cdef class Model:
    """
    Representation of an optimization model
    """
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
        """
        Create a constant expression
        """
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
        """
        Create a binary decision variable
        """
        cdef const char *err = NULL
        cdef long long v = umo_create_expression(self.ptr, UMO_OP_DEC_BOOL, 0, NULL, &err)
        unwrap_error(&err)
        return Expression.create(self, v)._asbool()

    def int_var(self, lb=UnboundedT.DUMMY, ub=UnboundedT.DUMMY):
        """
        Create an integer decision variable, with optional lower and upper bound
        """
        if isinstance(lb, UnboundedT):
            lb = -float("inf")
        if isinstance(ub, UnboundedT):
            ub = float("inf")
        if lb != -float("inf") and not is_int(lb):
            raise TypeError("The first int_var() argument must be an integer or -inf")
        if ub != float("inf") and not is_int(ub):
            raise TypeError("The second int_var() argument must be an integer or +inf")
        cdef Expression clb = self.constant(lb)
        cdef Expression cub = self.constant(ub)
        return Expression._binary_method_typed(clb, cub, UMO_OP_DEC_INT)._asint()

    def float_var(self, lb=UnboundedT.DUMMY, ub=UnboundedT.DUMMY):
        """
        Create a numeric decision variable, with optional lower and upper bound
        """
        if isinstance(lb, UnboundedT):
            lb = -float("inf")
        if isinstance(ub, UnboundedT):
            ub = float("inf")
        if lb != -float("inf") and not is_float(lb):
            raise TypeError("The first float_var() argument must be a number or -inf")
        if ub != float("inf") and not is_float(ub):
            raise TypeError("The second float_var() argument must be a number or +inf")
        cdef Expression clb = self.constant(lb)
        cdef Expression cub = self.constant(ub)
        return Expression._binary_method_typed(clb, cub, UMO_OP_DEC_FLOAT)._asfloat()

    @property
    def status(self):
        """
        Retrieve the current solution's status
        """
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
        elif status == UMO_STATUS_UNBOUNDED:
            return SolutionStatus.UNBOUNDED
        elif status == UMO_STATUS_UNKNOWN:
            return SolutionStatus.UNKNOWN
        else:
            raise RuntimeError("Unknown solution status")

    @property
    def time_limit(self):
        """
        The time limit to solve the problem, in seconds
        """
        return self.get_float_param("time_limit")

    @time_limit.setter
    def time_limit(self, value):
        self.set_float_param("time_limit", value)

    @property
    def solver(self):
        """
        The solver to be used
        """
        return self.get_string_param("solver")

    @solver.setter
    def solver(self, value):
        self.set_string_param("solver", value)

    def solve(self):
        """
        Solve the model
        """
        cdef const char *err = NULL
        umo_solve(self.ptr, &err)
        unwrap_error(&err)

    def check(self):
        """
        Check the model
        """
        cdef const char *err = NULL
        umo_check(self.ptr, &err)
        unwrap_error(&err)

    def get_float_param(self, param):
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        cdef double val = umo_get_float_parameter(self.ptr, cparam, &err)
        unwrap_error(&err)
        return val

    def set_float_param(self, param, value):
        if not is_float(value):
            raise TypeError("set_float_param() argument must be a number")
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        umo_set_float_parameter(self.ptr, cparam, float(value), &err)
        unwrap_error(&err)

    def get_string_param(self, param):
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        cdef const char* val = umo_get_string_parameter(self.ptr, cparam, &err)
        unwrap_error(&err)
        return (<bytes> val).decode('UTF-8')

    def set_string_param(self, param, value):
        if not isinstance(value, str):
            raise TypeError("set_string_param() argument must be a string")
        cdef const char* err = NULL
        cparam = param.encode('UTF-8')
        cvalue = value.encode('UTF-8')
        umo_set_string_parameter(self.ptr, cparam, cvalue, &err)
        unwrap_error(&err)


cdef class Expression:
    """
    An expression in an optimization model
    """
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
        """
        Forbid implicit conversion to bool
        """
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

    def _infer_int_float(self, o1, o2=None):
        if is_intexpr(o1) and (o2 is None or is_intexpr(o2)):
            return self._asint()
        else:
            return self._asfloat()

    def _infer_bool_int_float(self, o1, o2=None):
        if is_boolexpr(o1) and (o2 is None or is_boolexpr(o2)):
            return self._asbool()
        elif is_intexpr(o1) and (o2 is None or is_intexpr(o2)):
            return self._asint()
        else:
            return self._asfloat()


cdef class FloatExpression(Expression):
    """
    A numeric expression in an optimization model
    """
    @property
    def value(self):
        """
        Value in the current solution
        """
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
        return Expression._binary_method(o1, o2, UMO_OP_SUM)._infer_int_float(o1, o2)

    def __sub__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_MINUS_BINARY)._infer_int_float(o1, o2)

    def __mul__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_PROD)._infer_bool_int_float(o1, o2)

    def __div__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_DIV)._asfloat()

    def __truediv__(o1, o2):
        return Expression._binary_method(o1, o2, UMO_OP_DIV)._asfloat()

    def __neg__(self):
        return Expression._unary_method(self, UMO_OP_MINUS_UNARY)._infer_int_float(self)

    def __pos__(self):
        return self

    def __abs__(self):
        return Expression._unary_method(self, UMO_OP_ABS)._infer_int_float(self)

    def __round__(self):
        return Expression._unary_method(self, UMO_OP_ROUND)._asint()

    def __pow__(o1, o2, o3):
        return Expression._binary_method(o1, o2, UMO_OP_POW)._asfloat()


cdef class IntExpression(FloatExpression):
    """
    An integer expression in an optimization model
    """

    @property
    def value(self):
        """
        Value in the current solution
        """
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
    """
    A binary expression in an optimization model
    """

    @property
    def value(self):
        """
        Value in the current solution
        """
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
    """
    Constrain the binary expression to be true
    """
    if not isinstance(expr, BoolExpression):
        raise RuntimeError("constraint() argument must be a BoolExpression")
    cdef BoolExpression o = <BoolExpression> expr
    cdef const char* err = NULL
    umo_create_constraint(o.get_ptr(), o.v, &err)
    unwrap_error(&err)


def minimize(expr):
    """
    Add an objective function to be minimized
    """
    if not isinstance(expr, FloatExpression):
        raise RuntimeError("minimize() argument must be a FloatExpression")
    cdef FloatExpression o = <FloatExpression> expr
    cdef const char* err = NULL
    umo_create_objective(o.get_ptr(), o.v, UMO_OBJ_MINIMIZE, &err)
    unwrap_error(&err)


def maximize(expr):
    """
    Add an objective function to be maximized
    """
    if not isinstance(expr, FloatExpression):
        raise RuntimeError("maximize() argument must be a FloatExpression")
    cdef FloatExpression o = <FloatExpression> expr
    cdef const char* err = NULL
    umo_create_objective(o.get_ptr(), o.v, UMO_OBJ_MAXIMIZE, &err)
    unwrap_error(&err)


def exp(expr):
    """Exponential function."""
    return Expression._unary_method(expr, UMO_OP_EXP)._asfloat()


def log(expr):
    """Logarithm function."""
    return Expression._unary_method(expr, UMO_OP_LOG)._asfloat()


def sqrt(expr):
    """Square root function."""
    return Expression._unary_method(expr, UMO_OP_SQRT)._asfloat()


def square(expr):
    """Square function."""
    return Expression._unary_method(expr, UMO_OP_SQUARE)._infer_int_float(expr)


def round(expr):
    """Round to nearest function."""
    return Expression._unary_method(expr, UMO_OP_ROUND)._asint()


def floor(expr):
    """Floor function."""
    return Expression._unary_method(expr, UMO_OP_FLOOR)._asint()


def ceil(expr):
    """Ceil function."""
    return Expression._unary_method(expr, UMO_OP_CEIL)._asint()


def sign(expr):
    """Sign function (-1/1)."""
    return Expression._unary_method(expr, UMO_OP_SIGN)._asint()


def frac(expr):
    """Fractional part function."""
    return Expression._unary_method(expr, UMO_OP_FRAC)._asfloat()


def cos(expr):
    """Cosine function."""
    return Expression._unary_method(expr, UMO_OP_COS)._asfloat()


def sin(expr):
    """Sine function."""
    return Expression._unary_method(expr, UMO_OP_SIN)._asfloat()


def tan(expr):
    """Tangent function."""
    return Expression._unary_method(expr, UMO_OP_TAN)._asfloat()


def cosh(expr):
    """Hyperbolic cosine function."""
    return Expression._unary_method(expr, UMO_OP_COSH)._asfloat()


def sinh(expr):
    """Hyperbolic sine function."""
    return Expression._unary_method(expr, UMO_OP_SINH)._asfloat()


def tanh(expr):
    """Hyperbolic tangent function."""
    return Expression._unary_method(expr, UMO_OP_TANH)._asfloat()


def acos(expr):
    """Inverse cosine function."""
    return Expression._unary_method(expr, UMO_OP_ACOS)._asfloat()


def asin(expr):
    """Inverse sine function."""
    return Expression._unary_method(expr, UMO_OP_ASIN)._asfloat()


def atan(expr):
    """Inverse tangent function."""
    return Expression._unary_method(expr, UMO_OP_ATAN)._asfloat()


def acosh(expr):
    """Inverse hyperbolic cosine function."""
    return Expression._unary_method(expr, UMO_OP_ACOSH)._asfloat()


def asinh(expr):
    """Inverse hyperbolic sine function."""
    return Expression._unary_method(expr, UMO_OP_ASINH)._asfloat()


def atanh(expr):
    """Inverse hyperbolic tangent function."""
    return Expression._unary_method(expr, UMO_OP_ATANH)._asfloat()


def inf():
    """Return the infinite value."""
    return float("inf")


def unbounded():
    """Return a placeholder for variable bounds."""
    return UnboundedT.DUMMY

# TODO: n-ary operations, lpsum
