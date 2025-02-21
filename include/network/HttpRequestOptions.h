/**
 * \copyright    Copyright 2022-2025 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Steffen Brummer
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H
#define RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H

#include <network/ConnectionOptions.h>
#include <network/IStreamDataProvider.h>
#include <memory>
#include <string>
#include <map>
#include <posix/FileIO.h>
#include <optional>

namespace rsp::network {

/**
 * \enum HttpRequestType
 * \brief Enumeration of the supported requests types.
 */
enum class HttpRequestType
{
    NONE, GET, POST, PUT, HEAD, PATCH, DELETE
};

/**
 * \fn std::ostream operator <<&(std::ostream&, HttpRequestType)
 * \brief Output streaming of the request types
 *
 * \param o
 * \param aType
 * \return
 */
std::ostream& operator<<(std::ostream &o, HttpRequestType aType);

using HttpBody_t = std::shared_ptr<IStreamDataProvider>;

/**
 * \class HttpRequestOptions
 * \brief Common Options for a HttpRequest, inherits connection options.
 */
class HttpRequestOptions: public ConnectionOptions
{
public:
    std::map<std::string, std::string> Headers{};
    std::string Uri{};
    HttpBody_t RequestBody{};
    HttpBody_t ResponseBody{};
    HttpRequestType RequestType = HttpRequestType::GET;
    std::string BasicAuthUsername{};
    std::string BasicAuthPassword{};

    void Clear() {
        Headers.clear();
        Uri.clear();
        RequestBody.reset();
        ResponseBody.reset();
    }
};

std::ostream& operator<<(std::ostream &o, const HttpRequestOptions &arOptions);

} // namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H