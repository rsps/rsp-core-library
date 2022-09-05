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
#include <graphics/controls/Image.h>
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Image Test")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());
    Rect testRect(20, 20, 200, 100);
    Bitmap normal("testImages/Red.bmp");
    Random::Seed(1234);

    Image testImage;
    testImage.GetStyle(Control::States::normal).mBackground.SetBitmap(&normal);
    testImage.SetArea(testRect);

    SUBCASE("Render Image if Invalid")
    {
        // Arrange
        Color red(0xFFc41616);
        Point insidePoint(Random::Roll(testRect.GetLeft(), testRect.GetWidth()),
                          Random::Roll(testRect.GetTop(), testRect.GetHeight()));
        testImage.Invalidate();

        // Act
        testImage.Render(fb);
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

//        MESSAGE("insidePoint: " << insidePoint);
//        MESSAGE("Origin: " << testImage.GetOrigin());
//        MESSAGE("Destination: " << testImage.GetStyle(Control::States::normal).mBackground.GetDestination());
        // Assert
        CHECK_EQ(fb.GetPixel(insidePoint, true), red.AsUint());
        CHECK_NE(fb.GetPixel({19, 19}, true), red.AsUint());
        CHECK_NE(fb.GetPixel({20, 19}, true), red.AsUint());
        CHECK_NE(fb.GetPixel({19, 20}, true), red.AsUint());
        CHECK_NE(fb.GetPixel({119, 19}, true), red.AsUint());
        CHECK_NE(fb.GetPixel({220, 19}, true), red.AsUint());
        CHECK_NE(fb.GetPixel({119, 120}, true), red.AsUint());
        CHECK_EQ(fb.GetPixel({20, 20}, true), red.AsUint());
        CHECK_EQ(fb.GetPixel({219, 20}, true), red.AsUint());
        CHECK_EQ(fb.GetPixel({20, 119}, true), red.AsUint());
        CHECK_EQ(fb.GetPixel({219, 119}, true), red.AsUint());
        SUBCASE("Do not render if Image valid")
        {
            // Arrange
            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

            // Act
            testImage.Render(fb);

            // Assert
            CHECK_NE(fb.GetPixel(insidePoint), red.AsUint());
        }
    }
}

TEST_SUITE_END();
