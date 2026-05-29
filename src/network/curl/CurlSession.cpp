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
#include "CurlSession.h"

using namespace rsp::network;

std::unique_ptr<IHttpSession> rsp::network::IHttpSession::Create(size_t aSize)
{
    return std::make_unique<rsp::network::curl::CurlSession>(aSize);
}

namespace rsp::network::curl {

void CurlSession::ProcessRequests()
{
    for (auto p : mPending) {
        mMulti.Add(*p);
    }
    mPending.clear();

    mMulti.Execute();
}

IHttpSession& CurlSession::SetDefaultOptions(const HttpRequestOptions &arOptions)
{
    mDefaultOptions = arOptions;
    return *this;
}

IHttpRequest& CurlSession::Request(const HttpRequestType aType, const std::string_view aUri, const ResponseCallback_t aCallback)
{
    HttpRequestOptions opt = mDefaultOptions;
    opt.RequestType = aType;
    opt.Uri = aUri;

    auto &req = mPool.Get();
    req.mpSession = this;
    req.SetOptions(opt);
    req.mResponseHandler = aCallback;
    mPending.push_back(&req);

    return req;
}

void CurlSession::requestCompleted(CurlSessionHttpRequest& arRequest)
{
    mPool.Put(arRequest);
}


void CurlSessionHttpRequest::requestDone()
{
    CurlHttpRequest::requestDone();

    if (mResponseHandler) {
        mResponseHandler(mResponse);
    }
    mpSession->requestCompleted(*this);
}


} /* namespace rsp::network::curl */
