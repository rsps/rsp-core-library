/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <network/BinaryBody.h>

namespace rsp::network {

size_t BinaryBody::Write(std::span<const std::byte> aData)
{
    return size_t(mContent.PutN(aData.data(), std::streamsize(aData.size())));
}

size_t BinaryBody::Read(std::span<std::byte> aBuffer)
{
    return size_t(mContent.GetN(aBuffer.data(), std::streamsize(aBuffer.size())));
}

std::optional<size_t> BinaryBody::GetStreamSize()
{
    return mContent.GetStreamSize();
}

} // rsp::network