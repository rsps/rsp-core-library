/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_EXCEPTIONS_H_
#define SRC_NETWORK_CURL_EXCEPTIONS_H_

#include <string_view>
#include <curl/curl.h>
#include <network/NetworkException.h>


namespace rsp::network::curl {

/**
 * \class ECurlVersion
 *
 * \brief Exception regarding curl library version
 *
 */
class ECurlVersion: public rsp::network::NetworkException
{
public:
    explicit ECurlVersion(const std::string &aMsg, std::string_view aVersion)
        : NetworkException(aMsg)
    {
        mWhat.append(": ").append(aVersion);
    }
};

/**
 * \class ECurlError
 *
 * \brief Exception propagating curl errors
 *
 */
class ECurlError: public rsp::network::NetworkException
{
public:
    explicit ECurlError(const std::string &aMsg, CURLcode aCode = CURLE_OK)
        : NetworkException(aMsg)
    {
        if (aCode != CURLE_OK) {
            mWhat.append(" (")
                .append(std::to_string(static_cast<unsigned long>(aCode)))
                .append(") ")
                .append(curl_easy_strerror(aCode));
        }
    }
};

class ECurlMError: public rsp::network::NetworkException
{
public:
    explicit ECurlMError(const std::string &aMsg, CURLMcode aCode = CURLM_OK)
        : NetworkException(aMsg)
    {
        if (aCode != CURLM_OK) {
            mWhat.append(" (")
                .append(std::to_string(static_cast<unsigned long>(aCode)))
                .append(") ")
                .append(curl_multi_strerror(aCode));
        }
    }
};


} // rsp::network::curl

#endif /* SRC_NETWORK_CURL_EXCEPTIONS_H_ */
