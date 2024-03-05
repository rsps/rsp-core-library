/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <netdb.h>
#include "posix/AddressInfo.h"

namespace rsp::posix {

AddressInfo::AddressInfo(std::string_view aUrn, Domain aFamily, Type aType, Protocol aProtocol)
{
}

AddressInfo::AddressInfo(AddressInfo&& arOther)
    : mpAddrInfo(arOther.mpAddrInfo)
{
    arOther.mpAddrInfo = nullptr;
}

AddressInfo& AddressInfo::operator=(AddressInfo&& arOther)
{
    mpAddrInfo = arOther.mpAddrInfo;
    arOther.mpAddrInfo = nullptr;
}

} // rsp::posix
