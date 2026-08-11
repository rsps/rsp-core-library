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

#include "AddressInfo.h"
#include "SocketAddress.h"
#include "SocketTypes.h"

#include <rsp/exceptions/CoreException.h>
#include <rsp/exceptions/ExceptionHelper.h>
#include <rsp/logging/LogChannel.h>
#include <rsp/utils/SystemHandle.h>

#include <chrono>
#include <span>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace rsp::posix {

class ESocketError : public rsp::exceptions::CoreException
{
public:
    explicit ESocketError(const std::string &arMsg, const char *apSystemMsg = nullptr)
            : CoreException(((apSystemMsg == nullptr) ? arMsg : arMsg + std::string(apSystemMsg)))
    {
    }
    explicit ESocketError(const std::string &arMsg, const std::string &arSystemMsg)
          : CoreException(arMsg + arSystemMsg)
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
    [[nodiscard]] bool IsBlocking() const;
    Socket& SetBlocking(bool aBlocking);

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
    Socket& Close();
    [[nodiscard]] int GetOptions(SockOptions aOption, int aLevel = SOL_SOCKET) const;
    Socket& Listen(size_t aAcceptQueueSize = 0);
    [[nodiscard]] size_t Receive(std::span<std::byte> aBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t Receive(std::string &arStringBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t ReceiveFrom(Socket &arPeer, std::span<std::byte> aBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t ReceiveFrom(Socket &arPeer, std::string &arStringBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t Send(std::span<const std::byte> aBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t Send(const std::string &arStringBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t SendTo(const Socket &arPeer, std::span<const std::byte> aBuffer, int aFlags = 0) const;
    [[nodiscard]] size_t SendTo(const Socket &arPeer, const std::string &arStringBuffer, int aFlags = 0) const;

    Socket& SetOptions(SockOptions aOption, int aValue, int aLevel = SOL_SOCKET);
    Socket& Shutdown(ShutdownFlags aFlag);

    //---- Async IO ----
    [[nodiscard]] bool IsDataReady() const;
    [[nodiscard]] int GetFd() const;

protected:
    struct SocketHandleDeleter
    {
        void operator()(int aHandle) noexcept {
            ::shutdown(aHandle, int(ShutdownFlags::ReadWrite));
            ::close(aHandle);
        }
    };
    using SocketHandle_t = rsp::utils::SystemHandle<int, SocketHandleDeleter, -1>;

    SocketHandle_t mHandle{};
    SocketAddress mLocalAddress{};
    SocketAddress mPeerAddress{};
    Domain mDomain = Domain::Unspecified;
    Type mType = Type::Stream;
    Protocol mProtocol = Protocol::Unspecified;

    static void deleteOldSocketINode(const SocketAddress &arAddr);

    /**
     * \brief Special constructor for use in Accept
     * \param arServer
     * \param aHandle
     * \param aLocalAddress
     * \param aPeerAddress
     */
    Socket(const Socket &arServer, int aHandle, const SocketAddress& arLocalAddress, const SocketAddress& arPeerAddress);

    [[nodiscard]] std::chrono::system_clock::duration getTimeoutOption(SockOptions aOption) const;
    void setTimeoutOption(SockOptions aOption, std::chrono::system_clock::duration aValue) const;
};

} // rsp::posix

#endif //RSP_CORE_LIB_INCLUDE_POSIX_SOCKET_H
