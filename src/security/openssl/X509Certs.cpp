/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "BioBuffer.h"
#include <memory>
#include "X509Certs.h"
#include <posix/FileSystem.h>

namespace rsp::security {

struct InfoStackDeleter {
    void operator()(STACK_OF(X509_INFO)* x) { ::sk_X509_INFO_pop_free(x, X509_INFO_free); }
};
using X509InfoList = std::unique_ptr<STACK_OF(X509_INFO), InfoStackDeleter>;

X509Certs::X509Certs(SSL_CTX* apContext)
    : mpContext(apContext)
{
}

X509Certs::X509Certs(SSL* apConnection)
    : mpConnection(apConnection)
{
}

void X509Certs::LoadCertificateAuthority(std::string_view aCAPem)
{
    if (mpContext) {
        ContextLoadCertificateAuthority(aCAPem);
    }
    if (mpConnection) {
        ConnectionLoadCertificateAuthority(aCAPem);
    }
}

void X509Certs::LoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem)
{
    if (mpContext) {
        ContextLoadClientCertificate(aCertPem, aKeyPem);
    }
    if (mpConnection) {
        ConnectionLoadClientCertificate(aCertPem, aKeyPem);
    }
}

void X509Certs::LoadServerCertificateChain(std::string_view aChainPem)
{
    if (mpContext) {
        ContextLoadServerCertificateChain(aChainPem);
    }
    if (mpConnection) {
        ConnectionLoadServerCertificateChain(aChainPem);
    }
}

void X509Certs::ContextLoadCertificateAuthority(std::string_view aCAPem)
{
    if (rsp::posix::FileSystem::FileExists(std::string(aCAPem))) {
        int err = SSL_CTX_load_verify_file(mpContext, aCAPem.data());
//        int err = SSL_CTX_load_verify_locations(mpContext, aCAPem.data(), nullptr);
        CHK_SSL(err);
        return;
    }

    BioBuffer bio(aCAPem);
    X509_STORE *cts = SSL_CTX_get_cert_store(mpContext);
    CHK_NULL(cts);

    auto info_list = X509InfoList(PEM_X509_INFO_read_bio(bio.Get(), nullptr, nullptr, nullptr));
    CHK_NULL(info_list);

    // Iterate over all entries from the pem file, add them to the x509_store one by one
    for (int i = 0; i < sk_X509_INFO_num(info_list.get()); ++i) {
        X509_INFO* info = sk_X509_INFO_value(info_list.get(), i);
        if (info->x509) {
            X509_STORE_add_cert(cts, info->x509);
        }
        if (info->crl) {
            X509_STORE_add_crl(cts, info->crl);
        }
    }
}

void X509Certs::ContextLoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem)
{
    if (rsp::posix::FileSystem::FileExists(std::string(aCertPem))) {
        int err = SSL_CTX_use_certificate_file(mpContext, aCertPem.data(), SSL_FILETYPE_PEM);
        CHK_SSL(err);
        err = SSL_CTX_use_PrivateKey_file(mpContext, aKeyPem.data(), SSL_FILETYPE_PEM);
        CHK_SSL(err);
        return;
    }

    BioBuffer cert_bio(aCertPem);
    auto cert = PEM_read_bio_X509(cert_bio.Get(), nullptr, nullptr, nullptr);
    CHK_NULL(cert);
    int err = SSL_CTX_use_certificate(mpContext, cert);
    CHK_SSL(err);

//    X509_STORE_add_cert(SSL_CTX_get_cert_store(mpContext.get()), cert);;

    BioBuffer key_bio(aKeyPem);
    auto key = PEM_read_bio_PrivateKey(key_bio.Get(), nullptr, nullptr, nullptr);
    CHK_NULL(key);
    err = SSL_CTX_use_PrivateKey(mpContext, key);
    CHK_SSL(err);
}

void X509Certs::ContextLoadServerCertificateChain(std::string_view aChainPem)
{
    BioBuffer bio(aChainPem);

    auto info_list = X509InfoList(PEM_X509_INFO_read_bio(bio.Get(), nullptr, nullptr, nullptr));
    CHK_NULL(info_list);

    /* Iterate over contents of the PEM buffer, and add certs. */
    bool first = true;
    for (int i = 0; i < sk_X509_INFO_num(info_list.get()); i++) {
        X509_INFO *info = sk_X509_INFO_value(info_list.get(), i);
        if (info->x509) {
            /* First cert is server cert. Remaining, if any, are intermediate certs. */
            if (first) {
                first = false;

                /*
                 * Set server certificate. Note that this operation increments the
                 * reference count, which means that it is okay for cleanup to free it.
                 */
                int err = SSL_CTX_use_certificate(mpContext, info->x509);
                CHK_SSL(err);

                /* Get ready to store intermediate certs, if any. */
                SSL_CTX_clear_chain_certs(mpContext);
            }
            else {
                /* Add intermediate cert to chain. */
                int err = SSL_CTX_add0_chain_cert(mpContext, info->x509);
                CHK_SSL(err);

                /*
                 * Above function doesn't increment cert reference count. NULL the info
                 * reference to it in order to prevent it from being freed during cleanup.
                 */
                info->x509 = nullptr;
            }
        }
    }
}

void X509Certs::ConnectionLoadCertificateAuthority(std::string_view aCAPem)
{

}

void X509Certs::ConnectionLoadClientCertificate(std::string_view aCertPem, std::string_view aKeyPem)
{

}

void X509Certs::ConnectionLoadServerCertificateChain(std::string_view aChainPem)
{

}


} // rsp::security