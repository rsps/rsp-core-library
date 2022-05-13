/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/EncryptedFileDataStorage.h>
#include <security/Decrypt.h>
#include <security/Encrypt.h>
#include <logging/Logger.h>

using namespace rsp::logging;

namespace rsp::security {

void EncryptedFileDataStorage::Init(std::string_view aFileName, const SecureBuffer &arInitializationVector, const SecureBuffer &arKey)
{
    utils::FileDataStorage::Init(aFileName);
    mIv = arInitializationVector;
    mKey = arKey;
}

void EncryptedFileDataStorage::Read(uint8_t *apData, std::size_t aSize)
{
    std::size_t current = mFile.Seek(0, std::ios_base::cur); // Get current position
    std::size_t encrypted_content_size = mFile.GetSize() - current;

    SecureBuffer encrypted;
    encrypted.resize(encrypted_content_size);
    mFile.ExactRead(encrypted.data(), encrypted.size());

    Logger::GetDefault().Debug() << "\nEncrypted Data: (" << encrypted.size() << ")\n" << encrypted << std::endl;
    Logger::GetDefault().Debug() << "IV: " << mIv << ", Key: " << mKey << std::endl;

    Decrypt d;
    d.Init(mIv, mKey);
    d.Update(encrypted.data(), encrypted.size());
    SecureBuffer plain = d.Finalize();

//    Logger::GetDefault().Debug() << "\nPlain Data: (" << plain.size() << ")\n" << plain << std::endl;

    if (plain.size() < aSize) {
        Logger::GetDefault().Warning() << "Size of stored encrypted data is smaller than expected: " << plain.size() << " vs. " << aSize << std::endl;
        aSize = plain.size();
    }
    else if (plain.size() > aSize) {
        Logger::GetDefault().Warning() << "Size of stored encrypted data is larger than expected: " << plain.size() << " vs. " << aSize << std::endl;
    }

    std::size_t i = 0;
    for(auto b : plain) {
        apData[i++] = b;
        if (i == aSize) {
            break;
        }
    }
}

void EncryptedFileDataStorage::Write(const uint8_t *apData, std::size_t aSize)
{
    Logger::GetDefault().Debug() << "IV: " << mIv << ", Key: " << mKey << std::endl;

    Encrypt e;
    e.Init(mIv, mKey);
    e.Update(apData, aSize);
    SecureBuffer sb = e.Finalize();

    Logger::GetDefault().Debug() << "\nEncrypted Data: (" << sb.size() << ")\n" << sb << std::endl;
    mFile.ExactWrite(sb.data(), sb.size());
}

} /* namespace rsp::security */
