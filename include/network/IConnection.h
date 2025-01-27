/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_I_CONNECTION_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_I_CONNECTION_H

#include <span>
#include "ConnectionOptions.h"

namespace rsp::network {

/**
 * \brief Interface for network connections
 */
class IConnection
{
public:
    virtual ~IConnection() = default;

    virtual IConnection& SetOptions(const ConnectionOptions& arOptions) = 0;

    /**
     * \brief Establish the connection.
     * \return
     */
    virtual bool Connect() = 0;
    [[nodiscard]] virtual bool IsConnected() const = 0;
    [[nodiscard]] virtual bool IsClosed() const = 0;

    virtual void Write(const std::span<std::byte const> aData) = 0;
    virtual void Read(const std::span<std::byte> aBuffer) = 0;
};

} // namespace rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_I_CONNECTION_H
