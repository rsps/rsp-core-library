/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2022 RSP Systems A/S. All rights reserved.
 * \author      Steffen Brummer
 */

#include <exceptions/CoreException.h>
#include <posix/Clock.h>

using namespace rsp::utils;

namespace rsp::posix {

void Clock::SetTime(Types aClockType, const rsp::utils::DateTime &arDT)
{
    timespec ts = arDT.GetTimeSpec();
    int res = clock_settime(static_cast<int>(aClockType), &ts);
    if (res < 0) {
        THROW_SYSTEM("Failed to set system clock");
    }
}

DateTime Clock::GetTime(Types aClockType)
{
    timespec ts{};
    int res = clock_gettime(static_cast<int>(aClockType), &ts);
    if (res < 0) {
        THROW_SYSTEM("Failed to get system clock");
    }
    return {ts};
}

} /* namespace rsp::posix */
