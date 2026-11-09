/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#include <arpa/inet.h>
#include <charconv>
#include <fcntl.h>
#include <format>
#include <rsp/posix/FileSystem.h>
#include <rsp/posix/Socket.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <utility>

namespace rsp::posix {

namespace {

#ifdef SO_PEERCRED
PeerCredentials getPeerCredentials(int aHandle)
{
    ucred creds{};
    socklen_t len = sizeof(creds);
    if (getsockopt(aHandle, SOL_SOCKET, SO_PEERCRED, &creds, &len) < 0) {
        THROW_SYSTEM("getsockopt(SO_PEERCRED) failed.");
    }
    return {creds.pid, creds.uid, creds.gid};
}
#endif

} // namespace

Socket::Socket(Domain aDomain, Type aType, Protocol aProtocol)
{
    mHandle = socket(std::to_underlying(aDomain), std::to_underlying(aType), std::to_underlying(aProtocol));
    if (!mHandle) {
        THROW_SYSTEM("socket() failed.");
    }
    mDomain = aDomain;
    mType = aType;
    mProtocol = aProtocol;
}

Socket::~Socket()
{
    try {
        Close();
    }
    catch (const std::exception& e) {
    }
}

Socket::Socket(const Socket& arServer, int aHandle, const SocketAddress& arLocalAddress, const SocketAddress& arPeerAddress)
    : mHandle(aHandle)
    , mLocalAddress(arLocalAddress)
    , mPeerAddress(arPeerAddress)
    , mDomain(arServer.mDomain)
    , mType(arServer.mType)
    , mProtocol(arServer.mProtocol)
{
}

Socket::Socket(Socket&& arOther) noexcept
    : mHandle(std::move(arOther.mHandle))
    , mLocalAddress(arOther.mLocalAddress)
    , mPeerAddress(arOther.mPeerAddress)
    , mDomain(arOther.mDomain)
{
}

Socket& Socket::operator=(Socket&& arOther) noexcept
{
    if (this != &arOther) {
        mHandle.Close();
        mHandle = std::move(arOther.mHandle);
        mLocalAddress = arOther.mLocalAddress;
        mPeerAddress = arOther.mPeerAddress;
        mDomain = arOther.mDomain;
    }
    return *this;
}

bool Socket::IsConnected() const
{
    return !mPeerAddress.IsEmpty() && bool(mHandle);
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

Socket& Socket::SetBroadcastAllowed(bool aAllow)
{
    SetOptions(SockOptions::Broadcast, static_cast<int>(aAllow));
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

Socket& Socket::SetKeepAlive(bool aEnable)
{
    SetOptions(SockOptions::KeepAlive, static_cast<int>(aEnable));
    return *this;
}

size_t Socket::GetReceiveBufSize() const
{
    int result = GetOptions(SockOptions::ReceiveBufSize);
    return static_cast<size_t>(result);
}

Socket& Socket::SetReceiveBufSize(size_t aSize)
{
    SetOptions(SockOptions::ReceiveBufSize, static_cast<int>(aSize));
    return *this;
}

std::chrono::system_clock::duration Socket::GetReceiveTimeout() const
{
    return getTimeoutOption(SockOptions::ReceiveTimeout);
}

Socket& Socket::SetReceiveTimeout(std::chrono::system_clock::duration aTimeout)
{
    setTimeoutOption(SockOptions::ReceiveTimeout, aTimeout);
    return *this;
}

size_t Socket::GetSendBufSize() const
{
    int result = GetOptions(SockOptions::SendBufSize);
    return size_t(result);
}

Socket& Socket::SetSendBufSize(size_t aSize)
{
    SetOptions(SockOptions::SendBufSize, static_cast<int>(aSize));
    return *this;
}

std::chrono::system_clock::duration Socket::GetSendTimeout() const
{
    return getTimeoutOption(SockOptions::SendTimeout);
}

Socket& Socket::SetSendTimeout(std::chrono::system_clock::duration aTimeout)
{
    setTimeoutOption(SockOptions::SendTimeout, aTimeout);
    return *this;
}

SocketAddress Socket::GetAddr()
{
    if (mLocalAddress.IsEmpty()) {
        sockaddr_storage ss{};
        socklen_t len = sizeof(ss);

        int res = getsockname(mHandle.Get(), reinterpret_cast<sockaddr*>(&ss), &len);
        if (res < 0) {
            THROW_SYSTEM("getsockname() failed.");
        }
        mDomain = static_cast<Domain>(ss.ss_family);
        mLocalAddress = SocketAddress(reinterpret_cast<const sockaddr&>(ss), len, mDomain, mType, mProtocol);
    }
    return mLocalAddress;
}

#ifdef SO_PEERCRED
PeerCredentials Socket::GetPeerCredentials() const
{
    return getPeerCredentials(mHandle.Get());
}
#endif

Socket Socket::Accept()
{
    sockaddr_storage ss{};
    socklen_t len = sizeof(ss);

    int res = accept(mHandle.Get(), reinterpret_cast<sockaddr*>(&ss), &len);
    if (res < 0) {
        THROW_SYSTEM(std::format("accept() failed. Handle: {}", mHandle.Get()));
    }

    SocketAddress peer(reinterpret_cast<const sockaddr&>(ss), len, static_cast<Domain>(ss.ss_family), mType, mProtocol);

    std::string peer_name = peer.GetCanonicalName();
#ifdef SO_PEERCRED
    if (peer.GetUnixKind() == UnixKind::Anonymous) {
        try {
            auto creds = getPeerCredentials(res);
            peer_name += std::format(" (pid: {}, uid: {}, gid: {})", creds.Pid, creds.Uid, creds.Gid);
        }
        catch (const std::exception&) { // Credentials are only informational
        }
    }
#endif
    mLogger.Info() << "Accepted connection from: " << peer_name;

    return {*this, res, mLocalAddress, peer};
}

Socket& Socket::Bind(const AddressInfo& arAddrInfo, bool aBindAll)
{
    bool first = true;
    int res = 0;
    int err = 0;
    for (auto& sa : arAddrInfo.GetAddresses()) {
        if (sa.GetUnixKind() == UnixKind::Named) {
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

Socket& Socket::Connect(const AddressInfo& arAddrInfo)
{
    int res = -1;
    for (size_t i = 0; i < arAddrInfo.GetCount(); ++i) {
        auto& sa = arAddrInfo[i];
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

Socket& Socket::Listen(size_t aAcceptQueueSize)
{
    int res = listen(mHandle.Get(), static_cast<int>(aAcceptQueueSize));
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


size_t Socket::Receive(std::string& arStringBuffer, int aFlags) const
{
    return Receive({reinterpret_cast<std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

size_t Socket::ReceiveFrom(Socket& arPeer, std::span<std::byte> aBuffer, int aFlags) const
{
    sockaddr_storage ss{};
    socklen_t len = sizeof(ss);

    ssize_t res = recvfrom(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags, reinterpret_cast<sockaddr*>(&ss), &len);
    if (res == -1) {
        THROW_SYSTEM("recvfrom() failed.");
    }
    arPeer.mLocalAddress = SocketAddress(reinterpret_cast<const sockaddr&>(ss), len, static_cast<Domain>(ss.ss_family), mType, mProtocol);
    return size_t(res);
}

size_t Socket::ReceiveFrom(Socket& arPeer, std::string& arStringBuffer, int aFlags) const
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

size_t Socket::Send(const std::string& arStringBuffer, int aFlags) const
{
    return Send({reinterpret_cast<const std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

size_t Socket::SendTo(const Socket& arPeer, std::span<const std::byte> aBuffer, int aFlags) const
{
    ssize_t res = sendto(mHandle.Get(), aBuffer.data(), aBuffer.size_bytes(), aFlags, &arPeer.mLocalAddress.Get(), arPeer.mLocalAddress.GetSize());
    if (res == -1) {
        THROW_SYSTEM("sendto() failed.");
    }
    return size_t(res);
}

size_t Socket::SendTo(const Socket& arPeer, const std::string& arStringBuffer, int aFlags) const
{
    return SendTo(arPeer, {reinterpret_cast<const std::byte*>(arStringBuffer.data()), arStringBuffer.size()}, aFlags);
}

bool Socket::IsDataReady() const
{
    pollfd fd{};
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
    int res = getsockopt(mHandle.Get(), aLevel, std::to_underlying(aOption), &result, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return result;
}

Socket& Socket::SetOptions(SockOptions aOption, int aValue, int aLevel)
{
    int res = setsockopt(mHandle.Get(), aLevel, std::to_underlying(aOption), &aValue, sizeof(aValue));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
    return *this;
}

std::chrono::system_clock::duration Socket::getTimeoutOption(SockOptions aOption) const
{
    using namespace std::chrono;
    timeval tv{};
    socklen_t len = sizeof(tv);
    int res = getsockopt(mHandle.Get(), SOL_SOCKET, std::to_underlying(aOption), &tv, &len);
    if (res < 0) {
        THROW_SYSTEM("getsockopt() failed.");
    }
    return (seconds(tv.tv_sec + (tv.tv_usec / 1000000)) + microseconds(tv.tv_usec % 1000000));
}

void Socket::setTimeoutOption(SockOptions aOption, std::chrono::system_clock::duration aValue) const
{
    using namespace std::chrono;
    timeval tv{};
    tv.tv_sec = static_cast<time_t>(duration_cast<seconds>(aValue).count());
    tv.tv_usec = static_cast<suseconds_t>(duration_cast<microseconds>(aValue).count() % 1000000);
    int res = setsockopt(mHandle.Get(), SOL_SOCKET, std::to_underlying(aOption), &tv, sizeof(tv));
    if (res < 0) {
        THROW_SYSTEM("setsockopt() failed.");
    }
}

Socket& Socket::Shutdown(ShutdownFlags aFlag)
{
    int res = shutdown(mHandle.Get(), std::to_underlying(aFlag));
    if ((res < 0) && (errno != ENOTCONN)) {
        THROW_SYSTEM("shutdown() failed.");
    }
    return *this;
}

Socket& Socket::Close()
{
    if (IsConnected()) {
        Shutdown(ShutdownFlags::ReadWrite);
        mPeerAddress.SetDomain(Domain::Unspecified); // Mark as unconnected
    }
    return *this;
}

void Socket::deleteOldSocketINode(const SocketAddress& arAddr)
{
    FileSystem::DeleteFile(arAddr.AsString());
}

bool Socket::IsBlocking() const
{
    long arg;
    if ((arg = fcntl(mHandle.Get(), F_GETFL, NULL)) < 0) {
        THROW_SYSTEM("fcntl() failed.");
    }
    return ((arg & O_NONBLOCK) == 0);
}

Socket& Socket::SetBlocking(bool aBlocking)
{
    long arg;
    if ((arg = fcntl(mHandle.Get(), F_GETFL, NULL)) < 0) {
        THROW_SYSTEM("fcntl() failed.");
    }

    if (aBlocking) {
        arg &= ~O_NONBLOCK;
    }
    else {
        arg |= O_NONBLOCK;
    }

    if (fcntl(mHandle.Get(), F_SETFL, arg) < 0) {
        THROW_SYSTEM("fcntl() failed.");
    }

    return *this;
}

} // namespace rsp::posix
