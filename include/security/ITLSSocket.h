/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_POSIX_I_TLS_SOCKET_H
#define RSP_CORE_LIB_INCLUDE_POSIX_I_TLS_SOCKET_H

#include <memory>
#include <posix/Socket.h>
#include <network/ConnectionOptions.h>
#include <string_view>

namespace rsp::security {

class ITLSSocket
{
public:
    static std::shared_ptr<ITLSSocket> Create(network::ConnectionOptions& arOptions);

    virtual ~ITLSSocket() = default;

    virtual ITLSSocket& SetSocket(posix::Socket &arSocket) = 0;
    virtual ITLSSocket& Close() = 0;

    virtual size_t Write(std::span<std::byte const> aData) = 0;
    virtual size_t Read(std::span<std::byte> aData) = 0;
};


} // namespace rsp::security

#endif //RSP_CORE_LIB_INCLUDE_POSIX_I_TLS_SOCKET_H
