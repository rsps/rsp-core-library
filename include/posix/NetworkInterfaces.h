/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_POSIX_NETWORKINTERFACES_H_
#define INCLUDE_POSIX_NETWORKINTERFACES_H_

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

    const std::vector<std::string>& GetWireless() const;
    const std::vector<std::string>& GetCabled() const;
protected:
    std::vector<std::string> mWireless{};
    std::vector<std::string> mCabled{};
    rsp::logging::LogChannel mLogger;

    bool isWireless(const std::string &arInterfaceName);
};

} /* namespace rsp::posix */

#endif /* INCLUDE_POSIX_NETWORKINTERFACES_H_ */
