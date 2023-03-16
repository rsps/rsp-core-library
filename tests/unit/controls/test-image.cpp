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
#include <graphics/Image.h>
#include <graphics/Renderer.h>
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Image")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer::mpDevicePath = p.c_str();

    auto& renderer = Renderer::Get();
    CHECK_NOTHROW(renderer.Fill(Color::Grey));

    Canvas bmp(200, 100);
    CHECK_NOTHROW(bmp.Fill(Color::Red));

    Point pos(20, 20);

    SUBCASE("Default Constructor")
    {
        Image testImage;
        CHECK_NOTHROW(testImage = bmp);
        CHECK_NOTHROW(testImage.SetOrigin(pos));

        CHECK(testImage.UpdateData());
        CHECK_NOTHROW(testImage.Render(renderer));
        CHECK_NOTHROW(renderer.Present());

        Point insidePoint(Random::Roll(20, 220), Random::Roll(20, 120));

        CHECK_EQ(renderer.GetPixel(insidePoint.GetX(), insidePoint.GetY(), true), Color::Red);
        CHECK_NE(renderer.GetPixel(19,   19, true), Color::Red);
        CHECK_NE(renderer.GetPixel(20,   19, true), Color::Red);
        CHECK_NE(renderer.GetPixel(19,   20, true), Color::Red);
        CHECK_NE(renderer.GetPixel(119,  19, true), Color::Red);
        CHECK_NE(renderer.GetPixel(220,  19, true), Color::Red);
        CHECK_NE(renderer.GetPixel(119, 120, true), Color::Red);
        CHECK_EQ(renderer.GetPixel(20,   20, true), Color::Red);
        CHECK_EQ(renderer.GetPixel(219,  20, true), Color::Red);
        CHECK_EQ(renderer.GetPixel(20,  119, true), Color::Red);
        CHECK_EQ(renderer.GetPixel(219, 119, true), Color::Red);

    }

    SUBCASE("Render Image if Invalid")
    {
        // Arrange
        Color red(0xFFc41616);
        Point insidePoint(Random::Roll(testRect.GetLeft(), testRect.GetLeft() + testRect.GetWidth()),
                          Random::Roll(testRect.GetTop(), testRect.GetTop() + testRect.GetHeight()));
        testImage.Invalidate();

        // Act
        if (testImage.UpdateData()) {
            testImage.Render(renderer);
            renderer.Present();
        }

//        MESSAGE("insidePoint: " << insidePoint);
//        MESSAGE("Origin: " << testImage.GetOrigin());
//        MESSAGE("Destination: " << testImage.GetStyle(Control::States::normal).mBackground.GetDestination());
        // Assert
        CHECK_EQ(renderer.GetPixel(insidePoint.GetX(), insidePoint.GetY(), true), red.AsUint());
        CHECK_NE(renderer.GetPixel(19,   19, true), red.AsUint());
        CHECK_NE(renderer.GetPixel(20,   19, true), red.AsUint());
        CHECK_NE(renderer.GetPixel(19,   20, true), red.AsUint());
        CHECK_NE(renderer.GetPixel(119,  19, true), red.AsUint());
        CHECK_NE(renderer.GetPixel(220,  19, true), red.AsUint());
        CHECK_NE(renderer.GetPixel(119, 120, true), red.AsUint());
        CHECK_EQ(renderer.GetPixel(20,   20, true), red.AsUint());
        CHECK_EQ(renderer.GetPixel(219,  20, true), red.AsUint());
        CHECK_EQ(renderer.GetPixel(20,  119, true), red.AsUint());
        CHECK_EQ(renderer.GetPixel(219, 119, true), red.AsUint());

        SUBCASE("Do not render if Image valid")
        {
            // Arrange

            // Act
            if (testImage.UpdateData()) {
                testImage.Render(renderer);
                renderer.Present();
            }

            // Assert
            CHECK_NE(renderer.GetPixel(insidePoint.GetX(), insidePoint.GetY()), red.AsUint());
        }
    }
}

TEST_SUITE_END();
