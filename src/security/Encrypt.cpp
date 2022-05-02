/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/Encrypt.h>

namespace rsp::security {

Encrypt::Encrypt()
{

}

Encrypt::~Encrypt()
{
}

void Encrypt::Init(std::string_view aIvSeed, std::string_view aSecret)
{
}

std::vector<unsigned char, std::allocator<unsigned char> > Encrypt::Finalize()
{
    return mCryptData;
}

void Encrypt::Update(const uint8_t *apData, std::size_t aSize)
{
}

} /* namespace rsp::security */
