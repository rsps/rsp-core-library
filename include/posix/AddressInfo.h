/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_ADDRESSINFO_H
#define RSP_CORE_LIB_ADDRESSINFO_H

#include <string_view>
#include "SocketTypes.h"
#include "SocketAddress.h"

namespace rsp::posix {

class AddressInfo
{
public:
    AddressInfo() = default;
    AddressInfo(std::string_view aUrn, Domain aFamily = Domain::Unspecified, Type aType = Type::Unspecified,
                Protocol aProtocol = Protocol::Unspecified);
    AddressInfo(const AddressInfo&) = delete;
    AddressInfo(AddressInfo&&);
    ~AddressInfo();

    AddressInfo& operator=(const AddressInfo&) = delete;
    AddressInfo& operator=(AddressInfo&&);

    size_t GetCount();
    [[nodiscard]] const SocketAddress &operator[](size_t aIndex) const;

protected:
    struct addrinfo *mpAddrInfo = nullptr;

    static bool splitURN(std::string_view aUrn, std::string &arNode, std::string &arService);
};
} // rsp::posix

#endif //RSP_CORE_LIB_ADDRESSINFO_H
