/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_NETWORK_CURL_CURLHTTPFORM_H_
#define SRC_NETWORK_CURL_CURLHTTPFORM_H_

#include <network/IHttpForm.h>
#include "CurlHttpRequest.h"

namespace rsp::network::curl {

class CurlHttpForm: public rsp::network::IHttpForm, public CurlHttpRequest
{
public:
    CurlHttpForm();
    ~CurlHttpForm() override;

    CurlHttpForm(const CurlHttpForm&);
    CurlHttpForm(CurlHttpForm&&);

    CurlHttpForm& operator=(const CurlHttpForm&);
    CurlHttpForm& operator=(CurlHttpForm&&);


    rsp::network::IHttpForm& AddField(const std::string &arFieldName, const std::string &arValue) override;
    rsp::network::IHttpForm& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) override;

    rsp::network::IHttpResponse& Execute() override;

protected:
    curl_mime *mpForm = nullptr;
};

} /* namespace rsp::network */

#endif /* SRC_NETWORK_CURL_CURLHTTPFORM_H_ */
