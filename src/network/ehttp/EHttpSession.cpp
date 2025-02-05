/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "EHttpSession.h"

std::unique_ptr<rsp::network::IHttpSession> rsp::network::IHttpSession::Create(size_t aSize)
{
    return std::make_unique<rsp::network::ehttp::EHttpSession>(aSize);
}


namespace rsp::network::ehttp {

EHttpSession::EHttpSession(size_t aRequestPoolSize)
    : mPool(aRequestPoolSize)
{
}


void EHttpSession::ProcessRequests()
{
/**
 * \see https://stackoverflow.com/questions/7698488/turn-a-simple-socket-into-an-ssl-socket
 * \see https://github.com/openssl/openssl/blob/691064c47fd6a7d11189df00a0d1b94d8051cbe0/demos/ssl/cli.cpp
 * \see https://os.mbed.com/docs/mbed-os/v6.16/apis/tls-tutorial.html
 */

    for (auto r : mPending) {
        r->Execute();
        mPool.Put(*r);
    }
    mPending.clear();
}

IHttpSession& EHttpSession::SetDefaultOptions(const HttpRequestOptions& arOptions)
{
    mDefaultOptions = arOptions;
    return *this;
}

HttpRequestOptions& EHttpSession::GetDefaultOptions()
{
    return mDefaultOptions;
}

const HttpRequestOptions& EHttpSession::GetDefaultOptions() const
{
    return mDefaultOptions;
}

IHttpRequest& EHttpSession::Request(HttpRequestType aType, std::string_view aUri, IHttpSession::ResponseCallback_t aCallback)
{
    HttpRequestOptions opt = mDefaultOptions;
    opt.RequestType = aType;
    opt.Uri = aUri;

    auto &req = mPool.Get();
    req.SetOptions(opt);
    req.mResponseCallback = aCallback;
    req.mpSession = this;
    mPending.push_back(&req);

    return req;
}

} // rsp::network::ehttp