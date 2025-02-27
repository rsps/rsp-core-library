/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "TLSSocket.h"
#include <network/UrlParser.h>

using namespace rsp::network;

namespace rsp::security {

std::shared_ptr<ITLSSocket> ITLSSocket::Create(const network::ConnectionOptions& arOptions)
{
    return std::make_shared<TLSSocket>(arOptions);
}

TLSSocket::tlsRandom::tlsRandom(tlsEntropy& arEntropy, const security::SecureBuffer &arNonce)
    : mbedtls_ctr_drbg_context()
{
    mbedtls_ctr_drbg_init(this);
    CHK_0(mbedtls_ctr_drbg_seed(this, mbedtls_entropy_func, &arEntropy, arNonce.data(), arNonce.size()));
}

TLSSocket::TLSSocket(const network::ConnectionOptions& arOptions)
    : NamedLogChannel("MbedTLS-TLSSocket"),
      mrOptions(arOptions),
      mRandom(mEntropy, mrOptions.Nonce)
{
    CHK_0(mbedtls_ssl_config_defaults(&mConfig, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT));
    mbedtls_ssl_conf_authmode( &mConfig, MBEDTLS_SSL_VERIFY_REQUIRED );
    mbedtls_ssl_conf_rng( &mConfig, mbedtls_ctr_drbg_random, &mRandom );
    mbedtls_ssl_conf_dbg( &mConfig, debugLog, &mLogger );

    if (!mrOptions.CertCaPath.empty()) {
        CHK_0(mbedtls_x509_crt_parse_file(&mCaChain, mrOptions.CertCaPath.c_str()));
        mbedtls_ssl_conf_ca_chain(&mConfig, &mCaChain, nullptr);
    }

    if (!mrOptions.CertPath.empty()) {
        CHK_0(mbedtls_x509_crt_parse_file(&mClientCert, mrOptions.CertPath.c_str()));
        CHK_0(mbedtls_pk_parse_keyfile(&mPrivateKey, mrOptions.KeyPath.c_str(), mrOptions.KeyPasswd.c_str(), &rng_get, &mRandom));
        CHK_0(mbedtls_ssl_conf_own_cert(&mConfig, &mClientCert, &mPrivateKey));
    }

    CHK_0(mbedtls_ssl_setup(&mSsl, &mConfig));

    UrlParser up(mrOptions.BaseUrl);
    CHK_0(mbedtls_ssl_set_hostname(&mSsl, std::string(up.GetHost()).c_str()));

    CHK_0(psa_crypto_init());
}

TLSSocket& TLSSocket::SetSocket(posix::Socket& arSocket)
{
    mNet.fd = arSocket.GetFd();
    mbedtls_ssl_set_bio( &mSsl, &mNet, mbedtls_net_send, mbedtls_net_recv, nullptr);

    CHK_0(mbedtls_net_set_block(&mNet));

    int ret;
    while ((ret = mbedtls_ssl_handshake(&mSsl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
            ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED ||
                ret == MBEDTLS_ERR_SSL_BAD_CERTIFICATE) {
                mLogger.Error() <<
                        "    Unable to verify the server's certificate. "
                        "Either it is invalid,\n"
                        "    or you didn't set ca_file or ca_path "
                        "to an appropriate value.\n"
                        "    Alternatively, you may want to use "
                        "auth_mode=optional for testing purposes if "
                        "not using TLS 1.3.\n"
                        "    For TLS 1.3 server, try `ca_path=/etc/ssl/certs/`"
                        "or other folder that has root certificates\n";

                auto flags = mbedtls_ssl_get_verify_result(&mSsl);
                char vrfy_buf[512];
                mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
                mLogger.Error() << vrfy_buf;
            }
#endif

            THROW_WITH_BACKTRACE2(EMbedTLSError, "mbedtls_ssl_handshake", ret);
        }
    }

    return *this;
}

TLSSocket& TLSSocket::Close()
{
    // Todo: This entire object should be destroyed here...
    return *this;
}

size_t TLSSocket::Write(std::span<const std::byte> aData)
{
    size_t write_bytes = 0;
    while(write_bytes < aData.size()) {
        auto ret = mbedtls_ssl_write(&mSsl, reinterpret_cast<const unsigned char*>(aData.data() + write_bytes), aData.size() - write_bytes);
        if (ret <= 0) {
            THROW_WITH_BACKTRACE2(EMbedTLSError, "mbedtls_ssl_write", ret);
        }
        else {
            write_bytes += size_t(ret);
        }
    }
    return write_bytes;
}

size_t TLSSocket::Read(std::span<std::byte> aData)
{
    size_t read_bytes = 0;
    int ret;
    while(read_bytes < aData.size()) {
        ret = mbedtls_ssl_read(&mSsl, reinterpret_cast<unsigned char*>(aData.data()), aData.size());

        if (ret <= 0) {
            switch (ret) {
                case MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS:
                    mLogger.Info() << "got crypto in progress";
                    continue;

                case MBEDTLS_ERR_SSL_WANT_READ:
                case MBEDTLS_ERR_SSL_WANT_WRITE:
                    continue;

                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    mLogger.Info() << "connection was closed gracefully";
                    return read_bytes;

                case 0:
                case MBEDTLS_ERR_NET_CONN_RESET:
                    THROW_WITH_BACKTRACE2(EMbedTLSError, " connection was reset by peer", ret);

                case MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
                    /* We were waiting for application data but got
                     * a NewSessionTicket instead. */
                    mLogger.Info() << "got new session ticket";
                    continue;

                default:
                    THROW_WITH_BACKTRACE2(EMbedTLSError, "mbedtls_ssl_read", ret);
            }
        }
        else {
            read_bytes += size_t(ret);
        }
    }

    return read_bytes;
}

void TLSSocket::debugLog(void* ctx, [[maybe_unused]] int level, const char* file, int line, const char* str)
{
    if (ctx == nullptr) {
        return;
    }
    auto logger = static_cast<logging::LogChannel*>(ctx);
    logger->Debug().SetLevel(logging::LogLevel(5 - level)) << file << ":" << line << ": " << str;
}

int TLSSocket::rng_get(void* p_rng, unsigned char* output, size_t output_len)
{
    if (p_rng == nullptr) {
        return -1;
    }
    auto random = static_cast<tlsRandom*>(p_rng);
    return mbedtls_ctr_drbg_random(&random, output, output_len);
}

} // rsp::security
