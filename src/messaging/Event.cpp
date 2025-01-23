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
#ifdef USE_GFX
    #include <graphics/GfxEvents.h>
#endif /* USE_GFX */

namespace rsp::messaging {

std::ostream& operator<<(std::ostream &os, const rsp::messaging::Event &arEvent)
{
#ifdef USE_GFX
    if (arEvent.IsType<rsp::graphics::TouchEvent>()) {
        os << arEvent.CastTo<rsp::graphics::TouchEvent>();
    }
    else
#endif /* USE_GFX */
    {
        os << "Event (" << arEvent.Type << ") " << arEvent.Name;
    }
    return os;
}

} // namespace rsp::messaging
