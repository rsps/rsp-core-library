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

namespace rsp::security
{

/**
 * \brief A secure implementation of a DataContainer.
 *
 * The SecureContainer implements HMAC signing and optinal AES encryption of the
 * contents.
 */
class SecureContainerBase: public rsp::utils::DataContainerBase
{
public:
    using rsp::utils::DataContainerBase::DataContainerBase;

    /**
     * \brief Set the AES CBC initialization vector and the key to be used for
     * encryption and decryption.
     *
     * If both values are empty, encryption is disabled.
     *
     * \param aInitializationVector The AES initialization vector to use.
     * \param aKey The key to use for symmetric encryption.
     */
    void SetEncryption(SecureBuffer aInitializationVector, SecureBuffer aKey);

protected:
    SecureBuffer mInitializationVector{};
    SecureBuffer mKey{};

private:
    bool getSignature(rsp::utils::Signature_t &arSignature, std::string_view aSecret) override;
    bool checkSignature(std::string_view aSecret) override;
    void readPayloadFrom(rsp::posix::FileIO &arFile) override;
    bool writePayloadTo(rsp::posix::FileIO &arFile) override;
};

/**
 * \brief Template to be used for making secure data containers.
 * \tparam D struct or class defining container content.
 */
template <class D>
class SecureContainer : public rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>
{
public:
    using rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>::GetHeader;

    SecureContainer()
        : rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>()
    {
    }
};


} /* namespace rsp::security */


#endif /* INCLUDE_SECURITY_SECURECONTAINER_H_ */
