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
#include <utils/ObjectPool.h>
#include "MultiCurl.h"
#include "CurlHttpRequest.h"

namespace rsp::network {

namespace curl {

class CurlSession;

class CurlSessionHttpRequest : public CurlHttpRequest
{
protected:
    friend class CurlSession;
    IHttpSession::ResponseCallback_t mResponseHandler{};
    CurlSession* mpSession;
    void requestDone() override;
};


class CurlSession: public IHttpSession
{
public:
    CurlSession(std::size_t aSize) : mPool(aSize) {}
    void ProcessRequests() override;
    IHttpSession& SetDefaultOptions(const HttpRequestOptions &arOptions) override;
    IHttpRequest& Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback) override;

protected:
    MultiCurl mMulti{};
    HttpRequestOptions mDefaultOptions;
    rsp::utils::ObjectPool<CurlSessionHttpRequest> mPool;

private:
    friend class CurlSessionHttpRequest;
    void requestCompleted(CurlSessionHttpRequest* apRequest);
};

} /* namespace curl */

} /* namespace rsp::network */

#endif /* SRC_NETWORK_CURL_CURLSESSION_H_ */
