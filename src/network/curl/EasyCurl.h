/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_EASY_CURL_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_EASY_CURL_H

#include <string>
#include "CurlLibrary.h"
#include "Exceptions.h"
#include <logging/LogChannel.h>

namespace rsp::network::curl {

class MultiCurl;

/**
 * \class EasyCurl
 * \brief Low level wrapper for libcurl's Easy interface.
 * This wrapper allocates an easy handle, but can also lookup this class object from a given handle.
 */
class EasyCurl : public logging::NamedLogger<EasyCurl>
{
public:
    EasyCurl();
    EasyCurl(const EasyCurl& arOther);
    EasyCurl(EasyCurl&& arOther) noexcept;
    ~EasyCurl() override;

    static EasyCurl* GetFromHandle(CURL* apHandle);

    EasyCurl& operator=(const EasyCurl& arOther);
    EasyCurl& operator=(EasyCurl&& arOther) noexcept;

protected:
    static long const _followRedirects = 1L;
    static long const _maxRedirects = 1L;

    friend MultiCurl;
    CURL* mpCurl = nullptr;
    curl_slist *mpHeaders = nullptr;

    curl_mime* getForm();
    bool hasForm() { return (mpForm != nullptr); }

    virtual void prepareRequest();
    virtual void requestDone() {};

    template <typename T>
    void setCurlOption(CURLoption aOption, T aArg) {
//        std::cout << "Curl Option: " << static_cast<int>(aOption) << " : " << aArg << std::endl;
        auto err = curl_easy_setopt(mpCurl, aOption, aArg);
        if (err != CURLE_OK) {
            THROW_WITH_BACKTRACE2(ECurlError, "curl_easy_setopt() failed:", err);
        }
    }

    template <typename T>
    void getCurlInfo(CURLINFO aInfo, T aArg) {
        auto err = curl_easy_getinfo(mpCurl, aInfo, aArg);
        if (err != CURLE_OK) {
            THROW_WITH_BACKTRACE2(ECurlError, "curl_easy_getinfo() failed:", err);
        }
    }

private:
    char mErrorBuffer[CURL_ERROR_SIZE] = { 0 };
    curl_mime *mpForm = nullptr;
};

} /* namespace rsp::network::http::curl */

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_EASY_CURL_H
