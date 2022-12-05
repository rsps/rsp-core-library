/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <utils/DataContainer.h>
#include <utils/Crc32.h>

using namespace rsp::posix;

namespace rsp::utils {


void Crc32DataSignature::Init(std::string_view aSecret)
{
    mSecret = aSecret;
}

void Crc32DataSignature::Calc(const rsp::utils::IDataContent &arContent)
{
    mCRC = Crc32::Calc(arContent.GetData(), arContent.GetSize());
    mCRC = Crc32::Calc(reinterpret_cast<const std::uint8_t*>(mSecret.data()), mSecret.size(), mCRC);
}

void Crc32DataSignature::Verify(const rsp::utils::IDataContent &arContent)
{
    std::uint32_t value = Crc32::Calc(arContent.GetData(), arContent.GetSize());
    value = Crc32::Calc(reinterpret_cast<const std::uint8_t*>(mSecret.data()), mSecret.size(), value);

    if (value != mCRC) {
        THROW_WITH_BACKTRACE(EInvalidSignature);
    }
}



void FileDataStorage::Init(std::string_view aFileName)
{
    mFileName = aFileName;
}

void FileDataStorage::Read(rsp::utils::IDataContent &arContent)
{
    mFile.ExactRead(arContent.GetData(), arContent.GetSize());
}

void FileDataStorage::Write(const rsp::utils::IDataContent &arContent)
{
    mFile.ExactWrite(arContent.GetData(), arContent.GetSize());
}

void FileDataStorage::ReadSignature(rsp::utils::IDataSignature &arSignature)
{
    mFile.Open(mFileName, std::ios_base::in);
    mFile.ExactRead(arSignature.GetData(), arSignature.GetSize());
}

void FileDataStorage::WriteSignature(const rsp::utils::IDataSignature &arSignature)
{
    mFile.Open(mFileName, std::ios_base::out | std::ios_base::trunc, 0600);
    mFile.ExactWrite(arSignature.GetData(), arSignature.GetSize());
}

std::size_t FileDataStorage::GetSize()
{
    return mFile.GetSize();
}



std::ostream& operator <<(std::ostream &os, const IDataSignature &arSignature)
{
    os << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < arSignature.GetSize() ; i++) {
        os << std::setw(2) << static_cast<int>(arSignature.GetData()[i]);
    }
    return os << std::dec;
}

std::ostream& operator <<(std::ostream &os, const IDataContent &arContent)
{
    os << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < arContent.GetSize() ; i++) {
        os << std::setw(2) << static_cast<int>(arContent.GetData()[i]);
    }
    return os << std::dec;
}

} /* namespace rsp::utils */
