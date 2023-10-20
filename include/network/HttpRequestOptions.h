/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H
#define RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H

#include <network/ConnectionOptions.h>
#include <memory>
#include <string>
#include <map>
#include <posix/FileIO.h>
#include <utils/StructElement.h>

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

/**
 * \class HttpRequestOptions
 * \brief Common Options for a HttpRequest, inherits connection options.
 */
class HttpRequestOptions: public ConnectionOptions
{
public:
    std::map<std::string, std::string> Headers{};
    std::string Uri{};
    std::string Body{};
    HttpRequestType RequestType = HttpRequestType::GET;
    std::string BasicAuthUsername{};
    std::string BasicAuthPassword{};
    rsp::utils::StructElement<rsp::posix::FileIO*> WriteFile{};
    rsp::utils::StructElement<rsp::posix::FileIO*> ReadFile{};

    void Clear() {
        Headers.clear();
        Uri.clear();
        Body.clear();
        WriteFile.Clear();
        ReadFile.Clear();
    }
};

std::ostream& operator<<(std::ostream &o, const HttpRequestOptions &arOptions);

} // namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_HTTP_REQUEST_OPTIONS_H