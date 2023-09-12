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


struct APInfo
{
    std::string mSSID{};
    int  mSignalStrength = 0;
    bool mEncrypted = false;
};

class IWlanInterface
{
public:
    virtual ~IWlanInterface() {}

    virtual IWlanInterface& SetEnable(bool aEnable) = 0;
    virtual const APInfo& GetStatus() = 0;

    virtual std::vector<struct APInfo> GetAvailableNetworks() = 0;
    virtual std::vector<struct APInfo> GetKnownNetworks() = 0;
    virtual IWlanInterface& AddNetwork(const std::string &arSSID, const rsp::security::SecureString &arPassword) = 0;
    virtual IWlanInterface& RemoveNetwork(const std::string &arSSID) = 0;
    virtual IWlanInterface& SelectNetwork(const std::string &arSSID) = 0;
};

} /* namespace rsp::network */

#endif /* COMMON_CONTRACTS_IWLANINTERFACE_H_ */
