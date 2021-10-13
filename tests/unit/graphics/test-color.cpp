/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>
#include <graphics/primitives/Color.h>

using namespace rsp::graphics;

uint8_t red = 0x12;
uint8_t green = 0x34;
uint8_t blue = 0x56;
uint8_t alpha = 0x78;
uint32_t colourVal = 0x12345678;

TEST_CASE("Colour Contructors")
{
    SUBCASE("Separate values contructor")
    {
        //Arrange & Act
        Color col(red, green, blue, alpha);

        //Assert
        CHECK(col.GetAlpha() == alpha);
        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
    }
    SUBCASE("Single value contructor")
    {
        //Arrange & Act
        Color col(colourVal);

        //Assert
        CHECK(col.GetAlpha() == alpha);
        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
    }
    SUBCASE("Copy contructor")
    {
        //Arrange & Act
        Color testCol(colourVal);
        Color col(testCol);

        //Assert
        CHECK(col.GetAlpha() == alpha);
        CHECK(col.GetRed() == red);
        CHECK(col.GetGreen() == green);
        CHECK(col.GetBlue() == blue);
    }
}
TEST_CASE("Colour Sets")
{
    //Arrange
    Color testCol(colourVal);
    uint8_t newCol = 0x55;
    SUBCASE("Set Alpha")
    {
        //Act
        testCol.SetAlpha(newCol);

        //Assert only wanted part has changed
        CHECK(testCol.GetAlpha() == newCol);
        CHECK(testCol.GetRed() == red);
        CHECK(testCol.GetGreen() == green);
        CHECK(testCol.GetBlue() == blue);
    }
    SUBCASE("Set Red")
    {
        //Act
        testCol.SetRed(newCol);

        //Assert only wanted part has changed
        CHECK(testCol.GetRed() == newCol);
        CHECK(testCol.GetGreen() == green);
        CHECK(testCol.GetBlue() == blue);
        CHECK(testCol.GetAlpha() == alpha);
    }
    SUBCASE("Set Green")
    {
        //Act
        testCol.SetGreen(newCol);

        //Assert only wanted part has changed
        CHECK(testCol.GetGreen() == newCol);
        CHECK(testCol.GetRed() == red);
        CHECK(testCol.GetBlue() == blue);
        CHECK(testCol.GetAlpha() == alpha);
    }
    SUBCASE("Set Blue")
    {
        //Act
        testCol.SetBlue(newCol);

        //Assert only wanted part has changed
        CHECK(testCol.GetBlue() == newCol);
        CHECK(testCol.GetRed() == red);
        CHECK(testCol.GetGreen() == green);
        CHECK(testCol.GetAlpha() == alpha);
    }
}
TEST_CASE("Colour Operators")
{
    //Arrange
    Color testCol(colourVal);
    SUBCASE("uint32 Conversion")
    {
        //Act
        uint32_t newUint32 = testCol;

        //Assert
        CHECK(newUint32 == colourVal);
    }
    SUBCASE("= Operator")
    {
        //Act
        Color newColour = testCol;

        //Assert
        CHECK(static_cast<uint32_t>(newColour) == static_cast<uint32_t>(testCol));
    }
}
