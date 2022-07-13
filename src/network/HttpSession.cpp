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

namespace rsp::network {

HttpSession::HttpSession(std::size_t aSize)
    : mPimpl(MakePimpl(aSize))
{
}

void HttpSession::ProcessRequests()
{
    mPimpl->ProcessRequests();
}

IHttpSession& HttpSession::SetDefaultOptions(const HttpRequestOptions &arOptions)
{
    mPimpl->SetDefaultOptions(arOptions);
    return *this;
}

IHttpRequest& HttpSession::Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback)
{
    return mPimpl->Request(aType, aUri, aCallback);
}

} /* namespace rsp::network */
