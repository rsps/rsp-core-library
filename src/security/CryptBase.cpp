/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/CryptBase.h>
#include <security/Sha.h>

namespace rsp::security {

SecureBuffer CryptBase::KeyGen(const SecureBuffer& arSeed, std::size_t aDesiredKeyLen)
{
    Sha sha(arSeed, HashAlgorithms::Sha256);
    sha.Update(arSeed.data(), arSeed.size());
    SecureBuffer md = sha.Get();

    if (aDesiredKeyLen < md.size()) {
        md.resize(aDesiredKeyLen);
    }

    return md;
}

} // namespace rsp::security
