/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef __linux__
    #include <netdb.h>
#elif defined(ESP_PLATFORM)
    #include <lwip/netdb.h>
#endif

#include "posix/AddressInfo.h"
#include <posix/Socket.h>

namespace rsp::posix {

AddressInfo::AddressInfo(std::string_view aUrn, bool aServer, Domain aFamily, Type aType, Protocol aProtocol)
    : mServer(aServer),
      mFamilyHint(aFamily),
      mTypeHint(aType),
      mProtocolHint(aProtocol)
{
    if (splitURN(aUrn)) {
        lookup();
    }
}

size_t AddressInfo::GetCount()
{
    return mAddresses.size();
}

const SocketAddress &AddressInfo::operator[](size_t aIndex) const
{
    return mAddresses.at(aIndex);
}

const std::vector<SocketAddress> &AddressInfo::GetAddresses() const
{
    return mAddresses;
}

bool AddressInfo::splitURN(std::string_view aUrn)
{
    size_t colon_pos;
    if (aUrn[0] == '/') {
        mNode = aUrn;
        mAddresses.emplace_back(aUrn, mTypeHint, mProtocolHint);
        return false;
    }

    if ((colon_pos = aUrn.find(']')) != std::string::npos) { // IPv6 with service/port
//        if (mFamilyHint == Domain::Unspecified) {
//            mFamilyHint = Domain::Inet6;
//        }
        mNode = aUrn.substr(0, colon_pos);
        mService = aUrn.substr(colon_pos + 2);
        return true;
    }

    if ((colon_pos = aUrn.find(':')) != std::string::npos) { // IPv4 with service/port
//        if (mFamilyHint == Domain::Unspecified) {
//            mFamilyHint = Domain::Inet;
//        }
        mNode = aUrn.substr(0, colon_pos);
        mService = aUrn.substr(colon_pos + 1);
        return true;
    }

    mNode = aUrn;
    return true;
}

void AddressInfo::lookup()
{
    struct addrinfo *p_info = nullptr;
    struct addrinfo *p_hints = nullptr;
    struct addrinfo hints{};
    if ((mFamilyHint != Domain::Unspecified) ||
        (mTypeHint != Type::Unspecified) ||
        (mProtocolHint != Protocol::Unspecified))
    {
        hints.ai_family = int(mFamilyHint);
        hints.ai_socktype = int(mTypeHint);
        hints.ai_protocol = int(mProtocolHint);
        p_hints = &hints;
    }
    if (mServer) {
        hints.ai_flags |= AI_PASSIVE; // | AI_CANONNAME;
        p_hints = &hints;
    }

    auto res = getaddrinfo(mNode.empty() ? nullptr : mNode.c_str(), mService.empty() ? nullptr : mService.c_str(),
                           p_hints, &p_info);
    if (res) {
        if (res == EAI_SYSTEM) {
            THROW_SYSTEM("Failed to parse socket address.");
        }
        THROW_WITH_BACKTRACE2(ESocketError, "getaddrinfo failed to parse socket address: ", gai_strerror(res));
    }

    auto p = p_info;
    while (p) {
        mAddresses.emplace_back(*(p->ai_addr), p->ai_addrlen, Domain(p->ai_family), Type(p->ai_socktype), Protocol(p->ai_protocol));
        p = p->ai_next;
    }

    if (p_info) {
        freeaddrinfo(p_info);
    }
}
} // rsp::posix
