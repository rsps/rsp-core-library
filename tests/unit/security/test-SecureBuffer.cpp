/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include "doctest.h"
#include <security/SecureBuffer.h>
#include <security/SecureString.h>

using namespace rsp::security;


TEST_CASE("Secure Buffers")
{

    SUBCASE("String")
    {
        const char* cTestStr = "Hello World";
        const char* p;

        {
            SecureString s;

            s.resize(10);
            char v = '0';
            for(auto &b : s) {
                b = v++;
            }
//            CHECK_EQ(s, cTestStr);

            MESSAGE(s);

            p = s.data();

            CHECK_EQ(p[0], '0');
            CHECK_EQ(p[3], '3');
            CHECK_EQ(p[6], '6');
        }
        CHECK_NE(p[0], '0');
        CHECK_NE(p[3], '3');
        CHECK_NE(p[6], '6');
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
            CHECK_EQ(*p, 42);
            CHECK_EQ(p[0], 42);
            CHECK_EQ(p[9], 51);
        }
        CHECK_NE(p[0], 42);
        CHECK_NE(p[9], 51);
    }
}

