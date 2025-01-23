/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_HTTP_REQUEST_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_HTTP_REQUEST_H

#include "CurlHttpRequest.h"
#include <network/IHttpSession.h>

namespace rsp::network::curl {

class CurlSession;

class CurlSessionHttpRequest : public CurlHttpRequest
{
protected:
    friend class CurlSession;
    IHttpSession::ResponseCallback_t mResponseHandler{};
    CurlSession* mpSession = nullptr;
    void requestDone() override;
};

} /* namespace rsp::network::curl */

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_SESSION_HTTP_REQUEST_H
