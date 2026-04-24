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

#include <cctype>
#include <optional>
#include <ostream>
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

    size_t Write(std::string_view aString)
    {
        return Write({ reinterpret_cast<const std::byte*>(aString.data()), aString.size()});
    }

    size_t CopyFrom(IStreamDataProvider& arSource)
    {
        std::array<std::byte, 256> buffer{};
        size_t total_written = 0;

        while(auto sz = arSource.Read(buffer)) {
            total_written += Write({buffer.data(), sz});
        }

        return total_written;
    }

    /**
     * \brief Attempt to read data from the stream into the given buffer
     * \param aBuffer
     * \return The actual amount of bytes read into the buffer
     */
    [[nodiscard]] virtual size_t Read(std::span<std::byte> aBuffer) const = 0;

    /**
     * \brief Get the the size of this stream.
     * \return size_t
     */
    [[nodiscard]] virtual size_t GetStreamSize() const = 0;

    /**
     * \brief Rewind the stream, if possible.
     *        Useful for debugging, before calling PrintContent below.
     * \return self
     */
    virtual IStreamDataProvider& Rewind() = 0;

    /**
     * \brief Visualizer method. For debugging/trace output.
     * \param o ostream
     * \return self
     */
    virtual std::ostream& PrintContent(std::ostream& o) const
    {
        std::byte buffer[100];
        auto sp = std::span<std::byte>(buffer);
        while (auto written = Read(sp)) {
            for (size_t i = 0 ; i < written ; ++i) {
                auto c = int(buffer[i]);
                if (std::isprint(c) || c == '\n' || c == '\r') {
                    o << char(c);
                }
                else {
                    o << '.';
                }
            }
        }
        return o;
    }
};

inline std::ostream& operator<<(std::ostream& o, IStreamDataProvider &arStream)
{
    arStream.Rewind();
    return arStream.PrintContent(o);
}

} // namespace rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_I_STREAM_DATA_PROVIDER_H
