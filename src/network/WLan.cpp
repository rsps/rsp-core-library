/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <network/WLan.h>

namespace rsp::network {

WLan::WLan()
    : mPimpl(MakePimpl())
{
}

IWlanInterface& WLan::SetEnable(bool aEnable)
{
    mPimpl->SetEnable(aEnable);
    return *this;
}

const APInfo& WLan::GetStatus()
{
    return mPimpl->GetStatus();
}

IWlanInterface& WLan::AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword)
{
    mPimpl->AddNetwork(arSSID, arPassword);
    return *this;
}

IWlanInterface& WLan::SelectNetwork(const std::string &arSSID)
{
    mPimpl->SelectNetwork(arSSID);
    return *this;
}

std::vector<APInfo> WLan::GetKnownNetworks()
{
    return mPimpl->GetKnownNetworks();
}

std::vector<APInfo> WLan::GetAvailableNetworks()
{
    return mPimpl->GetAvailableNetworks();
}

IWlanInterface& WLan::RemoveNetwork(const std::string &arSSID)
{
    mPimpl->RemoveNetwork(arSSID);
    return *this;
}

} /* namespace tgm::network */
