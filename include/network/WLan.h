/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef COMMON_NETWORK_WLAN_H_
#define COMMON_NETWORK_WLAN_H_

#include "IWlanInterface.h"

namespace rsp::network {

class WLan: public IWlanInterface
{
public:
    WLan();

    APInfo GetStatus() override;
    IWlanInterface& SetEnable(bool aEnable) override;
    NetworkInfo AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword) override;
    IWlanInterface& SelectNetwork(const NetworkInfo &arNetwork) override;
    IWlanInterface& RemoveNetwork(const NetworkInfo &arNetwork) override;
    std::vector<NetworkInfo> GetKnownNetworks() override;
    std::vector<APInfo> GetAvailableNetworks() override;
    NetworkInfo FindNetwork(const std::string &arSSID) override;

protected:
    std::unique_ptr<IWlanInterface> mPimpl;
    static IWlanInterface* MakePimpl();
};

} /* namespace tgm::network */

#endif /* COMMON_NETWORK_WLAN_H_ */
