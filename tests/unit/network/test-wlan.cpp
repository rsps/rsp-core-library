/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <network/WLan.h>
#include <TestHelpers.h>

using namespace rsp::network;


TEST_CASE("WLAN")
{
    TestLogger logger;

    CHECK_NOTHROW(WLan());
    WLan wlan;

    SUBCASE("Scan")
    {
        auto networks = wlan.GetAvailableNetworks();
        CHECK_EQ(networks.size(), 1);
    }
}


