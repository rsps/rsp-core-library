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

template<class T, size_t N>
class Average
{
public:
    void Clear() {
        mBuffer.fill(T{});
        mIndex = 0;
        mCount = 0;
    }

    T Add(T aValue) {
        mBuffer[mIndex++] = aValue;
        if (mIndex >= N) {
            mIndex = 0;
        }
        if (mCount < N) {
            mCount++;
        }
        return (Sum() / T(mCount));
    }

    T Sum() {
        T result{};
        for (size_t i = 0; i < mCount ; i++) {
            result += mBuffer[i];
        }
        return result;
    }

    size_t Count() {
        return mCount;
    }

protected:
    size_t mIndex = 0;
    size_t mCount = 0;
    std::array<T, N> mBuffer{};
};

#endif //RSP_CORE_LIB_INCLUDE_UTILS_AVERAGE_H
