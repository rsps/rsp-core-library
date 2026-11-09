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
#include <cstddef>
#include <cstring>
#include <format>
#include <utility>
#include <arpa/inet.h>
#include <rsp/posix/Socket.h>
#include <rsp/exceptions/ExceptionHelper.h>
#include <rsp/posix/SocketAddress.h>

namespace rsp::posix {

namespace {

constexpr size_t cUnixPathOffset = offsetof(sockaddr_un, sun_path);

// Bytes of sun_path covered by the address length. Zero for an anonymous address.
size_t unixPathLength(socklen_t aAddrLen)
{
    return (aAddrLen > cUnixPathOffset) ? std::min<size_t>(aAddrLen - cUnixPathOffset, sizeof(sockaddr_un::sun_path)) : 0;
}

} // namespace

SocketAddress::SocketAddress(std::string_view aUnixPath, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    mAddress.Unix.sun_family = std::to_underlying(Domain::Unix);
    if (aUnixPath.size() >= sizeof(mAddress.Unix.sun_path)) { // guarantee room for null terminator
        THROW_WITH_BACKTRACE1(ESocketError, "Unix socket path is too long.");
    }
    std::copy_n(aUnixPath.data(), aUnixPath.size(), mAddress.Unix.sun_path);
    mLength = static_cast<socklen_t>(cUnixPathOffset + aUnixPath.size());
}

SocketAddress::SocketAddress(const sockaddr& arSockAddr, socklen_t aLen, Domain aDomain, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    mLength = static_cast<socklen_t>(std::min<size_t>(aLen, sizeof(mAddress)));
    std::memcpy(&mAddress, &arSockAddr, mLength);
    mAddress.Unspecified.sa_family = std::to_underlying(aDomain);
}

SocketAddress::SocketAddress(uint32_t aIP, uint16_t aPort, Type aType, Protocol aProtocol)
    : mType(aType)
    , mProtocol(aProtocol)
{
    mAddress.Inet.sin_family = std::to_underlying(Domain::Inet);
    mAddress.Inet.sin_addr.s_addr = aIP;
    mAddress.Inet.sin_port = aPort;
    mLength = sizeof(mAddress.Inet);
}

SocketAddress SocketAddress::Abstract(std::string_view aName, Type aType, Protocol aProtocol)
{
    std::string path(1, '\0');
    path.append(aName);
    return SocketAddress(path, aType, aProtocol);
}

size_t SocketAddress::GetSize() const
{
    return mLength;
}

UnixKind SocketAddress::GetUnixKind() const
{
    if (GetDomain() != Domain::Unix) {
        return UnixKind::None;
    }
    if (unixPathLength(mLength) == 0) {
        return UnixKind::Anonymous;
    }
    return (mAddress.Unix.sun_path[0] == '\0') ? UnixKind::Abstract : UnixKind::Named;
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
        case Domain::Unix: {
            const size_t path_len = unixPathLength(mLength);
            switch (GetUnixKind()) {
                case UnixKind::Named:
                    return std::string{mAddress.Unix.sun_path, strnlen(mAddress.Unix.sun_path, path_len)};
                case UnixKind::Abstract:
                    return std::string{mAddress.Unix.sun_path + 1, path_len - 1};
                default:
                    return {};
            }
        }

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
            switch (GetUnixKind()) {
                case UnixKind::Abstract:
                    return "@" + AsString();
                case UnixKind::Anonymous:
                    return "<unnamed>";
                default:
                    return AsString();
            }

        case Domain::Inet:
            return std::format("{}:{}", AsString(), ntohs(mAddress.Inet.sin_port));

        case Domain::Inet6:
            return std::format("[{}]:{}", AsString(), ntohs(mAddress.Inet6.sin6_port));

        default:
            return {mAddress.Unspecified.sa_data, strnlen(mAddress.Unspecified.sa_data, sizeof(mAddress.Unspecified.sa_data))};
    }
}

} // namespace rsp::posix
