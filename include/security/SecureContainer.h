/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SECURECONTAINER_H_
#define INCLUDE_SECURITY_SECURECONTAINER_H_

#include <utils/DataContainer.h>
#include <security/SecureBuffer.h>
#include <security/Sha256DataSignature.h>
#include <security/EncryptedFileDataStorage.h>

namespace rsp::security
{

/**
 * \brief Template to be used for making secure data containers with SHA-256 HMAC signatures and AES symmetric key encryption.
 * \tparam T struct or class defining container content.
 */
template <class T>
class SecureContainer : public rsp::utils::DataContainerBase<T, Sha256DataSignature, EncryptedFileDataStorage>
{
public:
    SecureContainer(std::string_view aFileName, const SecureBuffer& mrSecret, const SecureBuffer& arInitializationVector, const SecureBuffer& arKey)
    {
        this->GetSignature().Init(mrSecret);
        this->GetStorage().Init(aFileName, arInitializationVector, arKey);
    }
};


} /* namespace rsp::security */


#endif /* INCLUDE_SECURITY_SECURECONTAINER_H_ */
