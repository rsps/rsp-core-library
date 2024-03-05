/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include <charconv>
#include <exceptions/ExceptionHelper.h>
#include <posix/Socket.h>
#include <poll.h>
#include <unistd.h>
#include <filesystem>
#include <arpa/inet.h>
#include <sys/types.h>

namespace rsp::posix {

Socket::Socket(Domain aDomain, Type aType, Protocol aProtocol)
{
    mHandle = socket(int(aDomain), int(aType), int(aProtocol));
    if (mHandle < 0) {
        THROW_SYSTEM("socket() failed.");
    }
    mDomain = aDomain;
    mType = aType;
    mProtocol = aProtocol;
}

Socket::Socket(const Socket &arServer, int aHandle, SocketAddress aLocalAddress, SocketAddress aPeerAddress)
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

SocketAddress Socket::GetAddr()
{
    if (mLocalAddress.IsEmpty()) {
        struct sockaddr sa{};
        socklen_t len = sizeof(sa);

        int res = getsockname(mHandle, &sa, &len);
        if (res < 0) {
            THROW_SYSTEM("getsockname() failed.");
        }
        mLocalAddress = SocketAddress(sa, len);
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

    return {*this, res, SocketAddress(sa, len), mLocalAddress};
}

Socket &Socket::Bind(const std::string &arAddr)
{
    SocketAddress sa(arAddr);
    sa.SetDomain(mDomain);

    if (sa.GetDomain() == Domain::Unix) {
        deleteOldSocketInode(sa);
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
    SocketAddress sa(arAddr);
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
    SocketAddress sa{};
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

int Socket::GetOptions(SockOptions aOption, int aLevel) const
{
    int result;
    socklen_t len = sizeof(result);
    int res = getsockopt(mHandle, aLevel, int(aOption), &result, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return result;
}

Socket &Socket::SetOptions(SockOptions aOption, int aValue, int aLevel)
{
    int res = setsockopt(mHandle, aLevel, int(aOption), &aValue, sizeof(aValue));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
    return *this;
}

std::chrono::system_clock::duration Socket::getTimeoutOption(SockOptions aOption) const
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

void Socket::setTimeoutOption(SockOptions aOption, std::chrono::system_clock::duration aValue) const
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

void Socket::deleteOldSocketInode(const SocketAddress &arAddr)
{
    std::filesystem::remove(arAddr.AsString());
}

} // rsp::posix
