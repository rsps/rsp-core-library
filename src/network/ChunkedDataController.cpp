/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <network/ChunkedDataController.h>

namespace rsp::network {

bool ChunkedDataControllerBase::GetData(std::span<char> aBuffer, IChunkedDataProvider &arBody)
{
    bool done = false;
    mWritten = 0;
    while (mWritten < aBuffer.size()) {
        if (mrFiFo.IsEmpty()) {
            if (done) {
                return true;
            }
            mrFiFo.Clear(); // Fifo is empty, now make sure buffer Head is on index 0 for maximal continuous space.
            size_t sz = 0;
            done = arBody.GetChunk({mrFiFo.GetData(), mrFiFo.Free()}, sz, mChunkIndex, mPayloadIndex);
            mrFiFo.SetHead(sz);
        }
        mWritten += mrFiFo.Read(aBuffer.subspan(mWritten, aBuffer.size() - mWritten));
    }
    return false;
}

ChunkedDataControllerBase& ChunkedDataControllerBase::Clear()
{
    mrFiFo.Clear();
    mChunkIndex = 0;
    mPayloadIndex = 0;
    mWritten = 0;
    return *this;
}

} // rsp::network