/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_AVERAGE_H
#define RSP_CORE_LIB_INCLUDE_UTILS_AVERAGE_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <exceptions/CoreException.h>

namespace rsp::utils {

/**
 * \brief Template class to calculate sum and average of a set of values.
 * \tparam T Type of value
 * \tparam N Maximum number of contained values
 */
template<class T, size_t N>
class Average
{
public:
    /**
     * \brief Clear the internal buffer
     */
    void Clear()
    {
        mBuffer.fill(T{});
        mIndex = 0;
        mCount = 0;
    }

    /**
     * \brief Add a value to the container. If container is full, then drop oldest value (like in Fifo).
     * \param aValue
     * \return Average of contained values
     */
    T Add(T aValue)
    {
        mBuffer[mIndex++] = aValue;
        if (mIndex >= N) {
            mIndex = 0;
        }
        if (mCount < N) {
            mCount++;
        }
        return (Sum() / T(mCount));
    }

    /**
     * \brief Get the sum of the contained values
     * \return Sum of contained values
     */
    T Sum() const
    {
        T result{};
        for (size_t i = 0; i < mCount; i++) {
            result += mBuffer[i];
        }
        return result;
    }

    /**
     * \brief Get the number of contained values
     * \return Count of values
     */
    size_t Count()
    {
        return mCount;
    }

    /**
     * \brief Get the average of the contained values. If the container is empty, an exception is thrown.
     * \return Average value
     */
    T Get() const
    {
        if (mCount == 0) {
            THROW_WITH_BACKTRACE1(rsp::exceptions::AssertException, "Calculating Average have no values");
        }
        return Sum() / T(mCount);
    }

    /**
     * \brief Alias of Get()
     * \see Average::Get()
     * \return Average value
     */
    explicit operator T() const { return Get(); }

protected:
    size_t mIndex = 0;
    size_t mCount = 0;
    std::array<T, N> mBuffer{};
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_AVERAGE_H
