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
#include <utils/RSPCoreExceptions.h>

using namespace rsp::utils;
using namespace rsp::graphics;

TEST_CASE("Bitmap file loading")
{
    SUBCASE("Loading Bmp file")
    {
        std::string filepath = "testImages/testImage.bmp";
        CHECK_NOTHROW(
            Bitmap bitmap(filepath);
            Color col(bitmap.GetPixels()[0]);
            CHECK(bitmap.GetPixels().size() > 0);
            CHECK(bitmap.GetHeight() == 194);
            CHECK(bitmap.GetWidth() == 259);
            CHECK(col == bitmap.GetPixels()[0]););
    }
    SUBCASE("Loading Png file")
    {
        std::string filepath = "testImages/testImage.png";
        CHECK_THROWS_AS(Bitmap bitmap(filepath), const NotImplementedException &);
    }
}
