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

APInfo WLan::GetStatus()
{
    return mPimpl->GetStatus();
}

NetworkInfo WLan::AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword)
{
    return mPimpl->AddNetwork(arSSID, arPassword);
}

IWlanInterface& WLan::SelectNetwork(const NetworkInfo &arNetwork)
{
    mPimpl->SelectNetwork(arNetwork);
    return *this;
}

std::vector<NetworkInfo> WLan::GetKnownNetworks()
{
    return mPimpl->GetKnownNetworks();
}

std::vector<APInfo> WLan::GetAvailableNetworks()
{
    return mPimpl->GetAvailableNetworks();
}

IWlanInterface& WLan::RemoveNetwork(const NetworkInfo &arNetwork)
{
    mPimpl->RemoveNetwork(arNetwork);
    return *this;
}

rsp::network::NetworkInfo WLan::FindNetwork(const std::string &arSSID)
{
    return mPimpl->FindNetwork(arSSID);
}

WpaEvents WLan::GetMonitorEvent(std::string &arMessage)
{
    return mPimpl->GetMonitorEvent(arMessage);
}

} /* namespace tgm::network */
