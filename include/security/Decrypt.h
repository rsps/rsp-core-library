/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_DECRYPT_H_
#define INCLUDE_SECURITY_DECRYPT_H_

#include <security/CryptBase.h>

namespace rsp::security
{

/**
 * \brief Specialised crypto class used for decryption.
 */
class Decrypt: public CryptBase
{
public:
    Decrypt(CipherTypes aCipher = CipherTypes::AES_128_CBC);

    void Init(const SecureBuffer& arIvSeed, const SecureBuffer& arSecret) override;
    SecureBuffer Finalize() override { return pImpl->Finalize(); }
    void Update(const uint8_t *apData, std::size_t aSize) override { pImpl->Update(apData, aSize); }

protected:
    static std::unique_ptr<CryptBase> MakePimpl(CipherTypes aCipher);
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_DECRYPT_H_ */
