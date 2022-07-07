/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_MULTICURL_H_
#define SRC_NETWORK_CURL_MULTICURL_H_

#include <map>
#include <curl/curl.h>
#include <network/IHttpRequest.h>
#include "Exceptions.h"

namespace rsp::network::curl {

class MultiCurl
{
public:
    MultiCurl();
    ~MultiCurl();

    MultiCurl(const MultiCurl&) = delete;
    MultiCurl(const MultiCurl&&) = delete;
    MultiCurl& operator=(const MultiCurl&) = delete;
    MultiCurl& operator=(const MultiCurl&&) = delete;

    MultiCurl& Add(IHttpRequest *apRequest);
    MultiCurl& Remove(IHttpRequest *apRequest);

    void Execute();

protected:
    CURLM *mpMultiHandle = nullptr;

    bool poll(int aTimeoutMs);
    int perform();

    template <typename T>
    void setCurlOption(CURLMoption aOption, T aArg) {
        auto err = curl_multi_setopt(mpMultiHandle, aOption, aArg);
        if (err != CURLM_OK) {
            THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_setopt() failed:", err);
        }
    }

};

} /* namespace rsp::network::curl */

#endif /* SRC_NETWORK_CURL_MULTICURL_H_ */
