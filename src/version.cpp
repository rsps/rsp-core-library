/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <sstream>
#include <version-def.h>
#include <version.h>

using namespace std;

namespace rsp {

string get_library_version() {
    stringstream ss;

    ss << LIB_VERSION_MAJOR << "." << LIB_VERSION_MINOR << "." << LIB_VERSION_PATCH;

    return ss.str();
}

}

