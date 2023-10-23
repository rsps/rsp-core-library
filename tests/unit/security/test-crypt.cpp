/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <string>
#include <security/Encrypt.h>
#include <security/Decrypt.h>
#include <security/SecureString.h>
#include <security/SecureBuffer.h>

using namespace rsp::security;

/**
 * \brief Test case for AES encryption implementation
 *
 * For online validation of encrypted data (expected_result):
 * @see https://www.javainuse.com/aesgenerator
 */
TEST_CASE("Crypt")
{
    const std::string cTestStr("How much wood would a woodchuck chuck, if a woodchuck would chuck wood.");

    CHECK_NOTHROW(SecureString iv1("598ef20fc18d54d7"));
    CHECK_NOTHROW(SecureString key1("3167ceee9cb36d05a553fd48752e077e8e2c7edef0c046f7"));

    SecureString iv("598ef20fc18d54d7");
    SecureString key("3167ceee9cb36d05a553fd48752e077e8e2c7edef0c046f7");
    CipherTypes cipher;
    std::string expected_result;

    SUBCASE("AES-128-CBC")
    {
        key.resize(16);
        MESSAGE("IV: " << iv);
        MESSAGE("Key: " << key);
        cipher = CipherTypes::AES_128_CBC;
        expected_result = "361dc1a8822f9b5cd58704cbe9c75ae9d9b40aa5e4bd8dfc6182c424341f1ae156750c656001295bb21f7f6b7eb2061efb30f70c1ad0fc877c903112832c5ce2759f208eec505b621ac9acefc74b007e";
    }
    SUBCASE("AES-192-CBC")
    {
        key.resize(24);
        MESSAGE("Key: " << key);
        cipher = CipherTypes::AES_192_CBC;
        expected_result = "77c2688ead5f186398dbec07ccb74621dfaf212faa7b691c33c867c4b161bb3259ea85dc1619ba8bcb122837d443deb95bc390df30181d6a1c34b7aea91f840337d59b3bb2d13438f111361a7e3989fd";
    }
    SUBCASE("AES-256-CBC")
    {
        key.resize(32);
        MESSAGE("Key: " << key);
        cipher = CipherTypes::AES_256_CBC;
        expected_result = "694d7c4691fa769173e87e3523d362b8ad424ea0495551fb99d6e4e3f793446b963161512527249a3081747e92e417aa077fd24c211e2c28d608dcf090ab2851654d0874a931e7e1c4fca364c0f25dac";
    }

    CHECK_NOTHROW(SecureBuffer siv1(iv.data(), iv.size()));
    SecureBuffer siv(iv.data(), iv.size());
    SecureBuffer secure_key(key.data(), key.size());

    Encrypt e(cipher);
    e.Init(siv, secure_key);
    e.Update(reinterpret_cast<const std::uint8_t*>(cTestStr.data()), cTestStr.size());
    SecureBuffer encrypted = e.Finalize();

    CHECK_EQ(encrypted, expected_result);

    Decrypt d(cipher);
    d.Init(siv, secure_key);
    d.Update(encrypted.data(), encrypted.size());
    SecureString plain = d.Finalize();

    CHECK(bool(cTestStr == plain));
}


