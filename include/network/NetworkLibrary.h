/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_NETWORK_NETWORKLIBRARY_H_
#define INCLUDE_NETWORK_NETWORKLIBRARY_H_

#include <string_view>

namespace rsp::network {

class NetworkLibrary
{
public:
    virtual ~NetworkLibrary() {}

    static NetworkLibrary& Get();

    virtual std::string_view GetLibraryName() const = 0;
    virtual std::string_view GetVersion() const = 0;
    virtual std::string_view GetSslVersion() const = 0;
};

} // namespace rsp::network

#endif /* INCLUDE_NETWORK_NETWORKLIBRARY_H_ */
