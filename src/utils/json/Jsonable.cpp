/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <json/Jsonable.h>
#include <utils/CoreException.h>

namespace rsp::json {

void Jsonable::FromJson(const JsonValue &arJson)
{
    THROW_WITH_BACKTRACE1(rsp::utils::NotImplementedException, "Override the FromJson method to populate an object from a JSON string.");
}

} /* namespace rsp::utils */


