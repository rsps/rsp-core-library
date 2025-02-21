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
#include <utils/Base64.h>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

TEST_CASE("Base64")
{
    std::string_view cHello = "Hello World!";
    std::string enc;

    CHECK_NOTHROW(enc = Base64::Encode(cHello));
    CHECK_EQ(Base64::Decode(enc), cHello);
}

TEST_SUITE_END();
