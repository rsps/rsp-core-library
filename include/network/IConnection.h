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

    /**
     * \brief Set the options to be used for this connection
     * \param arOptions
     * \return self
     */
    virtual IConnection& SetOptions(const ConnectionOptions& arOptions) = 0;

    /**
     * \brief Establish a connection to the base url in the options.
     * \return self
     */
    virtual IConnection& Connect() = 0;

    /**
     * \brief Close the connection.
     * \return self
     */
    virtual IConnection& Close() = 0;

    /**
     * \brief Check if a connection is established
     * \return boolean
     */
    [[nodiscard]] virtual bool IsConnected() const = 0;

    /**
     * \brief Check if a connection has been closed
     * \return boolean
     */
    [[nodiscard]] virtual bool IsClosed() const = 0;

    /**
     * \brief Attempt to write the given data to the connection
     * \param aData
     * \return The actual amount of data written in bytes
     */
    virtual size_t Write(std::span<std::byte const> aData) = 0;

    size_t Write(std::string_view aData)
    {
        return Write({ reinterpret_cast<const std::byte*>(aData.data()), aData.size() });
    }

    /**
     * \brief Attempt to read data into the given buffer
     * \param aBuffer
     * \return The actual amount of bytes read into the buffer
     */
    virtual size_t Read(std::span<std::byte> aBuffer) = 0;
};

} // namespace rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_I_CONNECTION_H
