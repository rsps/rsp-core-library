/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "CurlHttpRequest.h"

using namespace rsp::network::http;

namespace rsp::network::http::curl {

CurlHttpRequest::CurlHttpRequest() 
    : resp(*this), opt()
{

}

void rsp::network::http::curl::CurlHttpRequest::Execute(
    std::function<void(rsp::network::http::IHttpResponse&)> callback)
{
}

IHttpResponse& rsp::network::http::curl::CurlHttpRequest::Execute()
{
    return resp;
}

HttpRequestOptions& rsp::network::http::curl::CurlHttpRequest::GetOptions()
{
    return opt;
}

} // namespace rsp::network::http::curl
