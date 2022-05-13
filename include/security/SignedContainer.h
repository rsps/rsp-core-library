/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SIGNEDCONTAINER_H_
#define INCLUDE_SECURITY_SIGNEDCONTAINER_H_

#include <utils/DataContainer.h>
#include <security/SecureBuffer.h>
#include <security/Sha256DataSignature.h>

namespace rsp::security
{

/**
 * \brief Template to be used for making secure data containers.
 * \tparam T struct or class defining container content.
 */
template <class T>
class SignedContainer : public rsp::utils::DataContainerBase<T, Sha256DataSignature, rsp::utils::FileDataStorage>
{
public:
    SignedContainer(std::string_view aFileName, const SecureBuffer& mrSecret)
    {
        this->GetSignature().Init(mrSecret);
        this->GetStorage().Init(aFileName);
    }
};


} /* namespace rsp::security */




#endif /* INCLUDE_SECURITY_SIGNEDCONTAINER_H_ */
