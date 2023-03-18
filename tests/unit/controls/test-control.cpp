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
#include <graphics/Framebuffer.h>
#include <graphics/Renderer.h>
#include <posix/FileSystem.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Control")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer::mpDevicePath = p.c_str();

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
        auto &renderer = Renderer::Get();
        CHECK_NOTHROW(renderer.Fill(Color::Grey));

        Canvas paper(150, 50);
        paper.Fill(Color::Yellow);
        auto texture = Texture::Create(paper);
        texture->SetDestination({100, 300});

        auto &style = myControl.GetStyle(Control::States::Normal);
        style.mBackgroundColor = Color::Blue;
        style.mTextures.push_back(texture->Clone());

        CHECK_NOTHROW(myControl.SetTransparent(false).SetArea({200, 40, 100, 500}).Show().Invalidate());

        CHECK(myControl.UpdateData());
        CHECK_NOTHROW(myControl.Render(renderer));
        CHECK_NOTHROW(renderer.Present());

        CHECK_EQ(renderer.GetPixel(200, 40, true), Color::Blue);
        CHECK_EQ(renderer.GetPixel(299, 539, true), Color::Blue);
        CHECK_EQ(renderer.GetPixel(300, 540, true), Color::Grey);

        CHECK_EQ(renderer.GetPixel(100, 300, true), Color::Yellow);
        CHECK_EQ(renderer.GetPixel(249, 349, true), Color::Yellow);
        CHECK_EQ(renderer.GetPixel(250, 350, true), Color::Blue);
}
}

TEST_SUITE_END();
