/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/TouchArea.h"
#include <chrono>
#include <doctest.h>

using namespace rsp::graphics;
using namespace rsp::messaging;

TEST_CASE("TouchArea Constructor")
{
    // Arrange
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    SUBCASE("Construct with Default Values")
    {
        // Act
        TouchArea area;

        // Assert
        CHECK(area.GetArea().GetHeight() == 0);
        CHECK(area.GetArea().GetWidth() == 0);
    }

    SUBCASE("Construct From Rect")
    {
        // Arrange
        int randomLeft = 1 + (rand() % 500);
        int randomTop = 1 + (rand() % 500);
        int randomWidth = 1 + (rand() % 500);
        int randomHeight = 1 + (rand() % 500);
        Rect myRect(randomLeft, randomTop, randomWidth, randomHeight);
        MESSAGE("Rect Dimensions: "
                << "\nTop Left: " << myRect.GetTop() << " " << myRect.GetLeft()
                << "\nBottom Right: " << myRect.GetBottom() << " " << myRect.GetRight());
        // Random point within rect
        // output = min + (rand() % static_cast<int>(max - min + 1))
        Point insidePoint(myRect.GetLeft() + (rand() % myRect.GetWidth()),
                          myRect.GetTop() + (rand() % myRect.GetHeight()));
        // Random point lower than inside
        Point lowerPoint(0 + (rand() % myRect.GetLeft()),
                         0 + (rand() % myRect.GetTop()));
        // Random point higher than inside
        Point higherPoint(myRect.GetRight() + (rand() % myRect.GetWidth()),
                          myRect.GetBottom() + (rand() % myRect.GetHeight()));

        // Act
        TouchArea area;
        area.SetArea(myRect);

        // Assert
        CHECK(area.IsHit(insidePoint));
        CHECK_FALSE(area.IsHit(lowerPoint));
        CHECK_FALSE(area.IsHit(higherPoint));
    }
}

TEST_CASE("Input Processing")
{
    // Arrange
    int hit_count = 0;
    bool pressed = false;
    bool moved = false;
    bool lifted = false;
    bool clicked = false;
    Rect aRect(0, 0, 200, 200);
    TouchArea area;
    area.SetArea(aRect);
    Input anInput;
    // output = min + (rand() % static_cast<int>(max - min + 1))
    anInput.x = 1 + (rand() % static_cast<int>(aRect.GetWidth() - 1 + 1));
    anInput.y = 1 + (rand() % static_cast<int>(aRect.GetHeight() - 1 + 1));

    CHECK_LT(anInput.x, aRect.GetWidth());
    CHECK_LT(anInput.y, aRect.GetHeight());
    CHECK_GT(anInput.x, 0);
    CHECK_GT(anInput.y, 0);

    area.GetOnPress() = [&](const Point &arPoint) noexcept {
        if (aRect.IsHit(arPoint)) {
            hit_count++;
        }
        pressed = true;
    };
    area.GetOnMove() = [&](const Point &arPoint) noexcept {
        if (aRect.IsHit(arPoint)) {
            hit_count++;
        }
        moved = true;
    };
    area.GetOnLift() = [&](const Point &arPoint) noexcept {
        if (aRect.IsHit(arPoint)) {
            hit_count++;
        }
        lifted = true;
    };
    area.GetOnClick() = [&](const Point &arPoint) noexcept {
        if (aRect.IsHit(arPoint)) {
            hit_count++;
        }
        clicked = true;
    };

    SUBCASE("Pressed Callback - Press Hit-Input")
    {
        // Arrange
        anInput.type = InputType::Press;

        CHECK_FALSE(pressed);

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK(hit_count == 1);
        CHECK(pressed);

        SUBCASE("Pressed Callback - Press Miss-Input")
        {
            // Arrange
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            pressed = false;

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
            CHECK(hit_count == 1);
            CHECK_FALSE(pressed);
        }
    }

    SUBCASE("Pressed Callback - Lift Hit-Input")
    {
        // Arrange
        anInput.type = InputType::Lift;

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK(hit_count == 2);
        CHECK(lifted);

        SUBCASE("Pressed Callback - Lift Miss-Input")
        {
            // Arrange
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            lifted = false;

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
            CHECK(hit_count == 2);
            CHECK_FALSE(lifted);
        }
    }

    SUBCASE("Pressed Callback - Drag Hit-input")
    {
        // Arrange
        anInput.type = InputType::Drag;

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK(hit_count == 2);
        CHECK(moved);

        SUBCASE("Pressed Callback - Drag Miss-input")
        {
            // Arrange
            anInput.type = InputType::Drag;
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            moved = false;

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
            CHECK(hit_count == 2);
            CHECK(moved);
        }
    }

    SUBCASE("Clicked Callback")
    {
        SUBCASE("Clicked Callback - Hit-Hit-Input")
        {
            // Act
            MESSAGE("Start");
            anInput.type = InputType::Press;
            area.ProcessInput(anInput);
            MESSAGE("Processed Press");
            anInput.type = InputType::Lift;
            area.ProcessInput(anInput);
            MESSAGE("Processed Lift");

            // Assert
            CHECK(hit_count == 2);
            CHECK(pressed);
            CHECK_FALSE(moved);
            CHECK(lifted);
            CHECK(clicked);
        }

        SUBCASE("Clicked Callback - Miss-Hit-Input")
        {
            // Act
            // Miss with press
            anInput.type = InputType::Press;
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            area.ProcessInput(anInput);
            // Drag into rect area
            anInput.type = InputType::Drag;
            anInput.x = anInput.x - aRect.GetWidth();
            anInput.y = anInput.y - aRect.GetHeight();
            area.ProcessInput(anInput);
            // Do a lift
            anInput.type = InputType::Lift;
            area.ProcessInput(anInput);

            // Assert
            CHECK(hit_count == 2);
            CHECK_FALSE(pressed);
            CHECK(moved);
            CHECK(lifted);
            CHECK_FALSE(clicked);
        }

        SUBCASE("Clicked Callback - Hit-Miss-Input")
        {
            // Act
            // Hit with press
            anInput.type = InputType::Press;
            area.ProcessInput(anInput);
            // Drag out of rect area
            anInput.type = InputType::Drag;
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            area.ProcessInput(anInput);
            // Do a lift
            anInput.type = InputType::Lift;
            area.ProcessInput(anInput);

            // Assert
            CHECK(hit_count == 1);
            CHECK(pressed);
            CHECK_FALSE(moved);
            CHECK_FALSE(lifted);
            CHECK_FALSE(clicked);
        }

        SUBCASE("Clicked Callback - Miss-Miss-Input")
        {
            // Act
            // Miss with press
            anInput.type = InputType::Press;
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();
            area.ProcessInput(anInput);
            // Do a lift
            anInput.type = InputType::Lift;
            area.ProcessInput(anInput);

            // Assert
            CHECK(hit_count == 0);
            CHECK_FALSE(pressed);
            CHECK_FALSE(moved);
            CHECK_FALSE(lifted);
            CHECK_FALSE(clicked);
        }
    }
}
