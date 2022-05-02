/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_SECURITY_SIGNEDDATA_H_
#define INCLUDE_SECURITY_SIGNEDDATA_H_

#include <security/Sha.h>
#include <array>
#include <string>
#include <string_view>
#include <utils/CoreException.h>

namespace rsp::security
{

class EInvalidSignature : public rsp::utils::CoreException
{
public:
    explicit EInvalidSignature()
      : CoreException("Invalid signature on data")
    {
    }
};

class SignedData
{
public:
    SignedData(std::string_view aSecret);

    void Verify(const uint8_t* apData, std::size_t aSize);
    void Verify(const std::string &arFileName);

    template <typename T>
    void Verify(const T &arData)
    {
        Verify(static_cast<uint8_t*>(&arData), sizeof(arData));
    }

    MessageDigest GetSignature(const uint8_t* apData, std::size_t aSize);
    MessageDigest GetSignature(const std::string &arFileName, bool aExcludeDigest = false);

    template <typename T>
    MessageDigest Sign(const T &arData)
    {
        return GetSignature(static_cast<uint8_t*>(&arData), sizeof(arData));
    }

protected:
    Sha mSha;
};

} /* namespace rsp::utils */

#endif /* INCLUDE_SECURITY_SIGNEDDATA_H_ */
