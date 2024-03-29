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
    TestControl() : Control(rsp::utils::MakeTypeInfo<TestControl>()) { mDirty = false; }

    void MakeValid() { mDirty = false; }
};

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Control Invalidation")
{
    // Arrange
    TestControl myControl;

    // Assert
    CHECK(!myControl.IsInvalid());

    // Act
    myControl.Invalidate();

    // Assert
    CHECK(myControl.IsInvalid());
    myControl.MakeValid();

    SUBCASE("Child Invalidation")
    {
        // Arrange
        TestControl childControl;
        myControl.AddChild(&childControl);

        // Act
        myControl.Invalidate();

        // Assert
        CHECK(childControl.IsInvalid());
    }

    SUBCASE("No Parent Invalidation")
    {
        // Arrange
        TestControl childControl;
        myControl.AddChild(&childControl);

        // Act
        childControl.Invalidate();

        // Assert
        CHECK(!myControl.IsInvalid());
    }

    SUBCASE("Transparent Parent Invalidation")
    {
        CHECK(!myControl.IsInvalid());

        TestControl childControl;
        myControl.AddChild(&childControl);

        CHECK(!myControl.IsInvalid());

        childControl.SetTransparent(true);

        CHECK(myControl.IsInvalid());
    }

}
TEST_CASE("Control States")
{
    // Arrange
    TestControl myControl;

    SUBCASE("Default State")
    {
        // Act & Assert
        CHECK(myControl.GetState() == Control::States::normal);
    }
    SUBCASE("Change State")
    {
        // Act
        myControl.SetState(Control::States::pressed);

        // Assert
        CHECK(myControl.GetState() == Control::States::pressed);
        CHECK(myControl.IsInvalid());
        myControl.MakeValid();

        SUBCASE("Child Invalidated by Parent State Change")
        {
            // Arrange
            TestControl childControl;
            myControl.AddChild(&childControl);

            // Act
            myControl.SetState(Control::States::normal);

            // Assert
            CHECK(childControl.IsInvalid());
        }
    }
}

TEST_SUITE_END();
