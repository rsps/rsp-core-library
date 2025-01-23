/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <cctype>
#include <logging/BufferToStream.h>

namespace rsp::logging {

BufferToStream::BufferToStream(char *apBuffer, size_t aSize, bool aShowNewLines)
    : mpBuffer(apBuffer),
      mSize(aSize),
      mShowNewLines(aShowNewLines)
{
}

void BufferToStream::Print(std::ostream& o) const
{
    for (size_t i = 0 ; i < mSize ; ++i) {
        auto c = mpBuffer[i];
        if (std::isprint(int(c))) {
            o << c;
        }
        else if (c == '\n') {
            if (mShowNewLines) {
                o << "\\n";
            }
            o << c;
        }
        else if (c == '\r') {
            if (mShowNewLines) {
                o << "\\r";
            }
            else {
                o << '.';
            }
        }
        else {
            o << '.';
        }
    }
}

std::ostream& operator<<(std::ostream& o, const BufferToStream& bts)
{
    bts.Print(o);
    return o;
}

} // rsp::logging