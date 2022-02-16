/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/Control.h"
#include <doctest.h>

using namespace rsp::graphics;

// To have a testable Control, that are not dependent on used types
class TestControl : public Control
{
  public:
    TestControl() : Control(){};
    TestControl(Rect &aRect, Control *aParent = nullptr) : Control(aRect, aParent){};
    ~TestControl(){};
    void Render(Canvas &aCanvas) override
    {
        (void)aCanvas;
        return;
    }
};

TEST_CASE("Control Constructor")
{
}
TEST_CASE("Control Invalidation")
{
    // Arrange
    TestControl myControl;

    // Act
    myControl.Invalidate();

    // Assert
    CHECK(myControl.mIsInvalid);
    CHECK(myControl.IsInvalid());

    SUBCASE("Child Invalidation")
    {
        // Arrange
        TestControl childControl;
        myControl.mChildren.push_back(&childControl);

        // Act
        myControl.Invalidate();

        // Assert
        CHECK(childControl.mIsInvalid);
        CHECK(childControl.IsInvalid());
    }
}
TEST_CASE("Control States")
{
    // Arrange
    TestControl myControl;

    SUBCASE("Default State")
    {
        // Act & Assert
        CHECK(myControl.mState == Control::States::normal);
    }
    SUBCASE("Change State")
    {
        // Act
        myControl.SetState(Control::States::pressed);

        // Assert
        CHECK(myControl.mState == Control::States::pressed);
        CHECK(myControl.IsInvalid());

        SUBCASE("Child Invalidated by Parent State Change")
        {
            // Arrange
            TestControl childControl;
            myControl.mChildren.push_back(&childControl);

            // Act
            myControl.SetState(Control::States::normal);

            // Assert
            CHECK(childControl.IsInvalid());
        }
    }
}
