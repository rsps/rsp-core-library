/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifdef USE_WPA_SUPPLICANT

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <logging/LogChannel.h>
#include <network/WLan.h>
#include <posix/FileSystem.h>
#include <posix/NetworkInterfaces.h>
#include <utils/HexStream.h>
#include <utils/StrUtils.h>
#include <wpa_ctrl.h>
#include "WpaSupplicant.h"

using namespace rsp::posix;
using namespace rsp::security;
using namespace rsp::utils;

namespace rsp::network {

Unsolicited::Unsolicited()
    : mLogger("WpaSupplicant")
{
}

Unsolicited& Unsolicited::Get()
{
    static Unsolicited instance;
    return instance;
}

bool Unsolicited::HasMessages() const
{
    return mMessages.size() > 0;
}

std::string Unsolicited::GetNext()
{
    std::string result = mMessages.front();
    mMessages.erase(mMessages.cbegin());
    return result;
}

Unsolicited& Unsolicited::Clear()
{
    mMessages.clear();
    return *this;
}

void Unsolicited::unsolicitedHandler(char *msg, size_t len)
{
    auto &ref = Get().mMessages.emplace_back(msg, len);
    Get().mLogger.Debug() << "Unsolicited reply: " << ref;
}



WpaSupplicant::WpaSupplicant()
    : mLogger("WpaSupplicant")
{
    NetworkInterfaces ifs;
    mInterfaceName = ifs.GetWireless()[0];
    std::string socket = std::string("/var/run/wpa_supplicant/") + mInterfaceName;

    mpWpaCtrl = wpa_ctrl_open(socket.c_str());
    if (!mpWpaCtrl) {
        THROW_WITH_BACKTRACE1(EWlanException, "Could not connect to wpa_supplicant on " + socket);
    }

    if (!ping()) {
        THROW_WITH_BACKTRACE1(EWlanException, "wpa_supplicant is not running");
    }
}

WpaSupplicant::~WpaSupplicant()
{
    if (mpWpaCtrl) {
        wpa_ctrl_close(mpWpaCtrl);
    }
}

std::vector<APInfo> WpaSupplicant::GetAvailableNetworks()
{
    std::vector<APInfo> result;
    std::string reply = request("SCAN");
    if (reply.compare(0, 2, "OK") == 0) {

        // Wait a little to allow for scanning to commence
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Available networks are tickling in as their SSID's are broadcast. See "beacon interval"
        // Poor mans solution: Loop repeatedly until result is stable
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::string s = request("SCAN_RESULTS");
            if (reply == s) {
                break;
            }
            reply = s;
        };

        const std::string header("bssid / frequency / signal level / flags / ssid\n");
        if (!StrUtils::StartsWith(reply, header)) {

            mLogger.Error() << "Bad scan_result header:\n"
                << HexStream(reinterpret_cast<const uint8_t*>(reply.data()), header.length(), 1)
                << "\n !=\n"
                << HexStream(reinterpret_cast<const uint8_t*>(header.data()), header.length(), 1);

            THROW_WITH_BACKTRACE1(EWlanException, "Wrong header in scan result");
        }

        std::vector<std::string> lines;
        if (StrUtils::Split(reply, lines, '\n') < 2) {
            return result;
        }
        lines.erase(lines.begin());

        for (const std::string &line : lines) {
            std::vector<std::string> fields;
            if (StrUtils::Split(line, fields, '\t') != 5) {
                THROW_WITH_BACKTRACE1(EWlanException, "Wrong field count in scan result: " + std::to_string(fields.size()));
            }

            std::string ssid = fields[4];
            int strength = std::stoi(fields[2]);
            bool encrypted = StrUtils::Contains(fields[3], "WPA2-PSK-CCMP");

            auto it = std::find_if(result.begin(), result.end(), [&ssid](const APInfo &arInfo) {
                return arInfo.mSSID == ssid;
            });

            if (it == result.end()) {
                result.emplace_back(ssid, strength, encrypted);
            }
            else if (it->mSignalStrength < strength) {
                it->mSignalStrength = strength;
            }
        }
    }

    return result;
}

IWlanInterface& WpaSupplicant::SetEnable(bool aEnable)
{
    std::string command("dhclient " + mInterfaceName);
    if (!aEnable) {
        command += " -r";
    }
    mLogger.Debug() << "Executing " << command;
    std::string result;
    std::string errors;
    FileSystem::ExecuteCommand(command, &result, &errors);
    mLogger.Debug() << "dhclient:  " << result << "\n  Errors: (" << errors << ")";

    return *this;
}

APInfo WpaSupplicant::GetStatus()
{
    std::string reply = request("STATUS");
    reply += request("SIGNAL_POLL");
    std::vector<std::string> lines;
    StrUtils::Split(reply, lines, '\n');

    APInfo status;
    for (auto &line : lines) {
        std::vector<std::string> fields;
        StrUtils::Split(line, fields, '=');

        if (fields[0] == "ssid") {
            status.mSSID = fields[1];
        }
        else if(fields[0] == "key_mgmt") {
            status.mEncrypted = StrUtils::Contains(fields[1], "WPA2-PSK");
        }
        else if(fields[0] == "ip_address") {
            status.mIpAddress = fields[1];
        }
        else if(fields[0] == "address") {
            status.mMacAddress = fields[1];
        }
        else if(fields[0] == "RSSI") {
            status.mSignalStrength = std::stoi(fields[1]);
        }
    }

    return status;
}

std::vector<NetworkInfo> WpaSupplicant::GetKnownNetworks()
{
    std::vector<NetworkInfo> result;
    std::string reply = request("LIST_NETWORKS");

    const std::string header("network id / ssid / bssid / flags\n");
    if (!StrUtils::StartsWith(reply, header)) {
        THROW_WITH_BACKTRACE1(EWlanException, "Wrong header in network list");
    }

    std::vector<std::string> lines;
    StrUtils::Split(reply, lines, '\n');
    lines.erase(lines.begin());

    for (auto &line : lines) {
        std::vector<std::string> fields;
        StrUtils::Split(line, fields, '\t');
        result.emplace_back(std::stoul(fields[0]), fields[1]);
    }

    return result;
}

NetworkInfo WpaSupplicant::AddNetwork(const std::string &arSSID, const SecureString &arPassword)
{
    std::string id = request("ADD_NETWORK");
    StrUtils::RightTrim(id);

    std::string reply = request(std::string("SET_NETWORK ") + id + " ssid \"" + arSSID + "\"");
    if (!StrUtils::StartsWith(reply, "OK")) {
        THROW_WITH_BACKTRACE1(EWlanException, "Failed to set ssid (" + arSSID + ") on network " + id);
    }

    reply = request(std::string("SET_NETWORK ") + id + " psk \"" + std::string(arPassword) + "\"");
    if (!StrUtils::StartsWith(reply, "OK")) {
        THROW_WITH_BACKTRACE1(EWlanException, "Failed to set psk on network " + id);
    }

    save(arSSID);

    return NetworkInfo(std::stoul(id), arSSID);
}

IWlanInterface& WpaSupplicant::SelectNetwork(const NetworkInfo &arNetwork)
{
    std::string reply = request("SELECT_NETWORK " + std::to_string(arNetwork.mId));
    if (!StrUtils::StartsWith(reply, "OK")) {
        THROW_WITH_BACKTRACE1(EWlanException, "Failed select network " + arNetwork.mSSID);
    }
    save(arNetwork.mSSID);
    return *this;
}

IWlanInterface& WpaSupplicant::RemoveNetwork(const NetworkInfo &arNetwork)
{
    std::string reply = request("REMOVE_NETWORK " + std::to_string(arNetwork.mId));
    if (!StrUtils::StartsWith(reply, "OK")) {
        THROW_WITH_BACKTRACE1(EWlanException, "Failed select network " + arNetwork.mSSID);
    }
    save(arNetwork.mSSID);
    return *this;
}


std::string WpaSupplicant::request(std::string_view aCmd)
{
    char mReplyBuffer[4096];
    size_t reply_len = sizeof(mReplyBuffer);

    mLogger.Debug() << "Request " << aCmd;
    int res = wpa_ctrl_request(mpWpaCtrl, aCmd.data(), aCmd.size(), mReplyBuffer, &reply_len, &Unsolicited::unsolicitedHandler);
    if (res < 0) {
        THROW_WITH_BACKTRACE1(EWlanException, "wpa_ctrl_request failed: " + std::to_string(res));
    }

    std::string result(mReplyBuffer, reply_len);

    while (Unsolicited::Get().HasMessages()) {
        // TODO: Add prober handling...
        mLogger.Debug() << "Unsolicited message: " << Unsolicited::Get().GetNext();
    }

    mLogger.Debug() << "Reply: " << result << ";";
//    mLogger.Debug() << "Reply: " << HexStream(reinterpret_cast<const uint8_t*>(result.data()), std::min(96ul, result.size()), 1) << ";";
    return result;
}

NetworkInfo WpaSupplicant::FindNetwork(const std::string &arSSID)
{
    auto networks = GetKnownNetworks();
    auto it = std::find_if(networks.begin(), networks.end(), [&arSSID](const NetworkInfo &arInfo) {
        return arInfo.mSSID == arSSID;
    });

    if (it == networks.end()) {
        THROW_WITH_BACKTRACE1(EWlanException, "No network found with SSID " + arSSID);
    }
    return *it;
}

bool WpaSupplicant::ping()
{
    return (request("PING").compare(0, 4, "PONG") == 0);
}

void WpaSupplicant::save(const std::string &arSSID)
{
    std::string reply = request(std::string("SAVE_CONFIG"));
    if (!StrUtils::StartsWith(reply, "OK")) {
        THROW_WITH_BACKTRACE1(EWlanException, "Failed to save network configuration for " + arSSID);
    }
    mLogger.Debug() << "Config saved.";
}


IWlanInterface* WLan::MakePimpl()
{
    return new WpaSupplicant();
}

} /* namespace rsp::network */

#endif /* USE_WPA_SUPPLICANT */