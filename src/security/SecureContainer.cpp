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
#include <security/SecureContainer.h>
#include <security/Sha.h>
#include <security/Encrypt.h>
#include <security/Decrypt.h>

using namespace rsp::utils;

namespace rsp::security
{

bool SecureContainer::getSignature(Signature_t &arSignature, std::string_view aSecret)
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

bool SecureContainer::checkSignature(std::string_view aSecret)
{
    Sha sha(aSecret, HashAlgorithms::Sha256);
    sha.Update(mpData, mDataSize);
    SecureBuffer md = sha.Get();

    auto &signature = getHeaderAs<rsp::utils::ContainerHeaderExtended>()->Signature;
    bool result = true;
    int i = 0;
    for(auto b : md) {
        if (signature[i] != b) {
            result = false;
            break;
        }
    }

    return result;
}

void SecureContainer::readPayloadFrom(rsp::posix::FileIO &arFile)
{
    if (mpHeader->Flags & ContainerFlags::Encrypted) {
        auto *header = getHeaderAs<ContainerHeaderExtended>();
        SecureBuffer encrypted;
        encrypted.resize(header->PayloadSize);
        std::size_t len = arFile.Read(encrypted.data(), encrypted.size());

        Decrypt d;
        d.Init("iv", "key");
        d.Update(encrypted.data(), encrypted.size());
        SecureBuffer sb = d.Finalize();

        std::size_t sz = std::min(sb.size(), mDataSize);

        for(auto c : plain) {
            mpData[i++] = static_cast<uint8_t>(c);
            if (i == mDataSize) {
                break;
            }
        }
    }
    else {
        arFile.ExactRead(mpData, mDataSize);
    }
}

void SecureContainer::writePayloadTo(rsp::posix::FileIO &arFile)
{
    if (mpHeader->Flags & ContainerFlags::Encrypted) {
    }
    else {
        arFile.ExactWrite(mpData, mDataSize);
    }
}

} /* namespace rsp::security */
