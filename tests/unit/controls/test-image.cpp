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
#include <graphics/Image.h>
#include <graphics/Renderer.h>
#include <graphics/SW/Framebuffer.h>
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <TestHelpers.h>
#include <pixmap/GfxResources.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Image")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
#ifdef USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    sw::Framebuffer::mpDevicePath = p.c_str();
#endif

    auto& renderer = Renderer::Init(480, 800);
    CHECK_NOTHROW(renderer.Fill(Color::Grey));
    CHECK_NOTHROW(renderer.Present());
    CHECK_NOTHROW(renderer.Fill(Color::Grey));

    auto fill_color = Color::Purple;

    Canvas bmp(200, 100);
    CHECK_NOTHROW(bmp.Fill(fill_color));

    Point pos(20, 20);
    Point insidePoint(Random::Roll(20, 219), Random::Roll(20, 119));

    std::unique_ptr<Image> image;

    SUBCASE("Default Constructor")
    {
        CHECK_NOTHROW(image = std::make_unique<Image>());
        CHECK_NOTHROW(*image = bmp);
    }

    SUBCASE("Copy Constructor")
    {
        CHECK_NOTHROW(image = std::make_unique<Image>(bmp));
    }

    CHECK_NOTHROW(image->SetOrigin(pos));

    CHECK(image->UpdateData());
    // Check that UpdateData returns false if not invalid
    CHECK_FALSE(image->UpdateData());
    CHECK_NOTHROW(image->Render(renderer));
    CHECK_NOTHROW(renderer.Present());
    CHECK_NOTHROW(image->Render(renderer));
    CHECK_NOTHROW(renderer.Present());

    CHECK_EQ(renderer.GetPixel(insidePoint.GetX(), insidePoint.GetY()), fill_color);
    CHECK_NE(renderer.GetPixel(19,   19), fill_color);
    CHECK_NE(renderer.GetPixel(20,   19), fill_color);
    CHECK_NE(renderer.GetPixel(19,   20), fill_color);
    CHECK_NE(renderer.GetPixel(119,  19), fill_color);
    CHECK_NE(renderer.GetPixel(220,  19), fill_color);
    CHECK_NE(renderer.GetPixel(119, 120), fill_color);
    CHECK_EQ(renderer.GetPixel(20,   20), fill_color);
    CHECK_EQ(renderer.GetPixel(219,  20), fill_color);
    CHECK_EQ(renderer.GetPixel(20,  119), fill_color);
    CHECK_EQ(renderer.GetPixel(219, 119), fill_color);
}

TEST_CASE("TestImage")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
#ifdef USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    sw::Framebuffer::mpDevicePath = p.c_str();
#endif

    auto& renderer = Renderer::Init(480, 800);
    CHECK_NOTHROW(renderer.Fill(Color::Grey));

    PixelData bmp;

    SUBCASE("Monochrome") {
        bmp = PixelData(cMonochrome);
    }
    SUBCASE("Alpha") {
        bmp = PixelData(cLowerCase);
//        bmp.SaveToCFile("LowerCase-compressed.cpp", true);
//        bmp.SaveToCFile("LowerCase-normal.cpp", false);
    }
    SUBCASE("RGB") {
        bmp = PixelData(cTestImage480x800);
//        bmp.SaveToCFile("TestImage480x800b.cpp");
    }

    BitmapView bv(bmp);
    Image image(bv);
    image.GetStyle(Control::States::Normal).mForegroundColor = Color::Black;
    image.GetStyle(Control::States::Normal).mBackgroundColor = Color::Yellow;

    image.SetTransparent(false).SetOrigin(Point((480 - bmp.GetWidth()) / 2, (800 - bmp.GetHeight()) / 2));

    CHECK(image.UpdateData());
    CHECK_NOTHROW(image.Render(renderer));
    CHECK_NOTHROW(renderer.Present());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

TEST_SUITE_END();
