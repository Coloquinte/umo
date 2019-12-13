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


