/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "EasyCurl.h"
#include <logging/Logger.h>

namespace rsp::network::curl {

EasyCurl::EasyCurl()
{
    mpCurl = curl_easy_init();
    if (!mpCurl) {
        THROW_WITH_BACKTRACE1(ECurlError, "Curl initialization failed.");
    }
}

EasyCurl::~EasyCurl()
{
    if (mpForm) {
        curl_mime_free(mpForm);
    }
    if (mpHeaders) {
        curl_slist_free_all(mpHeaders);
    }
    if (mpCurl) {
        curl_easy_cleanup(mpCurl);
    }
}

EasyCurl::EasyCurl(const EasyCurl &arOther)
{
    DLOG("EasyCurl Copy Construct")
    *this = arOther;
}

EasyCurl::EasyCurl(EasyCurl &&arOther) noexcept
{
    DLOG("EasyCurl Move Construct")
    *this = std::move(arOther);
}

void EasyCurl::prepareRequest()
{
    // Clear request to avoid old headers to be persistent.
    curl_easy_reset(mpCurl);

    setCurlOption(CURLOPT_ERRORBUFFER, mErrorBuffer);

    setCurlOption(CURLOPT_PRIVATE, this);
}

EasyCurl& EasyCurl::operator =(const EasyCurl &arOther)
{
    DLOG("EasyCurl Copy Assignment")
    if (this != &arOther) {
        mpCurl = curl_easy_duphandle(arOther.mpCurl);
    }
    return *this;
}

EasyCurl& EasyCurl::operator =(EasyCurl &&arOther) noexcept
{
    DLOG("EasyCurl Move Assignment")
    if (this != &arOther) {
        mpCurl = arOther.mpCurl;
        arOther.mpCurl = nullptr;
        mpForm = arOther.mpForm;
        arOther.mpForm = nullptr;
        mpHeaders = arOther.mpHeaders;
        arOther.mpHeaders = nullptr;
    }
    return *this;
}

EasyCurl* EasyCurl::GetFromHandle(CURL* apHandle)
{
    EasyCurl *p;
    auto err = curl_easy_getinfo(apHandle, CURLINFO_PRIVATE, &p);
    if (err != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "Curl private pointer GET failed.", err);
    }
    if (!p) {
        THROW_WITH_BACKTRACE1(ECurlError, "Curl private pointer was not set.");
    }
    return p;
}

curl_mime* EasyCurl::getForm()
{
    if (!mpForm) {
        mpForm = curl_mime_init(mpCurl);
    }
    return mpForm;
}

} /* namespace rsp::network::http::curl */
