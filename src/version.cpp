/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <version.h>

namespace rsp {

utils::semver::Version get_library_version()
{
    static constexpr auto version = utils::semver::Version{LIB_VERSION};
    return version;
}

} // namespace rsp
