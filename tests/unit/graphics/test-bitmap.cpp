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
#include <exceptions/CoreException.h>
#include <graphics/Bitmap.h>
#include <graphics/Color.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Bitmap")
{
    TestLogger logger;

    SUBCASE("Create")
    {
        CHECK_NOTHROW(Bitmap tmp(200, 200, ColorDepth::RGBA));
    }

    SUBCASE("Drawing")
    {
        Bitmap bitmap(200, 200, ColorDepth::RGBA);

        Color col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
        Point pt(100, 100);
        CHECK(bitmap.IsHit(pt));
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
        CHECK_HEX(bitmap.GetPixelData().GetPixelAt(0, 0, Color::White).AsUint(), 0xFF020A8F);
        CHECK_HEX(bitmap.GetPixelData().GetPixelAt(1, 0, Color::White).AsUint(), 0xFF020A8F);
        CHECK_HEX(bitmap.GetPixelData().GetPixelAt(55, 111, Color::White).AsUint(), 0xFFEAEFE8);

        SUBCASE("Drawing on loaded Img") {
            // Arrange
            Color cl(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
            Point pt(100, 100);

            // Act
            CHECK_NOTHROW(bitmap.SetPixel(pt, cl));

            // Assert
            CHECK_HEX(bitmap.GetPixel(pt).AsUint(), cl.AsUint());
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

    SUBCASE("Monochrome Bmp file")
    {
        // Arrange
        std::string filepath = "testImages/monochrome/Monochrome.bmp";
        uint32_t height = 100;
        uint32_t width = 100;

        // Act
        CHECK_NOTHROW(Bitmap bmp3(filepath));

        // Arrange
        Bitmap bitmap3(filepath);
        Color col3(bitmap3.GetPixelData().GetPixelAt(0,0,Color::White));

        // Assert
        CHECK(bitmap3.GetHeight() == height);
        CHECK(bitmap3.GetWidth() == width);
        CHECK_EQ(bitmap3.GetPixelData().GetDataSize(), (((width + 7) >> 3) * height));
        CHECK_EQ(bitmap3.GetPixelData().GetPixelAt(0,0,Color::White), Color(0x00FFFFFF));
        CHECK_EQ(bitmap3.GetPixelData().GetPixelAt(32,50,Color::White), Color(Color::White));
    }
/*
    SUBCASE("Loading PNG file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.png";
        uint32_t height = 194;
        uint32_t width = 259;

//        REQUIRE_NOTHROW(Bitmap png1(filepath));
        Bitmap png(filepath);

        CHECK(png.GetHeight() == height);
        CHECK(png.GetWidth() == width);
        CHECK_EQ(png.GetPixelData().GetDataSize(), (width * height * 3));
        CHECK_EQ(png.GetPixelData().GetPixelAt(0, 0, Color::White).AsUint(), 0xFF020A8F);
        CHECK_EQ(png.GetPixelData().GetPixelAt(1, 0, Color::White).AsUint(), 0xFF020A8F);
        CHECK_EQ(png.GetPixelData().GetPixelAt(55, 111, Color::White).AsUint(), 0xFFEAEFE8);

//        Color col(png.GetPixelData().GetPixelAt(0, 0, Color::White));
//        CHECK_EQ(col, Color(4292918232));
    }
*/
    SUBCASE("Loading unsupported file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.txt";

        // Act Assert
        CHECK_THROWS_AS(Bitmap bitmap(filepath), EUnsupportedFileformat);
    }
}

TEST_SUITE_END();
