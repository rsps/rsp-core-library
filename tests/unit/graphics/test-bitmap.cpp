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
#include <graphics/primitives/Bitmap.h>
#include <graphics/primitives/Color.h>
#include <utils/CoreException.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::graphics;

TEST_CASE("Bitmap")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    SUBCASE("Create")
    {
        CHECK_NOTHROW(Bitmap tmp(200, 200, 4));
    }

    SUBCASE("Drawing")
    {
        Bitmap bitmap(200, 200, 4);

        Color col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
        Point pt(100, 100);
        CHECK(bitmap.IsInsideScreen(pt));
        CHECK_NE(bitmap.GetPixel(pt), col);

        CHECK_NOTHROW(bitmap.SetPixel(pt, col));
        CHECK_EQ(bitmap.GetPixel(pt), col);
    }

    SUBCASE("Loading BMP file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.bmp";
        uint32_t height = 194;
        uint32_t width = 259;

        // Act
        CHECK_NOTHROW(Bitmap bmp(filepath));

        Bitmap bitmap(filepath);

        // Assert
        CHECK(bitmap.GetHeight() == height);
        CHECK(bitmap.GetWidth() == width);
        CHECK_EQ(bitmap.GetPixelData().GetDataSize(), (width * height * 3));
        Color col(bitmap.GetPixelData().GetPixelAt(0, 0, Color::White));
        CHECK_EQ(col, Color(Color::Black));
        // FFE0ABAD == FFE0BBD8₁
        Color col1(bitmap.GetPixelData().GetPixelAt(1, 0, Color::White));
        CHECK_EQ(col1, Color(0xFF03C1D9));
        // FF46297F == FFE0BBD8

        SUBCASE("Drawing on loaded Img") {
            // Arrange
            Color cl(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
            Point pt(100, 100);

            // Act
            CHECK_NOTHROW(bitmap.SetPixel(pt, cl));

            // Assert
            CHECK_EQ(bitmap.GetPixel(pt), cl);
        }
    }

    SUBCASE("Loading another Bmp file")
    {
        // Arrange
        std::string filepath = "testImages/Asset3.bmp";
        uint32_t height = 800;
        uint32_t width = 480;

        // Act
        CHECK_NOTHROW(Bitmap bmp2(filepath));

        // Arrange
        Bitmap bitmap2(filepath);
        Color col2(bitmap2.GetPixelData().GetPixelAt(0,0,Color::White));

        // Assert
        CHECK(bitmap2.GetHeight() == height);
        CHECK(bitmap2.GetWidth() == width);
        CHECK(bitmap2.GetPixelData().GetDataSize() == (width * height * 3));
        CHECK(col2 == bitmap2.GetPixelData().GetPixelAt(0,0,Color::White));
    }

    SUBCASE("Loading PNG file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.png";
        uint32_t height = 194;
        uint32_t width = 259;

        CHECK_NOTHROW(Bitmap png1(filepath));
        Bitmap png(filepath);

        CHECK(png.GetHeight() == height);
        CHECK(png.GetWidth() == width);
        CHECK_EQ(png.GetPixelData().GetDataSize(), (width * height * 3));
//        Color col(png.GetPixelData().GetPixelAt(0, 0, Color::White));
//        CHECK_EQ(col, Color(4292918232));
    }

    SUBCASE("Loading unsupported file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.txt";

        // Act Assert
        CHECK_THROWS_AS(Bitmap bitmap(filepath), EUnsupportedFileformat);
    }
}

