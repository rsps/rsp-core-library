/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "EHttpRequest.h"

namespace rsp::network::ehttp {

EHttpRequest::EHttpRequest()
    : mResponse(*this)
{

}

const HttpRequestOptions& EHttpRequest::GetOptions() const
{
    return mOptions;
}

IHttpRequest& EHttpRequest::SetOptions(const HttpRequestOptions& arOptions)
{
    mOptions = arOptions;
    return *this;
}

IHttpRequest& EHttpRequest::SetBody(std::shared_ptr<IChunkedDataProvider> apBody)
{
    mOptions.Body = apBody;
    return *this;
}

const IChunkedDataProvider& EHttpRequest::GetBody() const
{
    return *(mOptions.Body);
}

IHttpRequest& EHttpRequest::AddField(const std::string& arFieldName, const std::string& arValue)
{
    return *this;
}

IHttpRequest& EHttpRequest::AddFile(const std::string& arFieldName, posix::FileIO& arFile)
{
    return *this;
}

IHttpResponse& EHttpRequest::Execute()
{
    return mResponse;
}

uintptr_t EHttpRequest::GetHandle() const
{
    return 0;
}

} // rsp::network::ehttp


/**
 * \brief Factory function to decouple dependency
 *
 * \return IHttpRequest unique pointer
 */
std::unique_ptr<rsp::network::IHttpRequest> rsp::network::IHttpRequest::Create()
{
    return std::make_unique<rsp::network::ehttp::EHttpRequest>();
}
