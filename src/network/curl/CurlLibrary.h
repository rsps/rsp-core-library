/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_CURLLIBRARY_H_
#define SRC_NETWORK_CURL_CURLLIBRARY_H_

#include <curl/curl.h>
#include <string_view>
#include <network/NetworkLibrary.h>

namespace rsp::network::curl {

/**
 * \class CurlLibrary
 * \brief Meta object to initialize the libcurl globals and request library information.
 */
class CurlLibrary : public rsp::network::NetworkLibrary
{
public:
    ~CurlLibrary();

    static CurlLibrary& Get();

    std::string_view GetLibraryName() const override { return "libcurl"; }
    std::string_view GetVersion() const override { return mpVersionInfo->version; }
    std::string_view GetSslVersion() const override { return mpVersionInfo->ssl_version; }

protected:
    curl_version_info_data* mpVersionInfo = nullptr;

    void checkVersion();

    constexpr std::uint32_t minimumCurlVersion()
    {
        return 0x072100;
    }
private:
    CurlLibrary();
    CurlLibrary(const CurlLibrary&) = delete;
    CurlLibrary& operator=(const CurlLibrary&) = delete;
};

} /* namespace rsp::network::curl */

#endif /* SRC_NETWORK_CURL_CURLLIBRARY_H_ */
