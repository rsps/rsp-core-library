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
#include <security/SecureBuffer.h>

using namespace rsp::utils;
using namespace rsp::logging;

struct MyData {
    float Float = 43.1f;
    int Integer = 42;
    FixedString<10> String{"Hello"};
};


TEST_CASE("Data Container")
{
    const char* cFileName = "DataContainer.bin";

    Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    DataContainer<MyData> dc(cFileName);

    SUBCASE("Init")
    {
        CHECK_EQ(sizeof(MyData), 20);
        CHECK_EQ(sizeof(dc.Get()), sizeof(MyData));

        MESSAGE(dc.Get().Integer);
        MESSAGE(dc.Get().Float);

        CHECK_EQ(dc.Get().Integer, 42);
        CHECK_EQ(dc.Get().Float, 43.1f);
        CHECK_EQ(dc.Get().String, "Hello");
    }

    SUBCASE("Save")
    {
        const char *p = "Hello World";

        dc.Get().String = p;
        dc.Get().Integer = 44;

        CHECK_EQ(dc.Get().String, "Hello Worl");

        CHECK(dc.Get().String != std::string(p));
        CHECK(dc.Get().String != p);

        CHECK_NOTHROW(dc.Save());
    }

    SUBCASE("Load")
    {
        DataContainer<MyData> dcl(cFileName);
        CHECK_NOTHROW(dcl.Load());

        CHECK_EQ(dcl.Get().Integer, 44);
        CHECK_EQ(dcl.Get().Float, 43.1f);
        CHECK_EQ(dcl.Get().String, "Hello Worl");
    }

    SUBCASE("Assignment")
    {
        DataContainer<MyData> dcl(cFileName);
        CHECK_NOTHROW(dcl.Load());

        dc = dcl;

        CHECK_EQ(dc.Get().Integer, 44);
        CHECK_EQ(dc.Get().Float, 43.1f);
        CHECK_EQ(dc.Get().String, "Hello Worl");
    }

    SUBCASE("Integrity")
    {
        TestHelpers::TamperWithFile(cFileName, 14, 42);

        DataContainer<MyData> dcl(cFileName);
        CHECK_THROWS_AS(dcl.Load(), EInvalidSignature);

        rsp::posix::FileSystem::DeleteFile(cFileName);
    }
}


