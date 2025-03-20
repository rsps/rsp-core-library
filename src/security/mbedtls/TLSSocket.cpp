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
#include <utils/DateTime.h>

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
    CHK_0(mbedtls_ctr_drbg_seed(this, mbedtls_entropy_func, &arEntropy, arNonce.data(), arNonce.size()))
}

TLSSocket::TLSSocket(const network::ConnectionOptions& arOptions)
    : NamedLogChannel("MbedTLS-TLSSocket"),
      mrOptions(arOptions),
      mRandom(mEntropy, mrOptions.Nonce)
{
    CHK_0(mbedtls_ssl_config_defaults(&mConfig, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT))
    mbedtls_ssl_conf_authmode( &mConfig, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng( &mConfig, mbedtls_ctr_drbg_random, &mRandom );
    if (mrOptions.Verbose) {
        mbedtls_ssl_conf_dbg(&mConfig, debugLog, &mLogger);
        mbedtls_debug_set_threshold(4);
    }

    if (!mrOptions.CertCaPath.empty()) {
        CHK_0(mbedtls_x509_crt_parse_file(&mCaChain, mrOptions.CertCaPath.c_str()))
        mbedtls_ssl_conf_ca_chain(&mConfig, &mCaChain, nullptr);
    }
    else {
        THROW_WITH_BACKTRACE1(EmbedTLSCaChainMissing, "A server CA certificate chain MUST be provided for TLS-1.3");
    }

    if (!mrOptions.CertPath.empty()) {
        CHK_0(mbedtls_x509_crt_parse_file(&mClientCert, mrOptions.CertPath.c_str()))
        CHK_0(mbedtls_pk_parse_keyfile(&mPrivateKey, mrOptions.KeyPath.c_str(), mrOptions.KeyPasswd.c_str(), &rng_get, &mRandom))
        CHK_0(mbedtls_ssl_conf_own_cert(&mConfig, &mClientCert, &mPrivateKey))
    }

    mbedtls_ssl_conf_read_timeout(&mConfig, mrOptions.ResponseTimeout * 1000);

    CHK_0(mbedtls_ssl_setup(&mSsl, &mConfig))

//    mbedtls_ssl_set_timer_cb(&mSsl, mbedtls_timing_set_delay, mbedtls_timing_get_delay);

    UrlParser up(mrOptions.BaseUrl);
    CHK_0(mbedtls_ssl_set_hostname(&mSsl, std::string(up.GetHost()).c_str()))

    CHK_0(psa_crypto_init())
}

TLSSocket& TLSSocket::SetSocket(posix::Socket& arSocket)
{
    mNet.fd = arSocket.GetFd();
    mbedtls_ssl_set_bio( &mSsl, &mNet, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
    CHK_0(mbedtls_net_set_block(&mNet))

    int ret;
    while ((ret = mbedtls_ssl_handshake(&mSsl)) != 0) {
        if (resultHandler(ret)) {
            break;
        }
    }

    return *this;
}

TLSSocket& TLSSocket::Close()
{
    int ret;
    do {
        ret = mbedtls_ssl_close_notify(&mSsl);
    }
    while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
    return *this;
}

size_t TLSSocket::Write(std::span<const std::byte> aData)
{
    size_t write_bytes = 0;
    while(write_bytes < aData.size()) {
        auto ret = mbedtls_ssl_write(&mSsl, reinterpret_cast<const unsigned char*>(aData.data() + write_bytes), aData.size() - write_bytes);
        if (ret <= 0) {
            if (resultHandler(ret)) {
                break;
            }
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
            if (resultHandler(ret)) {
                break;
            }
        }
        else {
            read_bytes += size_t(ret);
            break;
        }
    }

    return read_bytes;
}

void TLSSocket::debugLog(void* ctx, [[maybe_unused]] int level, const char* file, int line, const char* str)
{
    using namespace rsp::logging;
    if (ctx == nullptr) {
        return;
    }
    auto logger = static_cast<LogChannel*>(ctx);
    auto msg = std::string_view(str);
    utils::DateTime dt;
    logger->Debug().SetLevel(LogLevel(level + int(LogLevel::Error))) << "[" << dt.ToLogging() << "] " << file << ":" << line << ": " << msg.substr(0, msg.size() - 1);
}

int TLSSocket::rng_get(void* p_rng, unsigned char* output, size_t output_len)
{
    if (p_rng == nullptr) {
        return -1;
    }
    auto random = static_cast<tlsRandom*>(p_rng);
    return mbedtls_ctr_drbg_random(&random, output, output_len);
}

bool TLSSocket::resultHandler(int aErr)
{
    switch (aErr) {
        case MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS:
            mLogger.Info() << "SSL got crypto in progress";
            break;

        case MBEDTLS_ERR_SSL_WANT_READ:
            mLogger.Debug() << "SSL want read";
            break;

        case MBEDTLS_ERR_SSL_WANT_WRITE:
            mLogger.Debug() << "SSL want write";
            break;

        case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
            mLogger.Info() << "SSL connection was closed gracefully";
            return true;

        case MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
            /* We were waiting for application data but got
             * a NewSessionTicket instead. */
            mLogger.Info() << "SSL got new session ticket";
            break;

        case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
        case MBEDTLS_ERR_SSL_BAD_CERTIFICATE:
        {
            auto flags = mbedtls_ssl_get_verify_result(&mSsl);
            char verify_info[512];
            mbedtls_x509_crt_verify_info(verify_info, sizeof(verify_info), "  ! ", flags);
            THROW_WITH_BACKTRACE2(EMbedTLSInvalidCertificate, verify_info, aErr);
        }

        case MBEDTLS_ERR_SSL_RECEIVED_EARLY_DATA:
            THROW_WITH_BACKTRACE2(EMbedTLSEarlyData, "SSL early data received", aErr);

        case 0:
        case MBEDTLS_ERR_NET_CONN_RESET:
            THROW_WITH_BACKTRACE1(ENetReconnect, "SSL connection was reset by peer");

        case MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED:
            THROW_WITH_BACKTRACE2(EMbedTLSInvalidCertificate, "A certificate chain is required", aErr);

        default:
            THROW_WITH_BACKTRACE2(EMbedTLSFatal, "mbedtls_ssl_read", aErr);
    }
    return false;
}

} // rsp::security
