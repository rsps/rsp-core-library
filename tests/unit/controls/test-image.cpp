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
#include "graphics/Framebuffer.h"
#include "graphics/controls/Image.h"
#include "posix/FileSystem.h"
#include <TestHelpers.h>

using namespace rsp::graphics;

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

    Image testImage;
    testImage.GetStyle(Control::States::normal).mBitmapView.SetBitmap(&normal);
    testImage.SetArea(testRect);

    SUBCASE("Render Image if Invalid")
    {
        // Arrange
        Color red(0xFFc41616);
        Point insidePoint(testRect.GetLeft() + (rand() % testRect.GetWidth()),
                          testRect.GetTop() + (rand() % testRect.GetHeight()));
        testImage.Invalidate();

        // Act
        testImage.Render(fb);
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

//        MESSAGE("insidePoint: " << insidePoint);
        // Assert
        CHECK_EQ(fb.GetPixel(insidePoint, true), red.AsUint());
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
