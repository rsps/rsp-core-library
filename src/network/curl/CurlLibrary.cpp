/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <memory>
#include "CurlLibrary.h"
#include "Exceptions.h"

namespace rsp::network::curl {

CurlLibrary& CurlLibrary::Get()
{
    static std::unique_ptr<CurlLibrary> instance;

    if (!instance) {
        instance = std::unique_ptr<CurlLibrary>(new CurlLibrary());
    }

    return *instance;
}

CurlLibrary::CurlLibrary()
{
    auto err = curl_global_init(CURL_GLOBAL_DEFAULT );
    if (err != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "curl_global_init() failed:", err);
    }
    checkVersion();
}

CurlLibrary::~CurlLibrary()
{
    curl_global_cleanup();
}

void CurlLibrary::checkVersion()
{
    curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);
    if (d->version_num < minimumCurlVersion()) {
        THROW_WITH_BACKTRACE2(ECurlVersion, "Wrong libcurl version", d->version_num);
    }
}

} /* namespace rsp::network::http::curl */
