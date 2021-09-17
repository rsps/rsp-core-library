/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <filesystem/FileIO.h>
#include <doctest.h>

using namespace rsp::filesystem;

TEST_CASE("File IO") {
    const std::string cFileName = "testImages/testImage.bmp";

    SUBCASE("Create") {
        CHECK_NOTHROW(FileIO f);
    }

    SUBCASE("Create And Load") {
        CHECK_THROWS({ FileIO f(cFileName, std::ios_base::in); });
    }

}
