/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_VERSION_H
#define RSP_CORE_LIB_VERSION_H

#include <utils/VersionNumber.h>

#define LIB_VERSION "0.1.0"

namespace rsp {

utils::VersionNumber get_library_version();

}

#endif // RSP_CORE_LIB_VERSION_H
