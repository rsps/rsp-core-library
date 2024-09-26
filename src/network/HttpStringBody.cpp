/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <cstring>
#include <network/HttpStringBody.h>

namespace rsp::network {

HttpStringBody::HttpStringBody(std::string aContent)
    : mContent(std::move(aContent))
{
}

HttpStringBody::HttpStringBody(const HttpStringBody &arOther)
      : mContent(arOther.mContent),
        mChunkReadIndex(arOther.mChunkReadIndex)
{
}

HttpStringBody::HttpStringBody(HttpStringBody &&arOther) noexcept
      : mContent(std::move(arOther.mContent)),
        mChunkReadIndex(arOther.mChunkReadIndex)
{
}

HttpStringBody& HttpStringBody::operator=(const HttpStringBody &arOther)
{
    if (&arOther != this) {
        mContent = arOther.mContent;
        mChunkReadIndex = arOther.mChunkReadIndex;
    }
    return *this;
}

HttpStringBody& HttpStringBody::operator=(HttpStringBody &&arOther) noexcept
{
    if (&arOther != this) {
        mContent = std::move(arOther.mContent);
        mChunkReadIndex = arOther.mChunkReadIndex;
    }
    return *this;
}

HttpStringBody& HttpStringBody::Set(const std::string &arContent)
{
    *this = arContent;
    return *this;
}

const std::string& HttpStringBody::Get() const
{
    return mContent;
}

HttpStringBody& HttpStringBody::operator=(const std::string &arContent)
{
    mContent = arContent;
    mChunkReadIndex = 0;
    return *this;
}

size_t HttpStringBody::GetChunk(char *apBuffer, size_t aBufferSize)
{
    size_t len = mContent.size() - mChunkReadIndex;
    if (len > aBufferSize) {
        len = aBufferSize;
    }
    std::memcpy(apBuffer, mContent.data() + mChunkReadIndex, len);
    mChunkReadIndex += len;
    return len;
}

size_t HttpStringBody::GetSize() const
{
    return mContent.size();
}

HttpStringBody& HttpStringBody::Reset()
{
    mChunkReadIndex = 0;
    return *this;
}

} // rsp::network