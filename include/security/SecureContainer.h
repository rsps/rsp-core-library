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

class SecureContainerBase: public rsp::utils::DataContainerBase
{
public:
    using rsp::utils::DataContainerBase::DataContainerBase;

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

template <class D>
class SecureContainer : public rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>
{
public:
    using rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>::GetHeader;

    SecureContainer()
        : rsp::utils::DataContainer<D, rsp::utils::ContainerHeaderExtended, SecureContainerBase>()
    {
        GetHeader().Flags |= rsp::utils::ContainerFlags::Signed;
    }
};


} /* namespace rsp::security */


#endif /* INCLUDE_SECURITY_SECURECONTAINER_H_ */
