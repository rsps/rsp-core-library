/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <cstring>
#include <charconv>
#include <exceptions/ExceptionHelper.h>
#include <posix/Socket.h>
#include <poll.h>
#include <unistd.h>
#include <filesystem>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

namespace rsp::posix {

Socket::Address::Address(const char *apAddr)
    : Address(std::string_view(apAddr))
{
}

Socket::Address::Address(const std::string &arAddr)
    : Address(std::string_view(arAddr))
{
}

Socket::Address::Address(std::string_view aAddr)
{
    size_t colonPos;
    if (aAddr[0] == '/') {
        if (aAddr.size() > sizeof(mAddress.Unix.sun_path)) {
            THROW_WITH_BACKTRACE1(ESocketError, "Socket address is to long.");
        }
        mAddress.Unix.sun_family = int(Socket::Domain::Unix);
        std::memcpy(mAddress.Unix.sun_path, aAddr.data(), aAddr.size());
    }
    else if ((colonPos = aAddr.find(']')) != std::string::npos) { // IPv6 with service/port
        struct addrinfo ainfo{};
        size_t colonPos = aAddr.find(':');
        if(colonPos != std::string::npos) {
            auto port_string = aAddr.substr(colonPos+1);
            uint16_t u2;
            auto result = std::from_chars(port_string.data(), port_string.data() + port_string.size(), u2);
            if (result.ec == std::errc()) {
                mAddress.Inet.sin_port = u2;
            }
        }

    }
    if (inet_pton(AF_INET, aAddr.data(), &(mAddress.Inet.sin_addr)) == 1) {

        mAddress.Inet.sin_family = AF_INET;
        size_t colonPos = aAddr.find(':');
        if(colonPos != std::string::npos) {
            auto port_string = aAddr.substr(colonPos+1);
            uint16_t u2;
            auto result = std::from_chars(port_string.data(), port_string.data() + port_string.size(), u2);
            if (result.ec == std::errc()) {
                mAddress.Inet.sin_port = u2;
            }
        }
    }
    else if (inet_pton(AF_INET6, aAddr.data(), &(mAddress.Inet6.sin6_addr)) == 1) {
        mAddress.Inet6.sin6_family = AF_INET6;
        size_t colonPos = aAddr.find(']'); // IPv6 url format: [<uint16_t * 8>::]:<port>
        if(colonPos != std::string::npos) {
            auto port_string = aAddr.substr(colonPos+2);
            uint16_t u2;
            auto result = std::from_chars(port_string.data(), port_string.data() + port_string.size(), u2);
            if (result.ec == std::errc()) {
                mAddress.Inet6.sin6_port = u2;
            }
        }
    }
    else {
        mAddress.Unspecified.sa_family = int(Socket::Domain::Unspecified);
    }
}

Socket::Address::Address(sockaddr &arSockAddr, socklen_t aLen)
    : mAddress(arSockAddr)
{
//    if (aLen != (Size() + sizeof(mAddress.Unspecified.sa_family))) {
//        THROW_WITH_BACKTRACE1(ESocketError, "Socket length is not same as Size().");
//    }
}

Socket::Address::Address(uint32_t aIP, uint16_t aPort)
{
    mAddress.Inet.sin_family = int(Socket::Domain::Inet);
    mAddress.Inet.sin_addr.s_addr = aIP;
    mAddress.Inet.sin_port = aPort;
}

Socket::Address& Socket::Address::operator=(const sockaddr &arSockAddr)
{
    mAddress.Unspecified.sa_family = arSockAddr.sa_family;
    std::memcpy(&mAddress.Unspecified.sa_data, &arSockAddr.sa_data, Size());
    return *this;
}

size_t Socket::Address::Size() const
{
    if (mAddress.Unspecified.sa_family == int(Socket::Domain::Unix)) {
//        for (size_t i = 0; i < sizeof(mAddress.Unix.sun_path); ++i) {
//            if (mAddress.Unix.sun_path[i] == '\0') {
//                return i;
//            }
//        }
        return SUN_LEN(&mAddress.Unix);
    }
    else if (mAddress.Inet.sin_family == int(Socket::Domain::Inet)) {
        return sizeof(mAddress.Inet) - sizeof(mAddress.Unspecified.sa_family);
    }
    else if (mAddress.Inet6.sin6_family == int(Socket::Domain::Inet6)) {
        return sizeof(mAddress.Inet6) - sizeof(mAddress.Unspecified.sa_family);
    }
    return sizeof(mAddress.Unspecified.sa_data);
}

Socket::Address &Socket::Address::SetDomain(Domain aDomain) noexcept
{
    mAddress.Unspecified.sa_family = int(aDomain);
    return *this;
}

const struct sockaddr& Socket::Address::Get() const
{
    return mAddress.Unspecified;
}

struct sockaddr& Socket::Address::Get()
{
    return mAddress.Unspecified;
}

std::string Socket::Address::AsString() const
{
    char buffer[INET6_ADDRSTRLEN+1];
    buffer[INET6_ADDRSTRLEN] = '\0';

    switch (Socket::Domain(mAddress.Unspecified.sa_family)) {
        case Socket::Domain::Unix:
            return std::string{mAddress.Unix.sun_path, Size()};

        case Socket::Domain::Inet:
            if (inet_ntop(AF_INET, &mAddress.Inet.sin_addr, buffer, sizeof(buffer))) {
                return {buffer};
            }
            break;

        case Socket::Domain::Inet6:
            if (inet_ntop(AF_INET6, &mAddress.Inet6.sin6_addr, buffer, sizeof(buffer))) {
                return {buffer};
            }
            break;

        default:
            break;
    }
    return {};
}

uint16_t Socket::Address::GetPort() const
{
    switch (Socket::Domain(mAddress.Unspecified.sa_family)) {
        case Socket::Domain::Inet:
            return mAddress.Inet.sin_port;
        case Socket::Domain::Inet6:
            return mAddress.Inet6.sin6_port;
        default:
            return 0;
    }
}

Socket::Address& Socket::Address::SetPort(uint16_t aPort) noexcept
{
    switch (Socket::Domain(mAddress.Unspecified.sa_family)) {
        case Socket::Domain::Inet:
            mAddress.Inet.sin_port = aPort;
            break;
        case Socket::Domain::Inet6:
            mAddress.Inet6.sin6_port = aPort;
            break;
        default:
            break;
    }
    return *this;
}

bool Socket::Address::IsEmpty() const
{
    return mAddress.Unspecified.sa_family == int(Domain::Unspecified);
}

Socket::Socket(Socket::Domain aDomain, Socket::Type aType, Socket::Protocol aProtocol)
{
    mHandle = socket(int(aDomain), int(aType), int(aProtocol));
    if (mHandle < 0) {
        THROW_SYSTEM("socket() failed.");
    }
    mDomain = aDomain;
    mType = aType;
    mProtocol = aProtocol;
}

Socket::Socket(const Socket &arServer, int aHandle, Socket::Address aLocalAddress, Socket::Address aPeerAddress)
    : mHandle(aHandle),
      mLocalAddress(aLocalAddress),
      mPeerAddress(aPeerAddress),
      mDomain(arServer.mDomain),
      mType(arServer.mType),
      mProtocol(arServer.mProtocol)
{
}

Socket::Socket(Socket &&arOther) noexcept
    : mHandle(arOther.mHandle),
      mLocalAddress(arOther.mLocalAddress),
      mPeerAddress(arOther.mPeerAddress),
      mDomain(arOther.mDomain)
{
    arOther.mHandle = 0;
}

Socket::~Socket()
{
    if (mHandle) {
        Shutdown(ShutdownFlags::ReadWrite);
        close(mHandle);
    }
}

Socket& Socket::operator=(Socket &&arOther) noexcept
{
    mHandle = arOther.mHandle;
    mLocalAddress = arOther.mLocalAddress;
    mPeerAddress  = arOther.mPeerAddress;
    mDomain = arOther.mDomain;
    arOther.mHandle = 0;
    return *this;
}

bool Socket::IsConnected() const
{
    return !mPeerAddress.IsEmpty();
}

bool Socket::IsListening() const
{
    int result = GetOptions(SockOptions::GetAcceptConnections);
    return (result != 0);
}

bool Socket::IsBroadcastAllowed() const
{
    int result = GetOptions(SockOptions::Broadcast);
    return (result != 0);
}

Socket &Socket::SetBroadcastAllowed(bool aAllow)
{
    SetOptions(SockOptions::Broadcast, int(aAllow));
    return *this;
}

int Socket::GetError() const
{
    return GetOptions(SockOptions::GetSocketError);
}

bool Socket::IsKeepAliveEnabled() const
{
    int result = GetOptions(SockOptions::KeepAlive);
    return (result != 0);
}

Socket &Socket::SetKeepAlive(bool aEnable)
{
    SetOptions(SockOptions::KeepAlive, int(aEnable));
    return *this;
}

size_t Socket::GetReceiveBufSize() const
{
    int result = GetOptions(SockOptions::ReceiveBufSize);
    return size_t(result);
}

Socket &Socket::SetReceiveBufSize(size_t aSize)
{
    SetOptions(SockOptions::ReceiveBufSize, int(aSize));
    return *this;
}

std::chrono::system_clock::duration Socket::GetReceiveTimeout() const
{
    return getTimeoutOption(SockOptions::ReceiveTimeout);
}

Socket &Socket::SetReceiveTimeout(std::chrono::system_clock::duration aTimeout)
{
    setTimeoutOption(SockOptions::ReceiveTimeout, aTimeout);
    return *this;
}

size_t Socket::GetSendBufSize() const
{
    int result = GetOptions(SockOptions::SendBufSize);
    return size_t(result);
}

Socket &Socket::SetSendBufSize(size_t aSize)
{
    SetOptions(SockOptions::SendBufSize, int(aSize));
    return *this;
}

std::chrono::system_clock::duration Socket::GetSendTimeout() const
{
    return getTimeoutOption(SockOptions::SendTimeout);
}

Socket &Socket::SetSendTimeout(std::chrono::system_clock::duration aTimeout)
{
    setTimeoutOption(SockOptions::SendTimeout, aTimeout);
    return *this;
}

Socket::Address Socket::GetAddr()
{
    if (mLocalAddress.IsEmpty()) {
        struct sockaddr sa{};
        socklen_t len = sizeof(sa);

        int res = getsockname(mHandle, &sa, &len);
        if (res < 0) {
            THROW_SYSTEM("getsockname() failed.");
        }
        mLocalAddress = Address(sa, len);
        mDomain = Domain(sa.sa_family);
    }
    return mLocalAddress;
}

Socket Socket::Accept() const
{
    struct sockaddr sa{};
    socklen_t len = sizeof(sa);

    int res = accept(mHandle, &sa, &len);
    if (res < 0) {
        THROW_SYSTEM("accept() failed. Handle: " + std::to_string(mHandle));
    }

    return {*this, res, Address(sa, len), mLocalAddress};
}

Socket &Socket::Bind(const std::string &arAddr)
{
    Address sa(arAddr);
    sa.SetDomain(mDomain);

    if (sa.GetDomain() == Domain::Unix) {
        std::filesystem::remove(sa.AsString());
    }

    int res = bind(mHandle, &sa.Get(), sa.Size());
    if (res < 0) {
        THROW_SYSTEM("bind() failed.");
    }
    mLocalAddress = sa;
    return *this;
}

Socket &Socket::Connect(const std::string &arAddr)
{
    Address sa(arAddr);
    sa.SetDomain(mDomain);
    int res = connect(mHandle, &sa.Get(), sa.Size());
    if (res < 0) {
        THROW_SYSTEM("connect() failed.");
    }
    mPeerAddress = sa;
    return *this;
}

Socket &Socket::Listen(size_t aAcceptQueueSize)
{
    int res = listen(mHandle, int(aAcceptQueueSize));
    if (res < 0) {
        THROW_SYSTEM("listen() failed.");
    }
    return *this;
}

size_t Socket::Receive(std::uint8_t *apBuffer, size_t aBufLen, int aFlags) const
{
    ssize_t res = recv(mHandle, apBuffer, aBufLen, aFlags);
    if (res == -1) {
        THROW_SYSTEM("recv() failed.");
    }
    return size_t(res);
}

size_t Socket::ReceiveFrom(Socket &arPeer, std::uint8_t *apBuffer, size_t aBufLen, int aFlags) const
{
    Address sa{};
    socklen_t len = sizeof(sa);

    ssize_t res = recvfrom(mHandle, apBuffer, aBufLen, aFlags, &sa.Get(), &len);
    if (res == -1) {
        THROW_SYSTEM("recvfrom() failed.");
    }
    arPeer.mLocalAddress = sa;
    return size_t(res);
}

size_t Socket::Send(const std::uint8_t *apBuffer, size_t aBufLen, int aFlags) const
{
    ssize_t res = send(mHandle, apBuffer, aBufLen, aFlags);
    if (res == -1) {
        THROW_SYSTEM("send() failed.");
    }
    return size_t(res);
}

bool Socket::IsDataReady() const
{
    struct pollfd fd{};
    int ret;
    bool result = false;
    int timeout = 0;

    fd.fd = mHandle;
    fd.events = POLLIN;

    ret = poll(&fd, 1, timeout);

    if (ret > 0) {
        if (fd.revents & POLLIN) {
            result = true;
        }
    }

    return result;
}

int Socket::GetFd() const
{
    return mHandle;
}

int Socket::GetOptions(Socket::SockOptions aOption, int aLevel) const
{
    int result;
    socklen_t len = sizeof(result);
    int res = getsockopt(mHandle, aLevel, int(aOption), &result, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return result;
}

Socket &Socket::SetOptions(Socket::SockOptions aOption, int aValue, int aLevel)
{
    int res = setsockopt(mHandle, aLevel, int(aOption), &aValue, sizeof(aValue));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
    return *this;
}

std::chrono::system_clock::duration Socket::getTimeoutOption(Socket::SockOptions aOption) const
{
    using namespace std::chrono;
    struct timeval tv{};
    socklen_t len = sizeof(tv);
    int res = getsockopt(mHandle, SOL_SOCKET, int(aOption), &tv, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return (seconds(tv.tv_sec + (tv.tv_usec / 1000000)) + microseconds(tv.tv_usec % 1000000));
}

void Socket::setTimeoutOption(Socket::SockOptions aOption, std::chrono::system_clock::duration aValue) const
{
    using namespace std::chrono;
    struct timeval tv{};
    tv.tv_sec = duration_cast<seconds>(aValue).count();
    tv.tv_usec = duration_cast<microseconds>(aValue).count() % 1000000;
    int res = setsockopt(mHandle, SOL_SOCKET, int(aOption), &tv, sizeof(tv));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
}

Socket& Socket::Shutdown(ShutdownFlags aFlag)
{
    int res = shutdown(mHandle, int(aFlag));
    if (res < 0) {
        THROW_SYSTEM("shutdown() failed.");
    }
    return *this;
}

} // rsp::posix
