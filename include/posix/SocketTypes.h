/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_SOCKETTYPES_H
#define RSP_CORE_LIB_SOCKETTYPES_H

#include <sys/socket.h>

namespace rsp::posix {

enum class Domain {
    Unspecified = AF_UNSPEC,   ///< Unspecified
#ifdef AF_LOCAL
    Local       = AF_LOCAL,    ///< Local to host (pipes and file-domain).
    Unix        = AF_LOCAL,    ///< POSIX name for AF_LOCAL.
    File        = AF_LOCAL,    ///< Another non-standard name for PF_LOCAL.
#else
    Unix        = 1,           ///< POSIX name for AF_LOCAL.
#endif
    Inet        = AF_INET,     ///< IP protocol family.
    Inet6       = AF_INET6,    ///< IP version 6.
#ifdef AF_PACKET
    Packet      = AF_PACKET,   ///< Packet family.
#endif
#ifdef AF_BLUETOOTH
    BlueTooth   = AF_BLUETOOTH ///< Bluetooth sockets.
#endif
};

enum class Type {
    Unspecified = 0,
    Stream = SOCK_STREAM,             ///< Provides sequenced, reliable, bidirectional, connection-mode byte streams, and may provide a transmission mechanism for out-of-band data.
    Datagram = SOCK_DGRAM,            ///< Provides datagrams, which are connectionless-mode, unreliable messages of fixed maximum length.
    SequentialPacket = SOCK_SEQPACKET ///< Provides sequenced, reliable, bidirectional, connection-mode transmission paths for records. A record can be sent using one or more output operations and received using one or more input operations, but a single operation never transfers part of more than one record. Record boundaries are visible to the receiver via the MSG_EOR flag.
};

enum class Protocol {
    Unspecified = PF_UNSPEC,   ///< Unspecified
#ifdef PF_LOCAL
    Local       = PF_LOCAL,    ///< Local to host (pipes and file-domain).
    Unix        = PF_LOCAL,    ///< POSIX name for PF_LOCAL.
    File        = PF_LOCAL,    ///< Another non-standard name for PF_LOCAL.
#else
    Unix        = 1,           ///< POSIX name for PF_LOCAL.
#endif
    Inet        = PF_INET,     ///< IP protocol family.
    Inet6       = PF_INET6,    ///< IP version 6.
#ifdef PF_PACKET
    Packet      = PF_PACKET,   ///< Packet family. (Datagram/UDP)
#endif
#ifdef PF_BLUETOOTH
    BlueTooth   = PF_BLUETOOTH ///< Bluetooth sockets.
#endif
};

enum class SockOptions {
    GetAcceptConnections = SO_ACCEPTCONN, ///< Non-zero indicates that socket listening is enabled (getsockopt() only).
    Broadcast = SO_BROADCAST,             ///< Non-zero requests permission to transmit broadcast datagrams (SOCK_DGRAM sockets only).
    ProtocolDebug = SO_DEBUG,             ///< Non-zero requests debugging in underlying protocol modules.
    DoNotRoute = SO_DONTROUTE,            ///< Non-zero requests bypass of normal routing; route based on destination address only.
    GetSocketError = SO_ERROR,            ///< Requests and clears pending error information on the socket (getsockopt() only).
    KeepAlive = SO_KEEPALIVE,             ///< Non-zero requests periodic transmission of keepalive messages (protocol-specific).
//        Linger = SO_LINGER, /*struct linger*/ ///< Specify actions to be taken for queued, unsent data on close(): linger on/off and linger time in seconds.
    OutOfBandInline = SO_OOBINLINE,       ///< Non-zero requests that out-of-band data be placed into normal data input queue as received.
    ReceiveBufSize = SO_RCVBUF,           ///< GetSize of receive buffer (in bytes).
    ReceiveLowAt = SO_RCVLOWAT,           ///< Minimum amount of data to return to application for input operations (in bytes).
    ReceiveTimeout = SO_RCVTIMEO, /*struct timeval*/ ///< Timeout value for a socket receive operation.
    ReUseAddress = SO_REUSEADDR,          ///< Non-zero requests reuse of local addresses in bind() (protocol-specific).
    SendBufSize = SO_SNDBUF,              ///< GetSize of send buffer (in bytes).
    SendLowAt = SO_SNDLOWAT,              ///< Minimum amount of data to send for output operations (in bytes).
    SendTimeout = SO_SNDTIMEO, /*struct timeval*/ ///< Timeout value for a socket send operation.
    GetSocketType = SO_TYPE,              ///< Identify socket type (getsockopt() only).
};

enum class ShutdownFlags {
    Read = SHUT_RD,
    Write = SHUT_WR,
    ReadWrite = SHUT_RDWR
};

} // namespace rsp::posix

#endif //RSP_CORE_LIB_SOCKETTYPES_H
