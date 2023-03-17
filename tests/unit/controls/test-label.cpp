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
#include <graphics/Framebuffer.h>
#include <graphics/Label.h>
#include <graphics/Renderer.h>
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Label")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer::mpDevicePath = p.c_str();

    CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-Italic-VariableFont_wght.ttf"));
    CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-VariableFont_wght.ttf"));

    auto& renderer = Renderer::Get();
    CHECK_NOTHROW(renderer.Fill(Color::Grey));
    CHECK_NOTHROW(renderer.Present());
    CHECK_NOTHROW(renderer.Fill(Color::Grey));

    Label label;

    CHECK_NOTHROW(
        label.SetFontSize(30).SetArea(Rect(50, 100, 380, 200));
        auto& style = label.GetStyle(Control::States::Normal);
        style.mForegroundColor = Color::Black;
        style.mBackgroundColor = Color::Yellow;
        label << "Hello World!";
    );

    CHECK(label.UpdateData());
    // Check that UpdateData returns false if not invalid
    CHECK_FALSE(label.UpdateData());
    CHECK_NOTHROW(label.Render(renderer));
    CHECK_NOTHROW(renderer.Present());

//    CHECK_EQ(renderer.GetPixel(insidePoint.GetX(), insidePoint.GetY(), true), fill_color);
//    CHECK_NE(renderer.GetPixel(19,   19, true), fill_color);
//    CHECK_NE(renderer.GetPixel(20,   19, true), fill_color);
//    CHECK_NE(renderer.GetPixel(19,   20, true), fill_color);
//    CHECK_NE(renderer.GetPixel(119,  19, true), fill_color);
//    CHECK_NE(renderer.GetPixel(220,  19, true), fill_color);
//    CHECK_NE(renderer.GetPixel(119, 120, true), fill_color);
//    CHECK_EQ(renderer.GetPixel(20,   20, true), fill_color);
//    CHECK_EQ(renderer.GetPixel(219,  20, true), fill_color);
//    CHECK_EQ(renderer.GetPixel(20,  119, true), fill_color);
//    CHECK_EQ(renderer.GetPixel(219, 119, true), fill_color);
}

TEST_SUITE_END();
