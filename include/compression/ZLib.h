/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_COMPRESSION_ZLIB_H_
#define INCLUDE_COMPRESSION_ZLIB_H_

#ifdef USE_ZLIB

#include <string>
#include <sstream>
#include <vector>
#include <exceptions/CoreException.h>

#define ZLIB_CONST
#include <zlib.h>

namespace rsp::compression {

class ZlibException: public exceptions::CoreException
{
public:
    explicit ZlibException(const char *apMsg, int aErrorCode)
    : CoreException(formatError(apMsg, aErrorCode))
    {
    }

    std::string formatError(const char *apMsg, int aErrorCode);
};


class ZLib
{
public:
    ZLib(size_t aBufferSize = 8192);

    void Inflate(const std::uint8_t *apData, size_t aSize);

    const std::ostream& GetResult();

protected:
    z_stream mZStream{};
    std::vector<std::uint8_t> mBuffer{};
    std::stringstream mResult{};
};


} /* namespace rsp::compression */

#endif /* USE_ZLIB */

#endif /* INCLUDE_COMPRESSION_ZLIB_H_ */
