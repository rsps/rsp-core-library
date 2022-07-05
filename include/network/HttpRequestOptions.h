/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HTTPREQUESTOPTIONS_H
#define HTTPREQUESTOPTIONS_H

#include <network/ConnectionOptions.h>
#include <memory>
#include <string>
#include <map>

namespace rsp::network {

enum class HttpRequestType
{
    NONE, GET, POST, PUT, HEAD, PATCH, DELETE, __count__
};

std::ostream& operator<<(std::ostream &o, HttpRequestType aType);

class HttpRequestOptions: public ConnectionOptions
{
public:
    std::map<std::string, std::string> Headers{};
    std::string Uri{};
    std::string Body{};
    HttpRequestType RequestType = HttpRequestType::NONE;
    std::string BasicAuthUsername{};
    std::string BasicAuthPassword{};
};

std::ostream& operator<<(std::ostream &o, const HttpRequestOptions &arOptions);

}

#endif
