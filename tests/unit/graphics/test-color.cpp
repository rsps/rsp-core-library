/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <Color.h>
#include <doctest.h>

uint8_t red = 0x12;
uint8_t green = 0x34;
uint8_t blue = 0x56;
uint8_t alpha = 0x78;
uint32_t coloVal = 0x12345678;
Color testCol(coloVal);

TEST_CASE("Color Contructors") {
    SUBCASE("Separate values contructor") {
        Color col(red, green, blue, alpha);

        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
        CHECK(col.GetAlpha() == alpha);
    }
    SUBCASE("Single value contructor") {
        Color col(coloVal);

        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
        CHECK(col.GetAlpha() == alpha);
    }
    SUBCASE("Copy contructor") {
        Color col(testCol);

        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
        CHECK(col.GetAlpha() == alpha);
    }
}
TEST_CASE("Color Sets") {
    uint8_t newCol = 0x55;
    SUBCASE("Set Red") {
        testCol.SetRed(newCol);

        CHECK(testCol.GetRed() == newCol);
    }
    SUBCASE("Set Green") {
        testCol.SetGreen(newCol);

        CHECK(testCol.GetGreen() == newCol);
    }
    SUBCASE("Set Blue") {
        testCol.SetBlue(newCol);

        CHECK(testCol.GetBlue() == newCol);
    }
    SUBCASE("Set Alpha") {
        testCol.SetAlpha(newCol);

        CHECK(testCol.GetAlpha() == newCol);
    }
}