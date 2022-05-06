/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <cstring>
#include <utils/DataContainer.h>
#include <utils/Crc32.h>

using namespace rsp::posix;

namespace rsp::utils {

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

    fin.ExactRead(mpHeader, mpHeader->Size);

    if (mpHeader->Size != expected_size) {
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

    if ((mpHeader->Flags & ContainerFlags::Signed) && (mpHeader->Flags & ContainerFlags::ExtendedContainer)) {
        getSignature(getHeaderAs<ContainerHeaderExtended>()->Signature, aSecret);
    }

    mpHeader->PayloadSize = mDataSize;

    FileIO fout(arFileName, std::ios_base::out | std::ios_base::trunc, 0600);

    fout.ExactWrite(mpHeader, mpHeader->Size);

    writePayloadTo(fout);
}

void DataContainerBase::readPayloadFrom(rsp::posix::FileIO &arFile)
{
    arFile.ExactRead(mpData, std::min(mDataSize, mpHeader->PayloadSize));
}

void DataContainerBase::writePayloadTo(rsp::posix::FileIO &arFile)
{
    arFile.ExactWrite(mpData, mDataSize);
}

std::uint32_t DataContainerBase::calcCRC() const
{
    return Crc32::Calc(mpData, mDataSize);
}

DataContainerBase& DataContainerBase::operator=(const DataContainerBase &arOther)
{
    if (&arOther != this) {
        if (mpHeader->Flags & arOther.mpHeader->Flags & ContainerFlags::ExtendedContainer) {
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
    if (Flags & ContainerFlags::Encrypted) {
        std::cout << "Encrypted";
    }
}


} /* namespace rsp::utils */
