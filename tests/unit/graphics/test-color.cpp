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
#include <graphics/Color.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

constexpr uint8_t cAlpha = 0x12;
constexpr uint8_t cRed = 0x34;
constexpr uint8_t cGreen = 0x56;
constexpr uint8_t cBlue = 0x78;
constexpr uint32_t cColorVal = 0x12345678;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Color")
{
    SUBCASE("Color Constructors")
    {
        SUBCASE("Separate values constructor")
        {
            // Arrange & Act
            Color col(cRed, cGreen, cBlue, cAlpha);

            // Assert
            CHECK_EQ(col.GetAlpha(), cAlpha);
            CHECK_EQ(col.GetRed(), cRed);
            CHECK_EQ(col.GetGreen(), cGreen);
            CHECK_EQ(col.GetBlue(), cBlue);
            CHECK_HEX(col.AsUint(), cColorVal);
#if __BYTE_ORDER == __LITTLE_ENDIAN
            CHECK_HEX(col.AsRaw(), 0x12785634);
#else
            CHECK_HEX(col.AsRaw(), cColorVal);
#endif
        }
        SUBCASE("Single value constructor")
        {
            // Arrange & Act
            Color col(cColorVal);

            // Assert
            CHECK_EQ(col.GetAlpha(), cAlpha);
            CHECK_EQ(col.GetRed(), cRed);
            CHECK_EQ(col.GetGreen(), cGreen);
            CHECK_EQ(col.GetBlue(), cBlue);
            CHECK_HEX(col.AsUint(), cColorVal);
        }
        SUBCASE("Copy constructor")
        {
            // Arrange & Act
            Color testCol(cColorVal);
            Color col(testCol);

            // Assert
            CHECK_EQ(col.GetAlpha(), cAlpha);
            CHECK_EQ(col.GetRed(), cRed);
            CHECK_EQ(col.GetGreen(), cGreen);
            CHECK_EQ(col.GetBlue(), cBlue);
        }
    }

    SUBCASE("Color Sets")
    {
        // Arrange
        Color testCol(cColorVal);
        uint8_t newCol = 0x55;
        SUBCASE("Set Alpha")
        {
            // Act
            testCol.SetAlpha(newCol);

            // Assert only wanted part has changed
            CHECK_EQ(testCol.GetAlpha(), newCol);
            CHECK_EQ(testCol.GetRed(), cRed);
            CHECK_EQ(testCol.GetGreen(), cGreen);
            CHECK_EQ(testCol.GetBlue(), cBlue);
        }
        SUBCASE("Set Red")
        {
            // Act
            testCol.SetRed(newCol);

            // Assert only wanted part has changed
            CHECK_EQ(testCol.GetRed(), newCol);
            CHECK_EQ(testCol.GetGreen(), cGreen);
            CHECK_EQ(testCol.GetBlue(), cBlue);
            CHECK_EQ(testCol.GetAlpha(), cAlpha);
        }
        SUBCASE("Set Green")
        {
            // Act
            testCol.SetGreen(newCol);

            // Assert only wanted part has changed
            CHECK_EQ(testCol.GetGreen(), newCol);
            CHECK_EQ(testCol.GetRed(), cRed);
            CHECK_EQ(testCol.GetBlue(), cBlue);
            CHECK_EQ(testCol.GetAlpha(), cAlpha);
        }
        SUBCASE("Set Blue")
        {
            // Act
            testCol.SetBlue(newCol);

            // Assert only wanted part has changed
            CHECK_EQ(testCol.GetBlue(), newCol);
            CHECK_EQ(testCol.GetRed(), cRed);
            CHECK_EQ(testCol.GetGreen(), cGreen);
            CHECK_EQ(testCol.GetAlpha(), cAlpha);
        }
    }

    SUBCASE("Color Operators")
    {
        // Arrange
        Color testCol(cColorVal);
        SUBCASE("uint32 Conversion")
        {
            // Act
            uint32_t newUint32 = testCol;

            // Assert
            CHECK_EQ(newUint32, cColorVal);
        }
        SUBCASE("= Operator")
        {
            // Act
            Color newColour = testCol;

            // Assert
            CHECK_EQ(static_cast<uint32_t>(newColour), static_cast<uint32_t>(testCol));
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

    SUBCASE("Color Constants")
    {
        SUBCASE("Red") {
            Color cl(Color::Red);
            CHECK_EQ(cl.GetAlpha(), 0xFF);
            CHECK_EQ(cl.GetRed(), 0xFF);
            CHECK_EQ(cl.GetGreen(), 0);
            CHECK_EQ(cl.GetBlue(), 0);
        }

        SUBCASE("Green") {
            Color cl(Color::Green);
            CHECK_EQ(cl.GetAlpha(), 0xFF);
            CHECK_EQ(cl.GetRed(), 0);
            CHECK_EQ(cl.GetGreen(), 0x80);
            CHECK_EQ(cl.GetBlue(), 0);
        }

        SUBCASE("Blue") {
            Color cl(Color::Blue);
            CHECK_EQ(cl.GetAlpha(), 0xFF);
            CHECK_EQ(cl.GetRed(), 0);
            CHECK_EQ(cl.GetGreen(), 0);
            CHECK_EQ(cl.GetBlue(), 0xFF);
        }
    }

    SUBCASE("Raw Values")
    {
        SUBCASE("Red") {
            Color cl(Color::Red);
            CHECK_HEX(cl.AsRaw(), Color(Color::Red).AsRaw());
        }

        SUBCASE("Green") {
            Color cl(Color::Green);
            CHECK_HEX(cl.AsRaw(), Color(Color::Green).AsRaw());
        }

        SUBCASE("Blue") {
            Color cl(Color::Blue);
            CHECK_HEX(cl.AsRaw(), Color(Color::Blue).AsRaw());
        }
    }
}

TEST_SUITE_END();
