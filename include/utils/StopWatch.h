/*
 * StopWatch.h
 *
 *  Created on: Jan 26, 2022
 *      Author: Steffen Brummer
 */

#ifndef INCLUDE_UTILS_STOPWATCH_H_
#define INCLUDE_UTILS_STOPWATCH_H_

#include <chrono>

namespace rsp::utils
{

class StopWatch
{
public:
    StopWatch() : mBegin(std::chrono::high_resolution_clock::now()) { }

    void Reset() { mBegin = std::chrono::high_resolution_clock::now(); }

    template<typename T>
    int64_t Elapsed() {
        return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - mBegin).count();
    }

protected:
    std::chrono::high_resolution_clock::time_point mBegin;
};

}

#endif /* INCLUDE_UTILS_STOPWATCH_H_ */
