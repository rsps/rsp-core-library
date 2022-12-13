/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <network/HttpSession.h>
#include "CurlSession.h"

using namespace rsp::network;

IHttpSession* rsp::network::HttpSession::MakePimpl(std::size_t aSize)
{
    return new rsp::network::curl::CurlSession(aSize);
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

IHttpRequest& CurlSession::Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback)
{
    HttpRequestOptions opt = mDefaultOptions;
    opt.RequestType = aType;
    opt.Uri = aUri;

    auto req = mPool.Get();
    req->mpSession = this;
    req->SetOptions(opt);
    req->mResponseHandler = aCallback;
    mPending.push_back(req);

    return *req;
}

void CurlSession::requestCompleted(CurlSessionHttpRequest *apRequest)
{
    mPool.Put(&apRequest);
}

void CurlSessionHttpRequest::requestDone()
{
    CurlHttpRequest::requestDone();

    if (mResponseHandler) {
        mResponseHandler(mResponse);
    }
    mpSession->requestCompleted(this);
}


} /* namespace rsp::network::curl */
