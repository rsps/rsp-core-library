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
#include <utils/StrUtils.h>
#include <magic_enum.hpp>

namespace rsp::network {


std::ostream& operator<<(std::ostream &o, HttpRequestType aType)
{
    return o << magic_enum::enum_name<HttpRequestType>(aType);
}

std::ostream& operator<<(std::ostream &o, const HttpRequestOptions &arOptions)
{
    o << *static_cast<const ConnectionOptions*>(&arOptions) << "\n" <<
        arOptions.RequestType << " " << arOptions.Uri << "\n";
    if (!arOptions.BasicAuthUsername.empty()) {
        o <<
            "Basic Auth User: " << arOptions.BasicAuthUsername << "\n"
            "Basic Auth Pw: " << arOptions.BasicAuthPassword << "\n";
    }
    o << "Headers:\n";

    for(auto &tuple : arOptions.Headers) {
        o << "  " << tuple.first << ": " << tuple.second << "\n";
    }

    o <<
        "Body:\n" << arOptions.Body;

    return o;
}

} // namespace rsp::network

