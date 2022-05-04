/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <posix/FileIO.h>
#include <security/SignedData.h>

namespace rsp::security {


SignedData::SignedData(std::string_view aSecret)
    : mSha(aSecret, HashAlgorithms::Sha256)
{
}

void SignedData::Verify(const uint8_t *apData, std::size_t aSize)
{
    SecureBuffer sgn;
    std::size_t sign_offset = aSize - sgn.size();
    sgn = GetSignature(apData, sign_offset);
    if (std::memcmp(&sgn, &apData[sign_offset], sgn.size())) {
        THROW_WITH_BACKTRACE(EInvalidSignature);
    }
}

void SignedData::Verify(const std::string &arFileName)
{
    SecureBuffer sgn = GetSignature(arFileName, true);

    rsp::posix::FileIO file(arFileName, std::ios_base::in);
    file.Seek(0 - sgn.size(), std::ios_base::end);

    SecureBuffer md;
    file.Read(md.data(), md.size());

    if (sgn != md) {
        THROW_WITH_BACKTRACE(EInvalidSignature);
    }
}

SecureBuffer SignedData::GetSignature(const uint8_t *apData, std::size_t aSize)
{
    mSha.Update(apData, aSize);
    return mSha.Get();
}

SecureBuffer SignedData::GetSignature(const std::string &arFileName, bool aExcludeDigest)
{
    rsp::posix::FileIO file(arFileName, std::ios_base::in);
    std::size_t aLimit = file.GetSize();

    if(aExcludeDigest) {
        SecureBuffer md;
        aLimit -= md.size();
    }

    while (aLimit > 0) {
        uint8_t buffer[1024];
        std::size_t len = file.Read(&buffer[0], std::min(aLimit, sizeof(buffer)));
        if (len == 0) {
            break;
        }
        mSha.Update(&buffer[0], len);
        aLimit -= len;
    }

    return mSha.Get();
}

}
