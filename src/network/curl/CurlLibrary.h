/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_LIBRARY_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_LIBRARY_H

#include <curl/curl.h>
#include <string_view>
#include <network/NetworkLibrary.h>
#include <cstdint>

namespace rsp::network::curl {

/**
 * \class CurlLibrary
 * \brief Meta object to initialize the libcurl globals and request library information.
 */
class CurlLibrary : public rsp::network::NetworkLibrary
{
public:
    CurlLibrary(const CurlLibrary&) = delete;
    CurlLibrary& operator=(const CurlLibrary&) = delete;

    ~CurlLibrary() override;

    static CurlLibrary& Get();

    [[nodiscard]] std::string_view GetLibraryName() const override { return "libcurl"; }
    [[nodiscard]] std::string_view GetVersion() const override { return mpVersionInfo->version; }
    [[nodiscard]] std::string_view GetSslVersion() const override { return mpVersionInfo->ssl_version; }

protected:
    curl_version_info_data* mpVersionInfo = nullptr;

    void checkVersion();

    static constexpr uint32_t minimumCurlVersion()
    {
        return 0x072100;
    }
private:
    CurlLibrary();
};

} /* namespace rsp::network::curl */

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_LIBRARY_H
