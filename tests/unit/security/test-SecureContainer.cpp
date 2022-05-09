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
#include <security/SecureContainer.h>
#include <utils/DataContainer.h>
#include <utils/FixedString.h>

using namespace rsp::utils;
using namespace rsp::security;

struct MyData {
    std::uint32_t Integer = 42;
    FixedString<100> String{};
};

TEST_CASE("Secure Container")
{
    const char* cFileName = "SecureContainer.bin";
    const char *cPlainText = "The big red fox, jumped over the fence.";

    SecureContainer<MyData> sc;

    SUBCASE("Init")
    {
        CHECK_EQ(sizeof(sc.GetHeader()), 76);
        CHECK_EQ(sizeof(sc.Get()), sizeof(MyData));
        CHECK_EQ(sc.GetHeader().Size, 76);
        CHECK_EQ(sc.GetHeader().Flags, (ContainerFlags::Extended | ContainerFlags::Encrypted | ContainerFlags::Signed));
        CHECK_EQ(sc.GetHeader().Version, 1);
        CHECK_EQ(sc.GetHeader().PayloadCRC, 0);
        CHECK_EQ(sc.GetHeader().PayloadSize, 0);

        CHECK_EQ(sc.Get().Integer, 42);
        CHECK_EQ(sc.Get().String, "");
    }

    SUBCASE("Save")
    {

        sc.Get().String = cPlainText;
        sc.Get().Integer = 44;

        MESSAGE("Integer: " << sc.Get().Integer);
        MESSAGE("String: " << std::string(sc.Get().String));
        CHECK_EQ(sc.Get().String, cPlainText);

        CHECK(sc.Get().String == std::string(cPlainText));
        CHECK(sc.Get().String == cPlainText);

        sc.Save(cFileName);
        CHECK_EQ(sc.GetHeader().PayloadCRC, 0x1269849B);
        unsigned int size = sc.GetHeader().PayloadSize;
        CHECK(size >= sizeof(MyData));
        CHECK(size <= sizeof(MyData) + 32);
    }

    SUBCASE("Load")
    {
        MESSAGE("Loading");

        SecureContainer<MyData> dcl;

        CHECK_NOTHROW(dcl.Load(cFileName));
        CHECK_EQ(dcl.GetHeader().Version, 1);
        CHECK_EQ(dcl.GetHeader().PayloadCRC, 0x1269849B);
        CHECK_EQ(dcl.GetHeader().PayloadSize, sizeof(MyData));

        MESSAGE("Integer: " << dcl.Get().Integer);
        MESSAGE("String: " << std::string(dcl.Get().String));

        CHECK_EQ(dcl.Get().Integer, 44);
        CHECK(dcl.Get().String == cPlainText);
    }
}


