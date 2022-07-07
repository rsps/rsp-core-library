/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_CURLSESSION_H_
#define SRC_NETWORK_CURL_CURLSESSION_H_

#include <vector>
#include <network/IHttpSession.h>
#include "MultiCurl.h"
#include "CurlHttpRequest.h"

namespace rsp::network::curl {

class CurlSession: public rsp::network::IHttpSession
{
public:
    void Execute() override;
    rsp::network::HttpRequestOptions& GetDefaultOptions() override;
    rsp::network::IHttpRequest& MakeRequest() override;
    rsp::network::IHttpRequest& MakeRequest(const rsp::network::HttpRequestOptions &arOptions) override;

protected:
    MultiCurl mMulti{};
    std::vector<CurlHttpRequest> mRequests{};
    rsp::network::HttpRequestOptions mDefaultOptions{};
};

} /* namespace rsp::network::curl */

#endif /* SRC_NETWORK_CURL_CURLSESSION_H_ */
