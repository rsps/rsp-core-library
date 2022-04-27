/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <configuration/Config.h>
#include <utils/SignedData.h>
#include <posix/FileIO.h>

using namespace rsp::utils;
using namespace rsp::posix;

namespace rsp::config {

ConfigBase::ConfigBase(std::uint8_t *apData, std::size_t aDataSize)
    : mpData(apData),
      mDataSize(aDataSize)
{
}

void ConfigBase::Load(const std::string &arFileName, std::string_view aSecret)
{
    SignedData sd(aSecret);
    sd.Verify(arFileName);

    FileIO fin(arFileName, std::ios_base::in);

    std::size_t len = fin.Read(mpData.get(), mDataSize);

    if (len != mDataSize) {
        // TODO: Handle different versions of the data structure.
        THROW_WITH_BACKTRACE2(EConfigSizeMismatch, mDataSize, len);
    }
}

void ConfigBase::Save(const std::string &arFileName, std::string_view aSecret)
{
    validate();

    SignedData sd(aSecret);
    MessageDigest sign = sd.GetSignature(mpData.get(), mDataSize);

    FileIO fout(arFileName, std::ios_base::out);

    if (fout.Write(mpData.get(), mDataSize) != mDataSize) {
        THROW_WITH_BACKTRACE(EConfigWrite);
    }

    if (fout.Write(sign.data(), sign.size()) != sign.size()) {
        THROW_WITH_BACKTRACE(EConfigWrite);
    }
}

} /* namespace rsp::config */
