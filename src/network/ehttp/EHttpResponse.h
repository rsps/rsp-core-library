/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H

#include <map>
#include <network/HttpResponse.h>

namespace rsp::network::ehttp {

class EHttpRequest;


class EHttpResponse : public HttpResponse
{
public:
    using HttpResponse::HttpResponse;

protected:
    friend class EHttpRequest;

    void addHeader(const std::string& arKey, std::string aValue)
    {
        mHeaders[arKey] = std::move(aValue);
    }

    void setStatusCode(int aCode)
    {
        mStatusCode = StatusCodes(aCode);
    }

    [[nodiscard]] std::string& getBody()
    {
        return mBody;
    }

    void clear()
    {
        mHeaders.clear();
        mStatusCode = StatusCodes::Unknown;
        mBody.clear();
    }
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H
