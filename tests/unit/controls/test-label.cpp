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

    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer::mpDevicePath = p.c_str();

    CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-Italic-VariableFont_wght.ttf"));
    CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-VariableFont_wght.ttf"));

    auto& renderer = Renderer::Get();
    CHECK_NOTHROW(renderer.Fill(Color::Grey));

    Rect r(50, 100, 380, 200);
    Label label;

    CHECK_NOTHROW(
        label.SetFontSize(30).SetArea(r);
        auto& style = label.GetStyle(Control::States::Normal);
        style.mForegroundColor = Color::Black;
        style.mBackgroundColor = Color::Yellow;
        label << "Hello World!";
    );

    SUBCASE("Left Top") {
        CHECK_NOTHROW(label << Text::HAlign::Left << Text::VAlign::Top);
    }
    SUBCASE("Left Center") {
        CHECK_NOTHROW(label << Text::HAlign::Left << Text::VAlign::Center);
    }
    SUBCASE("Left Bottom") {
        CHECK_NOTHROW(label << Text::HAlign::Left << Text::VAlign::Bottom);
    }
    SUBCASE("Center Top") {
        CHECK_NOTHROW(label << Text::HAlign::Center << Text::VAlign::Top);
    }
    SUBCASE("Center Center") {
        CHECK_NOTHROW(label << Text::HAlign::Center << Text::VAlign::Center);
    }
    SUBCASE("Center Bottom") {
        CHECK_NOTHROW(label << Text::HAlign::Center << Text::VAlign::Bottom);
    }
    SUBCASE("Right Top") {
        CHECK_NOTHROW(label << Text::HAlign::Right << Text::VAlign::Top);
    }
    SUBCASE("Right Center") {
        CHECK_NOTHROW(label << Text::HAlign::Right << Text::VAlign::Center);
    }
    SUBCASE("Right Bottom") {
        CHECK_NOTHROW(label << Text::HAlign::Right << Text::VAlign::Bottom);
    }
    SUBCASE("Scaled") {
        CHECK_NOTHROW(label.ScaleToFit(true));
    }

    CHECK(label.UpdateData());
    // Check that UpdateData returns false if not invalid
    CHECK_FALSE(label.UpdateData());
    CHECK_NOTHROW(label.Render(renderer));
    CHECK_NOTHROW(renderer.Present());

    // Dont know how to test that text is rendered correctly, but a least we can test that nothing paint outside background
    for (int y = r.GetTop(); y < r.GetBottom() ; ++y) {
        CHECK_EQ(renderer.GetPixel(49, y, true), Color::Grey);
        CHECK_EQ(renderer.GetPixel(430, y, true), Color::Grey);
        CHECK_EQ(renderer.GetPixel(50, y, true), Color::Yellow);
        CHECK_EQ(renderer.GetPixel(429, y, true), Color::Yellow);
    }

    for (int x = r.GetLeft(); x < r.GetRight() ; ++x) {
        CHECK_EQ(renderer.GetPixel(x, 99, true), Color::Grey);
        CHECK_EQ(renderer.GetPixel(x, 300, true), Color::Grey);
        CHECK_EQ(renderer.GetPixel(x, 100, true), Color::Yellow);
        CHECK_EQ(renderer.GetPixel(x, 299, true), Color::Yellow);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_SUITE_END();
