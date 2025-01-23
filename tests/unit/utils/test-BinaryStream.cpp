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
#include <utils/BinaryStream.h>
#include <fstream>
#include <vector>
#include <utils/DateTime.h>
#include <utils/HexStream.h>
#include <utils/StructElement.h>

using namespace rsp::utils;

enum class EType : uint8_t {
    NONE,
    ONE,
    TWO
};


TEST_CASE("BinaryStream") {

    const std::string cFileName("storage-file.dat");

    SUBCASE("Write") {
        std::ofstream o_file(cFileName, std::ios::binary);
        BinaryStream ss(o_file);
        std::string s1("This is a string.");
        std::string s2(128, 'A');
        size_t sz = 46922;
        auto now = DateTime::Now();
        StructElement<std::string> se_str(now.ToRFC3339Milli());
        StructElement<DateTime> se_dt(now);
        StructElement<int> se_null;
        StructElement<float> f1 = 1.234567f;
        CHECK_NOTHROW(ss << s1 << uint16_t(46222) << true << false << s2 << sz << se_str << se_dt << se_null << f1);
    }

    SUBCASE("Read") {
        BinaryFileStream ss(cFileName, std::ios::in);
        std::string s1;
        std::string s2;
        uint16_t u16;
        bool b1;
        bool b2;
        size_t sz;
        StructElement<std::string> se_str;
        StructElement<DateTime> se_dt;
        StructElement<int> se_null;
        StructElement<float> f1;
        CHECK_NOTHROW(ss >> s1 >> u16 >> b1 >> b2 >> s2 >> sz >> se_str >> se_dt >> se_null >> f1);
        CHECK_EQ(s1, std::string("This is a string."));
        CHECK_EQ(u16, uint16_t(46222));
        CHECK(b1);
        CHECK_FALSE(b2);
        CHECK_EQ(s2.size(), 128);
        CHECK_EQ(sz, 46922);
        CHECK_FALSE(se_str.IsNull());
        CHECK_FALSE(se_dt.IsNull());
        CHECK(se_null.IsNull());
        CHECK_EQ(se_str.Get(), se_dt.Get().ToRFC3339Milli());
        CHECK_EQ(f1.Get(), 1.234567f);
    }

    SUBCASE("IO") {
        std::vector<int> v{13, 423, 42, 76, -90};
        auto now = DateTime::Now();
        auto duration = std::chrono::milliseconds(113);
        std::string s1(256, 'A');
        std::string empty;
        BinaryStringStream bs;
        bs << v << EType::ONE << now << duration << empty << s1;

        bs.Reset();

        std::vector<int> r;
        DateTime dt;
        EType e;
        std::chrono::milliseconds ms;
        std::string s2, empty2;
        bs >> r >> e >> dt >> ms >> empty2 >> s2;

        CHECK_EQ(v.size(), r.size());
        size_t index = 0;
        for (auto &i : v) {
            CHECK_EQ(i, r.at(index++));
        }
        CHECK_EQ(e, EType::ONE);
        CHECK_EQ(dt, now);
        CHECK_EQ(ms.count(), 113);
        CHECK_EQ(s1, s2);
        CHECK_EQ(bs.Get().str().size(), 305);
    }
}
