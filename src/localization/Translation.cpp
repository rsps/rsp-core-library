/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <localization/Translation.h>

namespace rsp::localization {

std::string_view Translation::Translate(uint32_t /*aHash*/, std::string_view aDefault) const
{
    return aDefault;
}

} /* namespace rsp::localization */
