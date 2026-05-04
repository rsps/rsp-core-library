/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <network/HttpResponse.h>
#include <network/parser-helpers.h>
#include <stdexcept>
#include <utils/StrUtils.h>
#include <network/StringBody.h>
#include <network/BinaryBody.h>

namespace rsp::network {


std::string_view HttpResponse::GetHeader(std::string_view aName) const
{
    try {
        return mHeaders.at(aName);
    }
    catch (const std::out_of_range &) {
        THROW_WITH_BACKTRACE1(EHeaderNotFound, rsp::utils::StrUtils::Format("No response header named %s was found", aName.data()));
    }
}

[[nodiscard]] StatusCodes HttpResponse::GetStatusCode() const
{
    return static_cast<StatusCodes>(mStatusLine.GetStatusCode());
}

IHttpResponse& HttpResponse::MakeBody()
{
    if (mpBody) {
        mpBody.reset();
    }

    if (mrRequest.GetOptions().ResponseBody) {
        mpBody = mrRequest.GetOptions().ResponseBody;
    }
    else if (mHeaders.contains("content-type") && mHeaders["content-type"].starts_with("application/octet-stream")) {
        mpBody = std::make_shared<BinaryBody>();
    }
    else {
        mpBody = std::make_shared<StringBody>();
    }

    return *this;
}

size_t HttpResponse::GetContentLength() const
{
    if (!mContentLength.has_value()) {
        if (GetStatusCode() == StatusCodes::NoContent) {
            const_cast<HttpResponse*>(this)->mContentLength = 0;
        }
        else if (mHeaders.contains("content-length")) {
            const_cast<HttpResponse*>(this)->mContentLength = string_to_integral<size_t>(mHeaders.at("content-length"));
        }
    }
    if (mContentLength.has_value()) {
        return *mContentLength;
    }
    return 0;
}

IHttpResponse& HttpResponse::Clear()
{
    mCompleted = false;
    mStatusLine = {};
    mHeaders.clear();
    mHeaderData.clear();
    mContentLength.reset();
    MakeBody();
    return *this;
}

}
