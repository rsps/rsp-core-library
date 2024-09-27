/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_IHTTPBODYSTREAM_H
#define RSP_CORE_LIB_IHTTPBODYSTREAM_H

#include <string>

namespace rsp::network {

/**
 * \brief Interface to http body data.
 *        This allows for low memory usage during transmission of large body content.
 *        Body content can be encoded and output in smaller chunks.
 */
class IHttpBodyStream
{
public:
    virtual ~IHttpBodyStream() = default;

    /**
     * \brief Fill the given buffer with the next chunk of body data
     * \param apBuffer Pointer to buffer to fill
     * \param aBufferSize Size of buffer
     * \param arBytesWritten Size of chunk written to buffer
     * \param arChunkIndex Zero initialized index to delivered chunks, entirely controlled by implementation to keep function const.
     * \return True if entire body has been written
     */
    [[nodiscard]] virtual bool GetChunk(char *apBuffer, size_t aBufferSize, size_t &arBytesWritten, size_t &arChunkIndex) const = 0;
    /**
     * \brief Get the total size of the encoded body data
     * \return Size of encoded body
     */
    [[nodiscard]] virtual size_t GetSize() const = 0;

    /**
     * \brief Get up to aMaxLen characters from the body content.
     * \param aMaxLen Maximum length of returned string. If entire content does not fit, the result will be appended with 3 dots e.g. "This is content..."
     * \return string
     */
    [[nodiscard]] std::string GetString(size_t aMaxLen = 100) const
    {
        std::string result;
        result.resize(aMaxLen);
        size_t written;
        size_t chunk_index = 0;
        auto eof = GetChunk(result.data(), aMaxLen, written, chunk_index);
        if (!eof) {
            result.resize(written - 3);
            result += "...";
        }
        else {
            result.resize(written);
        }
        return result;
    }
};

} // rsp::network

#endif //RSP_CORE_LIB_IHTTPBODYSTREAM_H
