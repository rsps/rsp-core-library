/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_SOCKET_CONNECTION_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_SOCKET_CONNECTION_H

#include <network/IConnection.h>
#include <posix/Socket.h>
#include <security/ITLSSocket.h>

namespace rsp::network::ehttp {

class SocketConnection : public rsp::network::IConnection
{
public:
    SocketConnection& SetOptions(const ConnectionOptions& arOptions) override;
    SocketConnection& Connect() override;
    SocketConnection& Close() override;
    [[nodiscard]] bool IsConnected() const override;
    [[nodiscard]] bool IsClosed() const override;
    size_t Write(std::span<const std::byte> aData) override;
    using IConnection::Write;
    size_t Read(std::span<std::byte> aBuffer) override;

protected:
    ConnectionOptions mOptions{};
    posix::Socket mSocket{};
    std::shared_ptr<security::ITLSSocket> mpTls{};

    void destroy();
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_SOCKET_CONNECTION_H
