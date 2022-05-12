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
#include <security/CryptBase.h>
#include <security/SecureContainer.h>
#include <utils/DataContainer.h>
#include <utils/FixedString.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::security;

struct MyData {
    std::uint32_t Integer = 42;
    FixedString<100> String{};
};

TEST_CASE("Secure Container")
{
    const char* cFileName = "SecureContainer.bin";
    const char* cPlainText = "The big red fox, jumped over the fence.";
    const char* cShaSeed = "Seed for hash";

    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    SecureContainer<MyData> sc;
    CHECK_EQ(sc.GetHeader().Flags, ContainerFlags::Extended);
    sc.SetEncryption(CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

    SUBCASE("Init")
    {
        CHECK_EQ(sizeof(sc.GetHeader()), 76);
        CHECK_EQ(sizeof(sc.Get()), sizeof(MyData));
        CHECK_EQ(sc.GetHeader().Size, 76);
        CHECK_EQ(sc.GetHeader().Flags, (ContainerFlags::Extended | ContainerFlags::Encrypted));
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

        sc.Save(cFileName, cShaSeed);
        CHECK_EQ(sc.GetHeader().PayloadCRC, 0x1269849B);
        unsigned int size = sc.GetHeader().PayloadSize;
        CHECK(size >= sizeof(MyData));
        CHECK(size <= sizeof(MyData) + 32);
    }

    SUBCASE("Load")
    {
        MESSAGE("Loading");

        SecureContainer<MyData> dcl;
        dcl.SetEncryption(CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

        CHECK_NOTHROW(dcl.Load(cFileName, cShaSeed));
        CHECK_EQ(dcl.GetHeader().Version, 1);
        CHECK_EQ(dcl.GetHeader().PayloadCRC, 0x1269849B);
        CHECK_EQ(dcl.GetHeader().PayloadSize, sizeof(MyData));

        MESSAGE("Integer: " << dcl.Get().Integer);
        MESSAGE("String: " << std::string(dcl.Get().String));

        CHECK_EQ(dcl.Get().Integer, 44);
        CHECK(dcl.Get().String == cPlainText);
    }

    SUBCASE("CRC Integrity") {
        /*
         * Tamper with container file content
         */
        auto old = TestHelpers::TamperWithFile(cFileName, sizeof(sc.GetHeader())+1, 0x55);
        SecureContainer<MyData> dcl;
        dcl.SetEncryption(CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));
        CHECK_THROWS_AS(dcl.Load(cFileName, cShaSeed), EInvalidCRC);

        TestHelpers::TamperWithFile(cFileName, sizeof(sc.GetHeader())+1, old);
        CHECK_NOTHROW(dcl.Load(cFileName, cShaSeed));
    }

    SUBCASE("Signature Integrity") {
        /*
         * Tamper with container file: CRC takes precedence, so invalidate signature directly
         */
        auto old = TestHelpers::TamperWithFile(cFileName, 0x10, 0x58);
        SecureContainer<MyData> dcl;
        dcl.SetEncryption(CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));
        CHECK_THROWS_AS(dcl.Load(cFileName, cShaSeed), EInvalidSignature);

        TestHelpers::TamperWithFile(cFileName, 0x10, old);
        CHECK_NOTHROW(dcl.Load(cFileName, cShaSeed));
    }

    SUBCASE("Header Integrity") {
        /*
         * Tamper with container file header: Clear encrypted flag
         */
        auto old = TestHelpers::TamperWithFile(cFileName, 0x01, 0x01);
        SecureContainer<MyData> dcl;
        dcl.SetEncryption(CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));
        CHECK_THROWS_AS(dcl.Load(cFileName, cShaSeed), EInvalidHeader);

        TestHelpers::TamperWithFile(cFileName, 0x01, old);
        CHECK_NOTHROW(dcl.Load(cFileName, cShaSeed));
    }

}


