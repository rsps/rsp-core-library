/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/DataContainer.h>

using namespace rsp::posix;

namespace rsp::utils {

DataContainerBase::DataContainerBase(ContainerHeader *apHeader, std::uint8_t *apData, std::size_t aDataSize)
    : mpHeader(apHeader),
      mpData(apData),
      mDataSize(aDataSize)
{
}

void DataContainerBase::Load(const std::string &arFileName, std::string_view aSecret)
{
    FileIO fin(arFileName, std::ios_base::in);

    auto expected_size = mpHeader.get()->HeaderSize;

    fin.ExactRead(mpHeader.get(), mpHeader.get()->HeaderSize);

    if (mpHeader.get()->HeaderSize != expected_size) {
        THROW_WITH_BACKTRACE1(EInvalidHeader, arFileName);
    }

    readFrom(fin);

    if (!checkSignature(aSecret)) {
        THROW_WITH_BACKTRACE1(EInvalidSignature, arFileName);
    }
}

void DataContainerBase::Save(const std::string &arFileName, std::string_view aSecret)
{
    if ((mpHeader.get()->Flags & ContainerFlags::Signed) && (mpHeader.get()->Flags & ContainerFlags::ExtendedContainer)) {
        getSignature(getHeaderAs<ContainerHeaderExtended>()->Signature, aSecret);
    }

    mpHeader.get()->PayloadSize = mDataSize;

    FileIO fout(arFileName, std::ios_base::out);

    fout.ExactWrite(mpHeader.get(), mpHeader.get()->HeaderSize);

    writeTo(fout);
}

void DataContainerBase::readFrom(rsp::posix::FileIO &arFile)
{
    arFile.ExactRead(mpData.get(), mDataSize);
}

void DataContainerBase::writeTo(rsp::posix::FileIO &arFile)
{
    arFile.ExactWrite(mpData.get(), mDataSize);
}

} /* namespace rsp::utils */
