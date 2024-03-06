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
#include <cstring>
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
    mCanonicalName = aUnixPath;
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
    std::memcpy(&mAddress.Unspecified.sa_data, &arSockAddr.sa_data, Size());
    return *this;
}

size_t SocketAddress::Size() const
{
    if (mAddress.Unspecified.sa_family == int(Domain::Unix)) {
        return SUN_LEN(&mAddress.Unix);
    }
    else if (mAddress.Inet.sin_family == int(Domain::Inet)) {
        return sizeof(mAddress.Inet);
    }
    else if (mAddress.Inet6.sin6_family == int(Domain::Inet6)) {
        return sizeof(mAddress.Inet6);
    }
    return sizeof(mAddress.Unspecified);
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

SocketAddress& SocketAddress::SetCanonicalName(std::string_view aName)
{
    mCanonicalName = aName;
    return *this;
}

void SocketAddress::makeCanonicalName()
{
    switch (Domain(mAddress.Unspecified.sa_family)) {
        case Domain::Unix:
//            mCanonicalName = ;
            break;
        case Domain::Inet:
            break;
        case Domain::Inet6:
            break;
        default:
        case Domain::Unspecified:
            mCanonicalName.resize(sizeof(mAddress.Unspecified.sa_data) + 1);
            mCanonicalName.back() = '\0';
            std::strncpy(mCanonicalName.data(), mAddress.Unspecified.sa_data, sizeof(mAddress.Unspecified.sa_data));
            mCanonicalName.shrink_to_fit();
            break;
    }
}

} // namespace rsp::posix
