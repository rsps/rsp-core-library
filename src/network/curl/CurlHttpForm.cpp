/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "CurlHttpForm.h"
#include <network/HttpForm.h>
#include <logging/Logger.h>

namespace rsp::network::curl {


CurlHttpForm::CurlHttpForm()
{
    mpForm = curl_mime_init(mpCurl);
}

CurlHttpForm::~CurlHttpForm()
{
    curl_mime_free(mpForm);
}

CurlHttpForm::CurlHttpForm(const CurlHttpForm &arOther)
{
    DLOG("CurlHttpForm Copy Construct");
    *this = arOther;
}

CurlHttpForm::CurlHttpForm(CurlHttpForm &&arOther)
{
    DLOG("CurlHttpForm Move Construct");
    *this = std::move(arOther);
}

CurlHttpForm& CurlHttpForm::operator=(const CurlHttpForm &arOther)
{
    DLOG("CurlHttpForm Copy Assignment");
    if (this != &arOther) {
        mpCurl = curl_easy_duphandle(arOther.mpCurl);
        mpForm = curl_mime_init(mpForm);
    }
    return *this;
}

CurlHttpForm& CurlHttpForm::operator=(CurlHttpForm &&arOther)
{
    DLOG("CurlHttpForm Move Assignment");
    if (this != &arOther) {
        mpCurl = arOther.mpCurl;
        arOther.mpCurl = nullptr;
        mpForm = arOther.mpForm;
        arOther.mpForm = nullptr;
    }
    return *this;
}

rsp::network::IHttpForm& CurlHttpForm::AddField(const std::string &arFieldName, const std::string &arValue)
{
    curl_mimepart *field = curl_mime_addpart(mpForm);
    curl_mime_name(field, arFieldName.c_str());
    curl_mime_data(field, arValue.c_str(), CURL_ZERO_TERMINATED);
    return *this;
}

rsp::network::IHttpForm& CurlHttpForm::AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile)
{
    curl_mimepart *field = curl_mime_addpart(mpForm);
    curl_mime_name(field, arFieldName.c_str());
    curl_mime_filedata(field, arFile.GetFileName().c_str());
    return *this;
}

rsp::network::IHttpResponse& CurlHttpForm::Execute()
{
    setCurlOption(CURLOPT_MIMEPOST, mpForm);
    return CurlHttpRequest::Execute();
}

} /* namespace rsp::network */

/**
 * \brief Factory function to decouple dependency
 *
 * \return IHttpRequest*
 */
rsp::network::IHttpForm* rsp::network::HttpForm::MakeRequest()
{
    return new rsp::network::curl::CurlHttpForm();
}

