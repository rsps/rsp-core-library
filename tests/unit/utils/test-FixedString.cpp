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
#include <utils/FixedString.h>

using namespace rsp::utils;

TEST_CASE("Fixed String")
{
    const char *p = "Hello World";
    FixedString<10> fs;

    CHECK_EQ(sizeof(fs), 10 + 1);
    CHECK_EQ(fs.size(), 0);

    fs = p;
    CHECK_EQ(fs.size(), 10);
    CHECK_EQ(fs, "Hello Worl");

    CHECK_NE(fs, std::string(p));
    CHECK_NE(fs, p);

    std::string s = fs;

    CHECK_EQ(s, "Hello Worl");

    FixedString<8> f8(fs);
    CHECK_EQ(f8, "Hello Wo");

    FixedString<12> f12(fs);
    CHECK_EQ(f12, "Hello Worl");
}



