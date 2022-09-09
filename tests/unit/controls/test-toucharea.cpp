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
#include <graphics/controls/Control.h>
#include <utils/Random.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

static void Randomize()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    Random::Seed(ms.count());
}

class TestControl : public Control
{
public:
    TestControl() : Control(rsp::utils::MakeTypeInfo<TestControl>()) {}
};

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("TouchArea Constructor")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);
    Randomize();

    SUBCASE("Construct with Default Values")
    {
        // Act
        TestControl area;

        // Assert
        CHECK(area.GetArea().GetHeight() == 0);
        CHECK(area.GetArea().GetWidth() == 0);
    }

    SUBCASE("Construct From Rect")
    {
        // Arrange
        GuiUnit_t randomLeft = Random::Roll(1, 100);
        GuiUnit_t randomTop = Random::Roll(1, 500);
        GuiUnit_t randomWidth = Random::Roll(1, 300);
        GuiUnit_t randomHeight = Random::Roll(1, 300);
        Rect myRect(randomLeft, randomTop, randomWidth, randomHeight);
        MESSAGE("Rect Dimensions: " << myRect);
        // Random point within rect
        Point insidePoint(myRect.GetLeft() + Random::Roll(0, myRect.GetWidth()),
                          myRect.GetTop() + Random::Roll(0, myRect.GetHeight()));
        // Random point lower than inside
        Point lowerPoint(Random::Roll(0, myRect.GetLeft()),
                         Random::Roll(0, myRect.GetTop()));
        // Random point higher than inside
        Point higherPoint(myRect.GetRight() + Random::Roll(0, myRect.GetWidth()),
                          myRect.GetBottom() + Random::Roll(0, myRect.GetHeight()));

        // Act
        TestControl area;
        area.SetArea(myRect).SetTouchArea(myRect);

        // Assert
        CHECK(area.IsHit(insidePoint));
        CHECK_FALSE(area.IsHit(lowerPoint));
        CHECK_FALSE(area.IsHit(higherPoint));
    }
}

TEST_CASE("Input Processing")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);
    Randomize();

    // Arrange
    int hit_count = 0;
    bool pressed = false;
    bool moved = false;
    bool lifted = false;
    bool clicked = false;
    Rect aRect(10, 10, 200, 200);
    TestControl area;
    area.SetArea(aRect).SetTouchArea(aRect).SetDraggable(true);
    TouchEvent event;

    event.mCurrent = Point(aRect.GetLeft() + Random::Roll(0, aRect.GetWidth() - 1),
                         aRect.GetTop() + Random::Roll(0, aRect.GetHeight() - 1));

    MESSAGE("Touch Point: " << event.mCurrent);
    CHECK(aRect.IsHit(event.mCurrent));

    area.OnPress() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        pressed = true;
    };
    area.OnMove() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        moved = true;
    };
    area.OnLift() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        lifted = true;
    };
    area.OnClick() = [&](const Point &arPoint, int aId) noexcept {
        hit_count++;
        clicked = true;
    };

    SUBCASE("Pressed Callback - Press Hit-Input")
    {
        // Arrange
        event.mType = TouchEvent::Types::Press;
        event.mPress = event.mCurrent;

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
            event.mCurrent += aRect.GetBottomRight();
            event.mPress = event.mCurrent;
            pressed = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mCurrent));
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
            event.mCurrent += aRect.GetBottomRight();
            lifted = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mCurrent));
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
            event.mCurrent += aRect.GetBottomRight();
            moved = false;

            // Act
            area.ProcessInput(event);

            // Assert
            CHECK_FALSE(aRect.IsHit(event.mCurrent));
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
            event.mPress = event.mCurrent;
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
            event.mCurrent += aRect.GetBottomRight();
            event.mPress = event.mCurrent;
            area.ProcessInput(event);
            // Drag into rect area
            event.mType = TouchEvent::Types::Drag;
            event.mCurrent -= aRect.GetBottomRight();
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
            event.mPress = event.mCurrent;
            area.ProcessInput(event);
            // Drag out of rect area
            event.mType = TouchEvent::Types::Drag;
            event.mCurrent += aRect.GetBottomRight();
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
            event.mCurrent += aRect.GetBottomRight();
            event.mPress = event.mCurrent;
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

TEST_SUITE_END();
