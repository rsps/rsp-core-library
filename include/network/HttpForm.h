/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_NETWORK_HTTPFORM_H_
#define INCLUDE_NETWORK_HTTPFORM_H_

#include "IHttpForm.h"
#include "HttpRequest.h"

namespace rsp::network {

class HttpForm: public IHttpForm, public HttpRequest
{
public:
    HttpForm() : mPimpl(MakeRequest()) {}

    IHttpForm& AddField(const std::string &arFieldName, const std::string &arValue) override
    {
        return mPimpl->AddField(arFieldName, arValue);
    }

    IHttpForm& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) override
    {
        ReadFromFile(arFile);
        return mPimpl->AddFile(arFieldName, arFile);
    }

protected:
    std::unique_ptr<IHttpForm> mPimpl;
    static IHttpForm* MakeRequest();
};

} /* namespace rsp::network */

#endif /* INCLUDE_NETWORK_HTTPFORM_H_ */
