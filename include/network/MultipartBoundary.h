/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H
#define RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H

#include <ostream>
#include <string>
#include <string_view>

namespace rsp::network {

constexpr const char* CRLF = "\r\n";

class MultipartBoundary
{
public:
    MultipartBoundary();
    explicit MultipartBoundary(std::string_view aBoundary);

    [[nodiscard]] std::string GetContentTypeHeader() const;

    [[nodiscard]] std::string MakeContentDisposition(const std::string &arName, const std::string &arFileName = std::string(), const std::string &arContentType = std::string()) const;
    [[nodiscard]] std::ostream& StreamContentDisposition(std::ostream& o, const std::string &arName, const std::string &arFileName = std::string(), const std::string &arContentType = std::string()) const;

    [[nodiscard]] std::string GetEndBoundary() const;
    [[nodiscard]] std::ostream& StreamEndBoundary(std::ostream &) const;

    [[nodiscard]] const std::string& GetBoundary() const;

protected:
    std::string mBoundary{};
};

} // rsp::network

#endif //RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H
