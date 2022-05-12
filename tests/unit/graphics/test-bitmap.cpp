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

TEST_CASE("Bitmap file loading")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    SUBCASE("Loading Bmp file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.bmp";
        uint32_t height = 194;
        uint32_t width = 259;

        // Act
        CHECK_NOTHROW(
            Bitmap bitmap(filepath);
            Color col(bitmap.GetPixels()[0]);

            // Assert
            CHECK(bitmap.GetHeight() == height);
            CHECK(bitmap.GetWidth() == width);
            CHECK(bitmap.GetPixels().size() == (width * height));
            CHECK(col == bitmap.GetPixels()[0]);
            SUBCASE("Drawing on loaded Img") {
                // Arrange
                Color col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
                Point pt(100, 100);

                // Act
                bitmap.SetPixel(pt, col);

                // Assert
                CHECK(bitmap.GetPixel(pt) == col);
            });
    }
    SUBCASE("Loading another Bmp file")
    {
        // Arrange
        std::string filepath = "testImages/Asset3.bmp";
        uint32_t height = 800;
        uint32_t width = 480;

        // Act
        CHECK_NOTHROW(

            // Arrange
            Bitmap bitmap2(filepath);
            Color col2(bitmap2.GetPixels()[0]);

            // Assert
            CHECK(bitmap2.GetHeight() == height);
            CHECK(bitmap2.GetWidth() == width);
            CHECK(bitmap2.GetPixels().size() == (width * height));
            CHECK(col2 == bitmap2.GetPixels()[0]););
    }
    SUBCASE("Loading Png file")
    {
        // Arrange
        std::string filepath = "testImages/testImage.png";

        // Act / Assert
        CHECK_THROWS_WITH_AS(Bitmap bitmap(filepath), "Png file format is not supported",
                             const NotImplementedException &);
    }
    SUBCASE("Loading filetype not found")
    {
        // Arrange
        std::string filepath = "testImages/testImage.txt";

        // Act Assert
        CHECK_THROWS_WITH_AS(Bitmap bitmap(filepath), "Filetype loader not found: _Map_base::at",
                             const std::out_of_range &);
    }
}
TEST_CASE("Bitmap empty construction")
{
    // Arrange
    Bitmap bitmap(200, 200, 4);
    SUBCASE("Drawing inside empty bitmap")
    {
        // Arrange
        Color col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
        Point pt(100, 100);

        // Act
        bitmap.SetPixel(pt, col);

        // Assert
        CHECK(bitmap.IsInsideScreen(pt));
        CHECK(bitmap.GetPixel(pt) == col);
    }
}
