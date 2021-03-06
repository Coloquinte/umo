#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE COMPUTE

#include <boost/test/unit_test.hpp>

#include "api/umo.hpp"

#include <cmath>
#include <vector>
#include <algorithm>

using namespace umo;

BOOST_AUTO_TEST_CASE(FloatCompute) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(0.0, 10.0);
    FloatExpression x01 = dec1 + dec2;
    FloatExpression x02 = dec1 - dec2;
    FloatExpression x03 = dec1 * dec2;
    FloatExpression x04 = dec1 / dec2;
    FloatExpression x05 = -dec1;
    for (double val1 : {5.0, -4.0, 2.5}) {
        for (double val2 : {-7.0, 2.0, 1.3}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(), val1 + val2);
            BOOST_CHECK_EQUAL(x02.getValue(), val1 - val2);
            BOOST_CHECK_EQUAL(x03.getValue(), val1 * val2);
            BOOST_CHECK_EQUAL(x04.getValue(), val1 / val2);
            BOOST_CHECK_EQUAL(x05.getValue(), -val1);
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(IntCompute) {
    Model model;
    IntExpression dec1 = model.intVar(0, 10);
    IntExpression dec2 = model.intVar(0, 10);
    IntExpression x01 = dec1 + dec2;
    IntExpression x02 = dec1 - dec2;
    IntExpression x03 = dec1 * dec2;
    IntExpression x04 = dec1 / dec2;
    IntExpression x05 = dec1 % dec2;
    IntExpression x06 = -dec1;
    for (long long val1 : {5, -4, 0}) {
        for (long long val2 : {-7, 2}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(), val1 + val2);
            BOOST_CHECK_EQUAL(x02.getValue(), val1 - val2);
            BOOST_CHECK_EQUAL(x03.getValue(), val1 * val2);
            BOOST_CHECK_EQUAL(x04.getValue(), val1 / val2);
            BOOST_CHECK_EQUAL(x05.getValue(), val1 % val2);
            BOOST_CHECK_EQUAL(x06.getValue(), -val1);
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(BoolCompute) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression x01 = dec1 && dec2;
    BoolExpression x02 = dec1 || dec2;
    BoolExpression x03 = dec1 && !dec2;
    BoolExpression x04 = !dec1 || dec2;
    BoolExpression x05 = !(dec1 || dec2);
    BoolExpression x06 = !(dec1 && dec2);
    BoolExpression x07 = !dec1;
    for (bool val1 : {false, true}) {
        for (bool val2 : {false, true}) {
            dec1.setValue(val1);
            dec2.setValue(val2);
            BOOST_CHECK_EQUAL(x01.getValue(), val1 && val2);
            BOOST_CHECK_EQUAL(x02.getValue(), val1 || val2);
            BOOST_CHECK_EQUAL(x03.getValue(), val1 && !val2);
            BOOST_CHECK_EQUAL(x04.getValue(), !val1 || val2);
            BOOST_CHECK_EQUAL(x05.getValue(), !(val1 || val2));
            BOOST_CHECK_EQUAL(x06.getValue(), !(val1 && val2));
            BOOST_CHECK_EQUAL(x07.getValue(), !val1);
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(Operations) {
    Model model;
    FloatExpression dec = model.floatVar(-10.0, 10.0);
    FloatExpression x01 = umo::exp(dec);
    FloatExpression x02 = umo::cos(dec);
    FloatExpression x03 = umo::atan(dec);
    for (double val : {5.0, -4.0, 2.5}) {
        dec.setValue(val);
        BOOST_CHECK_EQUAL(x01.getValue(), std::exp(val));
        BOOST_CHECK_EQUAL(x02.getValue(), std::cos(val));
        BOOST_CHECK_EQUAL(x03.getValue(), std::atan(val));
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(Comparisons) {
    Model model;
    FloatExpression dec1 = model.floatVar(0.0, 10.0);
    FloatExpression dec2 = model.floatVar(0.0, 10.0);
    BoolExpression x01 = dec1 == dec2;
    BoolExpression x02 = dec1 != dec2;
    BoolExpression x03 = dec1 <= dec2;
    BoolExpression x04 = dec1 >= dec2;
    BoolExpression x05 = dec1 < dec2;
    BoolExpression x06 = dec1 > dec2;

    // Should compare equal
    dec1.setValue(1.0 - 1e-7);
    dec2.setValue(1.0);
    BOOST_CHECK_EQUAL(x01.getValue(), true);  // ==
    BOOST_CHECK_EQUAL(x02.getValue(), false); // !=
    BOOST_CHECK_EQUAL(x03.getValue(), true);  // <=
    BOOST_CHECK_EQUAL(x04.getValue(), true);  // >=
    BOOST_CHECK_EQUAL(x05.getValue(), false); // <
    BOOST_CHECK_EQUAL(x06.getValue(), false); // >

    // Should compare different
    dec1.setValue(1.0 - 1e-5);
    dec2.setValue(1.0);
    BOOST_CHECK_EQUAL(x01.getValue(), false); // ==
    BOOST_CHECK_EQUAL(x02.getValue(), true);  // !=
    BOOST_CHECK_EQUAL(x03.getValue(), true);  // <=
    BOOST_CHECK_EQUAL(x04.getValue(), false); // >=
    BOOST_CHECK_EQUAL(x05.getValue(), true);  // <
    BOOST_CHECK_EQUAL(x06.getValue(), false); // >

    // Should compare equal
    dec1.setValue(1e-9);
    dec2.setValue(0.0);
    BOOST_CHECK_EQUAL(x01.getValue(), true);  // ==
    BOOST_CHECK_EQUAL(x02.getValue(), false); // !=
    BOOST_CHECK_EQUAL(x03.getValue(), true);  // <=
    BOOST_CHECK_EQUAL(x04.getValue(), true);  // >=
    BOOST_CHECK_EQUAL(x05.getValue(), false); // <
    BOOST_CHECK_EQUAL(x06.getValue(), false); // >

    // Should compare different
    dec1.setValue(1e-7);
    dec2.setValue(0.0);
    BOOST_CHECK_EQUAL(x01.getValue(), false); // ==
    BOOST_CHECK_EQUAL(x02.getValue(), true);  // !=
    BOOST_CHECK_EQUAL(x03.getValue(), false); // <=
    BOOST_CHECK_EQUAL(x04.getValue(), true);  // >=
    BOOST_CHECK_EQUAL(x05.getValue(), false); // <
    BOOST_CHECK_EQUAL(x06.getValue(), true);  // >

    // Should compare different
    dec1.setValue(1e-5);
    dec2.setValue(0.0);
    BOOST_CHECK_EQUAL(x01.getValue(), false); // ==
    BOOST_CHECK_EQUAL(x02.getValue(), true);  // !=
    BOOST_CHECK_EQUAL(x03.getValue(), false); // <=
    BOOST_CHECK_EQUAL(x04.getValue(), true);  // >=
    BOOST_CHECK_EQUAL(x05.getValue(), false); // <
    BOOST_CHECK_EQUAL(x06.getValue(), true);  // >

    // Just for directions
    dec1.setValue(1.0);
    dec2.setValue(2.0);
    BOOST_CHECK_EQUAL(x03.getValue(), true);  // <=
    BOOST_CHECK_EQUAL(x04.getValue(), false); // >=
    BOOST_CHECK_EQUAL(x05.getValue(), true);  // <
    BOOST_CHECK_EQUAL(x06.getValue(), false); // >
    dec1.setValue(2.0);
    dec2.setValue(1.0);
    BOOST_CHECK_EQUAL(x03.getValue(), false); // <=
    BOOST_CHECK_EQUAL(x04.getValue(), true);  // >=
    BOOST_CHECK_EQUAL(x05.getValue(), false); // <
    BOOST_CHECK_EQUAL(x06.getValue(), true);  // >

    model.check();
}

BOOST_AUTO_TEST_CASE(ModelStatus) {
    Model model;
    BoolExpression dec = model.boolVar();
    constraint(dec);
    dec.setValue(false);
    BOOST_CHECK(model.getStatus() == Status::Invalid);
    dec.setValue(true);
    BOOST_CHECK(model.getStatus() == Status::Valid);
}

BOOST_AUTO_TEST_CASE(IntNary) {
    Model model;
    IntExpression dec1 = model.intVar();
    IntExpression dec2 = model.intVar();
    IntExpression dec3 = model.intVar();
    std::vector<IntExpression> vec{dec1, dec2, dec3};
    IntExpression x01 = sum(vec);
    IntExpression x02 = prod(vec);
    IntExpression x03 = min(vec);
    IntExpression x04 = max(vec);
    for (long long val1 : {5, -4, 0}) {
        for (long long val2 : {-7, 2, 8}) {
            for (long long val3 : {-17, -5, 0, 4}) {
                dec1.setValue(val1);
                dec2.setValue(val2);
                dec3.setValue(val3);
                std::vector<long long> vals{val1, val2, val3};
                BOOST_CHECK_EQUAL(x01.getValue(), val1 + val2 + val3);
                BOOST_CHECK_EQUAL(x02.getValue(), val1 * val2 * val3);
                BOOST_CHECK_EQUAL(x03.getValue(), *std::min_element(vals.begin(), vals.end()));
                BOOST_CHECK_EQUAL(x04.getValue(), *std::max_element(vals.begin(), vals.end()));
            }
        }
    }
    model.check();
}

BOOST_AUTO_TEST_CASE(BoolNary) {
    Model model;
    BoolExpression dec1 = model.boolVar();
    BoolExpression dec2 = model.boolVar();
    BoolExpression dec3 = model.boolVar();
    std::vector<BoolExpression> vec{dec1, dec2, dec3};
    BoolExpression x01 = logical_or(vec);
    BoolExpression x02 = logical_and(vec);
    BoolExpression x03 = logical_xor(vec);
    for (long long val1 : {false, true}) {
        for (long long val2 : {false, true}) {
            for (long long val3 : {false, true}) {
                dec1.setValue(val1);
                dec2.setValue(val2);
                dec3.setValue(val3);
                BOOST_CHECK_EQUAL(x01.getValue(), val1 || val2 || val3);
                BOOST_CHECK_EQUAL(x02.getValue(), val1 && val2 && val3);
                BOOST_CHECK_EQUAL(x03.getValue(), val1 ^ val2 ^ val3);
            }
        }
    }
    model.check();
}
