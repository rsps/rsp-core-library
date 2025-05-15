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

#include <memory>
#include "IStreamDataProvider.h"
#include <posix/FileIO.h>

namespace rsp::network {

class FileBody : public IStreamDataProvider
{
public:
    explicit FileBody(const std::string& arFileName);
    explicit FileBody(rsp::posix::FileIO& arFile);
    size_t Write(std::span<const std::byte> aData) override;
    [[nodiscard]] size_t Read(std::span<std::byte> aBuffer) const override;
    [[nodiscard]] size_t GetStreamSize() const override;

    posix::FileIO& Get() { return mrFile; }

protected:
    std::unique_ptr<posix::FileIO> mpFile{};
    posix::FileIO& mrFile;
};

} // rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_FILE_BODY_H
