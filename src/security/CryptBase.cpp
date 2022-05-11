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

SecureBuffer CryptBase::KeyGen(std::string_view aSeed, std::size_t aDesiredKeyLen)
{
    Sha sha(aSeed, HashAlgorithms::Sha256);
    sha.Update(reinterpret_cast<const uint8_t*>(aSeed.data()), aSeed.size());
    SecureBuffer md = sha.Get();

    if (aDesiredKeyLen < md.size()) {
        md.resize(aDesiredKeyLen);
    }

    return md;
}

} // namespace rsp::security
