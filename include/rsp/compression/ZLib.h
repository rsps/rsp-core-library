/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_COMPRESSION_Z_LIB_H
#define RSP_CORE_LIB_COMPRESSION_Z_LIB_H

#include <rsp/exceptions/CoreException.h>
#include <rsp/utils/SemVer.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace rsp::compression {

class ZlibException : public exceptions::CoreException
{
public:
    ZlibException(const char* apMsg, int aErrorCode);
};

class ZLib
{
public:
    explicit ZLib(size_t aBufferSize = 8192);
    ~ZLib();
    void Inflate(std::span<const uint8_t> aData);
    [[nodiscard]] const std::vector<uint8_t>& GetResult() const;
    [[nodiscard]] utils::Version GetLibraryVersion() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace rsp::compression

#endif // RSP_CORE_LIB_COMPRESSION_Z_LIB_H
