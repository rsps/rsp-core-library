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
#include <utils/CoreException.h>

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
    CHECK(aRect.GetBottom() == cTop + cHeight - 1);
    CHECK(aRect.GetLeft() == cLeft);
    CHECK(aRect.GetRight() == cLeft + cWidth - 1);
    CHECK(aRect.GetWidth() == cWidth);
    CHECK(aRect.GetHeight() == cHeight);
}

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Rect Contructors")
{
    SUBCASE("Simple value Rect")
    {
        Rect a(0, 0, 10, 10);
        CHECK_EQ(a.GetHeight(), 10);
        CHECK_EQ(a.GetWidth(), 10);
        CHECK_EQ(a.GetTop(), 0);
        CHECK_EQ(a.GetBottom(), 9);
        CHECK_EQ(a.GetLeft(), 0);
        CHECK_EQ(a.GetRight(), 9);
        CHECK(a.IsHit(Point(0,0)));
        CHECK_FALSE(a.IsHit(Point(10,10)));

        // Arrange & Act
        Rect rect_base_values(cLeft, cTop, cWidth, cHeight);

        // Assert
        checkRect(rect_base_values);
    }
    SUBCASE("Points Rect")
    {
        Rect a(Point(0, 0), Point(9, 9));
        CHECK_EQ(a.GetHeight(), 10);
        CHECK_EQ(a.GetWidth(), 10);
        CHECK_EQ(a.GetTop(), 0);
        CHECK_EQ(a.GetBottom(), 9);
        CHECK_EQ(a.GetLeft(), 0);
        CHECK_EQ(a.GetRight(), 9);
        CHECK(a.IsHit(Point(0,0)));
        CHECK_FALSE(a.IsHit(Point(10,10)));

        // Arrange & Act
        Rect rect_points(Point(cLeft, cTop), Point(cLeft + cWidth - 1, cTop + cHeight - 1));

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
//        CHECK_THROWS_AS(rect.SetBottom(cTop - 1), const AssertException &);
        CHECK_NOTHROW(rect.SetBottom(cTop - 1));

        // Assert
        CHECK(rect.GetTop() == cTop);
        CHECK(rect.GetBottom() == (cTop - 1));
        CHECK_EQ(rect.GetHeight(), 0);
    }
    SUBCASE("Set Top below Bottom")
    {
        // Act
//        CHECK_THROWS_AS(rect.SetTop(cTop + cHeight + 1), const AssertException &);
        CHECK_NOTHROW(rect.SetTop(cTop + cHeight + 1));

        // Assert
        CHECK_EQ(rect.GetTop(), cTop + cHeight + 1);
        CHECK_EQ(rect.GetBottom(), cTop + cHeight + cHeight);
        CHECK_EQ(rect.GetHeight(), cHeight);
    }
    SUBCASE("Set Right cLeft of Left")
    {
        // Act
//        CHECK_THROWS_AS(rect.SetRight(cLeft - 1), const AssertException &);
        CHECK_NOTHROW(rect.SetRight(cLeft - 1));

        // Assert
        CHECK(rect.GetRight() == (cLeft - 1));
        CHECK(rect.GetLeft() == cLeft);
        CHECK_EQ(rect.GetWidth(), 0);
    }
    SUBCASE("Set Left right of Right")
    {
        // Act
//        CHECK_THROWS_AS(rect.SetLeft(cLeft + cWidth + 1), const AssertException &);
        CHECK_NOTHROW(rect.SetLeft(cLeft + cWidth + 1));

        // Assert
        CHECK(rect.GetRight() == (cLeft + cWidth + cWidth));
        CHECK(rect.GetLeft() == cLeft + cWidth + 1);
        CHECK_EQ(rect.GetWidth(), cWidth);
    }
}

TEST_CASE("Rect Height and Width")
{
    Rect rect(cLeft, cTop, cWidth, cHeight);
    SUBCASE("Set Height set correctly")
    {
        GuiUnit_t oldBottom = rect.GetBottom();

        // Act
        rect.SetHeight(cHeight + 1);

        // Assert
        CHECK(rect.GetHeight() == (cHeight + 1));
        CHECK(rect.GetBottom() == (oldBottom + 1));
    }
    SUBCASE("Set Width set correctly")
    {
        // Act
        GuiUnit_t oldRight = rect.GetRight();
        rect.SetWidth(cWidth + 1);

        // Assert
        CHECK(rect.GetWidth() == (cWidth + 1));
        CHECK(rect.GetRight() == (oldRight + 1));
    }
}

TEST_SUITE_END();
