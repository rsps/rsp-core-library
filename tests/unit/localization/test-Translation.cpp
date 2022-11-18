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
#include <localization/Translation.h>

using namespace rsp::localization;

TEST_CASE("Translation")
{
    CHECK_NOTHROW(Translation tmp;);

    Translation tr("da_DK.utf8");

    CHECK_EQ(tr("Hello World"), "Hello World");

}



