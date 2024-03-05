/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <netdb.h>
#include <arpa/inet.h>
#include <posix/Socket.h>
#include <exceptions/ExceptionHelper.h>
#include <charconv>
#include <cstring>
#include <posix/SocketAddress.h>

namespace rsp::posix {

SocketAddress::SocketAddress(const char *apAddr)
    : SocketAddress(std::string_view(apAddr))
{
}

SocketAddress::SocketAddress(const std::string &arAddr)
    : SocketAddress(std::string_view(arAddr))
{
}

SocketAddress::SocketAddress(std::string_view aAddr)
{
    size_t colon_pos;
    struct addrinfo *p_addr_info = nullptr;
    if (aAddr[0] == '/') {
        if (aAddr.size() > sizeof(mAddress.Unix.sun_path)) {
            THROW_WITH_BACKTRACE1(ESocketError, "Socket address is to long.");
        }
        mAddress.Unix.sun_family = int(Domain::Unix);
        std::memcpy(mAddress.Unix.sun_path, aAddr.data(), aAddr.size());
    }
    else if ((colon_pos = aAddr.find(']')) != std::string::npos) { // IPv6 with service/port
        struct addrinfo hints{};
        hints.ai_family = AF_INET6;
        auto name = std::string(aAddr.substr(0, colon_pos - 1));
        auto port = std::string(aAddr.substr(colon_pos + 2));
        auto res = getaddrinfo(name.c_str(), port.c_str(), &hints, &p_addr_info);
        if (res) {
            if (res == EAI_SYSTEM) {
                THROW_SYSTEM("Failed to parse socket address.");
            }
            THROW_WITH_BACKTRACE2(ESocketError, "Failed to parse socket address: ", gai_strerror(res));
        }
    }
    else if ((colon_pos = aAddr.find(':')) != std::string::npos) { // IPv4 with service/port
        struct addrinfo hints{};
        hints.ai_family = AF_INET;
        auto name = std::string(aAddr.substr(0, colon_pos - 1));
        auto port = std::string(aAddr.substr(colon_pos + 1));
        auto res = getaddrinfo(name.c_str(), port.c_str(), &hints, &p_addr_info);
        if (res) {
            if (res == EAI_SYSTEM) {
                THROW_SYSTEM("Failed to parse socket address.");
            }
            THROW_WITH_BACKTRACE2(ESocketError, "Failed to parse socket address: ", gai_strerror(res));
        }
    }
    else {
        auto name = std::string(aAddr.substr(0, colon_pos - 1));
        auto res = getaddrinfo(name.c_str(), nullptr, nullptr, &p_addr_info);
        if (res) {
            if (res == EAI_SYSTEM) {
                THROW_SYSTEM("Failed to parse socket address.");
            }
            THROW_WITH_BACKTRACE2(ESocketError, "Failed to parse socket address: ", gai_strerror(res));
        }
    }

    struct addrinfo *p = p_addr_info;
    while (p) {
//            mAddress = *p;
        p = p->ai_next;
    }

    if (p_addr_info) {
        freeaddrinfo(p_addr_info);
    }
}

SocketAddress::SocketAddress(sockaddr &arSockAddr, socklen_t /*aLen*/)
    : mAddress(arSockAddr)
{
//    if (aLen != (Size() + sizeof(mAddress.Unspecified.sa_family))) {
//        THROW_WITH_BACKTRACE1(ESocketError, "Socket length is not same as Size().");
//    }
}

SocketAddress::SocketAddress(uint32_t
                             aIP,
                             uint16_t aPort
                            )
{
    mAddress.Inet.
        sin_family = int(Domain::Inet);
    mAddress.Inet.sin_addr.
        s_addr = aIP;
    mAddress.Inet.
        sin_port = aPort;
}

struct sockaddr &SocketAddress::Get()
{
    return mAddress.Unspecified;
}

SocketAddress &SocketAddress::operator=(const sockaddr &arSockAddr)
{
    mAddress.Unspecified.sa_family = arSockAddr.sa_family;
    std::memcpy(&mAddress.Unspecified.sa_data, &arSockAddr.sa_data, Size());
    return *this;
}

size_t SocketAddress::Size() const
{
    if (mAddress.Unspecified.sa_family == int(Domain::Unix)) {
//        for (size_t i = 0; i < sizeof(mAddress.Unix.sun_path); ++i) {
//            if (mAddress.Unix.sun_path[i] == '\0') {
//                return i;
//            }
//        }
        return SUN_LEN(&mAddress.Unix);
    }
    else if (mAddress.Inet.sin_family == int(Domain::Inet)) {
        return sizeof(mAddress.Inet) - sizeof(mAddress.Unspecified.sa_family);
    }
    else if (mAddress.Inet6.sin6_family == int(Domain::Inet6)) {
        return sizeof(mAddress.Inet6) - sizeof(mAddress.Unspecified.sa_family);
    }
    return sizeof(mAddress.Unspecified.sa_data);
}

SocketAddress &SocketAddress::SetDomain(Domain aDomain) noexcept
{
    mAddress.Unspecified.sa_family = int(aDomain);
    return *this;
}

const struct sockaddr &SocketAddress::Get() const
{
    return mAddress.Unspecified;
}

std::string SocketAddress::AsString() const
{
    char buffer[INET6_ADDRSTRLEN + 1];
    buffer[INET6_ADDRSTRLEN] = '\0';

    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Unix:return std::string{mAddress.Unix.sun_path, Size()};

        case Domain::Inet:
            if (inet_ntop(AF_INET, &mAddress.Inet.sin_addr, buffer, sizeof(buffer))) {
                return {buffer};
            }
            break;

        case Domain::Inet6:
            if (inet_ntop(AF_INET6, &mAddress.Inet6.sin6_addr, buffer, sizeof(buffer))) {
                return {buffer};
            }
            break;

        default:break;
    }
    return {};
}

uint16_t SocketAddress::GetPort() const
{
    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Inet:return mAddress.Inet.sin_port;
        case Domain::Inet6:return mAddress.Inet6.sin6_port;
        default:return 0;
    }
}

SocketAddress &SocketAddress::SetPort(uint16_t aPort) noexcept
{
    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Inet:mAddress.Inet.sin_port = aPort;
            break;
        case Domain::Inet6:mAddress.Inet6.sin6_port = aPort;
            break;
        default:break;
    }
    return *this;
}

bool SocketAddress::IsEmpty() const
{
    return mAddress.Unspecified.sa_family == int(Domain::Unspecified);
}

} // namespace rsp::posix
