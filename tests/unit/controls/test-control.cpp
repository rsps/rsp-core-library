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
#include <graphics/Control.h>
#include <graphics/Renderer.h>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Control")
{
    CHECK_NOTHROW(Control dummy);
    Control myControl;
    CHECK(myControl.IsInvalid());

    CHECK_NOTHROW(myControl.UpdateData());
    CHECK_FALSE(myControl.IsInvalid());

    SUBCASE("Child Invalidation")
    {
        Control childControl;
        CHECK_NOTHROW(myControl.AddChild(&childControl));
        CHECK_FALSE(myControl.IsInvalid());
        CHECK(childControl.IsInvalid());

        CHECK_NOTHROW(childControl.UpdateData());
        CHECK_FALSE(myControl.IsInvalid());
        CHECK_FALSE(childControl.IsInvalid());

        CHECK_NOTHROW(myControl.Invalidate());

        CHECK(myControl.IsInvalid());
        CHECK_FALSE(childControl.IsInvalid());

        CHECK_NOTHROW(myControl.UpdateData());
        CHECK_FALSE(myControl.IsInvalid());
        CHECK_FALSE(childControl.IsInvalid());

        CHECK_NOTHROW(childControl.Invalidate());
        CHECK_FALSE(myControl.IsInvalid());
        CHECK(childControl.IsInvalid());

        CHECK_NOTHROW(childControl.UpdateData());
        CHECK_FALSE(myControl.IsInvalid());
        CHECK_FALSE(childControl.IsInvalid());

        CHECK_NOTHROW(myControl.RemoveChild(&childControl));
        CHECK(myControl.IsInvalid());
        CHECK_FALSE(childControl.IsInvalid());
    }

    SUBCASE("Default State")
    {
        CHECK(myControl.GetState() == Control::States::Normal);
    }

    SUBCASE("Change")
    {
        CHECK_FALSE(myControl.IsInvalid());

        SUBCASE("State") {
            CHECK_NOTHROW(myControl.SetState(Control::States::Pressed));
            CHECK(myControl.GetState() == Control::States::Pressed);
        }

        SUBCASE("Transparent") {
            CHECK_NOTHROW(myControl.SetTransparent(true));
            CHECK(myControl.IsTransparent());
        }

        SUBCASE("Checkable") {
            CHECK_NOTHROW(myControl.SetCheckable(true));
            CHECK(myControl.IsCheckable());
        }

        SUBCASE("Draggable") {
            CHECK_NOTHROW(myControl.SetDraggable(true));
            CHECK(myControl.IsDraggable());
        }

        SUBCASE("Visible") {
            CHECK(myControl.IsVisible());
            CHECK_NOTHROW(myControl.Hide());
            CHECK_FALSE(myControl.IsVisible());
            CHECK_NOTHROW(myControl.Show());
            CHECK(myControl.IsVisible());
        }

        SUBCASE("Size") {
            CHECK_NOTHROW(myControl.SetArea(Rect(10, 10, 20, 30)));
            CHECK(myControl.GetArea() == Rect(10, 10, 20, 30));

            CHECK_NOTHROW(myControl.SetOrigin(Point(12, 12)));
            CHECK(myControl.GetArea() == Rect(12, 12, 20, 30));
        }

        CHECK(myControl.IsInvalid());
    }

    SUBCASE("Render")
    {
        Renderer::Get();
    }
}

TEST_SUITE_END();
