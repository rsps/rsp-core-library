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
#include <new>
#include <rsp/security/SecureBuffer.h>
#include <rsp/security/SecureString.h>

using namespace rsp::security;

TEST_SUITE_BEGIN("Security");

TEST_CASE("Secure Buffers")
{
    SUBCASE("String")
    {
        // Explicit lifetime in raw storage: the SSO buffer stays readable after destruction
        alignas(SecureString) unsigned char storage[sizeof(SecureString)];
        auto* s = new (storage) SecureString("Hello World");

        CHECK_EQ(*s, "Hello World");

        const char* p = s->data();
        CHECK_EQ(p[0], 'H');
        CHECK_EQ(p[3], 'l');
        CHECK_EQ(p[6], 'W');

        std::string unsecure = *s;

        s->~SecureString();

        CHECK_NE(p[0], 'H');
        CHECK_NE(p[3], 'l');
        CHECK_NE(p[6], 'W');

        CHECK_EQ(unsecure, "Hello World");
    }

    SUBCASE("Buffer")
    {
        SecureBuffer sb;
        sb.resize(10);
        uint8_t v = 42;
        for (auto& b : sb) {
            b = v++;
        }
        uint8_t* p = sb.data();

        CHECK_EQ(sb[0], 42);
        CHECK_EQ(sb[9], 51);
        CHECK_EQ(*sb.data(), 42);
        CHECK_EQ(p[0], 42);
        CHECK_EQ(p[9], 51);
    }

    SUBCASE("Allocator cleanse")
    {
        // Freed heap memory can't be inspected safely, so verify cleanse() on
        // live memory
        SecureAllocator<uint8_t> alloc;
        uint8_t* p = alloc.allocate(10);
        for (std::size_t i = 0; i < 10; ++i) {
            p[i] = static_cast<uint8_t>(42 + i);
        }

        alloc.cleanse(p, 10);

        for (std::size_t i = 0; i < 10; ++i) {
            CHECK_EQ(p[i], 0);
        }
        alloc.deallocate(p, 10);
    }
}

TEST_SUITE_END();
