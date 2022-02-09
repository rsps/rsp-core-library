/*
 * test-rounding.cpp
 *
 *  Created on: Feb 28, 2020
 *      Author: Steffen Brummer
 */

#include "doctest.h"
#include <utils/Rounding.h>

TEST_CASE("Testing the Rounding macros") {


    SUBCASE("Test 0.01 precision") {
        CHECK(ROUND001(0.0f) == 0.00f);
        CHECK(ROUND001(0.12345f) == 0.12f);
        CHECK(ROUND001(0.12512f) == 0.13f);
        CHECK(ROUND001(0.995f) == 1.00f);
    }

    SUBCASE("Test 0.1 precision") {
        CHECK(ROUND01(0.12345f) == 0.1f);
        CHECK(ROUND01(0.1512f) == 0.2f);
        CHECK(ROUND01(0.95f) == 1.00f);
    }

    SUBCASE("Test 0.2 precision") {
        CHECK(ROUND02(0.02345f) == 0.0f);
        CHECK(ROUND02(0.12345f) == 0.2f);
        CHECK(ROUND02(0.10f) == 0.2f);
        CHECK(ROUND02(0.0912f) == 0.0f);
        CHECK(ROUND02(0.95f) == 1.00f);
    }

    SUBCASE("Test 1 precision") {
        CHECK(ROUND1(0.12345f) == 0.0f);
        CHECK(ROUND1(0.10f) == 0.0f);
        CHECK(ROUND1(0.512f) == 1.00f);
        CHECK(ROUND1(0.95f) == 1.00f);
        CHECK(ROUND1(1.499f) == 1.00f);
        CHECK(ROUND1(1.501f) == 2.00f);
        CHECK(ROUND1(52860 / 1000.0f) == 53.00f);
    }

    SUBCASE("Test 2 precision") {
        CHECK(ROUND2(0.12345f) == 0.0f);
        CHECK(ROUND2(0.95f) == 0.0f);
        CHECK(ROUND2(1.000f) == 2.00f);
        CHECK(ROUND2(1.501f) == 2.00f);
        CHECK(ROUND2(1.99f) == 2.00f);
        CHECK(ROUND2(2.99f) == 2.00f);
        CHECK(ROUND2(3.00f) == 4.00f);
        CHECK(ROUND2(3.99f) == 4.00f);
    }

}


