/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_NETWORK_NETWORK_LIBRARY_H
#define RSP_CORE_LIB_NETWORK_NETWORK_LIBRARY_H

#include <string_view>

namespace rsp::network {

/**
 * \class NetworkLibrary
 * \brief Interface for a singleton network library.
 *
 * Implementations must implement the static Get factory and override the rest.
 */
class NetworkLibrary
{
public:
    virtual ~NetworkLibrary() = default;

    /**
     * \fn NetworkLibrary Get&()
     * \brief Factory for the library. This allows for lazy instantiation and initialization of the library.
     *
     * \return self
     */
    static NetworkLibrary& Get();

    /**
     * \fn std::string_view GetLibraryName()const =0
     * \brief Get the name of the underlying library
     *
     * \return string
     */
    [[nodiscard]] virtual std::string_view GetLibraryName() const = 0;

    /**
     * \fn std::string_view GetVersion()const =0
     * \brief Get the version of the underlying library
     *
     * \return string
     */
    [[nodiscard]] virtual std::string_view GetVersion() const = 0;

    /**
     * \fn std::string_view GetSslVersion()const =0
     * \brief Get the version of the SSL library used
     *
     * \return
     */
    [[nodiscard]] virtual std::string_view GetSslVersion() const = 0;
};

} // namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_NETWORK_LIBRARY_H
