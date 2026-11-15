#include <core/logger.h>

#include <math/kmath.h>

/**
 * @brief expects expected to be equal to actual
 */
#define expect_should_be(expected, actual)                                                              \
    if (actual != expected) {                                                                           \
        KERROR("--> Expected %lld, but got %lld. File: %s:%d", expected, actual, __FILE__, __LINE__);    \
        return false;                                                                                   \
    }

/**
 * @brief expects expected not to be equal to actual
 */
#define expect_should_not_be(expected, actual)                                                                  \
    if (actual == expected) {                                                                                   \
        KERROR("--> Expected %d != %d, but they are equal. File: %s:%d", expected, actual, __FILE__, __LINE__); \
        return false;                                                                                           \
    }

/**
 * @brief expects expected to be actual given a tolerance
 */
#define expect_float_to_be(expected, actual)                                                        \
    if (kabs(expected - actual) > 0.001f) {                                                         \
        KERROR("--> Expected %f but got %f. File: %s:%d", expected, actual, __FILE__, __LINE__);    \
        return false;                                                                               \
    }

/**
 * @brief expects actual to be true
 */
#define expect_true(actual)                                                             \
    if (!actual) {                                                                      \
        KERROR("--> Expected TRUE, but got FALSE. File: %s:%d", __FILE__, __LINE__);    \
        return false;                                                                   \
    }

/**
 * @brief expects actual to be false
 */
#define expect_false(actual)                                                            \
    if (actual) {                                                                       \
        KERROR("--> Expected FALSE, but got TRUE. File: %s:%d", __FILE__, __LINE__);    \
        return false;                                                                   \
    }