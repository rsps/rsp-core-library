/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Steffen Brummer
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_CONNECTION_OPTIONS_H
#define RSP_CORE_LIB_NETWORK_CONNECTION_OPTIONS_H

#include <ostream>
#include <string>
#include <rsp/security/SecureBuffer.h>
#include <rsp/security/SecureString.h>

namespace rsp::network {

/**
 * \class ConnectionOptions
 * \brief Options for a low level connection, including certificates for TLS encryption
 */
class ConnectionOptions
{
public:
    std::string BaseUrl{};
    long ConnectionTimeout = 5L; // In seconds
    long ResponseTimeout = 10L;  // In seconds
    std::string CertCaPath{};    // Path to CA chain PEM file
    std::string CertPath{};      // Path to client cert PEM file
    std::string KeyPath{};       // Path to client private key PEM file
    std::string KeyPasswd{};     // Password to unlock private key file
    security::SecureString CaChainPem{};    // Set directly or clear to force reload from CertCaPath
    security::SecureString ClientCertPem{}; // Set directly or clear to force reload from CertPath
    security::SecureString ClientKeyPem{};  // Set directly or clear to force reload from KeyPath
    security::SecureBuffer Nonce{};
    bool mVerifyPeer = true;
    bool Verbose = false;
};

std::ostream& operator<<(std::ostream &o, const ConnectionOptions &arOptions);

} //namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_CONNECTION_OPTIONS_H
