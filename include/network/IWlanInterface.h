/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef COMMON_CONTRACTS_IWLANINTERFACE_H_
#define COMMON_CONTRACTS_IWLANINTERFACE_H_

#include <string>
#include <vector>
#include <exceptions/CoreException.h>
#include <security/SecureString.h>

namespace rsp::network {

class EWlanException: public rsp::exceptions::CoreException
{
public:
    explicit EWlanException(const char *apMsg)
        : CoreException(apMsg)
    {
    }
    EWlanException(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};


enum class WpaStatus { None, Disabled, Inactive, Scanning, Authenticating, Disconnected, Completed };

struct APInfo
{
    std::string mSSID;
    std::string mIpAddress;
    std::string mMacAddress;
    int  mSignalStrength;
    bool mEncrypted;
    uint32_t mNetworkId = 0;
    WpaStatus mStatus = WpaStatus::None;

    APInfo(const std::string &arSSID = std::string(), int aStrength = 0, bool aEncrypted = false, const std::string &arIpAddr = std::string(), const std::string &arMacAddr = std::string())
        : mSSID(arSSID),
          mIpAddress(arIpAddr),
          mMacAddress(arMacAddr),
          mSignalStrength(aStrength),
          mEncrypted(aEncrypted)
    {
    }

    APInfo(const APInfo&) = default;
    APInfo(APInfo&&) = default;
    APInfo& operator=(const APInfo&) = default;
    APInfo& operator=(APInfo&&) = default;
};

struct NetworkInfo
{
    uint32_t mId;
    std::string mSSID;
    bool mSelected;

    NetworkInfo(uint32_t aId = uint32_t(-1), const std::string &arSSID = std::string(), bool aSelected = false) : mId(aId), mSSID(arSSID), mSelected(aSelected) {}

    NetworkInfo(const NetworkInfo&) = default;
    NetworkInfo(NetworkInfo&&) = default;
    NetworkInfo& operator=(const NetworkInfo&) = default;
    NetworkInfo& operator=(NetworkInfo&&) = default;
};

enum class WpaEvents { None, Connected, Disconnected, AuthRejected, APScanStarted, APScanComplete, Other };

class IWlanInterface
{
public:
    virtual ~IWlanInterface() {}

    virtual IWlanInterface& SetEnable(bool aEnable) = 0;

    virtual APInfo GetStatus() = 0;

    virtual IWlanInterface& Disconnect() = 0;
    virtual IWlanInterface& Reconnect() = 0;

    virtual std::vector<struct APInfo> GetAvailableNetworks() = 0;
    virtual std::vector<NetworkInfo> GetKnownNetworks() = 0;
    virtual NetworkInfo AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword) = 0;
    virtual IWlanInterface& RemoveNetwork(const NetworkInfo &arNetwork) = 0;
    virtual IWlanInterface& SelectNetwork(const NetworkInfo &arNetwork) = 0;
    virtual NetworkInfo FindNetwork(const std::string &arSSID) = 0;

    virtual std::string AquireIP() = 0;
    virtual IWlanInterface& ReleaseIP() = 0;

    virtual WpaEvents GetMonitorEvent(std::string &arMessage) = 0;
};

} /* namespace rsp::network */

#endif /* COMMON_CONTRACTS_IWLANINTERFACE_H_ */
