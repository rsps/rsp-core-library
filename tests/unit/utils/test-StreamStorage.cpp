/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <doctest.h>
#include <utils/StreamStorage.h>
#include <fstream>

using namespace rsp::utils;

TEST_CASE("StreamStorage") {

    const std::string cFileName("storage-file.dat");

    SUBCASE("Write") {
        std::ofstream o_file(cFileName, std::ios::binary);
        StreamStorage ss(o_file);
        ss << std::string("This is a string.") << uint16_t(46222) << true << false;
    }

    SUBCASE("Read") {
        std::ifstream i_file(cFileName, std::ios::binary);
        StreamStorage ss(i_file);
        std::string s1;
        uint16_t u32;
        bool b1;
        bool b2;
        ss >> s1 >> u32 >> b1 >> b2;
        CHECK_EQ(s1, std::string("This is a string."));
        CHECK_EQ(u32, uint16_t(46222));
        CHECK(b1);
        CHECK_FALSE(b2);
    }


}
