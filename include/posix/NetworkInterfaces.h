/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_POSIX_NETWORK_INTERFACES_H
#define RSP_CORE_LIB_POSIX_NETWORK_INTERFACES_H

#include <string>
#include <vector>
#include <logging/LogChannel.h>

namespace rsp::posix {


/**
 * \class NetworkInterfaces
 * \brief Helper to lookup all network interfaces and group them as wireless and cabled
 */
class NetworkInterfaces
{
public:
    NetworkInterfaces();

    [[nodiscard]] const std::vector<std::string>& GetWireless() const;
    [[nodiscard]] const std::vector<std::string>& GetCabled() const;
protected:
    std::vector<std::string> mWireless{};
    std::vector<std::string> mCabled{};
    rsp::logging::LogChannel mLogger;
};

} /* namespace rsp::posix */

#endif // RSP_CORE_LIB_POSIX_NETWORK_INTERFACES_H
