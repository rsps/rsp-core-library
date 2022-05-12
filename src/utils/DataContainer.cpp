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
#include <logging/Logger.h>

using namespace rsp::posix;
using namespace rsp::logging;

namespace rsp::utils {

std::ostream& operator<<(std::ostream& os, const ContainerHeader &arHeader)
{
    os <<
        "Size:           " << static_cast<int>(arHeader.Size) << "\n"
        "Flags:          0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(arHeader.Flags) << std::dec << "\n"
        "Version:        " << static_cast<int>(arHeader.Version) << "\n"
        "PayloadVersion: " << static_cast<int>(arHeader.PayloadVersion) << "\n"
        "PayloadCRC:     0x" << std::hex << std::setfill('0') << std::setw(8) << arHeader.PayloadCRC << std::dec << "\n"
        "PayloadSize:    " << arHeader.PayloadSize;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ContainerHeaderExtended &arHeader)
{
    os << *static_cast<const ContainerHeader*>(&arHeader) << "\n";
    os << "Signature:      " << arHeader.Signature;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Signature_t &arBuffer)
{
    os << std::hex << std::setfill('0');
    for (auto c : arBuffer) {
        os << std::setw(2) << static_cast<int>(c);
    }
    return os << std::dec;
}



DataContainerBase::DataContainerBase(ContainerHeader *apHeader, std::uint8_t *apData, std::size_t aDataSize)
    : mpHeader(apHeader),
      mpData(apData),
      mDataSize(aDataSize)
{
    // Make sure eventual alignment bytes are cleared. Otherwise CRC calculation might fail.
    std::size_t sz = (mDataSize > 8) ? 8 : mDataSize;
    std::memset(mpData + mDataSize - sz, 0, sz);
}

void DataContainerBase::Load(const std::string &arFileName, std::string_view aSecret)
{
    FileIO fin(arFileName, std::ios_base::in);

    auto expected_size = mpHeader->Size;
    auto expected_flags = mpHeader->Flags;

    fin.ExactRead(mpHeader, mpHeader->Size);

    if ((mpHeader->Size != expected_size) || (mpHeader->Flags != expected_flags)) {
        Logger::GetDefault().Error() << "Expected size: " << static_cast<int>(expected_size) << " was " << static_cast<int>(mpHeader->Size) << std::endl;
        Logger::GetDefault().Error() << "Expected flags: " << static_cast<int>(expected_flags) << " was " << static_cast<int>(mpHeader->Flags) << std::endl;
        mpHeader->Size = expected_size;
        mpHeader->Flags = expected_flags;
        THROW_WITH_BACKTRACE1(EInvalidHeader, arFileName);
    }

    readPayloadFrom(fin);

    if (calcCRC() != mpHeader->PayloadCRC) {
        THROW_WITH_BACKTRACE1(EInvalidCRC, arFileName);
    }

    if (!checkSignature(aSecret)) {
        THROW_WITH_BACKTRACE1(EInvalidSignature, arFileName);
    }
}

void DataContainerBase::Save(const std::string &arFileName, std::string_view aSecret)
{
    mpHeader->PayloadCRC = calcCRC();

    if (mpHeader->Flags & ContainerFlags::Extended) {
        getSignature(getExtHeader().Signature, aSecret);
    }

    mpHeader->PayloadSize = mDataSize;

    FileIO fout(arFileName, std::ios_base::out | std::ios_base::trunc, 0600);

    fout.ExactWrite(mpHeader, mpHeader->Size);

    if (writePayloadTo(fout)) {
        fout.Seek(0);
        fout.ExactWrite(mpHeader, mpHeader->Size);
    }
}

void DataContainerBase::readPayloadFrom(rsp::posix::FileIO &arFile)
{
    arFile.ExactRead(mpData, std::min(mDataSize, mpHeader->PayloadSize));
}

bool DataContainerBase::writePayloadTo(rsp::posix::FileIO &arFile)
{
    arFile.ExactWrite(mpData, mDataSize);
    return false;
}

std::uint32_t DataContainerBase::calcCRC() const
{
    return Crc32::Calc(mpData, mDataSize);
}

DataContainerBase& DataContainerBase::operator=(const DataContainerBase &arOther)
{
    if (&arOther != this) {
        if (mpHeader->Flags & arOther.mpHeader->Flags & ContainerFlags::Extended) {
            *reinterpret_cast<ContainerHeaderExtended*>(mpHeader) = *reinterpret_cast<ContainerHeaderExtended*>(arOther.mpHeader);
        }
        else {
            *mpHeader = *arOther.mpHeader;
        }
        if (mDataSize != arOther.mDataSize) {
            THROW_WITH_BACKTRACE(ESizeDiffersInAssignment);
        }
        std::memcpy(mpData, arOther.mpData, mDataSize);
    }
    return *this;
}

ContainerHeader::ContainerHeader(std::uint8_t aSize, ContainerFlags aFlags, std::uint8_t aVersion)
    : Size(aSize),
      Flags(aFlags),
      Version(aVersion)
{
}


} /* namespace rsp::utils */
