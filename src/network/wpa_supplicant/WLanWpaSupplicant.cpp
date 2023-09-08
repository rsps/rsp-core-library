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
#include <network/WLan.h>
#include <wpa_ctrl.h>

namespace rsp::network {

class Request
{
public:
    Request(struct wpa_ctrl *apWpaCtrl, std::string_view aCmd)
    {
        if (mpInstance) {
            THROW_WITH_BACKTRACE1(EWlanException, "A wpa_supplicant request is already pending");
        }
        mpInstance = this;

        size_t reply_len = sizeof(mReplyBuffer);

        int res = wpa_ctrl_request(apWpaCtrl, aCmd.data(), aCmd.size(), mReplyBuffer, &reply_len, &Request::callback);

        if (res < 0) {
            THROW_WITH_BACKTRACE1(EWlanException, "wpa_ctrl_request failed: " + std::to_string(res));
        }
    }

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

    static void callback(char *msg, size_t len)
    {
        mpInstance->mPromise.set_value(std::string(msg, len));
        mpInstance = nullptr; // Free callback lock
    }
};

Request* Request::mpInstance = nullptr;

class WLanWpaSupplicant : public IWlanInterface
{
public:
    WLanWpaSupplicant()
    {
        mpWpaCtrl = wpa_ctrl_open("/var/run/wpa_supplicant/");
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

    IWlanInterface& Disconnect() override
    {
        return *this;
    }

    IWlanInterface& Connect(const std::string &arSSID, const rsp::security::SecureString &arPassword) override
    {
        return *this;
    }

    std::vector<APInfo> GetNetworkList() override
    {
        std::vector<APInfo> result;
        mpRequest = std::make_unique<Request>(mpWpaCtrl, "SCAN");

        std::string scan = mpRequest->GetFuture().get();

        std::cout << scan << std::endl;

        return result;
    }

    IWlanInterface& SetEnable(bool aEnable) override
    {
        return *this;
    }

    const APInfo& GetStatus() override
    {
        // TODO: Refresh before returning
        return mStatus;
    }

protected:
    struct wpa_ctrl *mpWpaCtrl = nullptr;
    APInfo mStatus{};
    std::unique_ptr<Request> mpRequest{};

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

