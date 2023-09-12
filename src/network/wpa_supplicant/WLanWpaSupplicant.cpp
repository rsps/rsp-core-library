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
#include <utils/HexStream.h>
#include <utils/StrUtils.h>
#include <wpa_ctrl.h>

using namespace rsp::posix;
using namespace rsp::security;
using namespace rsp::utils;

namespace rsp::network {

class Unsolicited
{
public:
    static Unsolicited& Get()
    {
        static Unsolicited instance;
        return instance;
    }

    bool HasMessages() const
    {
        return mMessages.size() > 0;
    }

    std::string GetNext()
    {
        std::string result = mMessages.front();
        mMessages.erase(mMessages.cbegin());
        return result;
    }

    Unsolicited& Clear()
    {
        mMessages.clear();
        return *this;
    }

    static void unsolicitedHandler(char *msg, size_t len)
    {
        auto &ref = Get().mMessages.emplace_back(msg, len);
        Get().mLogger.Debug() << "Unsolicited reply: " << ref;
    }

protected:
    std::list<std::string> mMessages{};
    rsp::logging::LogChannel mLogger;

private:
    Unsolicited()
        : mLogger("WpaSupplicant")
    {
    }
};


class WLanWpaSupplicant : public IWlanInterface
{
public:
    WLanWpaSupplicant()
        : mLogger("WpaSupplicant")
    {
        std::string socket;

        std::vector<std::filesystem::path> net_phys = FileSystem::Glob("/sys/class/ieee80211/*", false, true);
        for (auto &path : net_phys) {
            if (FileSystem::DirectoryExists(path.native() + "/device/net/")) {
                std::vector<std::filesystem::path> net_ifs = FileSystem::Glob(path.native() + "/device/net/*", false, true);
                if (net_ifs.size() > 0) {
                    socket = std::string("/var/run/wpa_supplicant/") + net_ifs[0].stem().native();
                    break;
                }
            }
        }

        mLogger.Debug() << "Socket: " << socket;

        mpWpaCtrl = wpa_ctrl_open(socket.c_str());
        if (!mpWpaCtrl) {
            THROW_WITH_BACKTRACE1(EWlanException, "Could not connect to wpa_supplicant");
        }

        if (!ping()) {
            THROW_WITH_BACKTRACE1(EWlanException, "wpa_supplicant is not running");
        }
    }

    ~WLanWpaSupplicant() override
    {
        if (mpWpaCtrl) {
            wpa_ctrl_close(mpWpaCtrl);
        }
    }

    std::vector<APInfo> GetAvailableNetworks() override
    {
        std::vector<APInfo> result;
        std::string reply = request("SCAN");
        if (reply.compare(0, 2, "OK") == 0) {

            // Wait for scan results to stabilize...
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
                    APInfo &info = result.emplace_back();
                    info.mSSID = ssid;
                    info.mSignalStrength = strength;
                    info.mEncrypted = encrypted;
                }
                else if (it->mSignalStrength < strength) {
                    it->mSignalStrength = strength;
                }
            }
        }

        return result;
    }

    IWlanInterface& SetEnable(bool aEnable) override
    {
        std::string result;
        if (aEnable) {
            result = request("RECONNECT");
        }
        else {
            result = request("DISCONNECT");
        }

        return *this;
    }

    const APInfo& GetStatus() override
    {
        std::string result = request("STATUS");

        return mStatus;
    }

    IWlanInterface& AddNetwork(const std::string &arSSID, const SecureString &arPassword) override
    {
        std::string id = request("ADD_NETWORK");

        std::string result = request(std::string("SET_NETWORK ") + id + " SSID " + arSSID);

        request(std::string("SET_NETWORK ") + id + " PSK " + std::string(arPassword));

        return *this;
    }

    IWlanInterface& SelectNetwork(const std::string &arSSID) override
    {
        std::string result = request("SELECT_NETWORK " + arSSID);

        return *this;
    }

    std::vector<APInfo> GetKnownNetworks() override
    {
        std::vector<APInfo> result;
        std::string reply = request("LIST_NETWORKS");

        return result;
    }

    IWlanInterface& RemoveNetwork(const std::string &arSSID) override
    {
        std::string result = request("REMOVE_NETWORK " + arSSID);
        return *this;
    }

protected:
    struct wpa_ctrl *mpWpaCtrl = nullptr;
    APInfo mStatus{};
    rsp::logging::LogChannel mLogger;
    char mReplyBuffer[4096];

    WLanWpaSupplicant(const WLanWpaSupplicant&) = delete;
    WLanWpaSupplicant& operator=(const WLanWpaSupplicant&) = delete;

    std::string request(std::string_view aCmd)
    {
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
//        mLogger.Debug() << "Reply: " << HexStream(reinterpret_cast<const uint8_t*>(result.data()), std::min(96ul, result.size()), 1) << ";";
        return result;
    }

    bool ping()
    {
        return (request("PING").compare(0, 4, "PONG") == 0);
    }
};

IWlanInterface* WLan::MakePimpl()
{
    return new WLanWpaSupplicant();
}


} /* namespace rsp::network */

#endif /* USE_WPA_SUPPLICANT */
