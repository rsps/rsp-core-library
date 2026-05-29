/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_RESPONSE_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_RESPONSE_H

#include <rsp/network/HttpResponse.h>

namespace rsp::network::curl {

class CurlHttpRequest;

/**
 * \class CurlHttpResponse
 * \brief Derivation of HttpResponse, that allows the CurlHttpRequest to populate the response content
 *
 */
class CurlHttpResponse : public HttpResponse
{
public:
    using HttpResponse::HttpResponse;

protected:
    friend class CurlHttpRequest;
};

} /* namespace rsp::network::curl */

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_RESPONSE_H
