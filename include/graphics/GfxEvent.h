/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXEVENT_H_
#define INCLUDE_GRAPHICS_GFXEVENT_H_

#include <chrono>
#include "Point.h"

namespace rsp::graphics {

/**
 * \brief Enum type specifying the different types of GFX events
 */
enum class EventTypes {
    None,
    Press,
    Drag,
    Lift,
    Refresh,
    Quit
};

/**
 * \brief Object tracking touch interface signals
 */
class GfxEvent
{
public:
    std::chrono::steady_clock::time_point mTime{};
    EventTypes mType = EventTypes::None;
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    Point mPress{}; // Absolute coordinate of latest press

    GfxEvent() {}
    GfxEvent(int aOffset, EventTypes aType, const Point &arPoint);

    GfxEvent(std::chrono::steady_clock::time_point aTime, EventTypes aType, const Point &arPoint);

    GfxEvent(const GfxEvent&) = default;
    GfxEvent(GfxEvent&&) = default;

    GfxEvent& operator=(GfxEvent&&) = default;

    GfxEvent& operator=(const GfxEvent& arOther);

    void Assign(const GfxEvent &arOther);
};

std::ostream &operator<<(std::ostream &os, const GfxEvent &arGfxEvent);

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXEVENT_H_ */
