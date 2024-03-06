/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_POSIX_SOCKET_H
#define RSP_CORE_LIB_INCLUDE_POSIX_SOCKET_H

#include <chrono>
#include <string>
#include <netinet/in.h>
#include <sys/un.h>
#include <exceptions/CoreException.h>
#include "SocketAddress.h"
#include "SocketTypes.h"
#include "AddressInfo.h"
#include <logging/LogChannel.h>

namespace rsp::posix {

class ESocketError : public rsp::exceptions::CoreException
{
public:
    explicit ESocketError(const std::string &arMsg, const char *apSystemMsg = nullptr)
            : CoreException(((apSystemMsg == nullptr) ? arMsg : arMsg + std::string(apSystemMsg)))
    {
    }
};

class Socket : public logging::NamedLogger<Socket>
{
public:
    Socket() = default;
    Socket(Domain aDomain, Type aType, Protocol aProtocol = Protocol::Unspecified);
    Socket(const Socket&) = delete;
    Socket(Socket&&) noexcept;
    ~Socket() override;

    Socket& operator=(const Socket&) = delete;
    Socket& operator=(Socket&&) noexcept;

    //---- Socket Options ----
    [[nodiscard]] bool IsConnected() const;
    [[nodiscard]] bool IsListening() const;

    [[nodiscard]] bool IsBroadcastAllowed() const;
    Socket& SetBroadcastAllowed(bool aAllow = true);

    [[nodiscard]] int GetError() const;

    [[nodiscard]] bool IsKeepAliveEnabled() const;
    Socket& SetKeepAlive(bool aEnable = true);

    [[nodiscard]] size_t GetReceiveBufSize() const;
    Socket& SetReceiveBufSize(size_t aSize);
    [[nodiscard]] std::chrono::system_clock::duration GetReceiveTimeout() const;
    Socket& SetReceiveTimeout(std::chrono::system_clock::duration aTimeout);

    [[nodiscard]] size_t GetSendBufSize() const;
    Socket& SetSendBufSize(size_t aSize);
    [[nodiscard]] std::chrono::system_clock::duration GetSendTimeout() const;
    Socket& SetSendTimeout(std::chrono::system_clock::duration aTimeout);

    //---- Socket methods ----
    [[nodiscard]] SocketAddress GetAddr();

    [[nodiscard]] Socket Accept();
    Socket& Bind(const AddressInfo &arAddrInfo, bool aBindAll = false);
    Socket& Connect(const AddressInfo &arAddrInfo);
    [[nodiscard]] int GetOptions(SockOptions aOption, int aLevel = SOL_SOCKET) const;
    Socket& Listen(size_t aAcceptQueueSize = 0);
    size_t Receive(std::uint8_t *apBuffer, size_t aBufLen, int aFlags = 0) const;
    size_t ReceiveFrom(Socket &arPeer, std::uint8_t *apBuffer, size_t aBufLen, int aFlags = 0) const;
    size_t Send(const std::uint8_t *apBuffer, size_t aBufLen, int aFlags = 0) const;
    Socket& SetOptions(SockOptions aOption, int aValue, int aLevel = SOL_SOCKET);
    Socket& Shutdown(ShutdownFlags aFlag);

    //---- Async IO ----
    [[nodiscard]] bool IsDataReady() const;
    [[nodiscard]] int GetFd() const;

protected:
    int mHandle = 0;
    SocketAddress mLocalAddress{};
    SocketAddress mPeerAddress{};
    Domain mDomain = Domain::Unspecified;
    Type mType = Type::Stream;
    Protocol mProtocol = Protocol::Unspecified;

    static void deleteOldSocketInode(const SocketAddress &arAddr);

    /**
     * \brief Special constructor for use in Accept
     * \param arServer
     * \param aHandle
     * \param aLocalAddress
     * \param aPeerAddress
     */
    Socket(const Socket &arServer, int aHandle, SocketAddress aLocalAddress, SocketAddress aPeerAddress);

    [[nodiscard]] std::chrono::system_clock::duration getTimeoutOption(SockOptions aOption) const;
    void setTimeoutOption(SockOptions aOption, std::chrono::system_clock::duration aValue) const;
};

} // rsp::posix

#endif //RSP_CORE_LIB_INCLUDE_POSIX_SOCKET_H
