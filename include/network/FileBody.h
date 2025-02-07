/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_FILE_BODY_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_FILE_BODY_H

#include "IStreamDataProvider.h"
#include <posix/FileIO.h>

namespace rsp::network {

class FileBody : public IStreamDataProvider
{
public:
    size_t Write(std::span<const std::byte> aData) override;
    size_t Read(std::span<std::byte> aBuffer) override;
    std::optional<size_t> GetStreamSize() override;

    posix::FileIO& Get() { return mFile; }

protected:
    posix::FileIO mFile{};
};

} // rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_FILE_BODY_H
