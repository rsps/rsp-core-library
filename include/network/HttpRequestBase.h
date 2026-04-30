/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_HTTP_REQUEST_BASE_H
#define RSP_CORE_LIB_HTTP_REQUEST_BASE_H

#include "IHttpRequest.h"
#include "IHttpResponse.h"

namespace rsp::network {

class HttpRequestBase : public IHttpRequest
{
public:
    typedef std::function<void(IHttpResponse&)> ResponseCallback_t;

    [[nodiscard]] IHttpResponse& Execute() override;

protected:
    ResponseCallback_t mResponseCallback{};
    virtual IHttpResponse& execute() = 0;

    IHttpResponse& defaultExecute();
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTP_REQUEST_BASE_H
