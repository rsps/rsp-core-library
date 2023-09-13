/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <posix/NetworkInterfaces.h>
#include <exceptions/CoreException.h>
#include <cstring>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>

namespace rsp::posix {

NetworkInterfaces::NetworkInterfaces()
    : mLogger("posix")
{
    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1) {
        THROW_SYSTEM("getifaddrs failed");
    }

    // Walk through linked list of interfaces
    for (struct ifaddrs *ifa = ifaddr ; ifa != nullptr ; ifa = ifa->ifa_next) {

        if ((ifa->ifa_addr == nullptr) || (ifa->ifa_addr->sa_family != AF_PACKET)) {
            continue;
        }

        std::string if_name(ifa->ifa_name, IFNAMSIZ);
        if_name.resize(std::strlen(if_name.c_str()));

        if (isWireless(if_name)) {
            mWireless.emplace_back(if_name);
        }
        else {
            mCabled.emplace_back(if_name);
        }
    }

    freeifaddrs(ifaddr);
}

const std::vector<std::string>& NetworkInterfaces::GetWireless() const
{
    return mWireless;
}

const std::vector<std::string>& NetworkInterfaces::GetCabled() const
{
    return mCabled;
}

bool NetworkInterfaces::isWireless(const std::string &arInterfaceName)
{
    bool result = false;
    int sock = -1;
    struct iwreq pwrq;
    std::memset(&pwrq, 0, sizeof(pwrq));
    std::strncpy(pwrq.ifr_name, arInterfaceName.c_str(), IFNAMSIZ);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        THROW_SYSTEM("socket failed");
    }

    if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
//        char protocol[IFNAMSIZ] = { 0 };
//        std::strncpy(protocol, pwrq.u.name, IFNAMSIZ);
//        mLogger.Debug() << "Interface " << arInterfaceName << " is wireless, protocol " << std::string(protocol);
        result = true;
    }

    close(sock);
    return result;
}

} /* namespace rsp::posix */
