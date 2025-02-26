/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_MULTIPART_BODY_H
#define RSP_CORE_LIB_MULTIPART_BODY_H

#include "IStreamDataProvider.h"
#include "HttpRequestOptions.h"
#include "MultipartBoundary.h"
#include <posix/FileIO.h>
#include <vector>

namespace rsp::network {

class MultipartBody : public IStreamDataProvider
{
public:
    [[nodiscard]] size_t Write(std::span<const std::byte> aData) override;
    [[nodiscard]] size_t Read(std::span<std::byte> aBuffer) const override;
    [[nodiscard]] size_t GetStreamSize() const override;

    MultipartBody& Add(const std::string &arName, posix::FileIO &arFile, const std::string &arContentType = std::string());
    MultipartBody& Add(const std::string &arName, HttpBody_t apBody = nullptr, const std::string &arContentType = std::string());
    MultipartBody& Add(const std::string &arName, const std::string &arValue, const std::string &arContentType = std::string());

    [[nodiscard]] const MultipartBoundary& GetBoundary() const { return mBoundary; }

protected:
    struct MultipartPart_t
    {
        std::string mHeaders{};
        HttpBody_t mpBody{};

        size_t mReadIndex = 0;
    };

    std::vector<MultipartPart_t> mParts{};
    MultipartBoundary mBoundary{};
    size_t mReadPartIndex = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_MULTIPART_BODY_H
