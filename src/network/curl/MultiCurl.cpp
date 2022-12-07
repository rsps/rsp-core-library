/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <functional>
#include "MultiCurl.h"
#include <logging/Logger.h>
#include <network/HttpRequest.h>

using namespace rsp::logging;

namespace rsp::network::curl
{

MultiCurl::MultiCurl()
{
    mpMultiHandle = curl_multi_init();
}

MultiCurl::~MultiCurl()
{
    curl_multi_cleanup(mpMultiHandle);
}

MultiCurl& MultiCurl::Add(CurlSessionHttpRequest &arRequest)
{
    static_cast<EasyCurl*>(&arRequest)->prepareRequest(); // EasyCurl is friendly

    Logger::GetDefault().Debug() << "Adding Request: " << arRequest.GetOptions().BaseUrl << arRequest.GetOptions().Uri << std::endl;

    CURLMcode mc = curl_multi_add_handle(mpMultiHandle, reinterpret_cast<CURL*>(arRequest.GetHandle()));
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_add_handle() failed.", mc);
    }

    return *this;
}

MultiCurl& MultiCurl::Remove(CurlSessionHttpRequest &arRequest)
{
    Logger::GetDefault().Debug() << "Removing Request: " << arRequest.GetOptions().BaseUrl << arRequest.GetOptions().Uri << std::endl;

    CURLMcode mc = curl_multi_remove_handle(mpMultiHandle, reinterpret_cast<CURL*>(arRequest.GetHandle()));
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_remove_handle() failed.", mc);
    }

    return *this;
}

void MultiCurl::Execute()
{
    long timeout = 0;
    CURLMcode mc = curl_multi_timeout(mpMultiHandle, &timeout);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_timeout() failed.", mc);
    }
    timeout = (timeout < 0) ? 5000 : timeout;

    Logger::GetDefault().Debug() << "Executing MultiCurl with timeout: " << timeout << std::endl;

    int count = 0;
    do {
        count = perform();
        if (count > 0 && poll(timeout) == 0) {
            continue;
        }

        processMessages();
    }
    while(count > 0);
}

int MultiCurl::poll(int aTimeoutMs)
{
    int num_fds;
    CURLMcode mc = curl_multi_wait(mpMultiHandle, nullptr, 0, aTimeoutMs, &num_fds);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_poll() failed.", mc);
    }
    return num_fds;
}

int MultiCurl::perform()
{
    int still_running;
    CURLMcode mc = curl_multi_perform(mpMultiHandle, &still_running);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_perform() failed.", mc);
    }

    return still_running;
}

void MultiCurl::processMessages()
{
    int msgs_in_queue = 0;
    do {
        CURLMsg *msg = curl_multi_info_read(mpMultiHandle, &msgs_in_queue);
        if (msg && msg->msg == CURLMSG_DONE) {
            if (msg->data.result > 0 && msg->data.result < 100) {
                THROW_WITH_BACKTRACE2(ECurlError, "curl_multi failed.", msg->data.result);
            }
            auto req = EasyCurl::GetFromHandle(msg->easy_handle);
            Remove(*static_cast<CurlSessionHttpRequest*>(req));
            req->requestDone();
        }
    }
    while(msgs_in_queue > 0);
}

} /* namespace rsp::network::curl */
