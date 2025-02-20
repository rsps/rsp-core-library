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

class StringBody : public IStreamDataProvider
{
public:
    StringBody() = default;
    explicit StringBody(std::string aContent);

    StringBody& Set(const std::string &arContent);
    [[nodiscard]] const std::string& Get() const;
    StringBody& operator=(const std::string &arContent);

    [[nodiscard]] size_t Write(std::span<const std::byte> aData) override;
    [[nodiscard]] size_t Read(std::span<std::byte> aBuffer) override;
    std::optional<size_t> GetStreamSize() override;

protected:
    std::string mContent{};
    size_t mReadOffset = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTP_STRING_BODY_H
