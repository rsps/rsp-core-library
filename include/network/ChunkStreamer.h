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

    [[nodiscard]] size_t Read(const std::span<std::byte> aBuffer) const override
    {
        auto self = const_cast<ChunkStreamer*>(this);
        size_t written = 0;
        while (written < aBuffer.size()) {
            if (mFiFo.IsEmpty()) {
                if (mDone) {
                    return written;
                }
                self->mDone = self->loadFifo();
            }
            written += self->mFiFo.Read(aBuffer.subspan(written, aBuffer.size() - written));
        }
        return written;
    }

    [[nodiscard]] std::span<std::byte> GetChunk()
    {
        loadFifo();
        return mFiFo.GetBuffer();
    };

    /**
     * \brief Clear the internal book keeping. Used in case of errors that needs to restart streaming.
     * \return self
     */
    ChunkStreamer& Clear()
    {
        mFiFo.Clear();
        mChunkIndex = 0;
        mPayloadIndex = 0;
        mDone = false;
        return *this;
    }

    ChunkStreamer& Rewind() override
    {
        Clear();
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

    [[nodiscard]] size_t GetStreamSize() const override
    {
        return mrProvider.GetSize();
    }

protected:
    IChunkedDataProvider &mrProvider;
    rsp::utils::FifoBuffer<std::byte, N> mFiFo{};
    size_t mChunkIndex = 0;
    size_t mPayloadIndex = 0;
    bool mDone = false;

    [[nodiscard]] size_t Write(const std::span<const std::byte>) override
    {
        return 0;
    }

    bool loadFifo()
    {
        mFiFo.Clear(); // Fifo is empty, now make sure buffer Head is on index 0 for maximal continuous space.
        size_t sz = 0;
        auto done = mrProvider.GetChunk(std::span(reinterpret_cast<char*>(mFiFo.GetData()), mFiFo.Free()), sz, mChunkIndex, mPayloadIndex);
        mFiFo.SetHead(sz);
        return done;
    }
};

} // rsp::network

#endif //RSP_CORE_LIB_REQUEST_DATA_H
