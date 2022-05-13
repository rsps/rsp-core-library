/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/Sha.h>

namespace rsp::security {

Sha::Sha(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm)
    : mPimpl(DigestImpl::Create(arSecret, aAlgorithm))
{
}

Sha::~Sha()
{
}

} /* namespace rsp::utils */

