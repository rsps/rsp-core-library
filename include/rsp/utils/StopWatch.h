/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_STOP_WATCH_H
#define RSP_CORE_LIB_UTILS_STOP_WATCH_H

#include <chrono>

namespace rsp::utils
{

/**
 * \class StopWatch
 * \brief Simple class to perform execution timing to measure program performance.
 *
 */
class StopWatch
{
public:
    /**
     * \fn  StopWatch()
     * \brief Constructor that registers the start time
     */
    StopWatch() : mBegin(std::chrono::steady_clock::now()) { }

    /**
     * \fn void Reset()
     * \brief Reset the start time
     */
    void Reset() { mBegin = std::chrono::steady_clock::now(); }

    /**
     * \fn int64_t Elapsed()
     * \brief Templated lapse time lookup, template type gives the resolution of the output.
     *
     * \tparam T E.g. std::chrono::milliseconds, std::chrono::nanoseconds, std::chrono::hours
     * \return Integer count of elapsed time in given resolution.
     */
    template<typename T>
    int64_t Elapsed() {
        return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - mBegin).count();
    }

protected:
    std::chrono::steady_clock::time_point mBegin;
};

}

#endif // RSP_CORE_LIB_UTILS_STOP_WATCH_H
