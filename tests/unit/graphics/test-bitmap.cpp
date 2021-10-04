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

TEST_CASE("Bitmap file loading")
{
    SUBCASE("Loading Bmp file")
    {
        std::string filepath = "testImages/testImage.bmp";
        CHECK_NOTHROW(
            Bitmap bitmap(filepath);
            CHECK(bitmap.mImagePixels.size() > 0);
            CHECK(bitmap.mHeight == 194);
            CHECK(bitmap.mWidth == 259););
    }
    /*SUBCASE("Loading Png file")
    {
        std::string filepath = "testImages/testImage.png";
        CHECK_NOTHROW(
            Bitmap bitmap(filepath);
            //CHECK(bitmap.mImagePixels.size() > 0);
            CHECK(bitmap.mWidth == 259);
            CHECK(bitmap.mHeight == 194););
    }*/
}
