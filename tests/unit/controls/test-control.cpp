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
#include <graphics/Control.h>
#include <graphics/Renderer.h>
#include <posix/FileSystem.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Control")
{
    TestLogger logger;

#ifdef USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Renderer::SetDevicePath(p.string());
#endif

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
        CHECK(childControl.IsInvalid());

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
            CHECK_NOTHROW(myControl.SetPressed(true));
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
        auto &renderer = Renderer::Init(480, 800);

        Canvas paper(150, 50);
        paper.Fill(Color::Yellow);
        auto texture = Texture::Create(paper);
        texture->SetDestination({-100, 260}); // Relative to myControl

        auto &style = myControl.GetStyle(Control::States::Normal);
        style.mBackgroundColor = Color::Blue;
        style.mTextures.push_back(texture->Clone());

        CHECK_NOTHROW(myControl.SetTransparent(false).SetArea({200, 40, 100, 500}).Show().Invalidate());

        CHECK(myControl.UpdateData());
        CHECK_NOTHROW(renderer.Fill(Color::Grey));
        CHECK_NOTHROW(myControl.Render(renderer));
        CHECK_NOTHROW(renderer.Present());

        // Paint again on backbuffer to allow checks below
        CHECK_NOTHROW(renderer.Fill(Color::Grey));
        CHECK_NOTHROW(myControl.Render(renderer));
        CHECK_NOTHROW(renderer.Flush());

        CHECK_HEX(renderer.GetPixel(200,  40).AsUint(), Color::Blue);
        CHECK_HEX(renderer.GetPixel(299, 539).AsUint(), Color::Blue);
        CHECK_HEX(renderer.GetPixel(300, 540).AsUint(), Color::Grey);

        CHECK_HEX(renderer.GetPixel(100, 300).AsUint(), Color::Grey);
        CHECK_HEX(renderer.GetPixel(200, 300).AsUint(), Color::Yellow);
        CHECK_HEX(renderer.GetPixel(249, 349).AsUint(), Color::Yellow);
        CHECK_HEX(renderer.GetPixel(250, 350).AsUint(), Color::Blue);

//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(2500ms);
    }
}

TEST_SUITE_END();
