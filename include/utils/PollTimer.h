/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_POLL_TIMER_H
#define RSP_CORE_LIB_INCLUDE_UTILS_POLL_TIMER_H

#include <chrono>

namespace rsp::utils {

/**
 * \brief Simple polled timer, for easy use in run loops and polled state machines, that needs to pause for a while.
 *   Call IsExpired to check for timeout.
 */
class PollTimer
{
public:
    /**
     * \brief Construct unset timer.
     */
    PollTimer() : mExpireTime{} {}
    /**
     * \brief Construct timer set to given duration.
     * \param aDuration
     */
    explicit PollTimer(std::chrono::steady_clock::duration aDuration) : mExpireTime(std::chrono::steady_clock::now() + aDuration) {}
    /**
     * \brief Construct timer set to given duration.
     * \param aDuration
     */
    explicit PollTimer(uint32_t aDurationMilliseconds) : mExpireTime(std::chrono::steady_clock::now() + std::chrono::milliseconds(aDurationMilliseconds)) {}

    /**
     * \brief Set the timeout to occur the given duration into the future.
     * \param aDuration
     * \return self
     */
    PollTimer& Set(std::chrono::steady_clock::duration aDuration) {
        mExpireTime = (std::chrono::steady_clock::now() + aDuration);
        return *this;
    }
    /**
     * \brief Set the timeout to occur the given milliseconds into the future
     * \param aDurationMilliseconds
     * \return self
     */
    PollTimer& Set(uint32_t aDurationMilliseconds) {
        return Set(std::chrono::milliseconds(aDurationMilliseconds));
    }

    /**
     * \brief Check if the set time has passed
     * \return True if timer has expired
     */
    [[nodiscard]] bool IsExpired() const {
        return (mExpireTime < std::chrono::steady_clock::now());
    };

protected:
    std::chrono::steady_clock::time_point mExpireTime;
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_POLL_TIMER_H
