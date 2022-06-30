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

namespace rsp::network::curl {

class CurlLibrary
{
public:
    ~CurlLibrary();

    static CurlLibrary& Get();

protected:
    void checkVersion();

    constexpr std::uint32_t minimumCurlVersion()
    {
        return 0x072100;
    }
private:
    CurlLibrary();
};

} /* namespace rsp::network::http::curl */

#endif /* SRC_NETWORK_CURL_CURLLIBRARY_H_ */
