/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_H_
#define INCLUDE_CONFIGURATION_CONFIG_H_

#include <json/Jsonable.h>
#include <security/SecureContainer.h>
#include <memory>
#include <string>
#include <string_view>
#include <utils/DataContainer.h>
#include <utils/CoreException.h>

namespace rsp::config {


template <typename T>
class Config : public rsp::utils::DataContainer<T, rsp::utils::ContainerHeaderExtended, rsp::security::SecureContainerBase>, public rsp::json::Jsonable
{
public:

protected:
    virtual void validate() = 0;
};


} // namespace rsp::config


#endif /* INCLUDE_CONFIGURATION_CONFIG_H_ */
