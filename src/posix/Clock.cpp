/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2022 RSP Systems A/S. All rights reserved.
 * \author      Steffen Brummer
 */

#include <rsp/exceptions/CoreException.h>
#include <rsp/posix/Clock.h>
#include <utility>

using namespace rsp::utils;

namespace rsp::posix {

void Clock::SetTime(Types aClockType, const rsp::utils::DateTime& arDT)
{
    timespec ts = arDT.GetTimeSpec();
    int res = clock_settime(std::to_underlying(aClockType), &ts);
    if (res < 0) {
        THROW_SYSTEM("Failed to set system clock");
    }
}

DateTime Clock::GetTime(Types aClockType)
{
    timespec ts{};
    int res = clock_gettime(std::to_underlying(aClockType), &ts);
    if (res < 0) {
        THROW_SYSTEM("Failed to get system clock");
    }
    return DateTime(ts);
}

} // namespace rsp::posix
