/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <cstring>
#include <arpa/inet.h>
#include <posix/Socket.h>
#include <exceptions/ExceptionHelper.h>
#include <posix/SocketAddress.h>

namespace rsp::posix {

SocketAddress::SocketAddress(std::string_view aUnixPath, Type aType, Protocol aProtocol)
    : mType(aType),
      mProtocol(aProtocol)
{
    mAddress.Unix.sun_family = int(Domain::Unix);
    if (aUnixPath.size() > sizeof(mAddress.Unix.sun_path)) {
        THROW_WITH_BACKTRACE1(ESocketError, "Unix socket path is to long.");
    }
    std::memcpy(mAddress.Unix.sun_path, aUnixPath.data(), aUnixPath.size());
}

SocketAddress::SocketAddress(const sockaddr& arSockAddr, socklen_t aLen, Domain aDomain, Type aType, Protocol aProtocol)
    : mType(aType),
      mProtocol(aProtocol)
{
    mAddress.Unspecified.sa_family = int(aDomain);
    std::memcpy(&mAddress.Unspecified.sa_data, &arSockAddr.sa_data, aLen);
}

SocketAddress::SocketAddress(uint32_t aIP, uint16_t aPort, Type aType, Protocol aProtocol)
    : mType(aType),
      mProtocol(aProtocol)
{
    mAddress.Inet.sin_family = int(Domain::Inet);
    mAddress.Inet.sin_addr.s_addr = aIP;
    mAddress.Inet.sin_port = aPort;
}

struct sockaddr& SocketAddress::Get()
{
    return mAddress.Unspecified;
}

SocketAddress &SocketAddress::operator=(const sockaddr &arSockAddr)
{
    mAddress.Unspecified.sa_family = arSockAddr.sa_family;
    std::memcpy(&mAddress.Unspecified.sa_data, &arSockAddr.sa_data, GetSize());
    return *this;
}

size_t SocketAddress::GetSize() const
{
#ifdef SUN_LEN
    if (mAddress.Unspecified.sa_family == int(Domain::Unix)) {
        return SUN_LEN(&mAddress.Unix);
    }
#endif
    if (mAddress.Inet.sin_family == int(Domain::Inet)) {
        return sizeof(mAddress.Inet);
    }
    if (mAddress.Inet6.sin6_family == int(Domain::Inet6)) {
        return sizeof(mAddress.Inet6);
    }
    return sizeof(mAddress);
}

SocketAddress& SocketAddress::SetDomain(Domain aDomain) noexcept
{
    mAddress.Unspecified.sa_family = int(aDomain);
    return *this;
}

const struct sockaddr& SocketAddress::Get() const
{
    return mAddress.Unspecified;
}

std::string SocketAddress::AsString() const
{
    char buffer[INET6_ADDRSTRLEN + 1];
    buffer[INET6_ADDRSTRLEN] = '\0';

    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Unix:return std::string{mAddress.Unix.sun_path, GetSize()};

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

SocketAddress& SocketAddress::SetPort(uint16_t aPort) noexcept
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

std::string SocketAddress::GetCanonicalName() const
{
    std::string result;
    char ip[INET6_ADDRSTRLEN + 1];

    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Unix:
            result = mAddress.Unix.sun_path;
            break;

        case Domain::Inet:
            if (inet_ntop(int(Domain::Inet), &mAddress.Inet.sin_addr.s_addr, ip, sizeof(ip))) {
                result = std::string(ip) + ":" + std::to_string(int(ntohs(mAddress.Inet.sin_port)));
                break;
            }
            THROW_SYSTEM("inet_ntop failed");

        case Domain::Inet6:
            if (inet_ntop(int(Domain::Inet6), &mAddress.Inet6.sin6_addr, ip, sizeof(ip))) {
                result = std::string(ip) + ":" + std::to_string(int(ntohs(mAddress.Inet.sin_port)));
                break;
            }
            THROW_SYSTEM("inet_ntop failed");

        default:
        case Domain::Unspecified:
            result.resize(sizeof(mAddress.Unspecified.sa_data) + 1);
            result.back() = '\0';
            std::strncpy(result.data(), mAddress.Unspecified.sa_data, sizeof(mAddress) - sizeof(uint16_t));
            result.shrink_to_fit();
            break;
    }
    return result;
}

void SocketAddress::UpdateFromParent(const SocketAddress &arParent)
{
    if (GetDomain() == Domain::Unix) {
        mAddress.Unix = arParent.mAddress.Unix;
    }
    mProtocol = arParent.mProtocol;
    mType = arParent.mType;
}

} // namespace rsp::posix
