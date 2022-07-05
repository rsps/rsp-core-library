/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen, Steffen Brummer
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <network/ConnectionOptions.h>

namespace rsp::network {

std::ostream& operator <<(std::ostream &o, const ConnectionOptions &arOptions)
{
    o <<
        "BaseUrl:           " << arOptions.BaseUrl << "\n"
        "ConnectionTimeout: " << arOptions.ConnectionTimeoutMs << "ms\n"
        "OtherTimeout:      " << arOptions.OtherTimeoutMs << "ms\n"
        "CertCaPath:        " << arOptions.CertCaPath << "\n"
        "CertPath:          " << arOptions.CertPath;

        return o;
}

} // namespace rsp::network
