/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_CURLSESSIONHTTPREQUEST_H_
#define SRC_NETWORK_CURL_CURLSESSIONHTTPREQUEST_H_

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

#endif /* SRC_NETWORK_CURL_CURLSESSIONHTTPREQUEST_H_ */
