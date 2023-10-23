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

struct MyData
{
    std::uint32_t Integer = 42;
    FixedString<100> String{};
};

TEST_CASE("Secure Container")
{
    const char* cFileName = "SecureContainer.bin";
    const char* cPlainText = "The big red fox, jumped over the fence.";
    const char* cShaSeed = "Seed for hash";

    TestLogger logger;

    SecureContainer<MyData> sc(cFileName, cShaSeed, CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

    SUBCASE("Init")
    {
        CHECK_EQ(sizeof(sc.Get()), sizeof(MyData));
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

        CHECK_EQ(sc.Get().String, std::string(cPlainText));
        CHECK_EQ(sc.Get().String, cPlainText);

        CHECK_NOTHROW(sc.Save());
    }

    SUBCASE("Load")
    {
        MESSAGE("Loading");

        SecureContainer<MyData> dcl(cFileName, cShaSeed, CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

        CHECK_NOTHROW(dcl.Load());

        MESSAGE("Integer: " << dcl.Get().Integer);
        MESSAGE("String: " << std::string(dcl.Get().String));

        CHECK_EQ(dcl.Get().Integer, 44);
        CHECK_EQ(dcl.Get().String, cPlainText);
    }

    SUBCASE("Encryption Integrity") {
        SecureContainer<MyData> dcl(cFileName, cShaSeed, CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

        MESSAGE("Change encryption bit");
        /*
         * Tamper with container file: Change a single bit in encrypted data.
         */
        auto bitmask = TestHelpers::TamperWithFile(cFileName, dcl.GetSignature().GetSize() + 3, 0x00);
        bitmask ^= 0x20;
        TestHelpers::TamperWithFile(cFileName, dcl.GetSignature().GetSize() + 3, bitmask);

        CHECK_THROWS_AS(dcl.Load(), EInvalidSignature);

        bitmask ^= 0x20;
        TestHelpers::TamperWithFile(cFileName, dcl.GetSignature().GetSize() + 3, bitmask);
        CHECK_NOTHROW(dcl.Load());
    }

    SUBCASE("Signature Integrity") {
        /*
         * Tamper with container file: Cryptation takes precedence, so invalidate signature directly
         */
        auto old = TestHelpers::TamperWithFile(cFileName, 0x03, 0x58);
        SecureContainer<MyData> dcl(cFileName, cShaSeed, CryptBase::KeyGen("InitVector"), CryptBase::KeyGen("username:password"));

        CHECK_THROWS_AS(dcl.Load(), EInvalidSignature);

        TestHelpers::TamperWithFile(cFileName, 0x03, old);
        CHECK_NOTHROW(dcl.Load());
    }

}


