/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <Point.h>
#include <Rect.h>
#include <doctest.h>

#include <iostream>

int left = 100;
int top = 50;
int width = 400;
int height = 600;
void inline checkRect(Rect& aRect) {
    CHECK(aRect.GetTop() == top);
    CHECK(aRect.GetBottom() == top + height);
    CHECK(aRect.GetLeft() == left);
    CHECK(aRect.GetRight() == left + width);
    CHECK(aRect.GetWidth() == width);
    CHECK(aRect.GetHeight() == height);
}

TEST_CASE("Rect Contructors") {
    SUBCASE("Simple value Rect") {
        Rect rect_base_values(left, top, width, height);
        checkRect(rect_base_values);
    }
    SUBCASE("Points Rect") {
        Rect rect_points(Point(left, top), Point(left + width, top + height));
        checkRect(rect_points);
    }
    SUBCASE("Mixed Rect") {
        Rect rect_mix(Point(left, top), width, height);
        checkRect(rect_mix);
    }
}
TEST_CASE("Rect Swapping Edges") {
    //Arrange
    Rect rect(left, top, width, height);
    SUBCASE("Set Bottom above Top") {
        //Act
        CHECK_THROWS_AS(rect.SetBottom(top - 1), const AssertException&);

        CHECK(rect.GetTop() == top);
        CHECK(rect.GetBottom() == (top + height));
    }
    SUBCASE("Set Top below Bottom") {
        //Act
        CHECK_THROWS_AS(rect.SetTop(top + height + 1), const AssertException&);

        //Assert
        CHECK(rect.GetTop() == top);
        CHECK(rect.GetBottom() == (top + height));
    }
    SUBCASE("Set Right left of Left") {
        //Act
        CHECK_THROWS_AS(rect.SetRight(left - 1), const AssertException&);

        //Assert
        CHECK(rect.GetRight() == (left + width));
        CHECK(rect.GetLeft() == left);
    }
    SUBCASE("Set Left right of Right") {
        //Act
        CHECK_THROWS_AS(rect.SetLeft(left + width + 1), const AssertException&);

        //Assert
        CHECK(rect.GetRight() == (left + width));
        CHECK(rect.GetLeft() == left);
    }
}
//
TEST_CASE("Rect Height and Width") {
    Rect rect(left, top, width, height);
    SUBCASE("Set Height set correctly") {
        int oldBottom = rect.GetBottom();

        //Act
        rect.SetHeight(height + 1);

        CHECK(rect.GetHeight() == (height + 1));
        CHECK(rect.GetBottom() == (oldBottom + 1));
    }
    SUBCASE("Set Width set correctly") {
        int oldRight = rect.GetRight();

        //Act
        rect.SetWidth(width + 1);

        CHECK(rect.GetWidth() == (width + 1));
        CHECK(rect.GetRight() == (oldRight + 1));
    }
    SUBCASE("Set Negative Height") {
        //Not checking if the end result is negative (meaning outside the screen)
        bool exceptionThrown = false;

        //Act
        CHECK_THROWS_AS(rect.SetHeight(-50), const AssertException&);
        CHECK(rect.GetHeight() != -50);
    }
    SUBCASE("Set Negative Width") {
        //Not checking if the end result is negative (meaning outside the screen)
        bool exceptionThrown = false;

        //Act
        CHECK_THROWS_AS(rect.SetWidth(-50), const AssertException&);
        CHECK(rect.GetWidth() != -50);
    }
}