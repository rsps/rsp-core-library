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

TEST_CASE("TouchArea Contructor")
{
    // Arrange
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    SUBCASE("Constuct with Default Values")
    {
        // Act
        TouchArea area;

        // Assert
        CHECK(area.mTouchArea.GetHeight() == 0);
        CHECK(area.mTouchArea.GetWidth() == 0);
    }
    SUBCASE("Construct From Rect")
    {
        // Arrange
        int randomLeft = 0 + (rand() % 500 + 1);
        int randomTop = 0 + (rand() % 500 + 1);
        int randomWidth = 0 + (rand() % 500 + 1);
        int randomHeight = 0 + (rand() % 500 + 1);
        Rect myRect(randomLeft, randomTop, randomWidth, randomHeight);
        MESSAGE("Rect Dimensions: "
                << "\nTop Left: " << myRect.GetTop() << " " << myRect.GetLeft()
                << "\nBottom Right: " << myRect.GetBottom() << " " << myRect.GetRight());
        // Random point within rect
        // output = min + (rand() % static_cast<int>(max - min + 1))
        Point insidePoint(myRect.GetLeft() + (rand() % static_cast<int>((myRect.GetLeft() + myRect.GetWidth()) - myRect.GetLeft() + 1)),
                          myRect.GetTop() + (rand() % static_cast<int>((myRect.GetTop() + myRect.GetHeight()) - myRect.GetTop() + 1)));
        // Random point lower than inside
        Point lowerPoint(0 + (rand() % static_cast<int>(myRect.GetLeft() + 1)),
                         0 + (rand() % static_cast<int>(myRect.GetTop() + 1)));
        // Random point higher than inside
        Point higherPoint(myRect.GetRight() + (rand() % static_cast<int>((myRect.GetRight() + myRect.GetWidth()) - myRect.GetRight() + 1)),
                          myRect.GetBottom() + (rand() % static_cast<int>((myRect.GetBottom() + myRect.GetHeight()) - myRect.GetBottom() + 1)));

        // Act
        TouchArea area(myRect);

        // Assert
        CHECK(area.IsHit(insidePoint));
        CHECK_FALSE(area.IsHit(lowerPoint));
        CHECK_FALSE(area.IsHit(higherPoint));
    }
}
TEST_CASE("Input Processing")
{
    // Arrange
    bool wasCalled = false;
    Rect aRect(0, 0, 200, 200);
    TouchArea area(aRect);
    Input anInput;
    // output = min + (rand() % static_cast<int>(max - min + 1))
    anInput.x = 1 + (rand() % static_cast<int>(aRect.GetWidth() - 1 + 1));
    anInput.y = 1 + (rand() % static_cast<int>(aRect.GetHeight() - 1 + 1));

    CHECK_LT(anInput.x, aRect.GetWidth());
    CHECK_LT(anInput.y, aRect.GetHeight());
    CHECK_GT(anInput.x, 0);
    CHECK_GT(anInput.y, 0);

    SUBCASE("Pressed Callback - Press Hit-Input")
    {
        // Arrange
        std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
            if (aState == Control::States::pressed) {
                wasCalled = true;
            }
        };
        area.RegisterOnPressed(pressedReciever);
        anInput.type = InputType::Press;

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK(wasCalled);

        SUBCASE("Pressed Callback - Press Miss-Input")
        {
            // Arrange
            std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
                if (aState == Control::States::pressed) {
                    wasCalled = false;
                }
            };
            area.RegisterOnPressed(pressedReciever);
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
            CHECK(wasCalled);
        }
    }
    SUBCASE("Pressed Callback - Lift Hit-Input")
    {
        // Arrange
        std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
            if (aState == Control::States::normal) {
                wasCalled = true;
            }
        };
        area.RegisterOnPressed(pressedReciever);
        anInput.type = InputType::Lift;

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK(wasCalled);

        SUBCASE("Pressed Callback - Lift Miss-Input")
        {
            // Arrange
            std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
                if (aState == Control::States::normal) {
                    wasCalled = false;
                }
            };
            area.RegisterOnPressed(pressedReciever);
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
            CHECK_FALSE(wasCalled);
        }
    }
    SUBCASE("Pressed Callback - Drag Hit-input")
    {
        // Arrange
        std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
            if (aState == Control::States::normal) {
                wasCalled = true;
            }
        };
        area.RegisterOnPressed(pressedReciever);
        anInput.type = InputType::Drag;

        // Act
        area.ProcessInput(anInput);

        // Assert
        CHECK_FALSE(wasCalled);

        SUBCASE("Pressed Callback - Drag Miss-input")
        {
            // Arrange
            wasCalled = false;
            std::function<void(Control::States)> pressedReciever = [&](Control::States aState) {
                if (aState == Control::States::normal) {
                    wasCalled = true;
                }
            };
            area.RegisterOnPressed(pressedReciever);
            anInput.type = InputType::Drag;
            anInput.x = anInput.x + aRect.GetWidth();
            anInput.y = anInput.y + aRect.GetHeight();

            // Act
            area.ProcessInput(anInput);

            // Assert
            CHECK(wasCalled);
            CHECK_GT(anInput.x, aRect.GetWidth());
            CHECK_GT(anInput.y, aRect.GetHeight());
        }
    }
    SUBCASE("Clicked Callback")
    {
        // Arrange
        std::function<void(Topic, ClickedEvent &)> clickedReciever = [&](Topic tp, ClickedEvent &event) {
            wasCalled = true;
            (void)tp;
            (void)event;
        };
        area.RegisterOnClicked(clickedReciever);

        SUBCASE("Clicked Callback - Hit-Hit-Input")
        {
            // Act
            anInput.type = InputType::Press;
            area.ProcessInput(anInput);
            anInput.type = InputType::Lift;
            area.ProcessInput(anInput);

            // Assert
            CHECK(wasCalled);
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
            CHECK_FALSE(wasCalled);
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
            CHECK_FALSE(wasCalled);
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
            CHECK_FALSE(wasCalled);
        }
    }
}