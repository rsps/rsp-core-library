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

namespace rsp::network {

/**
 * \brief Container class to convert IChunkedDataProvider objects into a byte streamable buffer
 */
class ChunkedDataControllerBase
{
public:
    explicit ChunkedDataControllerBase(rsp::utils::FifoBufferBase<char> &arFifo) : mrFiFo(arFifo) {}

    /**
     * \brief Fetch up to the "aLen" size of data from the chunked stream object. Call this repetitively until it returns true.
     * \param apBuffer Pointer to destination buffer
     * \param aLen Size of destination buffer
     * \param arBody Object with IHttpBodyStream interface
     * \return True when all data hase been fetched
     */
    bool GetData(std::span<char> aBuffer, IChunkedDataProvider &arBody);

    /**
     * \brief Clear the internal book keeping. Used in case of errors that needs to restart streaming.
     * \return self
     */
    ChunkedDataControllerBase& Clear();

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
    rsp::utils::FifoBufferBase<char> &mrFiFo;
    size_t mChunkIndex = 0;
    size_t mPayloadIndex = 0;
    size_t mWritten = 0;
};

template<size_t BUFFER_SIZE = 256>
class ChunkedDataController : public ChunkedDataControllerBase
{
public:
    ChunkedDataController() : ChunkedDataControllerBase(mFiFo) {}
protected:
    rsp::utils::FifoBuffer<char, BUFFER_SIZE> mFiFo{};
};

} // rsp::network

#endif //RSP_CORE_LIB_REQUEST_DATA_H
