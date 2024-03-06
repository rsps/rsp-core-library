/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_SOCKETADDRESS_H
#define RSP_CORE_LIB_SOCKETADDRESS_H

#include "exceptions/CoreException.h"
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <chrono>
#include "SocketTypes.h"

namespace rsp::posix {

class SocketAddress
{
public:
    SocketAddress() = default;
    explicit SocketAddress(const struct sockaddr &arSockAddr, socklen_t aLen, Domain aDomain, Type aType,
                           Protocol aProtocol);
    explicit SocketAddress(std::string_view aUnixPath, Type aType, Protocol aProtocol = Protocol::Unspecified);
    explicit SocketAddress(uint32_t aIP, uint16_t aPort, Type aType, Protocol aProtocol = Protocol::Unspecified);

    SocketAddress &operator=(const struct sockaddr &arSockAddr);

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] bool IsEmpty() const;
    SocketAddress &SetDomain(Domain aDomain) noexcept;

    [[nodiscard]] uint16_t GetPort() const;
    SocketAddress &SetPort(uint16_t aPort) noexcept;

    [[nodiscard]] const struct sockaddr &Get() const;
    [[nodiscard]] struct sockaddr &Get();

    [[nodiscard]] std::string AsString() const;
    [[nodiscard]] explicit operator std::string() const { return AsString(); }

    [[nodiscard]] Domain GetDomain() const { return Domain(mAddress.Unspecified.sa_family); }
    [[nodiscard]] Type GetType() const { return mType; }
    [[nodiscard]] Protocol GetProtocol() const { return mProtocol; }
    [[nodiscard]] const std::string& GetCanonicalName() const { return mCanonicalName; }
    SocketAddress& SetCanonicalName(std::string_view aName);

protected:
    union
    {
        struct sockaddr Unspecified;
        struct sockaddr_in Inet;
        struct sockaddr_in6 Inet6;
        struct sockaddr_un Unix;
    } mAddress{};
    Type mType = Type::Unspecified;
    Protocol mProtocol = Protocol::Unspecified;
    std::string mCanonicalName{};

    void makeCanonicalName();
};
} // rsp::posix

#endif //RSP_CORE_LIB_SOCKETADDRESS_H
