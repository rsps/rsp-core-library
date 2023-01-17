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

#include <ostream>
#include <string>

namespace rsp::network {

/**
 * \class ConnectionOptions
 * \brief Options for a low level connection, including certificates for TLS encryption
 */
class ConnectionOptions
{
public:
    std::string BaseUrl{};
    long ConnectionTimeout = 5L;
    long ResponseTimeout = 10L;
    std::string CertCaPath{};
    std::string CertPath{};
    std::string KeyPath{};
    std::string KeyPasswd{};
    int Verbose = 0;
};

std::ostream& operator<<(std::ostream &o, const ConnectionOptions &arOptions);

} //namespace rsp::network

#endif
