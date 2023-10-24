/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_WPA_SUPPLICANT_WPA_SUPPLICANT_H
#define RSP_CORE_LIB_SRC_NETWORK_WPA_SUPPLICANT_WPA_SUPPLICANT_H

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
    WpaSupplicant(const WpaSupplicant&) = delete;
    ~WpaSupplicant() override;

    WpaSupplicant& operator=(const WpaSupplicant&) = delete;

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
    std::string AcquireIp() override;

protected:
    struct wpa_ctrl *mpWpaCtrl = nullptr;
    struct wpa_ctrl *mpMonitorCtrl = nullptr;
    rsp::logging::LogChannel mLogger;
    std::string mInterfaceName{};

    std::string request(std::string_view aCmd);
    bool ping();
    void save(const std::string &arSSID);
    void runCommand(const std::string &arCommand);
    static WpaStatus parseWpaStatus(const std::string &arStatus);
};


} /* namespace rsp::network */

#endif // RSP_CORE_LIB_SRC_NETWORK_WPA_SUPPLICANT_WPA_SUPPLICANT_H
