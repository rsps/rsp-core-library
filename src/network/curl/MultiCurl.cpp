/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "MultiCurl.h"
#include "CurlHttpRequest.h"

namespace rsp::network::curl
{

MultiCurl::MultiCurl()
{
    mpMultiHandle = curl_multi_init();
    CURLMcode mc = curl_multi_timeout(mpMultiHandle, &mMaxTimeout);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_timeout() failed.", mc);
    }
}

MultiCurl::~MultiCurl()
{
    curl_multi_cleanup(mpMultiHandle);
}

MultiCurl& MultiCurl::Add(IHttpRequest *apRequest)
{
    CurlHttpRequest *p = reinterpret_cast<CurlHttpRequest*>(apRequest);

    CURLMcode mc = curl_multi_add_handle(mpMultiHandle, p->mpCurl);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_add_handle() failed.", mc);
    }

    return *this;
}

MultiCurl& MultiCurl::Remove(IHttpRequest *apRequest)
{
    CurlHttpRequest *p = reinterpret_cast<CurlHttpRequest*>(apRequest);

    CURLMcode mc = curl_multi_remove_handle(mpMultiHandle, p->mpCurl);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_remove_handle() failed.", mc);
    }

    return *this;
}

bool MultiCurl::Poll(int aTimeoutMs)
{
    int num_fds;
    CURLMcode mc = curl_multi_poll(mpMultiHandle, nullptr, 0, aTimeoutMs, &num_fds);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_poll() failed.", mc);
    }
    return (num_fds > 0);
}

int MultiCurl::Perform()
{
    int still_running;
    CURLMcode mc = curl_multi_perform(mpMultiHandle, &still_running);
    if (mc != CURLM_OK) {
        THROW_WITH_BACKTRACE2(ECurlMError, "curl_multi_perform() failed.", mc);
    }

    return still_running;
}

void MultiCurl::Execute()
{
    while (Perform()) {
        if (!Poll(mMaxTimeout)) {
            continue;
        }

        int msgs_in_queue = 0;
        do {

            CURLMsg *msg = curl_multi_info_read(mpMultiHandle, &msgs_in_queue);
            if (msg && msg->msg == CURLMSG_DONE)
            {
                CurlHttpRequest *req = static_cast<CurlHttpRequest*>(EasyCurl::GetFromHandle(msg->easy_handle));

                // TODO: Handle response of easy request.
            }
        } while(msgs_in_queue > 0);
    }
}

} /* namespace rsp::network::curl */
