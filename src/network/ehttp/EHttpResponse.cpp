/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "EHttpResponse.h"

namespace rsp::network::ehttp {

const std::map<std::string, std::string>& EHttpResponse::GetHeaders() const
{
    return mHeaders;
}

const std::string& EHttpResponse::GetHeader(const std::string& arName) const
{
    return mHeaders.at(arName);
}

StatusCodes EHttpResponse::GetStatusCode() const
{
    return mStatusCode;
}

const IHttpRequest& EHttpResponse::GetRequest() const
{
    return dynamic_cast<rsp::network::IHttpRequest&>(mrRequest);
}

const std::string& EHttpResponse::GetBody() const
{
    return mBody;
}

} // rsp::network::ehttp