/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_ENCRYPT_H_
#define INCLUDE_SECURITY_ENCRYPT_H_

#include <string>
#include <memory>
#include <security/CryptBase.h>

namespace rsp::security {

class Encrypt: public CryptBase
{
public:
    Encrypt();

    void Init(std::string_view aIvSeed, std::string_view aSecret) override;
    SecureBuffer Finalize() override { return pImpl->Finalize(); }
    void Update(const uint8_t *apData, std::size_t aSize) override { pImpl->Update(apData, aSize); }

protected:
    static std::unique_ptr<CryptBase> MakePimpl(CipherTypes aCipher);
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_ENCRYPT_H_ */
