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

namespace rsp::network {

IHttpSession* HttpSession::MakePimpl()
{
    return new curl::CurlSession();
}

} /* namespace rsp::network */

namespace rsp::network::curl {

void CurlSession::Execute()
{
    mMulti.Execute();
}

rsp::network::HttpRequestOptions& CurlSession::GetDefaultOptions()
{
    return mDefaultOptions;
}

rsp::network::IHttpRequest& CurlSession::MakeRequest()
{
    return MakeRequest(mDefaultOptions);
}

rsp::network::IHttpRequest& CurlSession::MakeRequest(const rsp::network::HttpRequestOptions &arOptions)
{
    auto &req = mRequests.emplace_back();
    req.SetOptions(arOptions);
    mMulti.Add(&req);
    return req;
}

} /* namespace rsp::network::curl */
