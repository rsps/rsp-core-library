/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_UTILS_CLOCK_CAST_H_
#define SRC_UTILS_CLOCK_CAST_H_

#include <exceptions/CoreException.h>
#include <chrono>

namespace rsp::utils
{

/**
 * \namespace detail
 * \brief Private helper template functions
 */
namespace detail
{
    template<typename DurationT, typename ReprT = typename DurationT::rep>
    constexpr DurationT max_duration() noexcept
    {
        return DurationT { std::numeric_limits<ReprT>::max() };
    }

    template<typename DurationT>
    constexpr DurationT abs_duration(const DurationT d) noexcept
    {
        return DurationT { (d.count() < 0) ? -d.count() : d.count() };
    }
}

/**
 * \fn DstTimePointT ClockCast(const SrcTimePointT, const SrcDurationT=std::chrono::nanoseconds={100}, const int=10)
 * \brief Helper functions to convert between the different C++ clock types.
 *
 * The C++ chrono library has three different clock domains, this function helps convert
 * timestamps (TimePoint) from one domain to another.
 *
 * \see https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
 *
 * \tparam DstTimePointT
 * \tparam SrcTimePointT
 * \tparam DstDurationT
 * \tparam SrcDurationT
 * \tparam DstClockT
 * \tparam SrcClockT
 * \param tp
 * \param tolerance
 * \param limit
 * \return TimePoint in destination domain
 */
template<
    typename DstTimePointT,
    typename SrcTimePointT,
    typename DstDurationT = typename DstTimePointT::duration,
    typename SrcDurationT = typename SrcTimePointT::duration,
    typename DstClockT = typename DstTimePointT::clock,
    typename SrcClockT = typename SrcTimePointT::clock
>
DstTimePointT ClockCast(const SrcTimePointT tp,
    const SrcDurationT tolerance = std::chrono::nanoseconds { 100 },
    const int limit = 10)
{
    ASSERT(limit > 0);
    auto itercnt = 0;
    auto src_now = SrcTimePointT { };
    auto dst_now = DstTimePointT { };
    auto epsilon = detail::max_duration<SrcDurationT>();
    do {
        const auto src_before = SrcClockT::now();
        const auto dst_between = DstClockT::now();
        const auto src_after = SrcClockT::now();
        const auto src_diff = src_after - src_before;
        const auto delta = detail::abs_duration(src_diff);
        if (delta < epsilon) {
            src_now = src_before + src_diff / 2;
            dst_now = dst_between;
            epsilon = delta;
        }
        if (++itercnt >= limit)
            break;
    }
    while (epsilon > tolerance);
#ifdef GLOBAL_ITERATION_COUNTER
  GLOBAL_ITERATION_COUNTER = itercnt;
#endif
    return dst_now + (tp - src_now);
}

} // namespace rsp

#endif /* SRC_UTILS_CLOCK_CAST_H_ */
