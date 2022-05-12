/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <cstring>
#include <array>
#include <utils/DataContainer.h>
#include <utils/FixedString.h>
#include <posix/FileSystem.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::logging;

struct MyData {
    int Integer = 42;
    float Float = 43.0f;
    FixedString<10> String{};
};

TEST_CASE("Data Container")
{
    const char* cFileName = "DataContainer.bin";

    Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    DataContainer<MyData> dc;

    SUBCASE("Init")
    {
        CHECK_EQ(sizeof(dc.GetHeader()), 12);
        CHECK_EQ(sizeof(dc.Get()), sizeof(MyData));
        CHECK_EQ(dc.GetHeader().Size, 12);
        CHECK_EQ(dc.GetHeader().Flags, ContainerFlags::None);
        CHECK_EQ(dc.GetHeader().Version, 1);
        CHECK_EQ(dc.GetHeader().PayloadCRC, 0);
        CHECK_EQ(dc.GetHeader().PayloadSize, 0);

        CHECK_EQ(dc.Get().Integer, 42);
        CHECK_EQ(dc.Get().Float, 43.0);
        CHECK_EQ(dc.Get().String, "");
    }

    SUBCASE("Save")
    {
        const char *p = "Hello World";

        dc.Get().String = p;
        dc.Get().Integer = 44;

        CHECK_EQ(dc.Get().String, "Hello Worl");

        CHECK(dc.Get().String != std::string(p));
        CHECK(dc.Get().String != p);

        dc.Save(cFileName);
        CHECK_EQ(dc.GetHeader().PayloadCRC, 0xD65B5F49);
        CHECK_EQ(dc.GetHeader().PayloadSize, sizeof(MyData));
    }

    SUBCASE("Load")
    {
        DataContainer<MyData> dcl;

        CHECK_NOTHROW(dcl.Load(cFileName));
        CHECK_EQ(dcl.GetHeader().Version, 1);
        CHECK_EQ(dcl.GetHeader().PayloadCRC, 0xD65B5F49);
        CHECK_EQ(dcl.GetHeader().PayloadSize, sizeof(MyData));

        CHECK_EQ(dcl.Get().Integer, 44);
        CHECK_EQ(dcl.Get().Float, 43.0);
        CHECK_EQ(dcl.Get().String, "Hello Worl");
    }

    SUBCASE("Assignment")
    {
        DataContainer<MyData> dcl;
        CHECK_NOTHROW(dcl.Load(cFileName));

        dc = dcl;

        CHECK_EQ(dc.GetHeader().Version, 1);
        CHECK_EQ(dc.GetHeader().PayloadCRC, 0xD65B5F49);
        CHECK_EQ(dc.GetHeader().PayloadSize, sizeof(MyData));

        CHECK_EQ(dc.Get().Integer, 44);
        CHECK_EQ(dc.Get().Float, 43.0);
        CHECK_EQ(dc.Get().String, "Hello Worl");
    }

    SUBCASE("Integrity")
    {
        {
            rsp::posix::FileIO f(cFileName, std::ios_base::in | std::ios_base::out);
            f.Seek(14);
            std::uint8_t b = 42;
            CHECK_EQ(f.Write(&b, 1), 1);
        }

        DataContainer<MyData> dcl;
        CHECK_THROWS_AS(dcl.Load(cFileName), EInvalidCRC);

        rsp::posix::FileSystem::DeleteFile(cFileName);
    }
}


