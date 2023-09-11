/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <thread>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <logging/LogChannel.h>
#include <network/WLan.h>
#include <wpa_ctrl.h>

using namespace rsp::security;

namespace rsp::network {

class Request
{
public:
    Request(struct wpa_ctrl *apWpaCtrl, std::string_view aCmd)
        : mLogger("WLan")
    {
        if (mpInstance) {
            THROW_WITH_BACKTRACE1(EWlanException, "A wpa_supplicant request is already pending");
        }
        mpInstance = this;

        size_t reply_len = sizeof(mReplyBuffer);

        mLogger.Debug() << "Request " << aCmd;
        int res = wpa_ctrl_request(apWpaCtrl, aCmd.data(), aCmd.size(), mReplyBuffer, &reply_len, &Request::callback);

        if (res < 0) {
            THROW_WITH_BACKTRACE1(EWlanException, "wpa_ctrl_request failed: " + std::to_string(res));
        }

        mLogger.Debug() << "Reply2: " << reinterpret_cast<const char*>(&mReplyBuffer);
    }

    // TODO: Refactor: callback is only for unsolicited replies...

    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    std::future<std::string> GetFuture()
    {
        return mPromise.get_future();
    }

protected:
    char mReplyBuffer[4096];
    std::promise<std::string> mPromise{};
    static Request* mpInstance;
    rsp::logging::LogChannel mLogger;

    static void callback(char *msg, size_t len)
    {
        mpInstance->mLogger.Debug() << "Reply: " << msg;
        mpInstance->mPromise.set_value(std::string(msg, len));
        mpInstance = nullptr; // Free callback lock
    }
};

Request* Request::mpInstance = nullptr;

class WLanWpaSupplicant : public IWlanInterface
{
public:
    WLanWpaSupplicant()
        : mLogger("WLan")
    {
        mpWpaCtrl = wpa_ctrl_open("/var/run/wpa_supplicant/wlx200db0144e41");
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
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "SCAN");

        std::string scan = mpRequest->GetFuture().get();
        std::cout << scan << std::endl;

        return result;
    }

    IWlanInterface& SetEnable(bool aEnable) override
    {
        if (aEnable) {
            mpRequest = std::make_unique<Request>(mpWpaCtrl, "RECONNECT");
        }
        else {
            mpRequest = std::make_unique<Request>(mpWpaCtrl, "DISCONNECT");
        }
        std::string result = mpRequest->GetFuture().get();
        std::cout << result << std::endl;

        return *this;
    }

    const APInfo& GetStatus() override
    {
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "STATUS");
        std::string result = mpRequest->GetFuture().get();
        std::cout << result << std::endl;

        return mStatus;
    }

    IWlanInterface& AddNetwork(const std::string &arSSID, const SecureString &arPassword) override
    {
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "ADD_NETWORK");
        std::string id = mpRequest->GetFuture().get();
        std::cout << id << std::endl;

        mpRequest = std::make_unique<Request>(mpWpaCtrl, std::string("SET_NETWORK ") + id + " SSID " + arSSID);
        std::string result = mpRequest->GetFuture().get();
        std::cout << result << std::endl;

        mpRequest = std::make_unique<Request>(mpWpaCtrl, std::string("SET_NETWORK ") + id + " PSK " + std::string(arPassword));
        result = mpRequest->GetFuture().get();
        std::cout << result << std::endl;

        return *this;
    }

    IWlanInterface& SelectNetwork(const std::string &arSSID) override
    {
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "SELECT_NETWORK");
        std::string scan = mpRequest->GetFuture().get();
        std::cout << scan << std::endl;
        return *this;
    }

    std::vector<APInfo> GetKnownNetworks() override
    {
        std::vector<APInfo> result;
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "LIST_NETWORKS");

        std::string scan = mpRequest->GetFuture().get();
        std::cout << scan << std::endl;

        return result;
    }

    IWlanInterface& RemoveNetwork(const std::string &arSSID) override
    {
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "REMOVE_NETWORK");
        std::string scan = mpRequest->GetFuture().get();
        std::cout << scan << std::endl;
        return *this;
    }

protected:
    struct wpa_ctrl *mpWpaCtrl = nullptr;
    APInfo mStatus{};
    std::unique_ptr<Request> mpRequest{};
    rsp::logging::LogChannel mLogger;

    WLanWpaSupplicant(const WLanWpaSupplicant&) = delete;
    WLanWpaSupplicant& operator=(const WLanWpaSupplicant&) = delete;

    bool ping()
    {
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "PING");
        return (mpRequest->GetFuture().get() == std::string("PONG"));
    }
};

IWlanInterface* WLan::MakePimpl()
{
    return new WLanWpaSupplicant();
}


} /* namespace rsp::network */

