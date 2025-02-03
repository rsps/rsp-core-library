/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_I_STREAM_DATA_PROVIDER_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_I_STREAM_DATA_PROVIDER_H

#include <span>

namespace rsp::network {

class IStreamDataProvider
{
public:
    virtual ~IStreamDataProvider() = default;

    /**
     * \brief Attempt to write the given data to the stream
     * \param aData
     * \return The actual amount of data written in bytes
     */
    virtual size_t Write(std::span<std::byte const> aData) = 0;

    /**
     * \brief Attempt to read data from the stream into the given buffer
     * \param aBuffer
     * \return The actual amount of bytes read into the buffer
     */
    virtual size_t Read(std::span<std::byte> aBuffer) = 0;
};

} // namespace rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_I_STREAM_DATA_PROVIDER_H
