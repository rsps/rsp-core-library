/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Validator.h>

namespace rsp::utils {


void Validator::NotEmpty(const char *apString)
{
    if (*apString == 0) {
        THROW_WITH_BACKTRACE(ENotEmpty);
    }
}

void Validator::NotEmpty(const std::string &arString)
{
    if (arString.empty()) {
        THROW_WITH_BACKTRACE(ENotEmpty);
    }
}

} /* namespace rsp::utils */
