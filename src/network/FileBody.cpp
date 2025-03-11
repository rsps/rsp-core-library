/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <network/FileBody.h>

namespace rsp::network {

FileBody::FileBody(posix::FileIO& arFile)
    : mrFile(arFile)
{
}

size_t FileBody::Write(std::span<const std::byte> aData)
{
    return mrFile.Write(aData.data(), aData.size());
}

size_t FileBody::Read(std::span<std::byte> aBuffer) const
{
    return mrFile.Read(aBuffer.data(), aBuffer.size());
}

size_t FileBody::GetStreamSize() const
{
    if (mrFile.IsOpen()) {
        return mrFile.GetSize();
    }
    return 0;
}

} // rsp::network