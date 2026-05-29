/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <rsp/network/MultipartBoundary.h>
#include <sstream>
#include <string>
#include <rsp/utils/Random.h>

namespace rsp::network {

MultipartBoundary::MultipartBoundary()
{
    mBoundary.reserve(32);
    for (size_t i = 0 ; i < 32 ; ++i) {
        switch (utils::Random::Roll(0u, 2u)) {
            case 0:
                mBoundary += utils::Random::Roll('A', 'Z');
                break;

            case 1:
                mBoundary += utils::Random::Roll('a', 'z');
                break;

            default:
                mBoundary += '_';
        }
    }
}

MultipartBoundary::MultipartBoundary(std::string_view aBoundary)
    : mBoundary(aBoundary)
{
}

std::string MultipartBoundary::GetContentTypeHeader() const
{
    std::stringstream result;
    result << "multipart/form-data; boundary=" << mBoundary;
    return result.str();
}

std::string MultipartBoundary::MakeContentDisposition(const std::string& arName, const std::string& arFileName, const std::string& arContentType) const
{
    std::stringstream result;
    (void)StreamContentDisposition(result, arName, arFileName, arContentType);
    return result.str();
}

std::ostream& MultipartBoundary::StreamContentDisposition(std::ostream& o, const std::string& arName, const std::string& arFileName, const std::string& arContentType) const
{
    /** \see https://datatracker.ietf.org/doc/html/rfc7578#section-4.1 */
    o << CRLF << "--" << mBoundary;
    if (!arName.empty()) {
        o << CRLF << "Content-Disposition: form-data; name=\"" << arName << "\"";
    }
    if (!arFileName.empty()) {
        o << "; filename=\"" << arFileName << "\"";
    }
    if (!arContentType.empty()) {
        o << CRLF << "Content-Type: " << arContentType;
    }
    o << CRLF << CRLF;
    return o;
}

std::string MultipartBoundary::GetEndBoundary() const
{
    std::stringstream result;
    (void)StreamEndBoundary(result);
    return result.str();
}

std::ostream& MultipartBoundary::StreamEndBoundary(std::ostream& o) const
{
    o << CRLF << "--" << mBoundary << "--";
    return o;
}

const std::string& MultipartBoundary::GetBoundary() const
{
    return mBoundary;
}

} // rsp::network
