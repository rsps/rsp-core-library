/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXEVENTS_H_
#define INCLUDE_GRAPHICS_GFXEVENTS_H_

#include <chrono>
#include <memory>
#include <messaging/Event.h>
#include <logging/LogStream.h>
#include "Point.h"

namespace rsp::graphics {

enum class TouchTypes : uint32_t {
    None,
    Press,
    Drag,
    Lift
};

class Control;

/**
 * \brief Object tracking touch interface events
 */
struct TouchEvent: public rsp::messaging::EventBase<TouchEvent>
{
    TouchTypes mType = TouchTypes::None;
    std::chrono::steady_clock::time_point mTime{};
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    std::chrono::steady_clock::time_point mPressTime{};
    Point mPress{-1, -1}; // Absolute coordinate of latest press

    TouchEvent() {};
    TouchEvent(int aOffset, TouchTypes aType, const Point &arPoint);
    TouchEvent(const TouchEvent&);
//    TouchEvent(TouchEvent&&);
    TouchEvent& operator=(const TouchEvent&);
//    TouchEvent& operator=(TouchEvent&&);

    void Assign(const TouchEvent &arOther);

protected:
    friend Control;
    Control *mpCtrl = nullptr;
};

std::ostream &operator<<(std::ostream &os, const TouchEvent &arEvent);

class RefreshEvent : public rsp::messaging::EventBase<RefreshEvent>
{
};

class QuitEvent : public rsp::messaging::EventBase<QuitEvent>
{
};

using GfxEvent = rsp::messaging::EventPtr_t;

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXEVENTS_H_ */
