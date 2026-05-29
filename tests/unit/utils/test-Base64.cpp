/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */
#include <doctest.h>
#include <rsp/utils/Base64.h>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

TEST_CASE("Base64")
{
    SUBCASE("Encode") {
        std::string enc;
        CHECK_NOTHROW(enc = Base64::Encode("Man"));
        CHECK_EQ(enc.size(), 4u);
        CHECK_EQ(enc, "TWFu");

        CHECK_NOTHROW(enc = Base64::Encode("Ma"));
        CHECK_EQ(enc.size(), 4u);
        CHECK_EQ(enc, "TWE=");

        CHECK_NOTHROW(enc = Base64::Encode("M"));
        CHECK_EQ(enc.size(), 4u);
        CHECK_EQ(enc, "TQ==");
    }

    SUBCASE("Decode") {
        std::string dec;
        CHECK_NOTHROW(dec = Base64::Decode("TWFu"));
        CHECK_EQ(dec.size(), 3u);
        CHECK_EQ(dec, "Man");

        CHECK_NOTHROW(dec = Base64::Decode("TWE="));
        CHECK_EQ(dec.size(), 2u);
        CHECK_EQ(dec, "Ma");

        CHECK_NOTHROW(dec = Base64::Decode("TQ=="));
        CHECK_EQ(dec.size(), 1u);
        CHECK_EQ(dec, "M");
    }

    SUBCASE("Short String") {
        std::string_view cText = "Hello World!";
        std::string s;

        CHECK_NOTHROW(s = Base64::Encode(cText));
        CHECK_EQ(s.size(), 16u);
        CHECK_EQ(s, "SGVsbG8gV29ybGQh");

        CHECK_NOTHROW(s = Base64::Decode(s));
        CHECK_EQ(s.size(), cText.size());
        CHECK_EQ(s, cText);
    }

    SUBCASE("Long String") {
        std::string_view cText(R"(
              O freddled gruntbuggly thy micturations are to me
                  As plured gabbleblochits on a lurgid bee.
              Groop, I implore thee my foonting turlingdromes.
           And hooptiously drangle me with crinkly bindlewurdles,
Or I will rend thee in the gobberwarts with my blurlecruncheon, see if I don't.
)");
        std::string s;

        CHECK_NOTHROW(s = Base64::Encode(cText));
        CHECK_EQ(s.size(), 448u);
        CHECK_EQ(s, "CiAgICAgICAgICAgICAgTyBmcmVkZGxlZCBncnVudGJ1Z2dseSB0aHkgbWljdHVyYXRpb25zIGFyZSB0byBtZQogICAgICAgICAgICAgICAgICBBcyBwbHVyZWQgZ2FiYmxlYmxvY2hpdHMgb24gYSBsdXJnaWQgYmVlLgogICAgICAgICAgICAgIEdyb29wLCBJIGltcGxvcmUgdGhlZSBteSBmb29udGluZyB0dXJsaW5nZHJvbWVzLgogICAgICAgICAgIEFuZCBob29wdGlvdXNseSBkcmFuZ2xlIG1lIHdpdGggY3JpbmtseSBiaW5kbGV3dXJkbGVzLApPciBJIHdpbGwgcmVuZCB0aGVlIGluIHRoZSBnb2JiZXJ3YXJ0cyB3aXRoIG15IGJsdXJsZWNydW5jaGVvbiwgc2VlIGlmIEkgZG9uJ3QuCg==");

        CHECK_NOTHROW(s = Base64::Decode(s));
        CHECK_EQ(s.size(), cText.size());
        CHECK_EQ(s, cText);
    }
}

TEST_SUITE_END();
