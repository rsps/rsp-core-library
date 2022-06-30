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

namespace rsp::network {

std::ostream& operator<<(std::ostream &arStream, IHttpResponse &resp)
{
    std::cout << "Response stream operator called" << std::endl;
    for (auto &h : resp.GetHeaders())
        arStream << "[" << h.first << ", " << h.second << "]\n";
    return arStream << "\n" << "StatusCode: " << resp.GetStatusCode();
}

}
