/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_GFX_EVENTS_H
#define RSP_CORE_LIB_GRAPHICS_GFX_EVENTS_H

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

/**
 * \brief Object tracking touch interface events
 */
struct TouchEvent: public messaging::EventBase<TouchEvent>
{
    TouchTypes mType = TouchTypes::None;
    std::chrono::steady_clock::time_point mTime{};
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    std::chrono::steady_clock::time_point mPressTime{};
    Point mPress{-1, -1}; // Absolute coordinate of latest press

    TouchEvent() = default;
    TouchEvent(int aOffset, TouchTypes aType, const Point &arPoint);
    TouchEvent(const TouchEvent&);
    TouchEvent(TouchEvent&&) = delete;
    TouchEvent& operator=(const TouchEvent&);
    TouchEvent& operator=(TouchEvent&&) = delete;

    void Assign(const TouchEvent &arOther);

    void ToStream(std::ostream &os) const override
    {
        const auto int_msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mTime);
        os << int_msec.count() << " "
            << std::string(magic_enum::enum_name<TouchTypes>(mType)) << "(" << mCurrent << ")";
    }
};

class RefreshEvent : public messaging::EventBase<RefreshEvent>
{
};

class QuitEvent : public messaging::EventBase<QuitEvent>
{
};

using GfxEvent = messaging::EventPtr_t;

enum class GfxEvents : size_t {
    Touch = TouchEvent::ClassType,
    Refresh = RefreshEvent::ClassType,
    Quit = QuitEvent::ClassType
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_GFX_EVENTS_H
