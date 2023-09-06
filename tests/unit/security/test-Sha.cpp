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
#include <string>
#include <security/Sha.h>
#include <security/SecureString.h>

using namespace rsp::security;

/**
 * \brief Test case for SHA implementation
 *
 * For online validation of SHA results:
 * @see https://codebeautify.org/hmac-generator
 */
TEST_CASE("SHA")
{
    const std::string cTestStr("How much wood would a woodchuck chuck, if a woodchuck would chuck wood.");

    SUBCASE("Library Version")
    {
        Sha sha(HashAlgorithms::Sha3);
        CHECK_EQ(sha.GetLibraryName(), std::string("openssl"));
        CHECK_EQ(sha.GetLibraryVersion(), std::string("3.0.2"));
    }

    SUBCASE("SHA1-HMAC")
    {
        Sha sha("MySecret", HashAlgorithms::Sha1);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        CHECK(md == "78bb1131dcdfe96b4a0eb2de9cb0d8ed75dec97f");
    }

    SUBCASE("SHA256-HMAC")
    {
        Sha sha("MySecret", HashAlgorithms::Sha256);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        CHECK(md == "3937baa5432706e916264cb43ce87c02bd4939ee2588bfbe2cec19b978ccf48d");
    }

    SUBCASE("SHA3-HMAC")
    {
        Sha sha("MySecret", HashAlgorithms::Sha3);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        /**
         * SHA3-256-HMAC is not supported by the codebautify generator. Instead it was validated using:
         * @https://www.liavaag.org/English/SHA-Generator/HMAC/
         */
        CHECK(md == "3ee16249305308e171910e27e1eb66803db4389696b49b2cd08b38e3cc2114dd");
    }

    SUBCASE("SHA1")
    {
        Sha sha(HashAlgorithms::Sha1);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        CHECK(md == "56e581dcc416215b41474b7d269f11bcbe7ab3e2");
    }

    SUBCASE("SHA256")
    {
        Sha sha(HashAlgorithms::Sha256);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        CHECK(md == "242fdbb7f8993f14f6fdad9098ca4d0f102760debbee49c76aae404073e84ed3");
    }

    SUBCASE("SHA3")
    {
        Sha sha(HashAlgorithms::Sha3);
        sha.Update(reinterpret_cast<const uint8_t*>(cTestStr.data()), cTestStr.size());
        SecureString md = sha.Get().GetHex();

        CHECK(md == "ea90f6707eaa4444a9ef4828fd67e31b43f591a487d77a5ad207399da387faea");
    }

}



