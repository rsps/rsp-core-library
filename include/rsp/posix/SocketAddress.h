/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SOCKET_ADDRESS_H
#define RSP_CORE_LIB_SOCKET_ADDRESS_H

#include "rsp/exceptions/CoreException.h"
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <chrono>
#include "SocketTypes.h"

namespace rsp::posix {

/**
 * \brief The three kinds of Unix domain socket addresses.
 */
enum class UnixKind {
    None,      ///< Not a Unix address
    Anonymous, ///< No name (socketpair, or a client that never called bind)
    Abstract,  ///< Linux only: name starts with a NUL byte, no filesystem inode
    Named,     ///< Filesystem path created by bind
};

class SocketAddress
{
public:
    SocketAddress() = default;
    explicit SocketAddress(const sockaddr& arSockAddr, socklen_t aLen, Domain aDomain, Type aType, Protocol aProtocol);
    explicit SocketAddress(std::string_view aUnixPath, Type aType, Protocol aProtocol = Protocol::Unspecified);
    explicit SocketAddress(uint32_t aIP, uint16_t aPort, Type aType, Protocol aProtocol = Protocol::Unspecified);

    /**
     * \brief Create an abstract Unix socket address (Linux only).
     * \param aName Name without the leading NUL byte
     */
    [[nodiscard]] static SocketAddress Abstract(std::string_view aName, Type aType, Protocol aProtocol = Protocol::Unspecified);

    /**
     * \brief Number of valid bytes in the native address, as given by the kernel or computed on construction.
     */
    [[nodiscard]] size_t GetSize() const;
    [[nodiscard]] bool IsEmpty() const;
    SocketAddress& SetDomain(Domain aDomain) noexcept;

    [[nodiscard]] uint16_t GetPort() const;
    SocketAddress& SetPort(uint16_t aPort) noexcept;

    [[nodiscard]] const sockaddr& Get() const;

    /**
     * \brief Address as text. For Unix sockets this is the path (Named) or the name without leading NUL (Abstract).
     */
    [[nodiscard]] std::string AsString() const;
    [[nodiscard]] explicit operator std::string() const { return AsString(); }

    [[nodiscard]] Domain GetDomain() const { return static_cast<Domain>(mAddress.Unspecified.sa_family); }
    [[nodiscard]] Type GetType() const { return mType; }
    [[nodiscard]] Protocol GetProtocol() const { return mProtocol; }
    [[nodiscard]] UnixKind GetUnixKind() const;

    /**
     * \brief Human readable name. Abstract Unix sockets are prefixed with '@' and anonymous ones are "<unnamed>".
     */
    [[nodiscard]] std::string GetCanonicalName() const;

protected:
    union
    {
        sockaddr_storage Storage; // only the first union member is zero-initialized
        sockaddr Unspecified;
        sockaddr_in Inet;
        sockaddr_in6 Inet6;
        sockaddr_un Unix;
    } mAddress{};
    socklen_t mLength = 0;
    Type mType = Type::Unspecified;
    Protocol mProtocol = Protocol::Unspecified;

    static_assert(sizeof(mAddress) == sizeof(sockaddr_storage));
};

} // namespace rsp::posix

#endif // RSP_CORE_LIB_SOCKET_ADDRESS_H
