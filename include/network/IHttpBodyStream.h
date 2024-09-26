/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_IHTTPBODYSTREAM_H
#define RSP_CORE_LIB_IHTTPBODYSTREAM_H

#include <cstddef>

namespace rsp::network {

class IHttpBodyStream
{
public:
    virtual ~IHttpBodyStream() = default;

    [[nodiscard]] virtual size_t GetChunk(char *apBuffer, size_t aBufferSize) = 0;
    [[nodiscard]] virtual size_t GetSize() const = 0;
    virtual IHttpBodyStream& Reset() = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_IHTTPBODYSTREAM_H
