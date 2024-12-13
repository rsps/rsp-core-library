/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_REQUEST_DATA_H
#define RSP_CORE_LIB_REQUEST_DATA_H

#include <utils/FifoBuffer.h>
#include "IHttpBodyStream.h"

namespace rsp::network {

class RequestData
{
public:
    bool GetData(char *apBuffer, size_t aLen, IHttpBodyStream &arBody);
    RequestData& Clear();
    [[nodiscard]] size_t GetChunkIndex() const { return mChunkIndex; }
    [[nodiscard]] size_t GetPayloadIndex() const { return mPayloadIndex; }
    [[nodiscard]] size_t GetWritten() const { return mWritten; }
protected:
    rsp::utils::FifoBuffer<char, 256> mFiFo{};
    size_t mChunkIndex = 0;
    size_t mPayloadIndex = 0;
    size_t mWritten = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_REQUEST_DATA_H
