/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2022 RSP Systems A/S. All rights reserved.
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_POSIX_CLOCK_H
#define RSP_CORE_LIB_POSIX_CLOCK_H

#include <ctime>
#include <utils/DateTime.h>

namespace rsp::posix {

/**
 * \brief Wrapper for POSIX clock (gettime/settime) functions.
 */
class Clock
{
public:
    enum class Types {
        RealTime = CLOCK_REALTIME,
        Monotonic = CLOCK_MONOTONIC, // Monotonic system-wide clock.
#ifdef CLOCK_PROCESS_CPUTIME_ID
        HighResolution = CLOCK_PROCESS_CPUTIME_ID, // High-resolution timer from the CPU.
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
        ThreadTime = CLOCK_THREAD_CPUTIME_ID, // Thread-specific CPU-time clock.
#endif
#ifdef CLOCK_MONOTONIC_RAW
        MonotonicRaw = CLOCK_MONOTONIC_RAW, // Monotonic system-wide clock, not adjusted for frequency scaling.
#endif
#ifdef CLOCK_REALTIME_COARSE
        RealTimeCoarse = CLOCK_REALTIME_COARSE, // Identifier for system-wide realtime clock, updated only on ticks.
#endif
#ifdef CLOCK_MONOTONIC_COARSE
        MonotonicCoarse = CLOCK_MONOTONIC_COARSE, // Monotonic system-wide clock, updated only on ticks.
#endif
#ifdef CLOCK_BOOTTIME
        BootTime = CLOCK_BOOTTIME, // Monotonic system-wide clock that includes time spent in suspension.
#endif
#ifdef CLOCK_REALTIME_ALARM
        RealTimeAlarm = CLOCK_REALTIME_ALARM, // Like CLOCK_REALTIME but also wakes suspended system.
#endif
#ifdef CLOCK_BOOTTIME_ALARM
        BootTimeAlarm = CLOCK_BOOTTIME_ALARM, // Like CLOCK_BOOTTIME but also wakes suspended system.
#endif
#ifdef CLOCK_TAI
        TAI = CLOCK_TAI // Like CLOCK_REALTIME but in International Atomic Time.
#endif
    };

    static void SetTime(Types aClockType, const rsp::utils::DateTime &arDT);
    static rsp::utils::DateTime GetTime(Types aClockType);
};

} /* namespace rsp::posix */

#endif // RSP_CORE_LIB_POSIX_CLOCK_H
