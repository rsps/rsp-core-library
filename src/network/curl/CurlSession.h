/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_H

#include <vector>
#include <network/IHttpSession.h>
#include <utils/ObjectPool.h>
#include "MultiCurl.h"
#include "CurlSessionHttpRequest.h"

namespace rsp::network::curl {


class CurlSession: public IHttpSession
{
public:
    explicit CurlSession(size_t aSize) : mPool(aSize) {}
    void ProcessRequests() override;
    IHttpSession& SetDefaultOptions(const HttpRequestOptions &arOptions) override;
    HttpRequestOptions& GetDefaultOptions() override { return mDefaultOptions; }
    const HttpRequestOptions& GetDefaultOptions() const override { return mDefaultOptions; }

    IHttpRequest& Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback) override;

protected:
    MultiCurl mMulti{};
    HttpRequestOptions mDefaultOptions{};
    rsp::utils::ObjectPool<CurlSessionHttpRequest> mPool;
    std::vector<CurlSessionHttpRequest*> mPending{};

private:
    friend class CurlSessionHttpRequest;
    void requestCompleted(CurlSessionHttpRequest* apRequest);
};

} /* namespace rsp::network::curl */

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_H
