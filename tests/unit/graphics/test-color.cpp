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

constexpr uint8_t cAlpha = 0x12;
constexpr uint8_t cRed = 0x34;
constexpr uint8_t cGreen = 0x56;
constexpr uint8_t cBlue = 0x78;
constexpr uint32_t cColorVal = 0x12345678;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Color Contructors")
{
    SUBCASE("Separate values contructor")
    {
        // Arrange & Act
        Color col(cRed, cGreen, cBlue, cAlpha);

        // Assert
        CHECK(col.GetAlpha() == cAlpha);
        CHECK(col.GetRed() == cRed);
        CHECK(col.GetGreen() == cGreen);
        CHECK(col.GetBlue() == cBlue);
    }
    SUBCASE("Single value contructor")
    {
        // Arrange & Act
        Color col(cColorVal);

        // Assert
        CHECK(col.GetAlpha() == cAlpha);
        CHECK(col.GetRed() == cRed);
        CHECK(col.GetGreen() == cGreen);
        CHECK(col.GetBlue() == cBlue);
    }
    SUBCASE("Copy contructor")
    {
        // Arrange & Act
        Color testCol(cColorVal);
        Color col(testCol);

        // Assert
        CHECK(col.GetAlpha() == cAlpha);
        CHECK(col.GetRed() == cRed);
        CHECK(col.GetGreen() == cGreen);
        CHECK(col.GetBlue() == cBlue);
    }
}

TEST_CASE("Color Sets")
{
    // Arrange
    Color testCol(cColorVal);
    uint8_t newCol = 0x55;
    SUBCASE("Set Alpha")
    {
        // Act
        testCol.SetAlpha(newCol);

        // Assert only wanted part has changed
        CHECK(testCol.GetAlpha() == newCol);
        CHECK(testCol.GetRed() == cRed);
        CHECK(testCol.GetGreen() == cGreen);
        CHECK(testCol.GetBlue() == cBlue);
    }
    SUBCASE("Set Red")
    {
        // Act
        testCol.SetRed(newCol);

        // Assert only wanted part has changed
        CHECK(testCol.GetRed() == newCol);
        CHECK(testCol.GetGreen() == cGreen);
        CHECK(testCol.GetBlue() == cBlue);
        CHECK(testCol.GetAlpha() == cAlpha);
    }
    SUBCASE("Set Green")
    {
        // Act
        testCol.SetGreen(newCol);

        // Assert only wanted part has changed
        CHECK(testCol.GetGreen() == newCol);
        CHECK(testCol.GetRed() == cRed);
        CHECK(testCol.GetBlue() == cBlue);
        CHECK(testCol.GetAlpha() == cAlpha);
    }
    SUBCASE("Set Blue")
    {
        // Act
        testCol.SetBlue(newCol);

        // Assert only wanted part has changed
        CHECK(testCol.GetBlue() == newCol);
        CHECK(testCol.GetRed() == cRed);
        CHECK(testCol.GetGreen() == cGreen);
        CHECK(testCol.GetAlpha() == cAlpha);
    }
}

TEST_CASE("Color Operators")
{
    // Arrange
    Color testCol(cColorVal);
    SUBCASE("uint32 Conversion")
    {
        // Act
        uint32_t newUint32 = testCol;

        // Assert
        CHECK(newUint32 == cColorVal);
    }
    SUBCASE("= Operator")
    {
        // Act
        Color newColour = testCol;

        // Assert
        CHECK(static_cast<uint32_t>(newColour) == static_cast<uint32_t>(testCol));
    }
    SUBCASE("Blend")
    {
        Color c1(Color::Red);
        Color c2(Color::Blue);
        CHECK_EQ(Color::Blend(c1, c2), Color(Color::Blue));

        c2.SetAlpha(127);
        CHECK_EQ(Color::Blend(c1, c2), Color(0xFF7F007F));

        c2.SetAlpha(0);
        CHECK_EQ(Color::Blend(c1, c2), Color(Color::Red));
    }
}

TEST_CASE("Color Constants")
{
    SUBCASE("Red") {
        Color cl(Color::Red);
        CHECK(cl.GetAlpha() == 0xFF);
        CHECK(cl.GetRed() == 0xFF);
        CHECK(cl.GetGreen() == 0);
        CHECK(cl.GetBlue() == 0);
    }

    SUBCASE("Green") {
        Color cl(Color::Green);
        CHECK(cl.GetAlpha() == 0xFF);
        CHECK(cl.GetRed() == 0);
        CHECK(cl.GetGreen() == 0x80);
        CHECK(cl.GetBlue() == 0);
    }

    SUBCASE("Blue") {
        Color cl(Color::Blue);
        CHECK(cl.GetAlpha() == 0xFF);
        CHECK(cl.GetRed() == 0);
        CHECK(cl.GetGreen() == 0);
        CHECK(cl.GetBlue() == 0xFF);
    }
}

TEST_SUITE_END();
