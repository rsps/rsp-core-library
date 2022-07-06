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

namespace rsp::network::curl {

EasyCurl::EasyCurl()
{
    mpCurl = curl_easy_init();
    if (!mpCurl) {
        THROW_WITH_BACKTRACE1(ECurlError, "Curl initialization failed.");
    }

    auto err = curl_easy_setopt(mpCurl, CURLOPT_ERRORBUFFER, mErrorBuffer);
    if (err != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "Curl error buffer initialization failed.", err);
    }

    err = curl_easy_setopt(mpCurl, CURLOPT_PRIVATE, this);
    if (err != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "Curl private pointer SET failed.", err);
    }
}

EasyCurl::~EasyCurl()
{
    if (mpCurl) {
        curl_easy_cleanup(mpCurl);
    }
}

EasyCurl::EasyCurl(const EasyCurl &arOther)
{
    *this = arOther;
}

EasyCurl::EasyCurl(EasyCurl &&arOther)
{
    *this = std::move(arOther);
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

EasyCurl& EasyCurl::operator =(const EasyCurl &arOther)
{
    if (this != &arOther) {
        mpCurl = curl_easy_duphandle(arOther.mpCurl);
    }
    return *this;
}

EasyCurl& EasyCurl::operator =(EasyCurl &&arOther)
{
    if (this != &arOther) {
        mpCurl = arOther.mpCurl;
        arOther.mpCurl = nullptr;
    }
    return *this;
}

} /* namespace rsp::network::http::curl */
