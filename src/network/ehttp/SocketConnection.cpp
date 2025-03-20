/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "SocketConnection.h"
#include <network/NetworkException.h>
#include <network/UrlParser.h>

namespace rsp::network::ehttp {


SocketConnection& SocketConnection::SetOptions(const ConnectionOptions& arOptions)
{
    mOptions = arOptions;
    return *this;
}

SocketConnection& SocketConnection::Connect()
{
    using namespace rsp::posix;

    if (mSocket.IsConnected()) {
        return *this;
    }

    UrlParser up(mOptions.BaseUrl);

    auto urn = std::string(up.GetHost()) + ":" + std::to_string(static_cast<unsigned int>(up.GetPort()));
    AddressInfo ai(urn);

    mSocket = Socket(Domain::Inet, Type::Stream, Protocol::Unspecified);
    mSocket.SetSendTimeout(std::chrono::seconds(mOptions.ConnectionTimeout))
        .Connect(ai)
        .SetReceiveTimeout(std::chrono::seconds(mOptions.ResponseTimeout));

    if (up.RequiresTLS()) {
        mpTls = security::ITLSSocket::Create(mOptions);
        mpTls->SetSocket(mSocket);
    }

    return *this;
}

SocketConnection& SocketConnection::Close()
{
    mSocket.Close();
    mpTls = nullptr;
    return *this;
}

bool SocketConnection::IsConnected() const
{
    return mSocket.IsConnected();
}

bool SocketConnection::IsClosed() const
{
    return !mSocket.IsConnected();
}

size_t SocketConnection::Write(const std::span<const std::byte> aData)
{
    int retries = 3;
    for (;;) {
        try {
            if (mpTls) {
                return mpTls->Write(aData);
            }
            return mSocket.Send(aData);
        }
        catch (const network::ENetReconnect &e) {
            if (mpTls) {
                mpTls->Close();
            }
            mSocket.Close();
            Connect();
            if (--retries <= 0) {
                throw;
            }
        }
    }
}

size_t SocketConnection::Read(const std::span<std::byte> aBuffer)
{
    int retries = 3;
    for (;;) {
        try {
            if (mpTls) {
                return mpTls->Read(aBuffer);
            }
            return mSocket.Receive(aBuffer);
        }
        catch (const network::ENetReconnect &e) {
            if (mpTls) {
                mpTls->Close();
            }
            mSocket.Close();
            Connect();
            if (--retries <= 0) {
                throw;
            }
        }
    }
}

} // rsp::network::ehttp