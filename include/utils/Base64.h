/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_BASE64_H
#define RSP_CORE_LIB_INCLUDE_UTILS_BASE64_H

#include <span>
#include <string>

namespace rsp::utils {

class Base64
{
public:
    static std::string Encode(std::span<const std::byte> aData);
    static std::string Encode(std::string_view aText)     { return Encode({ reinterpret_cast<const std::byte*>(aText.data()), aText.size() }); }
    static std::string Encode(const std::string& arText)  { return Encode({ reinterpret_cast<const std::byte*>(arText.data()), arText.size() }); }

    static std::string Decode(std::string_view aBase64);

protected:
    static std::array<char, 64> mMap64;
};

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_BASE64_H
