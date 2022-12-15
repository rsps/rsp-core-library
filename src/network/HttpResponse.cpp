/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <network/HttpResponse.h>
#include <iostream>
#include <stdexcept>
#include <utils/StrUtils.h>

namespace rsp::network {

std::ostream& operator<<(std::ostream &o, const IHttpResponse &arResponse)
{
    o <<
        "Headers:\n";

    for(auto &tuple : arResponse.GetHeaders()) {
        o << "  " << tuple.first << ": " << tuple.second << "\n";
    }

    o <<
        "StatusCode: " << arResponse.GetStatusCode() << "\n"
        "Body: " << arResponse.GetBody() << "\n";

    return o;
}

const std::string& HttpResponse::GetHeader(const std::string &arName) const
{
    try {
        return mHeaders.at(arName);
    }
    catch (const std::out_of_range &e) {
    }
    THROW_WITH_BACKTRACE1(EHeaderNotFound, rsp::utils::StrUtils::Format("No response header named %s was found", arName.c_str()));
}

}
