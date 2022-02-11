/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_STOPWATCH_H_
#define INCLUDE_UTILS_STOPWATCH_H_

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
    StopWatch() : mBegin(std::chrono::high_resolution_clock::now()) { }

    /**
     * \fn void Reset()
     * \brief Reset the start time
     */
    void Reset() { mBegin = std::chrono::high_resolution_clock::now(); }

    /**
     * \fn int64_t Elapsed()
     * \brief Templated lapse time lookup, template type gives the resolution of the output.
     *
     * \tparam T E.g. std::chrono::millisecond, std::chrono::nanoseconds, std::chrono::hours
     * \return Integer count of elapsed time in given resolution.
     */
    template<typename T>
    int64_t Elapsed() {
        return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - mBegin).count();
    }

protected:
    std::chrono::high_resolution_clock::time_point mBegin;
};

}

#endif /* INCLUDE_UTILS_STOPWATCH_H_ */
