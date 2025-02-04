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
#include <posix/Socket.h>
#include <sys/poll.h>
#include <unistd.h>
#include <posix/FileSystem.h>
#include <arpa/inet.h>
#include <sys/types.h>

namespace rsp::posix {

Socket::Socket(Domain aDomain, Type aType, Protocol aProtocol)
{
    mHandle = socket(int(aDomain), int(aType), int(aProtocol));
    if (!mHandle) {
        THROW_SYSTEM("socket() failed.");
    }
    mDomain = aDomain;
    mType = aType;
    mProtocol = aProtocol;
}

Socket::~Socket()
{
    Close();
}

Socket::Socket(const Socket &arServer, int aHandle, const SocketAddress& arLocalAddress, const SocketAddress& arPeerAddress)
    : mHandle(aHandle),
      mLocalAddress(arLocalAddress),
      mPeerAddress(arPeerAddress),
      mDomain(arServer.mDomain),
      mType(arServer.mType),
      mProtocol(arServer.mProtocol)
{
}

Socket::Socket(Socket &&arOther) noexcept
    : mHandle(std::move(arOther.mHandle)),
      mLocalAddress(arOther.mLocalAddress),
      mPeerAddress(arOther.mPeerAddress),
      mDomain(arOther.mDomain)
{
}

Socket& Socket::operator=(Socket &&arOther) noexcept
{
    if (this != &arOther) {
        mHandle = std::move(arOther.mHandle);
        mLocalAddress = arOther.mLocalAddress;
        mPeerAddress = arOther.mPeerAddress;
        mDomain = arOther.mDomain;
    }
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

        int res = getsockname(mHandle.Get(), &sa, &len);
        if (res < 0) {
            THROW_SYSTEM("getsockname() failed.");
        }
        mLocalAddress = SocketAddress(sa, len, mDomain, mType, mProtocol);
        mDomain = Domain(sa.sa_family);
    }
    return mLocalAddress;
}

Socket Socket::Accept()
{
    SocketAddress result;
    socklen_t len = result.GetSize();

    int res = accept(mHandle.Get(), &result.Get(), &len);
    if (res < 0) {
        THROW_SYSTEM("accept() failed. Handle: " + std::to_string(mHandle.Get()));
    }
    result.UpdateFromParent(mLocalAddress);

    mLogger.Info() << "Accepted connection from: " << result.GetCanonicalName();

    return {*this, res, result, mLocalAddress};
}

Socket &Socket::Bind(const AddressInfo &arAddrInfo, bool aBindAll)
{
    bool first = true;
    int res = 0;
    int err = 0;
    for (auto &sa : arAddrInfo.GetAddresses()) {
        if (sa.GetDomain() == Domain::Unix) {
            deleteOldSocketINode(sa);
        }
        res = bind(mHandle.Get(), &sa.Get(), sa.GetSize());
        if (res == 0) {
            if (first) {
                mLocalAddress = sa;
                first = false;
            }
            mLogger.Notice() << "Socket bound to: " << sa.GetCanonicalName();
            if (!aBindAll) {
                break;
            }
        }
        else {
            err = errno;
            mLogger.Warning() << "(" << err << ") Socket could not bind to " << sa.GetCanonicalName();
        }
    }
    if (res < 0) {
        errno = err;
        THROW_SYSTEM("bind() failed.");
    }
    return *this;
}

Socket &Socket::Connect(const AddressInfo &arAddrInfo)
{
    int res;
    for (size_t i=0 ; i < arAddrInfo.GetCount() ; ++i) {
        auto &sa = arAddrInfo[i];
        res = connect(mHandle.Get(), &sa.Get(), sa.GetSize());
        if (res == 0) {
            mPeerAddress = sa;
            break;
        }
    }
    if (res < 0) {
        THROW_SYSTEM("connect() failed.");
    }
    return *this;
}

Socket &Socket::Listen(size_t aAcceptQueueSize)
{
    int res = listen(mHandle.Get(), int(aAcceptQueueSize));
    if (res < 0) {
        THROW_SYSTEM("listen() failed.");
    }
    return *this;
}

size_t Socket::Receive(std::span<std::byte> aBuffer, int aFlags) const
{
    ssize_t res = recv(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags);
    if (res == -1) {
        THROW_SYSTEM("recv() failed.");
    }
    return size_t(res);
}


size_t Socket::Receive(std::string &arStringBuffer, int aFlags) const
{
    return Receive({reinterpret_cast<std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

size_t Socket::ReceiveFrom(Socket &arPeer, std::span<std::byte> aBuffer, int aFlags) const
{
    SocketAddress sa{};
    socklen_t len = sizeof(sa);

    ssize_t res = recvfrom(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags, &sa.Get(), &len);
    if (res == -1) {
        THROW_SYSTEM("recvfrom() failed.");
    }
    arPeer.mLocalAddress = sa;
    return size_t(res);
}

size_t Socket::ReceiveFrom(Socket &arPeer, std::string &arStringBuffer, int aFlags) const
{
    return ReceiveFrom(arPeer, {reinterpret_cast<std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

size_t Socket::Send(std::span<const std::byte> aBuffer, int aFlags) const
{
    ssize_t res = send(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags);
    if (res == -1) {
        THROW_SYSTEM("send() failed.");
    }
    return size_t(res);
}

size_t Socket::Send(const std::string &arStringBuffer, int aFlags) const
{
    return Send({reinterpret_cast<const std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

size_t Socket::SendTo(const Socket &arPeer, std::span<const std::byte> aBuffer, int aFlags) const
{
    ssize_t res = sendto(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags, &arPeer.mLocalAddress.Get(), sizeof(arPeer.mLocalAddress));
    if (res == -1) {
        THROW_SYSTEM("sendto() failed.");
    }
    return size_t(res);
}

size_t Socket::SendTo(const Socket &arPeer, const std::string &arStringBuffer, int aFlags) const
{
    return SendTo(arPeer, {reinterpret_cast<const std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

bool Socket::IsDataReady() const
{
    struct pollfd fd{};
    int ret;
    bool result = false;
    int timeout = 0;

    fd.fd = mHandle.Get();
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
    return mHandle.Get();
}

int Socket::GetOptions(SockOptions aOption, int aLevel) const
{
    int result;
    socklen_t len = sizeof(result);
    int res = getsockopt(mHandle.Get(), aLevel, int(aOption), &result, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return result;
}

Socket &Socket::SetOptions(SockOptions aOption, int aValue, int aLevel)
{
    int res = setsockopt(mHandle.Get(), aLevel, int(aOption), &aValue, sizeof(aValue));
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
    int res = getsockopt(mHandle.Get(), SOL_SOCKET, int(aOption), &tv, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return (seconds(tv.tv_sec + (tv.tv_usec / 1000000)) + microseconds(tv.tv_usec % 1000000));
}

void Socket::setTimeoutOption(SockOptions aOption, std::chrono::system_clock::duration aValue) const
{
    using namespace std::chrono;
    struct timeval tv{};
    tv.tv_sec = time_t(duration_cast<seconds>(aValue).count());
    tv.tv_usec = suseconds_t(duration_cast<microseconds>(aValue).count() % 1000000);
    int res = setsockopt(mHandle.Get(), SOL_SOCKET, int(aOption), &tv, sizeof(tv));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
}

Socket& Socket::Shutdown(ShutdownFlags aFlag)
{
    int res = shutdown(mHandle.Get(), int(aFlag));
    if (res < 0) {
        THROW_SYSTEM("shutdown() failed.");
    }
    return *this;
}

Socket& Socket::Close()
{
    if (IsConnected()) {
        Shutdown(ShutdownFlags::ReadWrite);
    }
    return *this;
}

void Socket::deleteOldSocketINode(const SocketAddress &arAddr)
{
    FileSystem::DeleteFile(arAddr.AsString());
}

} // rsp::posix
