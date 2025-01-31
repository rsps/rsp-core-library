/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_CHUNKED_DATA_PROVIDER_INTERFACE_H
#define RSP_CORE_LIB_CHUNKED_DATA_PROVIDER_INTERFACE_H

#include <spanstream>
#include <string>

namespace rsp::network {

/**
 * \brief Interface for data containers with content that can only be delivered in chunks bigger than on byte.
 *        This allows for low memory usage during transmission of large body content.
 *        Body content can be encoded and output in smaller chunks.
 */
class IChunkedDataProvider
{
public:
    virtual ~IChunkedDataProvider() = default;

    /**
     * \brief Fill the given buffer with the next chunk of data.
     *        This function must be able to correctly handle a buffer size as small as 1 byte.
     * \param aBuffer Span of the buffer to fill
     * \param arBytesWritten Size of chunk written to buffer
     * \param arChunkIndex Zero initialized index to delivered chunks, entirely controlled by implementation to keep function const.
     * \param arPayloadIndex Zero initialized sub index to delivered chunk, entirely controlled by implementation to keep function const.
     * \return True if entire body has been written
     */
    [[nodiscard]] virtual bool GetChunk(std::span<char> aBuffer, size_t &arBytesWritten, size_t &arChunkIndex, size_t &arPayloadIndex) = 0;
    /**
     * \brief Get the total size of the encoded body data
     * \return Size of encoded body
     */
    [[nodiscard]] virtual size_t GetSize() = 0;
};

std::ostream& operator<<(std::ostream &o, IChunkedDataProvider &s);

} // rsp::network

#endif //RSP_CORE_LIB_CHUNKED_DATA_PROVIDER_INTERFACE_H
