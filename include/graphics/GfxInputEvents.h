/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <chrono>
#include <utils/Singleton.h>
#include "Point.h"

#ifndef INCLUDE_GRAPHICS_GFXEVENTS_H_
#define INCLUDE_GRAPHICS_GFXEVENTS_H_

namespace rsp::graphics {


/**
 * \brief Enum type specifying the different types of GFX events
 */
enum class EventTypes {
    None,
    Press,
    Drag,
    Lift,
    Quit
};

/**
 * \brief Object tracking touch interface signals
 */
struct GfxEvent
{
    std::chrono::steady_clock::time_point mTime{};
    EventTypes mType = EventTypes::None;
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    Point mPress{}; // Absolute coordinate of latest press

    GfxEvent() {}
    GfxEvent(int aOffset, EventTypes aType, const Point &arPoint)
        : mTime(std::chrono::steady_clock::now()),
          mType(aType),
          mCurrent(arPoint)
    {
        mTime += std::chrono::milliseconds(aOffset);
        if (mType == EventTypes::Press) {
            mPress = arPoint;
        }
    }

    GfxEvent(std::chrono::steady_clock::time_point aTime, EventTypes aType, const Point &arPoint)
        : mTime(aTime),
          mType(aType),
          mCurrent(arPoint)
    {
        if (mType == EventTypes::Press) {
            mPress = arPoint;
        }
    }

    GfxEvent(const GfxEvent&) = default;
    GfxEvent(GfxEvent&&) = default;

    GfxEvent& operator=(GfxEvent&&) = default;

    GfxEvent& operator=(const GfxEvent& arOther)
    {
        Assign(arOther);
        return *this;
    }

    void Assign(const GfxEvent &arOther)
    {
        if (this == &arOther) {
            return;
        }
        mTime = arOther.mTime;
        mType = arOther.mType;
        mCurrent = arOther.mCurrent;
        if (mType == EventTypes::Press) {
            mPress = arOther.mCurrent;
        }
    }
};

std::ostream &operator<<(std::ostream &os, const GfxEvent &arGfxEvent);

/**
 * \brief Interface for GFX events parser
 */
class GfxInputEvents
{
public:
    static GfxInputEvents& Get();

    virtual ~GfxInputEvents() {}

    /**
     * \brief Parse input from event driver
     * \param Reference to the event object to be populated
     * \return bool True if the event is successfully filled
     */
    virtual bool Poll(GfxEvent &arInput) = 0;

    /**
     * \brief Flush the input buffer for remaining input events
     */
    virtual void Flush() = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXEVENTS_H_ */
