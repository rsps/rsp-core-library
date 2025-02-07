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
#include <network/StringBody.h>

namespace rsp::network {

StringBody::StringBody(std::string aContent)
    : mContent(std::move(aContent))
{
}

StringBody& StringBody::Set(const std::string &arContent)
{
    *this = arContent;
    return *this;
}

const std::string& StringBody::Get() const
{
    return mContent;
}

StringBody& StringBody::operator=(const std::string &arContent)
{
    mContent = arContent;
    return *this;
}

//bool StringBody::GetChunk(std::span<char> aBuffer, size_t &arWritten, size_t &arChunkIndex, size_t &)
//{
//    mChunkReadIndex = arChunkIndex;
//    arWritten = Read(std::as_writable_bytes(aBuffer));
//    arChunkIndex += arWritten;
//    return (arChunkIndex == mContent.size());
//}
//
//size_t StringBody::GetSize()
//{
//    return mContent.size();
//}

size_t StringBody::Write(const std::span<const std::byte> aData)
{
    auto sz = aData.size();
    auto spc = std::span<const char>(reinterpret_cast<const char*>(aData.data()), sz);
    mContent += std::string(spc.data(), sz);
    return sz;
}

size_t StringBody::Read(const std::span<std::byte> aBuffer)
{
    size_t len = mContent.size() - mChunkReadIndex;
    if (len > aBuffer.size()) {
        len = aBuffer.size();
    }
    std::memcpy(aBuffer.data(), mContent.data() + mChunkReadIndex, len);
    mChunkReadIndex += len;
    return len;
}

std::optional<size_t> StringBody::GetStreamSize()
{
    return { mContent.size() };
}

} // rsp::network