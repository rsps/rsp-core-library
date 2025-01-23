/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_LOGGING_BUFFER_TO_STREAM_H
#define RSP_CORE_LIB_INCLUDE_LOGGING_BUFFER_TO_STREAM_H

#include <ostream>

namespace rsp::logging {

/**
 * \brief Stream formatter class for streaming binary content in a human readable format
 */
class BufferToStream
{
public:
    /**
     * \brief Construct the formatter object
     * \param apBuffer Pointer to binary data
     * \param aSize Size of the binary data
     * \param aShowNewLines Set to show bytes containing '\n' (10) and '\r' (13) newlines as the two literal characters
     */
    BufferToStream(char *apBuffer, size_t aSize, bool aShowNewLines = false);

    /**
     * \brief Print the buffer to the given stream
     * \param o
     */
    void Print(std::ostream &o) const;
protected:
    friend std::ostream& operator<<(std::ostream &o, const BufferToStream &bts);
    char *mpBuffer;
    size_t mSize;
    bool mShowNewLines;
};

/**
 * \brief Stream the given BufferToStream object to the given output stream.
 * \param o
 * \param bts
 * \return o
 */
std::ostream& operator<<(std::ostream &o, const BufferToStream &bts);

} // rsp::logging

#endif //RSP_CORE_LIB_INCLUDE_LOGGING_BUFFER_TO_STREAM_H
