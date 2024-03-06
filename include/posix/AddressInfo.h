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
    explicit AddressInfo(std::string_view aUrn, bool aServer = false, Domain aFamily = Domain::Unspecified,
                         Type aType = Type::Unspecified, Protocol aProtocol = Protocol::Unspecified);

    size_t GetCount();
    [[nodiscard]] const SocketAddress &operator[](size_t aIndex) const;
    [[nodiscard]] const std::vector<SocketAddress> &GetAddresses() const;

protected:
    bool mServer;
    Domain mFamilyHint;
    Type mTypeHint;
    Protocol mProtocolHint;
    std::string mNode{};
    std::string mService{};
    std::vector<SocketAddress> mAddresses{};

    bool splitURN(std::string_view aUrn);
    void lookup();
};
} // rsp::posix

#endif //RSP_CORE_LIB_ADDRESSINFO_H
