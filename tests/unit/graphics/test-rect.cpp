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

int x = 100;
int y = 50;
int width = 400;
int height = 600;
void inline checkRect(Rect& aRect) {
    CHECK(aRect.GetTop() == y);
    CHECK(aRect.GetBottom() == y + height);
    CHECK(aRect.GetLeft() == x);
    CHECK(aRect.GetRight() == x + width);
    CHECK(aRect.GetWidth() == width);
    CHECK(aRect.GetHeight() == height);
}

TEST_CASE("Rect Contructors") {
    SUBCASE("Simple value Rect") {
        Rect rect_base_values(x, y, width, height);
        checkRect(rect_base_values);
    }
    SUBCASE("Points Rect") {
        Rect rect_points(Point(x, y), Point(x + width, y + height));
        checkRect(rect_points);
    }
    SUBCASE("Mixed Rect") {
        Rect rect_mix(Point(x, y), width, height);
        checkRect(rect_mix);
    }
}
TEST_CASE("Rect Swapping Edges") {
    SUBCASE("Set Bottom above Top") {
        Rect rect(x, y, width, height);
        int oldTop = rect.GetTop();
        int oldBottom = rect.GetBottom();

        rect.SetBottom(oldTop - 1);
        CHECK(rect.GetTop() == (oldTop - 1));
        CHECK(rect.GetBottom() == oldTop);
    }
    SUBCASE("Set Top below Bottom") {
        Rect rect(x, y, width, height);
        int oldTop = rect.GetTop();
        int oldBottom = rect.GetBottom();

        rect.SetTop(oldBottom + 1);
        CHECK(rect.GetTop() == oldBottom);
        CHECK(rect.GetBottom() == (oldBottom + 1));
    }
    SUBCASE("Set Right left of Left") {
        Rect rect(x, y, width, height);
        int oldRight = rect.GetRight();
        int oldLeft = rect.GetLeft();

        rect.SetRight(oldLeft - 1);
        CHECK(rect.GetRight() == oldLeft);
        CHECK(rect.GetLeft() == (oldLeft - 1));
    }
    SUBCASE("Set Left right of Right") {
        Rect rect(x, y, width, height);
        int oldRight = rect.GetRight();
        int oldLeft = rect.GetLeft();

        rect.SetLeft(oldRight + 1);
        CHECK(rect.GetLeft() == oldRight);
        CHECK(rect.GetRight() == (oldRight + 1));
    }
}
TEST_CASE("Rect Height and Width") {
    SUBCASE("Update Height set correctly") {
        Rect rect(x, y, width, height);
        int oldBottom = rect.GetBottom();
        rect.SetHeight(height + 1);

        CHECK(rect.GetHeight() == (height + 1));
        CHECK(rect.GetBottom() == (oldBottom + 1));
    }
    SUBCASE("Update Width set correctly") {
        Rect rect(x, y, width, height);
        int oldRight = rect.GetRight();
        rect.SetWidth(width + 1);

        CHECK(rect.GetWidth() == (width + 1));
        CHECK(rect.GetRight() == (oldRight + 1));
    }
    SUBCASE("Negative Height") {
        //Not checking if the end result is negative (meaning outside the screen)
        Rect rect(x, y, width, height);
        int oldHeight = rect.GetHeight();
        rect.SetHeight(-50);

        CHECK(rect.GetHeight() == 50);
        CHECK(rect.GetTop() < rect.GetBottom());
    }
    SUBCASE("Negative Width") {
        //Not checking if the end result is negative (meaning outside the screen)
        Rect rect(x, y, width, height);
        int oldWidth = rect.GetWidth();
        rect.SetWidth(-50);

        CHECK(rect.GetWidth() == 50);
        CHECK(rect.GetLeft() < rect.GetRight());
    }
}