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

using namespace rsp::logging;

namespace rsp::security {

EncryptedFileDataStorage::EncryptedFileDataStorage()
        : mLogger("security")
{
}

void EncryptedFileDataStorage::Init(std::string_view aFileName, const SecureBuffer &arInitializationVector, const SecureBuffer &arKey)
{
    utils::FileDataStorage::Init(aFileName);
    mIv = arInitializationVector;
    mKey = arKey;
}

void EncryptedFileDataStorage::Read(rsp::utils::IDataContent &arContent)
{
    std::size_t current = mFile.Seek(0, std::ios_base::cur); // Get current position
    std::size_t encrypted_content_size = mFile.GetSize() - current;

    SecureBuffer encrypted;
    encrypted.resize(encrypted_content_size);
    mFile.ExactRead(encrypted.data(), encrypted.size());

    mLogger.Debug() << "\nEncrypted Data: (" << encrypted.size() << ")\n" << encrypted;
    mLogger.Debug() << "IV: " << mIv << ", Key: " << mKey;

    Decrypt d;
    d.Init(mIv, mKey);
    d.Update(encrypted.data(), encrypted.size());
    SecureBuffer plain = d.Finalize();

//    mLogger().Debug() << "\nPlain Data: (" << plain.size() << ")\n" << plain;

    std::size_t sz = arContent.GetSize();

    if (plain.size() < sz) {
        mLogger.Warning() << "Size of stored encrypted data is smaller than expected: " << plain.size() << " vs. " << sz;
        sz = plain.size();
    }
    else if (plain.size() > sz) {
        mLogger.Warning() << "Size of stored encrypted data is larger than expected: " << plain.size() << " vs. " << sz;
    }

    std::size_t i = 0;
    for(auto b : plain) {
        arContent.GetData()[i++] = b;
        if (i == sz) {
            break;
        }
    }
}

void EncryptedFileDataStorage::Write(const rsp::utils::IDataContent &arContent)
{
    mLogger.Debug() << "IV: " << mIv << ", Key: " << mKey;

    Encrypt e;
    e.Init(mIv, mKey);
    e.Update(arContent.GetData(), arContent.GetSize());
    SecureBuffer sb = e.Finalize();

    mLogger.Debug() << "\nEncrypted Data: (" << sb.size() << ")\n" << sb;
    mFile.ExactWrite(sb.data(), sb.size());
}

} /* namespace rsp::security */
