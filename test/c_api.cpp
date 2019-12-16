#define BOOST_TEST_DYN_LINK           // A ajouter dans le cas d'une liaison dynamique à Boost Test
#define BOOST_TEST_MODULE MyTest      // Nom du module de test

#include <boost/test/unit_test.hpp>

#include "api/umo_api.h"

BOOST_AUTO_TEST_CASE(Creation) {
    const char *err = NULL;
    umo_model *model = umo_create_model(&err);
    BOOST_CHECK(model);
    BOOST_CHECK(!err);
    umo_destroy_model(model, &err);
    BOOST_CHECK(!err);
}

BOOST_AUTO_TEST_CASE(Constant) {
    const char *err = NULL;
    umo_model *model = umo_create_model(&err);
    BOOST_CHECK(!err);
    long long a1 = umo_create_constant(model, 0.0, &err);
    long long a2 = umo_create_constant(model, 0.0, &err);
    // Check that constants are deduplicated
    BOOST_CHECK_EQUAL(a1, a2);
    long long b = umo_create_constant(model, 1.0, &err);
    BOOST_CHECK(a1 != b);
    // Check that different constants are Ok
    double va = umo_get_float_value(model, a1, &err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(0.0, va);
    double vb = umo_get_float_value(model, b, &err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(1.0, vb);
    umo_destroy_model(model, &err);
    BOOST_CHECK(!err);
}

BOOST_AUTO_TEST_CASE(Decision) {
    const char *err = NULL;
    umo_model *model = umo_create_model(&err);
    // Create a boolean decision variable
    long long bool_dec = umo_create_expression(model, UMO_OP_DEC_BOOL, 0, NULL, &err);
    umo_set_float_value(model, bool_dec, 1.0, &err);
    BOOST_CHECK(!err);
    // Create an integer decision variable
    long long int_lb = umo_create_constant(model, -1.0, &err);
    long long int_ub = umo_create_constant(model, 3.0, &err);
    long long int_ops[2] = {int_lb, int_ub};
    long long int_dec = umo_create_expression(model, UMO_OP_DEC_INT, 2, int_ops, &err);
    umo_set_float_value(model, int_dec, 2.0, &err);
    BOOST_CHECK(!err);
    // Create a floar decision variable
    long long float_lb = umo_create_constant(model, -1.5, &err);
    long long float_ub = umo_create_constant(model, 3.5, &err);
    long long float_ops[2] = {float_lb, float_ub};
    long long float_dec = umo_create_expression(model, UMO_OP_DEC_FLOAT, 2, float_ops, &err);
    umo_set_float_value(model, float_dec, -0.5, &err);
    BOOST_CHECK(!err);
    umo_destroy_model(model, &err);
    BOOST_CHECK(!err);
}

