/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/RunTime.h>

namespace rsp::utils {

std::chrono::milliseconds RunTime::mStartedAt = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch();


void RunTime::Init()
{
    mStartedAt = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch();
}

RunTime::RunTime()
    : mValue(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch())
{
}

RunTime::RunTime(const std::chrono::milliseconds &arValue)
    : mValue(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch() + arValue)
{
}

bool RunTime::operator <=(const RunTime &arOther)
{
    return mValue <= arOther.mValue;
}

bool RunTime::operator <(const RunTime &arOther)
{
    return mValue < arOther.mValue;
}

int64_t RunTime::Milliseconds() const
{
    return (mValue - mStartedAt).count();
}

std::ostream& operator <<(std::ostream &os, const RunTime &arRunTime)
{
    os << arRunTime.Milliseconds();
    return os;
}

} /* namespace rsp::utils */
