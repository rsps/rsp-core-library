/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_AES_H_
#define INCLUDE_SECURITY_AES_H_

#include <string_view>

namespace rsp::security {

class Aes
{
public:
    constexpr static int cBlockSize = 16;
    constexpr static int cKeySize = 32;

    Aes(std::string_view aIvSeed, std::string_view aSecret);
    ~Aes();

    void Encrypt(const std::string &arPlainText, std::string &arCryptText);
    void Decrypt(const std::string &arCryptText, std::string &arPlainText);

protected:
    std::uint8_t mIv[cBlockSize];
    std::uint8_t mKey[cKeySize];

    void randomize(std::uint8_t *apBuffer, int aLen, std::string_view aSeed);
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_AES_H_ */
