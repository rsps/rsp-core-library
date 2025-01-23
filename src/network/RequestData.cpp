/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <network/RequestData.h>

namespace rsp::network {

bool RequestData::GetData(char *apBuffer, size_t aLen, IHttpBodyStream &arBody)
{
    bool done = false;
    mWritten = 0;
    while (mWritten < aLen) {
        if (mFiFo.IsEmpty()) {
            if (done) {
                return true;
            }
            mFiFo.Clear(); // Fifo is empty, now make sure buffer Head is on index 0 for maximal continuous space.
            size_t sz = 0;
            done = arBody.GetChunk(mFiFo.GetData(), mFiFo.Free(), sz, mChunkIndex, mPayloadIndex);
            mFiFo.SetHead(sz);
        }
        mWritten += mFiFo.Read(&apBuffer[mWritten], aLen - mWritten);
    }
    return false;
}

RequestData& RequestData::Clear()
{
    mFiFo.Clear();
    mChunkIndex = 0;
    mPayloadIndex = 0;
    mWritten = 0;
    return *this;
}

} // rsp::network