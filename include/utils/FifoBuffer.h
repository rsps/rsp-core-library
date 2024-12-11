/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_FIFO_BUFFER_H
#define RSP_CORE_LIB_FIFO_BUFFER_H

#include <array>
#include <cstring>
#include <exceptions/CoreException.h>

namespace rsp::utils {

template<class T, size_t N>
class FifoBuffer
{
public:
    void Clear()
    {
        mHead = mTail = 0;
    }

    [[nodiscard]] bool IsEmpty() const
    {
        return mHead == mTail;
    }

    [[nodiscard]] size_t Used() const
    {
        if (mTail <= mHead) {
            return mHead - mTail;
        }
        return (N - mTail) + mHead;
    }

    [[nodiscard]] size_t Free() const
    {
        if (mHead < mTail) {
            return mTail - mHead;
        }
        return (N - mHead) + mTail;
    }

    T Pop()
    {
        ASSERT(!IsEmpty())
        T result;
        Read(&result, 1);
        return result;
    }

    void Push(T aValue)
    {
        auto sz = Write(&aValue, 1);
        ASSERT(sz == 1)
    }

    size_t Read(T *apBuffer, size_t aMaxSize)
    {
        size_t size = std::min(Used(), aMaxSize);
        if (!IsEmpty()) {
            if (mHead == N) {
                std::memcpy(apBuffer, &mBuffer.at(mTail), size);
                Clear();
            }
            else if (mHead > mTail) {
                std::memcpy(apBuffer, &mBuffer.at(mTail), size);
            }
            else {
                auto sz = mBuffer.size() - mTail;
                std::memcpy(apBuffer, &mBuffer.at(mTail), sz);
                std::memcpy(&apBuffer[sz], &mBuffer.at(0), mHead);
            }
            mTail = (mTail + size) % N;
        }
        return size;
    }

    size_t Write(const T *apBuffer, size_t aSize)
    {
        size_t size = std::min(Free(), aSize);
        if (mHead == N) {
            mHead = 0;
        }
        if (mHead >= mTail) {
            auto sz = std::min(size, mBuffer.size() - mHead);
            std::memcpy(&mBuffer.at(mHead), apBuffer, sz);
            if (sz < size) {
                std::memcpy(&mBuffer.at(sz), &apBuffer[sz], size - sz);
            }
        }
        else {
            auto sz = std::min(size, mTail - mHead - 1);
            std::memcpy(&mBuffer.at(mHead), apBuffer, sz);
        }
        mHead = (mHead + size) % N;
        if (mHead == 0) {
            mHead = N;
        }
        return size;
    }

protected:
    // mIn == mOut means empty, so add one extra element for N usable slots.
    std::array<T, N> mBuffer{};
    size_t mHead = 0;
    size_t mTail = 0;
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_FIFO_BUFFER_H
