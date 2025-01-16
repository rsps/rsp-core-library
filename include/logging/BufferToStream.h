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

class BufferToStream
{
public:
    BufferToStream(char *apBuffer, size_t aSize, bool aShowNewLines = false);

    void Print(std::ostream &o) const;
protected:
    friend std::ostream& operator<<(std::ostream &o, const BufferToStream &bts);
    char *mpBuffer;
    size_t mSize;
    bool mShowNewLines;
};

std::ostream& operator<<(std::ostream &o, const BufferToStream &bts);

} // rsp::logging

#endif //RSP_CORE_LIB_INCLUDE_LOGGING_BUFFER_TO_STREAM_H
