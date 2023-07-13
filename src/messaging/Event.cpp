/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <messaging/Event.h>
#include <graphics/GfxEvents.h>

namespace rsp::messaging {

std::ostream& operator<<(std::ostream &os, const rsp::messaging::Event &arEvent)
{
    if (arEvent.IsType<rsp::graphics::TouchEvent>()) {
        os << arEvent.CastTo<rsp::graphics::TouchEvent>();
    }
    else {
        os << "Event (" << arEvent.Type << ") " << arEvent.Name;
    }
    return os;
}

} // namespace rsp::messaging
