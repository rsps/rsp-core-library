/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <cstring>
#include <format>
#include <utility>
#include <arpa/inet.h>
#include <rsp/posix/Socket.h>
#include <rsp/exceptions/ExceptionHelper.h>
#include <rsp/posix/SocketAddress.h>

namespace rsp::posix {

SocketAddress::SocketAddress(std::string_view aUnixPath, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    mAddress.Unix.sun_family = std::to_underlying(Domain::Unix);
    if (aUnixPath.size() >= sizeof(mAddress.Unix.sun_path)) { // guarantee room for null terminator
        THROW_WITH_BACKTRACE1(ESocketError, "Unix socket path is too long.");
    }
    std::copy_n(aUnixPath.data(), aUnixPath.size(), mAddress.Unix.sun_path);
}

SocketAddress::SocketAddress(const sockaddr& arSockAddr, socklen_t aLen, Domain aDomain, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    std::memcpy(&mAddress, &arSockAddr, std::min<size_t>(aLen, sizeof(mAddress)));
    mAddress.Unspecified.sa_family = std::to_underlying(aDomain);
}

SocketAddress::SocketAddress(uint32_t aIP, uint16_t aPort, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    mAddress.Inet.sin_family = std::to_underlying(Domain::Inet);
    mAddress.Inet.sin_addr.s_addr = aIP;
    mAddress.Inet.sin_port = aPort;
}

sockaddr& SocketAddress::Get()
{
    return mAddress.Unspecified;
}

size_t SocketAddress::GetSize() const
{
    switch (GetDomain()) {
#ifdef SUN_LEN
        case Domain::Unix:
            return SUN_LEN(&mAddress.Unix);
#endif

        case Domain::Inet:
            return sizeof(mAddress.Inet);

        case Domain::Inet6:
            return sizeof(mAddress.Inet6);

        default:
            return sizeof(sockaddr);
    }
}

SocketAddress& SocketAddress::SetDomain(Domain aDomain) noexcept
{
    mAddress.Unspecified.sa_family = std::to_underlying(aDomain);
    return *this;
}

const sockaddr& SocketAddress::Get() const
{
    return mAddress.Unspecified;
}

std::string SocketAddress::AsString() const
{
    std::array<char, INET6_ADDRSTRLEN + 1> buffer{};

    switch (GetDomain()) {
        case Domain::Unix:
            return std::string{mAddress.Unix.sun_path, strnlen(mAddress.Unix.sun_path, sizeof(mAddress.Unix.sun_path))};

        case Domain::Inet:
            if (inet_ntop(AF_INET, &mAddress.Inet.sin_addr, buffer.data(), buffer.size()) == nullptr) {
                THROW_SYSTEM("inet_ntop failed");
            }
            return {buffer.data()};

        case Domain::Inet6:
            if (inet_ntop(AF_INET6, &mAddress.Inet6.sin6_addr, buffer.data(), buffer.size()) == nullptr) {
                THROW_SYSTEM("inet_ntop failed");
            }
            return {buffer.data()};

        default:
            return {};
    }
}

uint16_t SocketAddress::GetPort() const
{
    switch (GetDomain()) {
        case Domain::Inet:
            return mAddress.Inet.sin_port;
        case Domain::Inet6:
            return mAddress.Inet6.sin6_port;
        default:
            return 0;
    }
}

SocketAddress& SocketAddress::SetPort(uint16_t aPort) noexcept
{
    switch (GetDomain()) {
        case Domain::Inet:
            mAddress.Inet.sin_port = aPort;
            break;
        case Domain::Inet6:
            mAddress.Inet6.sin6_port = aPort;
            break;
        default:
            break;
    }
    return *this;
}

bool SocketAddress::IsEmpty() const
{
    return (GetDomain() == Domain::Unspecified);
}

std::string SocketAddress::GetCanonicalName() const
{
    switch (GetDomain()) {
        case Domain::Unix:
            return AsString();

        case Domain::Inet:
            return std::format("{}:{}", AsString(), ntohs(mAddress.Inet.sin_port));

        case Domain::Inet6:
            return std::format("[{}]:{}", AsString(), ntohs(mAddress.Inet6.sin6_port));

        default:
            return {mAddress.Unspecified.sa_data, strnlen(mAddress.Unspecified.sa_data, sizeof(mAddress.Unspecified.sa_data))};
    }
}

void SocketAddress::UpdateFromParent(const SocketAddress& arParent)
{
    if (GetDomain() == Domain::Unix) {
        mAddress.Unix = arParent.mAddress.Unix;
    }
    mProtocol = arParent.mProtocol;
    mType = arParent.mType;
}

} // namespace rsp::posix
