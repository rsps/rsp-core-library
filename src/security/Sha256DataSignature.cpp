/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/Sha256DataSignature.h>
#include <security/Sha.h>

namespace rsp::security {

void Sha256DataSignature::Init(const SecureBuffer &arSecret)
{
    mrSecret = arSecret;
    mSignature.resize(GetSize());
}

void Sha256DataSignature::Calc(const uint8_t *apData, std::size_t aSize)
{
    Sha sha(mrSecret, HashAlgorithms::Sha256);
    sha.Update(apData, aSize);
    mSignature = sha.Get();
}

void Sha256DataSignature::Verify(const uint8_t *apData, std::size_t aSize)
{
    Sha sha(mrSecret, HashAlgorithms::Sha256);
    sha.Update(apData, aSize);
    if (mSignature != sha.Get()) {
        THROW_WITH_BACKTRACE(rsp::utils::EInvalidSignature);
    }
}

} /* namespace rsp::security */
