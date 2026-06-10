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
#include <rsp/exceptions/CoreException.h>
#include <span>

namespace rsp::utils {

/**
 * \class FifoBufferBase
 * \brief Adds FIFO capability to the given data buffer.
 * \tparam T Type of fifo elements
 */
template<class T>
class FifoBufferBase
{
public:
    /**
     * \brief Construct a fifo on the given buffer
     * \param aBuffer
     */
    explicit FifoBufferBase(std::span<T> aBuffer) : mBuffer(aBuffer) {}

    /**
     * \brief Get a span of the currently filled buffer.
     * \return span
     */
    std::span<T> GetBuffer()
    {
        ASSERT(mHead == 0)
        return { mBuffer.data(), mTail };
    }

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
    FifoBufferBase& SetHead(size_t aHead)
    {
        ASSERT(mHead == 0)
        ASSERT(mHead <= mBuffer.size())
        ASSERT(mTail == 0)
        if (aHead > 0) {
            mHead = aHead % mBuffer.size();
            mFull = (mHead == mTail);
        }
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
        return ((mHead == mTail) && (!mFull));
    }

    /**
     * \brief Get the number of stored elements in buffer
     * \return size
     */
    [[nodiscard]] size_t Used() const
    {
        if (mFull) {
            return mBuffer.size();
        }
        if (mTail <= mHead) {
            return mHead - mTail;
        }
        return (mBuffer.size() - mTail) + mHead;
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
        return (mBuffer.size() - mHead) + mTail;
    }

    /**
     * \brief Pop the first element of the buffer
     * \return T
     */
    T Pop()
    {
        ASSERT(!IsEmpty())
        T result;
        Read({&result, 1});
        return result;
    }

    /**
     * \brief Push the given element to the end of the buffer
     * \param aValue Value of type T
     */
    void Push(T aValue)
    {
        auto sz = Write({&aValue, 1});
        ASSERT(sz == 1)
    }

    /**
     * \brief Read up to max size elements from the buffer
     * \param apBuffer Pointer to continuous memory block able to hold aMaxSize elements
     * \param aMaxSize Number of elements the buffer can hold
     * \return Number of elements stored in given buffer
     */
    size_t Read(std::span<T> aBuffer)
    {
        size_t size = std::min(Used(), aBuffer.size());
        if (IsEmpty()) {
            return size;
        }

        if (mHead > mTail) {
            std::memcpy(aBuffer.data(), &mBuffer[mTail], size);
        }
        else {
            auto sz = std::min(size, mBuffer.size() - mTail);
            std::memcpy(aBuffer.data(), &mBuffer[mTail], sz);
            if (size > sz) {
                sz = std::min(mHead, size - sz);
                std::memcpy(aBuffer.data() + sz, mBuffer.data(), sz);
            }
        }
        mTail = (mTail + size) % mBuffer.size();
        mFull = false;
        return size;
    }

    /**
     * \brief Copy the given number of elements from given buffer to this fifo.
     * \param apBuffer Span of continuous memory region with elements.
     * \return Number of elements written (Can be smaller than given size, if fifo does not have enough free slots.
     */
    size_t Write(std::span<const T> aBuffer)
    {
        size_t size = std::min(Free(), aBuffer.size());
        if (size == 0) {
            return 0;
        }
        if (mHead >= mTail) {
            auto sz = std::min(size, mBuffer.size() - mHead);
            std::memcpy(&mBuffer[mHead], aBuffer.data(), sz);
            if (sz < size) {
                std::memcpy(&mBuffer[sz], &aBuffer[sz], size - sz);
            }
        }
        else {
            auto sz = std::min(size, mTail - mHead - 1);
            std::memcpy(&mBuffer[mHead], aBuffer.data(), sz);
        }
        mHead = (mHead + size) % mBuffer.size();
        mFull = (mHead == mTail);
        return size;
    }

private:
    std::span<T> mBuffer;
    size_t mHead = 0;
    size_t mTail = 0;
    bool mFull = false;
};

/**
 * \class FifoBuffer
 * \brief FIFO implementation with chunked read and write capability.
 *        The internal memory structure is continuous, so direct C style buffer pointer access is also possible.
 *        This class can be used as proxy for chunked data to true streaming.
 * \tparam T Type of fifo elements
 * \tparam N Number of elements in fifo
 */
template<class T, size_t N>
class FifoBuffer : public FifoBufferBase<T>
{
public:
    FifoBuffer() : FifoBufferBase<T>(mArrayBuffer) {}

protected:
    std::array<T, N> mArrayBuffer{};
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_FIFO_BUFFER_H
