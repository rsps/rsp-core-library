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
#include <string_view>
#include <rsp/exceptions/CoreException.h>

namespace rsp::utils {

class EBase64FormatError : public exceptions::CoreException
{
public:
    using rsp::exceptions::CoreException::CoreException;
};

/**
 * \brief An Base64 encoder and decoder for HTTP encoding
 * \see https://en.wikipedia.org/wiki/Base64
 */
class Base64
{
public:
    static std::string Encode(std::span<const std::byte> aData);

    static std::string Encode(std::string_view aText)
    {
        return Encode(std::as_bytes(std::span{aText}));
    }

    static std::string Decode(std::string_view aBase64);
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_INCLUDE_UTILS_BASE64_H
