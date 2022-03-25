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
#include <graphics/primitives/Point.h>
#include <graphics/primitives/Rect.h>

#include <iostream>

using namespace rsp::utils;
using namespace rsp::graphics;

constexpr int cLeft = 100;
constexpr int cTop = 50;
constexpr int cWidth = 400;
constexpr int cHeight = 600;

inline void checkRect(Rect &aRect)
{
    CHECK(aRect.GetTop() == cTop);
    CHECK(aRect.GetBottom() == cTop + cHeight);
    CHECK(aRect.GetLeft() == cLeft);
    CHECK(aRect.GetRight() == cLeft + cWidth);
    CHECK(aRect.GetWidth() == cWidth);
    CHECK(aRect.GetHeight() == cHeight);
}

TEST_CASE("Rect Contructors")
{
    SUBCASE("Simple value Rect")
    {
        // Arrange & Act
        Rect rect_base_values(cLeft, cTop, cWidth, cHeight);

        // Assert
        checkRect(rect_base_values);
    }
    SUBCASE("Points Rect")
    {
        // Arrange & Act
        Rect rect_points(Point(cLeft, cTop), Point(cLeft + cWidth, cTop + cHeight));

        // Assert
        checkRect(rect_points);
    }
    SUBCASE("Mixed Rect")
    {
        // Arrange & Act
        Rect rect_mix(Point(cLeft, cTop), cWidth, cHeight);

        // Assert
        checkRect(rect_mix);
    }
}

TEST_CASE("Rect Swapping Edges")
{
    // Arrange
    Rect rect(cLeft, cTop, cWidth, cHeight);
    SUBCASE("Set Bottom above Top")
    {
        // Act
        CHECK_THROWS_AS(rect.SetBottom(cTop - 1), const AssertException &);

        // Assert
        CHECK(rect.GetTop() == cTop);
        CHECK(rect.GetBottom() == (cTop + cHeight));
    }
    SUBCASE("Set Top below Bottom")
    {
        // Act
        CHECK_THROWS_AS(rect.SetTop(cTop + cHeight + 1), const AssertException &);

        // Assert
        CHECK(rect.GetTop() == cTop);
        CHECK(rect.GetBottom() == (cTop + cHeight));
    }
    SUBCASE("Set Right cLeft of Left")
    {
        // Act
        CHECK_THROWS_AS(rect.SetRight(cLeft - 1), const AssertException &);

        // Assert
        CHECK(rect.GetRight() == (cLeft + cWidth));
        CHECK(rect.GetLeft() == cLeft);
    }
    SUBCASE("Set Left right of Right")
    {
        // Act
        CHECK_THROWS_AS(rect.SetLeft(cLeft + cWidth + 1), const AssertException &);

        // Assert
        CHECK(rect.GetRight() == (cLeft + cWidth));
        CHECK(rect.GetLeft() == cLeft);
    }
}

TEST_CASE("Rect Height and Width")
{
    Rect rect(cLeft, cTop, cWidth, cHeight);
    SUBCASE("Set Height set correctly")
    {
        int oldBottom = rect.GetBottom();

        // Act
        rect.SetHeight(cHeight + 1);

        // Assert
        CHECK(rect.GetHeight() == (cHeight + 1));
        CHECK(rect.GetBottom() == (oldBottom + 1));
    }
    SUBCASE("Set Width set correctly")
    {
        // Act
        int oldRight = rect.GetRight();
        rect.SetWidth(cWidth + 1);

        // Assert
        CHECK(rect.GetWidth() == (cWidth + 1));
        CHECK(rect.GetRight() == (oldRight + 1));
    }
    SUBCASE("Set Negative Height")
    {
        // Not checking if the end result is negative (meaning outside the screen)

        // Act & Assert
        CHECK_THROWS_AS(rect.SetHeight(-50), const AssertException &);
        CHECK(rect.GetHeight() != -50);
    }
    SUBCASE("Set Negative Width")
    {
        // Not checking if the end result is negative (meaning outside the screen)

        // Act & Assert
        CHECK_THROWS_AS(rect.SetWidth(-50), const AssertException &);
        CHECK(rect.GetWidth() != -50);
    }
}
