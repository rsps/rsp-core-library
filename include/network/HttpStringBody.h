/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_HTTP_STRING_BODY_H
#define RSP_CORE_LIB_HTTP_STRING_BODY_H

#include <network/IChunkedDataProvider.h>
#include <network/IStreamDataProvider.h>
#include <string>

namespace rsp::network {

class HttpStringBody : public IChunkedDataProvider, public IStreamDataProvider
{
public:
    HttpStringBody() = default;
    explicit HttpStringBody(std::string aContent);
    HttpStringBody(const HttpStringBody &arOther);
    HttpStringBody(HttpStringBody &&arOther) noexcept;

    HttpStringBody& operator=(const HttpStringBody &arOther);
    HttpStringBody& operator=(HttpStringBody &&arOther) noexcept;

    HttpStringBody& Set(const std::string &arContent);
    [[nodiscard]] const std::string& Get() const;
    HttpStringBody& operator=(const std::string &arContent);
    [[nodiscard]] bool GetChunk(std::span<char> aBuffer, size_t &arWritten, size_t &arChunkIndex, size_t &arPayloadIndex) override;
    [[nodiscard]] size_t GetSize() override;

    [[nodiscard]] size_t Write(std::span<const std::byte> aData) override;
    [[nodiscard]] size_t Read(std::span<std::byte> aBuffer) override;
protected:
    std::string mContent{};
    size_t mChunkReadIndex = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTP_STRING_BODY_H
