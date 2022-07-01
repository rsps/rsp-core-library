/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CONNECTIONOPTIONS_H
#define CONNECTIONOPTIONS_H

#include <iostream>
#include <string>

namespace rsp::network {

class ConnectionOptions
{
public:
    std::string BaseUrl = "";
    int ConnectionTimeoutMs = 5000;
    int OtherTimeoutMs = 30000;
    std::string CertType = "";
    std::string CertCaPath = "/";
    std::string CertKeyPath = "/";
    std::string CertPass = "";
};

std::ostream& operator<<(std::ostream &o, const ConnectionOptions &arOptions);

} //namespace rsp::network

#endif
