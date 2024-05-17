/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_RUN_TIME_H
#define RSP_CORE_LIB_UTILS_RUN_TIME_H

#include <chrono>
#include <ostream>

namespace rsp::utils {

class RunTime
{
public:
    static void Init();

    RunTime();
    explicit RunTime(const std::chrono::milliseconds &arValue);
    RunTime(const RunTime&) = default;
    RunTime& operator=(const RunTime&) = default;

    bool operator<=(const RunTime& arOther);
    bool operator<(const RunTime& arOther);

    [[nodiscard]] int64_t Milliseconds() const;

    explicit operator std::chrono::milliseconds() { return mValue; }

protected:
    static std::chrono::milliseconds mStartedAt;
    std::chrono::milliseconds mValue;
};

std::ostream &operator<<(std::ostream &os, const RunTime &arRunTime);

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_RUN_TIME_H
