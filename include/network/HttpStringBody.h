/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_HTTPSTRINGBODY_H
#define RSP_CORE_LIB_HTTPSTRINGBODY_H

#include <network/IHttpBodyStream.h>
#include <string>

namespace rsp::network {

class HttpStringBody : public IHttpBodyStream
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
    [[nodiscard]] bool GetChunk(char *apBuffer, size_t aBufferSize, size_t &arWritten, size_t &arChunkIndex) const override;
    [[nodiscard]] size_t GetSize() const override;
protected:
    std::string mContent{};
    size_t mChunkReadIndex = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTPSTRINGBODY_H
