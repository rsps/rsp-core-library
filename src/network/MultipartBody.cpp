/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <cstring>
#include <exceptions/CoreException.h>
#include <filesystem>
#include <network/FileBody.h>
#include <network/MimeTypes.h>
#include <network/MultipartBody.h>
#include <network/StringBody.h>

namespace rsp::network {


size_t MultipartBody::Write(std::span<const std::byte> aData)
{
    return 0;
}

size_t MultipartBody::Read(std::span<std::byte> aBuffer) const
{
    if (mReadPartIndex >= mParts.size()) {
        return 0;
    }

    size_t result = 0;
    while (result == 0 && mReadPartIndex < mParts.size()) {
        auto& part = const_cast<MultipartPart_t&>(mParts.at(mReadPartIndex));
        if (part.mReadIndex < part.mHeaders.size()) {
            result = std::min(part.mHeaders.size(), aBuffer.size());
            std::memcpy(aBuffer.data(), part.mHeaders.data() + part.mReadIndex, result);
            part.mReadIndex += result;
        }
        else if (part.mpBody) {
            result = part.mpBody->Read(aBuffer);
            if (result == 0) {
                const_cast<MultipartBody*>(this)->mReadPartIndex++;
            }
        }
        else {
            const_cast<MultipartBody*>(this)->mReadPartIndex++;
        }
    }

    if (mReadPartIndex == mParts.size()) {
        // Here we expect end boundary to fit in buffer...
        auto end = mBoundary.GetEndBoundary();
        ASSERT(end.size() < aBuffer.size());
        result = end.size();
        std::memcpy(aBuffer.data(), end.data(), result);
        const_cast<MultipartBody*>(this)->mReadPartIndex++;
    }

    return result;
}

size_t MultipartBody::GetStreamSize() const
{
    size_t result = 0;
    for (auto &part : mParts) {
        result += part.mHeaders.size();
        if (part.mpBody) {
            result += part.mpBody->GetStreamSize();
        }
    }
    result += mBoundary.GetEndBoundary().size();
    return result;
}

MultipartBody& MultipartBody::Add(const std::string& arName, posix::FileIO& arFile, const std::string& arContentType)
{
    return Add(arName, std::make_shared<FileBody>(arFile), arContentType);
}

MultipartBody& MultipartBody::Add(const std::string& arName, HttpBody_t apBody, const std::string& arContentType)
{
    auto &part = mParts.emplace_back();
    auto file_body = dynamic_cast<FileBody*>(apBody.get());
    if (file_body) {
        std::filesystem::path fn = file_body->Get().GetFileName();
        std::string ext = fn.extension();
        std::string c_type = arContentType;
        if (arContentType.empty()) {
            c_type = MimeTypes::GetType(ext);
        }
        part.mHeaders = mBoundary.MakeContentDisposition(arName, fn.filename(), c_type);
    }
    else {
        part.mHeaders = mBoundary.MakeContentDisposition(arName, "", arContentType);
    }
    if (apBody) {
        part.mpBody = std::move(apBody);
    }

    return *this;
}

MultipartBody& MultipartBody::Add(const std::string& arName, const std::string& arValue, const std::string &arContentType)
{
    auto &part = mParts.emplace_back();
    part.mHeaders = mBoundary.MakeContentDisposition(arName, "", arContentType);
    part.mpBody = std::make_shared<StringBody>(arValue);
    return *this;
}

} // rsp::network