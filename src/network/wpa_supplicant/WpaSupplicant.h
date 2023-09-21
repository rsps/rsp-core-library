/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_WPA_SUPPLICANT_WPASUPPLICANT_H_
#define SRC_NETWORK_WPA_SUPPLICANT_WPASUPPLICANT_H_

#include <list>
#include <string>
#include <logging/LogChannel.h>
#include <network/IWlanInterface.h>
#include <security/SecureString.h>

namespace rsp::network {


class WpaSupplicant : public IWlanInterface
{
public:
    WpaSupplicant();
    ~WpaSupplicant() override;

    std::vector<APInfo> GetAvailableNetworks() override;
    IWlanInterface& SetEnable(bool aEnable) override;
    APInfo GetStatus() override;
    std::vector<NetworkInfo> GetKnownNetworks() override;
    NetworkInfo AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword) override;
    IWlanInterface& SelectNetwork(const NetworkInfo &arNetwork) override;
    IWlanInterface& RemoveNetwork(const NetworkInfo &arNetwork) override;
    NetworkInfo FindNetwork(const std::string &arSSID) override;

    WpaEvents GetMonitorEvent(std::string &arMessage) override;
    IWlanInterface& Reconnect() override;
    IWlanInterface& Disconnect() override;
    IWlanInterface& ReleaseIP() override;
    std::string AquireIP() override;

protected:
    struct wpa_ctrl *mpWpaCtrl = nullptr;
    struct wpa_ctrl *mpMonitorCtrl = nullptr;
    rsp::logging::LogChannel mLogger;
    std::string mInterfaceName{};

    WpaSupplicant(const WpaSupplicant&) = delete;
    WpaSupplicant& operator=(const WpaSupplicant&) = delete;

    std::string request(std::string_view aCmd);
    bool ping();
    void save(const std::string &arSSID);
    void runCommand(const std::string &arCommand);
    WpaStatus parseWpaStatus(const std::string &arStatus) const;
};


} /* namespace rsp::network */

#endif /* SRC_NETWORK_WPA_SUPPLICANT_WPASUPPLICANT_H_ */
