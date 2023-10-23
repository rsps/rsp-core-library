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
#include <utils/Crc32.h>

TEST_CASE("CRC32") {

    const std::string cString = "The red-brown lazy fox jumped over the chicken fence.";
    const uint32_t cCorrectCRC = 0x1C5D8B51; // Verified result from: https://www.functions-online.com/crc32.html

    SUBCASE("Full buffer CRC") {
        CHECK_EQ(rsp::utils::Crc32::Calc(cString.data(), cString.size()), cCorrectCRC);
    }

    SUBCASE("Incremental CRC") {
        rsp::utils::Crc32 crc;

        for (auto c : cString) {
            crc.Add(static_cast<uint8_t>(c));
        }

        CHECK_THROWS_AS(bool a = crc.Verify(~cCorrectCRC, true), const rsp::utils::ECrcError&);
        CHECK(crc.Verify(cCorrectCRC, false));
    }
}


