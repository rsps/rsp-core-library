/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_REQUEST_DATA_H
#define RSP_CORE_LIB_REQUEST_DATA_H

#include <utils/FifoBuffer.h>
#include "IChunkedDataProvider.h"
#include "IStreamDataProvider.h"

namespace rsp::network {

/**
 * \brief Container class to convert IChunkedDataProvider objects into a byte streamable buffer
 */
template <size_t N>
class ChunkStreamer : public IStreamDataProvider
{
public:
    explicit ChunkStreamer(IChunkedDataProvider &arProvider) : mrProvider(arProvider) {}

    [[nodiscard]] size_t Write(const std::span<const std::byte>) override
    {
        return 0;
    }

    [[nodiscard]] size_t Read(const std::span<std::byte> aBuffer) override
    {
        mWritten = 0;
        while (mWritten < aBuffer.size()) {
            if (mFiFo.IsEmpty()) {
                mFiFo.Clear(); // Fifo is empty, now make sure buffer Head is on index 0 for maximal continuous space.
                size_t sz = 0;
                auto done = mrProvider.GetChunk(std::span(reinterpret_cast<char*>(mFiFo.GetData()), mFiFo.Free()), sz, mChunkIndex, mPayloadIndex);
                if (done && sz == 0) {
                    return mWritten;
                }
                mFiFo.SetHead(sz);
            }
            mWritten += mFiFo.Read(aBuffer.subspan(mWritten, aBuffer.size() - mWritten));
        }
        return mWritten;
    }

    /**
     * \brief Clear the internal book keeping. Used in case of errors that needs to restart streaming.
     * \return self
     */
    ChunkStreamer& Clear()
    {
        mFiFo.Clear();
        mChunkIndex = 0;
        mPayloadIndex = 0;
        mWritten = 0;
        return *this;
    }

    /**
     * \brief Get current chunk index
     * \return size_t
     */
    [[nodiscard]] size_t GetChunkIndex() const { return mChunkIndex; }

    /**
     * \brief Get Current payload index (sub chunk)
     * \return size_t
     */
    [[nodiscard]] size_t GetPayloadIndex() const { return mPayloadIndex; }

    /**
     * \brief Get the amount of bytes stored in last call to GetData()
     * \return size_t
     */
    [[nodiscard]] size_t GetWritten() const { return mWritten; }
protected:
    IChunkedDataProvider &mrProvider;
    rsp::utils::FifoBuffer<std::byte, N> mFiFo{};
    size_t mChunkIndex = 0;
    size_t mPayloadIndex = 0;
    size_t mWritten = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_REQUEST_DATA_H
