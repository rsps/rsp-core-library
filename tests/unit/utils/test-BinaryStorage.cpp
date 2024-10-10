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
#include <utils/BinaryStorage.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <utils/DateTime.h>

using namespace rsp::utils;

enum class EType : uint8_t {
    NONE,
    ONE,
    TWO
};


TEST_CASE("BinaryStorage") {

    const std::string cFileName("storage-file.dat");

    SUBCASE("Write") {
        std::ofstream o_file(cFileName, std::ios::binary);
        BinaryStorage ss(o_file);
        std::string s1("This is a string.");
        CHECK_NOTHROW(ss << s1 << uint16_t(46222) << true << false);
    }

    SUBCASE("Read") {
        std::ifstream i_file(cFileName, std::ios::binary);
        BinaryStorage ss(i_file);
        std::string s1;
        uint16_t u16;
        bool b1;
        bool b2;
        CHECK_NOTHROW(ss >> s1 >> u16 >> b1 >> b2);
        CHECK_EQ(s1, std::string("This is a string."));
        CHECK_EQ(u16, uint16_t(46222));
        CHECK(b1);
        CHECK_FALSE(b2);
    }

    SUBCASE("IO") {
        std::vector<int> v{13, 423, 42, 76, -90};
        auto now = DateTime::Now();
        auto duration = std::chrono::milliseconds(113);
        std::stringstream ss;
        BinaryStorage bs(ss);
        bs << v << EType::ONE << now << duration;

        ss.seekg(0);

        std::vector<int> r;
        DateTime dt;
        EType e;
        std::chrono::milliseconds ms;
        bs >> r >> e >> dt >> ms;

        CHECK_EQ(v.size(), r.size());
        size_t index = 0;
        for (auto &i : v) {
            CHECK_EQ(i, r.at(index++));
        }
        CHECK_EQ(e, EType::ONE);
        CHECK_EQ(dt, now);
        CHECK_EQ(ms.count(), 113);
    }
}
