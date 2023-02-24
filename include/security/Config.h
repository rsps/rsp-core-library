/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_CONFIG_H_
#define INCLUDE_SECURITY_CONFIG_H_

#include <utils/DynamicData.h>
#include <security/SignedContainer.h>
#include <utils/Validator.h>

namespace rsp::security {

/**
 * \brief Template class for using data struct as configure content
 * \tparam T struct or class type.
 *
 * Usage: Config<MyConfigData> config;
 *
 * Implement the Validate method to make runtime validation of the data content.
 * Implement the Jsonable interface so configuration can be updated from
 * json network data.
 */
template <typename T>
class Config : public SignedContainer<T>, public rsp::utils::DynamicData::Serializable
{
public:
    using SignedContainer<T>::SignedContainer;

    /**
     * \brief Interface method for content validation.
     */
    virtual void Validate() = 0;
};


} // namespace rsp::security


#endif /* INCLUDE_SECURITY_CONFIG_H_ */
