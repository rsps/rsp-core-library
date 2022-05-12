/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <climits>
#include <iostream>
#include <security/Sha.h>
#include <security/Encrypt.h>
#include <security/Decrypt.h>
#include <security/SecureContainer.h>
#include <logging/Logger.h>

using namespace rsp::utils;
using namespace rsp::logging;

namespace rsp::security
{

bool SecureContainerBase::getSignature(Signature_t &arSignature, std::string_view aSecret)
{
    Sha sha(aSecret, HashAlgorithms::Sha256);
    sha.Update(mpData, mDataSize);
    SecureBuffer md = sha.Get();

    int i = 0;
    for(auto b : md) {
        arSignature[i++] = b;
    }

    return true;
}

bool SecureContainerBase::checkSignature(std::string_view aSecret)
{
    Sha sha(aSecret, HashAlgorithms::Sha256);
    sha.Update(mpData, mDataSize);
    SecureBuffer md = sha.Get();

    Logger::GetDefault().Debug() << "In signature: " << md << std::endl;

    auto &signature = getExtHeader().Signature;
    bool result = true;
    int i = 0;
    for(auto b : md) {
        if (signature[i++] != b) {
            result = false;
            break;
        }
    }

    return result;
}

void SecureContainerBase::readPayloadFrom(rsp::posix::FileIO &arFile)
{
    if (mpHeader->Flags & ContainerFlags::Encrypted) {
        auto &header = getExtHeader();
        Logger::GetDefault().Debug() << "Read Header:\n" << getExtHeader() << std::endl;

        SecureBuffer encrypted;
        encrypted.resize(header.PayloadSize);
        std::size_t len = arFile.Read(encrypted.data(), encrypted.size());

        Logger::GetDefault().Debug() << "\nEncrypted Data: (" << encrypted.size() << ")\n" << encrypted << std::endl;

        Logger::GetDefault().Debug() << "IV: " << mInitializationVector << ", Key: " << mKey << std::endl;
        Decrypt d;
        d.Init(mInitializationVector, mKey);
        d.Update(encrypted.data(), encrypted.size());
        SecureBuffer plain = d.Finalize();

        Logger::GetDefault().Debug() << "\nPlain Data: (" << plain.size() << ")\n" << plain << std::endl;

        header.PayloadSize = mDataSize;

        std::size_t sz = (plain.size() < mDataSize) ? plain.size() : mDataSize;

        std::size_t i = 0;
        for(auto b : plain) {
            mpData[i++] = b;
            if (i == mDataSize) {
                break;
            }
        }
    }
    else {
        arFile.ExactRead(mpData, mDataSize);
    }
}

void SecureContainerBase::SetEncryption(SecureBuffer aInitializationVector, SecureBuffer aKey)
{
    mInitializationVector = aInitializationVector;
    mKey = aKey;

    if (aInitializationVector.empty() && mKey.empty()) {
        getExtHeader().Flags &= ~ContainerFlags::Encrypted;
    }
    else {
        getExtHeader().Flags |= ContainerFlags::Encrypted;
    }
}

bool SecureContainerBase::writePayloadTo(rsp::posix::FileIO &arFile)
{
    if (mpHeader->Flags & ContainerFlags::Encrypted) {
        Logger::GetDefault().Debug() << "IV: " << mInitializationVector << ", Key: " << mKey << std::endl;
        Encrypt e;
        e.Init(mInitializationVector, mKey);
        e.Update(mpData, mDataSize);
        SecureBuffer sb = e.Finalize();

        getExtHeader().PayloadSize = sb.size();

        Logger::GetDefault().Debug() << "Write Header:\n" << getExtHeader() << std::endl;
        Logger::GetDefault().Debug() << "\nEncrypted Data: (" << sb.size() << ")\n" << sb << std::endl;
        arFile.ExactWrite(sb.data(), sb.size());

        return true;
    }

    arFile.ExactWrite(mpData, mDataSize);

    return false;
}

} /* namespace rsp::security */
