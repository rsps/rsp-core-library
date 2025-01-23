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
#include "IHttpBodyStream.h"

namespace rsp::network {

/**
 * \brief Gateway class to convert chunked IHttpBodyStream objects into a byte streamable buffer
 */
class RequestData
{
public:
    /**
     * \brief Fetch up to the "aLen" size of data from the chunked stream object. Call this repetitively until it returns true.
     * \param apBuffer Pointer to destination buffer
     * \param aLen Size of destination buffer
     * \param arBody Object with IHttpBodyStream interface
     * \return True when all data hase been fetched
     */
    bool GetData(char *apBuffer, size_t aLen, IHttpBodyStream &arBody);

    /**
     * \brief Clear the internal book keeping. Used in case of errors that needs to restart streaming.
     * \return self
     */
    RequestData& Clear();

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
    rsp::utils::FifoBuffer<char, 256> mFiFo{};
    size_t mChunkIndex = 0;
    size_t mPayloadIndex = 0;
    size_t mWritten = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_REQUEST_DATA_H
