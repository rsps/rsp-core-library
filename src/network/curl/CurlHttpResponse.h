/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef SRC_NETWORK_CURL_CURLHTTPRESPONSE_H_
#define SRC_NETWORK_CURL_CURLHTTPRESPONSE_H_

#include <network/HttpResponse.h>

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
    void addHeader(const std::string& arKey, std::string aValue)
    {
        mHeaders[arKey] = std::move(aValue);
    }
    void setStatusCode(int aCode)
    {
        mStatusCode = aCode;
    }
    [[nodiscard]] std::string& getBody() // NOLINT
    {
        return mBody;
    }
    void clear()
    {
        mHeaders.clear();
        mStatusCode = 0;
        mBody.clear();
    }


};

} /* namespace rsp::network::curl */

#endif /* SRC_NETWORK_CURL_CURLHTTPRESPONSE_H_ */
