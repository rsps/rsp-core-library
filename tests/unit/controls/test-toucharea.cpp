/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <chrono>
#include <doctest.h>
#include <graphics/controls/TouchControl.h>

using namespace rsp::graphics;

static void Randomize()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
}


TEST_CASE("TouchArea Constructor")
{
    Randomize();

    SUBCASE("Construct with Default Values")
    {
        // Act
        TouchControl area;

        // Assert
        CHECK(area.GetArea().GetHeight() == 0);
        CHECK(area.GetArea().GetWidth() == 0);
    }

    SUBCASE("Construct From Rect")
    {
        // Arrange
        int randomLeft = 1 + (rand() % 100);
        int randomTop = 1 + (rand() % 500);
        int randomWidth = 1 + (rand() % 300);
        int randomHeight = 1 + (rand() % 300);
        Rect myRect(randomLeft, randomTop, randomWidth, randomHeight);
        MESSAGE("Rect Dimensions: " << myRect);
        // Random point within rect
        // output = min + (rand() % static_cast<int>(max - min + 1))
        Point insidePoint(myRect.GetLeft() + (rand() % static_cast<int>(myRect.GetWidth())),
                          myRect.GetTop() + (rand() % static_cast<int>(myRect.GetHeight())));
        // Random point lower than inside
        Point lowerPoint(0 + (rand() % myRect.GetLeft()),
                         0 + (rand() % myRect.GetTop()));
        // Random point higher than inside
        Point higherPoint(myRect.GetRight() + (rand() % static_cast<int>(myRect.GetWidth())),
                          myRect.GetBottom() + (rand() % static_cast<int>(myRect.GetHeight())));

        // Act
        TouchControl area;
        area.SetArea(myRect);

        // Assert
        CHECK(area.IsHit(insidePoint));
        CHECK_FALSE(area.IsHit(lowerPoint));
        CHECK_FALSE(area.IsHit(higherPoint));
    }
}

TEST_CASE("Input Processing")
{
    Randomize();

    // Arrange
    int hit_count = 0;
    bool pressed = false;
    bool moved = false;
    bool lifted = false;
    bool clicked = false;
    Rect aRect(10, 10, 200, 200);
    TouchControl area;
    area.SetArea(aRect);
    TouchEvent event;

    event.mPoint = Point(aRect.GetLeft() + (rand() % static_cast<int>(aRect.GetWidth() - 1)),
                         aRect.GetTop() + (rand() % static_cast<int>(aRect.GetHeight() - 1)));

    MESSAGE("Touch Point: " << event.mPoint);
    CHECK(aRect.IsHit(event.mPoint));

    area.GetOnPress() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        pressed = true;
    };
    area.GetOnMove() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        moved = true;
    };
    area.GetOnLift() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        lifted = true;
    };
    area.GetOnClick() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        clicked = true;
    };

    SUBCASE("Pressed Callback - Press Hit-Input")
    {
        // Arrange
        event.mType = TouchEvent::Types::Press;

        CHECK_FALSE(pressed);

        // Act
        area.ProcessInput(event);

        // Assert
        CHECK(hit_count == 1);
        CHECK(pressed);

        SUBCASE("Pressed Callback - Press Miss-Input")
        {
            // Arrange
            event.mType = TouchEvent::Types::Press;
            event.mPoint += aRect.GetBottomRight();
            pressed = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mPoint));
            CHECK(hit_count == 1);
            CHECK_FALSE(pressed);
        }

        SUBCASE("Pressed Callback - Lift Hit-Input")
        {
            // Arrange
            event.mType = TouchEvent::Types::Lift;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK(hit_count == 3);
            CHECK(lifted);
            CHECK(clicked);
        }

        SUBCASE("Pressed Callback - Lift Miss-Input")
        {
            // Arrange
            event.mType = TouchEvent::Types::Lift;
            event.mPoint += aRect.GetBottomRight();
            lifted = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mPoint));
            CHECK(hit_count == 2);
            CHECK(lifted);
            CHECK_FALSE(clicked);
        }

        SUBCASE("Pressed Callback - Drag Hit-input")
        {
            // Arrange
            event.mType = TouchEvent::Types::Drag;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK(hit_count == 2);
            CHECK(moved);
        }

        SUBCASE("Pressed Callback - Drag Miss-input")
        {
            // Arrange
            event.mType = TouchEvent::Types::Drag;
            event.mPoint += aRect.GetBottomRight();
            moved = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mPoint));
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
            event.mType = TouchEvent::Types::Press;
            area.ProcessInput(event);
            MESSAGE("Processed Press");
            event.mType = TouchEvent::Types::Lift;
            area.ProcessInput(event);
            MESSAGE("Processed Lift");

            // Assert
            CHECK(hit_count == 3);
            CHECK(pressed);
            CHECK_FALSE(moved);
            CHECK(lifted);
            CHECK(clicked);
        }

        SUBCASE("Clicked Callback - Miss-Hit-Input")
        {
            // Act
            // Miss with press
            event.mType = TouchEvent::Types::Press;
            event.mPoint += aRect.GetBottomRight();
            area.ProcessInput(event);
            // Drag into rect area
            event.mType = TouchEvent::Types::Drag;
            event.mPoint -= aRect.GetBottomRight();
            area.ProcessInput(event);
            // Do a lift
            event.mType = TouchEvent::Types::Lift;
            area.ProcessInput(event);

            // Assert
            CHECK(hit_count == 0);
            CHECK_FALSE(pressed);
            CHECK_FALSE(moved);
            CHECK_FALSE(lifted);
            CHECK_FALSE(clicked);
        }

        SUBCASE("Clicked Callback - Hit-Miss-Input")
        {
            // Act
            // Hit with press
            event.mType = TouchEvent::Types::Press;
            area.ProcessInput(event);
            // Drag out of rect area
            event.mType = TouchEvent::Types::Drag;
            event.mPoint += aRect.GetBottomRight();
            area.ProcessInput(event);
            // Do a lift
            event.mType = TouchEvent::Types::Lift;
            area.ProcessInput(event);

            // Assert
            CHECK(hit_count == 3);
            CHECK(pressed);
            CHECK(moved);
            CHECK(lifted);
            CHECK_FALSE(clicked);
        }

        SUBCASE("Clicked Callback - Miss-Miss-Input")
        {
            // Act
            // Miss with press
            event.mType = TouchEvent::Types::Press;
            event.mPoint += aRect.GetBottomRight();
            area.ProcessInput(event);
            // Do a lift
            event.mType = TouchEvent::Types::Lift;
            area.ProcessInput(event);

            // Assert
            CHECK(hit_count == 0);
            CHECK_FALSE(pressed);
            CHECK_FALSE(moved);
            CHECK_FALSE(lifted);
            CHECK_FALSE(clicked);
        }
    }
}
