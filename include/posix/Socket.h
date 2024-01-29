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
#include <sys/socket.h>

namespace rsp::posix {

class Socket
{
public:
    using SockAddress_t = std::string;

    enum class Domain {
        Unspecified = AF_UNSPEC,   ///< Unspecified
        Local       = AF_LOCAL,    ///< Local to host (pipes and file-domain).
        Unix        = AF_UNIX,     ///< POSIX name for PF_LOCAL.
        File        = AF_FILE,     ///< Another non-standard name for PF_LOCAL.
        Inet        = AF_INET,     ///< IP protocol family.
        Inet6       = AF_INET6,    ///< IP version 6.
        BlueTooth   = AF_BLUETOOTH ///< Bluetooth sockets.
    };

    enum class Type {
        Stream = SOCK_STREAM,             ///< Provides sequenced, reliable, bidirectional, connection-mode byte streams, and may provide a transmission mechanism for out-of-band data.
        Datagram = SOCK_DGRAM,            ///< Provides datagrams, which are connectionless-mode, unreliable messages of fixed maximum length.
        SequentialPacket = SOCK_SEQPACKET ///< Provides sequenced, reliable, bidirectional, connection-mode transmission paths for records. A record can be sent using one or more output operations and received using one or more input operations, but a single operation never transfers part of more than one record. Record boundaries are visible to the receiver via the MSG_EOR flag.
    };

    enum class Protocol {
        Unspecified = PF_UNSPEC,   ///< Unspecified
        Local       = PF_LOCAL,    ///< Local to host (pipes and file-domain).
        Unix        = PF_UNIX,     ///< POSIX name for PF_LOCAL.
        File        = PF_FILE,     ///< Another non-standard name for PF_LOCAL.
        Inet        = PF_INET,     ///< IP protocol family.
        Inet6       = PF_INET6,    ///< IP version 6.
        BlueTooth   = PF_BLUETOOTH ///< Bluetooth sockets.
    };

    Socket() = default;
    Socket(Domain aDomain, Type aType, Protocol aProtocol);

    //---- Socket Options ----
    [[nodiscard]] bool IsConnected() const;
    [[nodiscard]] bool IsListening() const;

    [[nodiscard]] bool IsBroadcastAllowed() const;
    Socket& SetBroadcastAllowed(bool aAllow = true);

    [[nodiscard]] std::string GetError() const;

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
    [[nodiscard]] SockAddress_t GetAddr() const;
    [[nodiscard]] SockAddress_t GetPeerAddr() const;

    Socket& Accept(Socket &arPeer);
    Socket& Bind(const SockAddress_t &arAddr);
    Socket& Connect(const SockAddress_t &arAddr);
    Socket& Listen(size_t aAcceptQueueSize = 0);
    size_t Receive(std::uint8_t *apBuffer, size_t aBufLen);
    size_t Send(const std::uint8_t *apBuffer, size_t aBufLen);

    //---- Async IO ----
    [[nodiscard]] bool IsDataReady() const;
    [[nodiscard]] int GetFd() const;

protected:
    int mHandle = 0;
};

class ClientSocket : public Socket
{

};

} // rsp::posix

#endif //RSP_CORE_LIB_INCLUDE_POSIX_SOCKET_H
