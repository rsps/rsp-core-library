/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "doctest.h"
#include <utils/Rounding.h>
#include <cmath>

TEST_CASE("Rounding") {

#define COMPARE_AND_CHECK(a, b) CHECK_LT(std::fabs(a - b), 0.00000001f)

    SUBCASE("Test 0.01 precision") {
        COMPARE_AND_CHECK(ROUND001(0.0f), 0.00f);
        COMPARE_AND_CHECK(ROUND001(0.12345f), 0.12f);
        COMPARE_AND_CHECK(ROUND001(0.12512f), 0.13f);
        COMPARE_AND_CHECK(ROUND001(0.995f), 1.00f);
    }

    SUBCASE("Test 0.1 precision") {
        COMPARE_AND_CHECK(ROUND01(0.12345f), 0.1f);
        COMPARE_AND_CHECK(ROUND01(0.1512f), 0.2f);
        COMPARE_AND_CHECK(ROUND01(0.95f), 1.00f);
    }

    SUBCASE("Test 0.2 precision") {
        COMPARE_AND_CHECK(ROUND02(0.02345f), 0.0f);
        COMPARE_AND_CHECK(ROUND02(0.12345f), 0.2f);
        COMPARE_AND_CHECK(ROUND02(0.10f), 0.2f);
        COMPARE_AND_CHECK(ROUND02(0.0912f), 0.0f);
        COMPARE_AND_CHECK(ROUND02(0.95f), 1.00f);
    }

    SUBCASE("Test 1 precision") {
        COMPARE_AND_CHECK(ROUND1(0.12345f), 0.0f);
        COMPARE_AND_CHECK(ROUND1(0.10f), 0.0f);
        COMPARE_AND_CHECK(ROUND1(0.512f), 1.00f);
        COMPARE_AND_CHECK(ROUND1(0.95f), 1.00f);
        COMPARE_AND_CHECK(ROUND1(1.499f), 1.00f);
        COMPARE_AND_CHECK(ROUND1(1.501f), 2.00f);
        COMPARE_AND_CHECK(ROUND1(52860 / 1000.0f), 53.00f);
    }

    SUBCASE("Test 2 precision") {
        COMPARE_AND_CHECK(ROUND2(0.12345f), 0.0f);
        COMPARE_AND_CHECK(ROUND2(0.95f), 0.0f);
        COMPARE_AND_CHECK(ROUND2(1.000f), 2.00f);
        COMPARE_AND_CHECK(ROUND2(1.501f), 2.00f);
        COMPARE_AND_CHECK(ROUND2(1.99f), 2.00f);
        COMPARE_AND_CHECK(ROUND2(2.99f), 2.00f);
        COMPARE_AND_CHECK(ROUND2(3.00f), 4.00f);
        COMPARE_AND_CHECK(ROUND2(3.99f), 4.00f);
    }

}


