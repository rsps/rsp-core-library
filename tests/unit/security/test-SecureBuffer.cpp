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
#include <security/SecureBuffer.h>
#include <security/SecureString.h>

using namespace rsp::security;

TEST_CASE("Secure Buffers")
{

    SUBCASE("String")
    {
        const char* p;
        std::string unsecure;

        {
            SecureString s("Hello World");

            CHECK_EQ(s, "Hello World");

            MESSAGE(s);
            unsecure = s;

            p = s.data();

            CHECK_EQ(p[0], 'H');
            CHECK_EQ(p[3], 'l');
            CHECK_EQ(p[6], 'W');
        }
        CHECK_NE(p[0], 'H');
        CHECK_NE(p[3], 'l');
        CHECK_NE(p[6], 'W');

        CHECK_EQ(unsecure, "Hello World");
    }

    SUBCASE("Buffer")
    {
        uint8_t *p;
        {
            SecureBuffer sb;
            sb.resize(10);
            uint8_t v = 42;
            for(auto &b : sb) {
                b = v++;
            }
            MESSAGE(sb);

            p = sb.data();

            CHECK_EQ(sb[0], 42);
            CHECK_EQ(sb[9], 51);
            CHECK_EQ(*sb.data(), 42);
            CHECK_EQ(p[0], 42);
            CHECK_EQ(p[9], 51);
        }
        CHECK_NE(p[0], 42);
        CHECK_NE(p[9], 51);
    }
}

