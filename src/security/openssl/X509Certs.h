/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_SECURITY_OPENSSL_X509CERTS_H
#define RSP_CORE_LIB_SRC_SECURITY_OPENSSL_X509CERTS_H

#include <string_view>
#define OPENSSL_NO_DEPRECATED 1
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace rsp::security {

class X509Certs
{
public:
    explicit X509Certs(SSL_CTX* apContext);
    explicit X509Certs(SSL* apConnection);

    void LoadCertificateAuthority(std::string_view aCAPem);
    void LoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem);
    void LoadServerCertificateChain(std::string_view aChainPem);

protected:
    SSL_CTX* mpContext = nullptr;
    SSL* mpConnection = nullptr;

    void ContextLoadCertificateAuthority(std::string_view aCAPem);
    void ContextLoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem);
    void ContextLoadServerCertificateChain(std::string_view aChainPem);

    void ConnectionLoadCertificateAuthority(std::string_view aCAPem);
    void ConnectionLoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem);
    void ConnectionLoadServerCertificateChain(std::string_view aChainPem);

};

} // rsp::security

#endif //RSP_CORE_LIB_SRC_SECURITY_OPENSSL_X509CERTS_H
