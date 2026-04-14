/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/localization/Translation.h>
#include <doctest.h>
#include <locale>
#include <stdexcept>
#include <string_view>
#include <string>

using namespace rsp::localization;

TEST_SUITE_BEGIN("Localization");

TEST_CASE("Translation")
{
    CHECK_NOTHROW(Translation tmp;);

    try {
        std::locale{"da_DK.utf8"};
    }
    catch (const std::runtime_error&) {
        MESSAGE("Skipping, locale da_DK not installed");
        return;
    }

    Translation tr("da_DK.utf8");

    CHECK_EQ(tr("Hello World"), "Hello World");

    std::string_view sv("Hello World 1");
    CHECK_EQ(tr(sv), "Hello World 1");

    std::string s("Hello World 2");
    CHECK_EQ(tr(s), "Hello World 2");
}

TEST_SUITE_END();
