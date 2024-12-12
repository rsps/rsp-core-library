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

/**
 * \class FifoBuffer
 * \brief FIFO implementation with chunked read and write capability.
 *        The internal memory structure is continuous, so direct C style buffer pointer access is also possible.
 *        This class can be used as proxy for chunked data to true streaming.
 * \tparam T Type of fifo elements
 * \tparam N Number of elements in fifo
 */
template<class T, size_t N>
class FifoBuffer
{
public:
    /**
     * \brief Get a pointer to the first element in the buffer
     * \return Pointer of type T
     */
    T* GetData()
    {
        ASSERT(mHead == 0)
        ASSERT(mTail == 0)
        return mBuffer.data();
    }

    /**
     * \brief Set the internal head index. Use this after directly writing data to buffer memory.
     * \param aHead
     * \return
     */
    FifoBuffer& SetHead(size_t aHead)
    {
        ASSERT(mHead == 0)
        ASSERT(mTail == 0)
        mHead = aHead;
        return *this;
    }

    /**
     * \brief Set the internal head an tail pointers to 0.
     */
    void Clear()
    {
        mHead = mTail = 0;
        mFull = false;
    }

    /**
     * \brief Check if the buffer holds any content
     * \return true if empty
     */
    [[nodiscard]] bool IsEmpty() const
    {
        return mHead == mTail && !mFull;
    }

    /**
     * \brief Get the number of stored elements in buffer
     * \return size
     */
    [[nodiscard]] size_t Used() const
    {
        if (mFull) {
            return N;
        }
        if (mTail <= mHead) {
            return mHead - mTail;
        }
        return (N - mTail) + mHead;
    }

    /**
     * \brief Get the number of free elements in buffer
     * \return size
     */
    [[nodiscard]] size_t Free() const
    {
        if (mFull) {
            return 0;
        }
        if (mHead < mTail) {
            return mTail - mHead;
        }
        return (N - mHead) + mTail;
    }

    /**
     * \brief Pop the first element of the buffer
     * \return T
     */
    T Pop()
    {
        ASSERT(!IsEmpty())
        T result;
        Read(&result, 1);
        return result;
    }

    /**
     * \brief Push the given element to the end of the buffer
     * \param aValue Value of type T
     */
    void Push(T aValue)
    {
        auto sz = Write(&aValue, 1);
        ASSERT(sz == 1)
    }

    /**
     * \brief Read up to max size elements from the buffer
     * \param apBuffer Pointer to continuous memory block able to hold aMaxSize elements
     * \param aMaxSize Number of elements the buffer can hold
     * \return Number of elements stored in given buffer
     */
    size_t Read(T *apBuffer, size_t aMaxSize)
    {
        size_t size = std::min(Used(), aMaxSize);
        if (IsEmpty()) {
            return size;
        }

        if (mHead > mTail) {
            std::memcpy(apBuffer, &mBuffer.at(mTail), size);
        }
        else {
            auto sz = std::min(size, mBuffer.size() - mTail);
            std::memcpy(apBuffer, &mBuffer.at(mTail), sz);
            if (size > sz) {
                sz = std::min(mHead, size - sz);
                std::memcpy(&apBuffer[sz], &mBuffer.at(0), sz);
            }
        }
        mTail = (mTail + size) % N;
        mFull = false;
        return size;
    }

    /**
     * \brief Copy the given number of elements from given buffer to this fifo.
     * \param apBuffer Pointer to continuous memory region with elements.
     * \param aSize Number of elements to copy
     * \return Number of elements written (Can be smaller than given size, if fifo does not have enough free slots.
     */
    size_t Write(const T *apBuffer, size_t aSize)
    {
        size_t size = std::min(Free(), aSize);
        if (size == 0) {
            return 0;
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
        mFull = (mHead == mTail);
        return size;
    }

protected:
    std::array<T, N> mBuffer{};
    size_t mHead = 0;
    size_t mTail = 0;
    bool mFull = false;
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_FIFO_BUFFER_H
