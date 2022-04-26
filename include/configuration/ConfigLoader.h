/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_CONFIGURATION_CONFIGLOADER_H_
#define INCLUDE_CONFIGURATION_CONFIGLOADER_H_

#include <string>
#include "Config.h"

namespace rsp::config {

class ConfigLoader
{
public:
    ConfigLoader();
    virtual ~ConfigLoader();

    void Load(const std::string &arFileName, Config &arConfig);
};

} /* namespace rsp::config */

#endif /* INCLUDE_CONFIGURATION_CONFIGLOADER_H_ */
