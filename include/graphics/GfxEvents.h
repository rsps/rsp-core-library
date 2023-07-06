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
#include <messaging/Event.h>
#include "Point.h"

namespace rsp::graphics {

enum TouchTypes {
    None,
    Press,
    Drag,
    Lift
};

/**
 * \brief Object tracking touch interface events
 */
struct TouchEvent: public rsp::messaging::EventBase<TouchEvent>
{
    uint32_t mType{}; // = TouchTypes::None;
    std::chrono::steady_clock::time_point mTime{};
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    std::chrono::steady_clock::time_point mPressTime{};
    Point mPress{-1, -1}; // Absolute coordinate of latest press

    TouchEvent();
    TouchEvent(int aOffset, TouchTypes aType, const Point &arPoint);
    TouchEvent(const TouchEvent&);
//    TouchEvent(TouchEvent&&);
    TouchEvent& operator=(const TouchEvent&);
//    TouchEvent& operator=(TouchEvent&&);

    void Assign(const TouchEvent &arOther);
};

std::ostream &operator<<(std::ostream &os, const TouchEvent &arEvent);

class RefreshEvent : public rsp::messaging::EventBase<RefreshEvent>
{
};

class QuitEvent : public rsp::messaging::EventBase<QuitEvent>
{
};

// TODO: Take at look at std::variant and std::any

union GfxEvent {
    rsp::messaging::Event mEvent;
    TouchEvent mTouchEvent;
    RefreshEvent mRefreshEvent;
    QuitEvent mQuitEvent;

    GfxEvent() : mEvent(0, "") {}
//    GfxEvent(int aOffset, TouchTypes aType, const Point &arPoint) : mTouchEvent(aOffset, aType, arPoint) {}
    GfxEvent(const TouchEvent &arEvent) : mTouchEvent(arEvent) {}
    GfxEvent(const RefreshEvent &arEvent) : mRefreshEvent(arEvent) {}
    GfxEvent(const QuitEvent &arEvent) : mQuitEvent(arEvent) {}
};

std::ostream &operator<<(std::ostream &os, const GfxEvent &arEvent);

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXEVENTS_H_ */
