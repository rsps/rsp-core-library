/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <network/HttpRequestOptions.h>

namespace rsp::network {


std::ostream& operator<<(std::ostream &o, HttpRequestType aType)
{
    const char *cmap[int(HttpRequestType::__count__)] = {
        "NONE",
        "GET",
        "POST",
        "PUT",
        "HEAD",
        "DELETE"
    };

    return o << cmap[int(aType)];
}

std::ostream& operator<<(std::ostream &o, const HttpRequestOptions &arOptions)
{
    o << *static_cast<const ConnectionOptions*>(&arOptions) << "\n"
        "Uri:  " << arOptions.Uri << "\n"
        "Request Type: " << arOptions.RequestType << "\n"
        "Basic Auth User: " << arOptions.BasicAuthUsername << "\n"
        "Basic Auth Pw: " << arOptions.BasicAuthPassword << "\n"
        "Headers:\n";

    for(auto &tuple : arOptions.Headers) {
        o << "  " << tuple.first << ": " << tuple.second << "\n";
    }

    o <<
        "Body size: " << arOptions.Body.size();

    return o;
}

} // namespace rsp::network

