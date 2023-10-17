/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <network/WLan.h>
#include <posix/NetworkInterfaces.h>
#include <posix/FileSystem.h>
#include <TestHelpers.h>
#include <cstdlib>

using namespace rsp::network;
using namespace rsp::posix;

static void FetchMonitorEvents(WLan &arWlan)
{
    rsp::network::WpaEvents event;
    do {
        std::string msg;
        CHECK_NOTHROW(event = arWlan.GetMonitorEvent(msg));
        MESSAGE("Monitor: (" << int(event) << ") " << msg);
    }
    while (event != rsp::network::WpaEvents::None);
}

static const char* GetEnv(const char *apName, const char *apDefault)
{
    const char *p = std::getenv(apName);
    if (p) {
        return p;
    }
    return apDefault;
}

TEST_CASE("WLAN")
{
    const char* cSSID = GetEnv("SSID", "MyWLan");
    const char* cPSK = GetEnv("PSK", "VerySecurePW");

    TestLogger logger;

    SUBCASE("Interfaces")
    {
        CHECK_NOTHROW(NetworkInterfaces dummy;);
        NetworkInterfaces ifs;

        std::stringstream out;
        out << "\nWireless:\n";
        for (auto &w : ifs.GetWireless()) {
            out << "  " << w << "\n";
        }

        out << "\nCabled:\n";
        for (auto &c : ifs.GetCabled()) {
            out << "  " << c << "\n";
        }

        MESSAGE(out.str());

        std::string wpa_dir("/var/run/wpa_supplicant/");
        std::string wifi_if(ifs.GetWireless()[0]);
        REQUIRE_MESSAGE(FileSystem::FileExists(wpa_dir + wifi_if), "The wpa_supplicant directory " << wpa_dir << wifi_if << " is not accessible by this program/user");
    }

    SUBCASE("Constructors") {
        NetworkInterfaces ifs;
        MESSAGE("Using interface " << ifs.GetWireless()[0] << " with ssid=" << std::string(cSSID) << " and psk=" << std::string(cPSK));
        CHECK_NOTHROW(WLan());
    }

    SUBCASE("Scan")
    {
        WLan wlan;
        CHECK_NOTHROW(
            auto networks = wlan.GetAvailableNetworks();
            for (const APInfo &info : networks) {
                MESSAGE(info.mSSID << ", " << info.mSignalStrength << ", " << info.mEncrypted);
            }
        );

        FetchMonitorEvents(wlan);
    }

    SUBCASE("Status")
    {
        WLan wlan;
        CHECK_NOTHROW(
            auto info = wlan.GetStatus();
            MESSAGE("Status: " << info.mSSID << ", " << info.mSignalStrength << ", " << info.mEncrypted << ", " << info.mIpAddress << ", " << info.mMacAddress);
        );
    }

    SUBCASE("AddNetwork")
    {
        WLan wlan;
        NetworkInfo network;
        CHECK_NOTHROW(
            network = wlan.AddNetwork(cSSID, cPSK);
        );
        MESSAGE("Network: " << network.mId << ", " << network.mSSID);
        CHECK_EQ(network.mSSID, std::string(cSSID));
        CHECK_NE(network.mId, uint32_t(-1));
    }

    SUBCASE("ListNetworks")
    {
        WLan wlan;
        CHECK_NOTHROW(
            auto networks = wlan.GetKnownNetworks();
            for (auto &network : networks) {
                MESSAGE("Network: " << network.mId << ", " << network.mSSID);
            }
        );
    }

    SUBCASE("SelectNetwork")
    {
        WLan wlan;
        CHECK_THROWS_AS(wlan.SelectNetwork(wlan.FindNetwork(std::string("Not") + cSSID)), EWlanException);
        CHECK_NOTHROW(wlan.SelectNetwork(wlan.FindNetwork(cSSID)));

        NetworkInfo network;
        CHECK_NOTHROW(network = wlan.FindNetwork(cSSID));
        CHECK(network.mSelected);

        FetchMonitorEvents(wlan);
    }

    SUBCASE("Enable")
    {
        WLan wlan;

        FetchMonitorEvents(wlan);

        NetworkInfo network;
        CHECK_NOTHROW(network = wlan.FindNetwork(cSSID));
        CHECK(network.mSelected);

        CHECK_NOTHROW(wlan.SetEnable(false));
        FetchMonitorEvents(wlan);

        CHECK_NOTHROW(network = wlan.FindNetwork(cSSID));
        CHECK_FALSE(network.mSelected);

        CHECK_NOTHROW(wlan.SetEnable(true));
        CHECK_NOTHROW(wlan.SelectNetwork(wlan.FindNetwork(cSSID)));
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        FetchMonitorEvents(wlan);

        CHECK_NOTHROW(network = wlan.FindNetwork(cSSID));
        CHECK(network.mSelected);
    }

    SUBCASE("Connect")
    {
        if (std::string("MyWLan").compare(0, 6, cSSID) != 0) {
            WLan wlan;

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            FetchMonitorEvents(wlan);

            APInfo status;
            CHECK_NOTHROW(status = wlan.GetStatus());
            CHECK_EQ(status.mStatus, WpaStatus::Completed);

            CHECK_NOTHROW(wlan.Disconnect());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            FetchMonitorEvents(wlan);

            CHECK_NOTHROW(status = wlan.GetStatus());
            CHECK_NE(status.mStatus, WpaStatus::Completed);

            CHECK_NOTHROW(wlan.Reconnect());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            FetchMonitorEvents(wlan);

            CHECK_NOTHROW(status = wlan.GetStatus());
            CHECK_EQ(status.mStatus, WpaStatus::Completed);
        }
    }

    SUBCASE("Get IP")
    {
        if (std::string("MyWLan").compare(0, 6, cSSID) != 0) {
            WLan wlan;

            CHECK_NOTHROW(wlan.AquireIP());

            FetchMonitorEvents(wlan);
            auto info = wlan.GetStatus();
            CHECK_EQ(info.mSSID, std::string(cSSID));
            CHECK_FALSE(info.mIpAddress.empty());

            MESSAGE("Obtained IP: " << info.mIpAddress);

            CHECK_NOTHROW(wlan.ReleaseIP());

            FetchMonitorEvents(wlan);
            info = wlan.GetStatus();
            CHECK_EQ(info.mSSID, std::string(cSSID));
            CHECK(info.mIpAddress.empty());

            rsp::network::WpaEvents event;
            do {
                std::string msg;
                CHECK_NOTHROW(event = wlan.GetMonitorEvent(msg));
                MESSAGE("Monitor: (" << int(event) << ") " << msg);
            }
            while (event != rsp::network::WpaEvents::None);
        }
    }

    SUBCASE("RemoveNetwork")
    {
        WLan wlan;
        CHECK_NOTHROW(wlan.RemoveNetwork(wlan.FindNetwork(cSSID)));
        FetchMonitorEvents(wlan);
    }

    SUBCASE("Monitor")
    {
        WLan wlan;
        CHECK_NOTHROW(FetchMonitorEvents(wlan));
    }
}
