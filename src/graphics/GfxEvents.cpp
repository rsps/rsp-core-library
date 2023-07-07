/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/GfxEvents.h>
#include <magic_enum.hpp>

namespace rsp::graphics {

std::ostream &operator<<(std::ostream &os, const TouchEvent &arEvent)
{
    os << arEvent.mTime.time_since_epoch().count() << " "
        << std::string(magic_enum::enum_name<TouchTypes>(TouchTypes(arEvent.mType))) << "(" << arEvent.mCurrent << ")";
    return os;
}

std::ostream& operator <<(std::ostream &os, const GfxEvent &arEvent)
{
    auto &event = std::get<rsp::messaging::Event>(arEvent);
    os << "Event (" << event.Type << ") " << event.Name;
    return os;
}


TouchEvent::TouchEvent(int aOffset, TouchTypes aType, const Point &arPoint)
    : mType(aType),
      mTime(std::chrono::steady_clock::now()),
      mCurrent(arPoint)
{
    mTime += std::chrono::milliseconds(aOffset);
    if (mType == TouchTypes::Press) {
        mPress = arPoint;
        mPressTime = mTime;
    }
}

//TouchEvent::TouchEvent(std::chrono::steady_clock::time_point aTime, EventTypes aType, const Point &arPoint)
//    : mTime(aTime),
//      mType(aType),
//      mCurrent(arPoint)
//{
//    if (mType == EventTypes::Press) {
//        mPress = arPoint;
//        mPressTime = mTime;
//    }
//}

TouchEvent::TouchEvent(const TouchEvent& arOther)
    : messaging::EventBase<TouchEvent>(arOther.Type, arOther.Name)
{
    Assign(arOther);
}

TouchEvent& TouchEvent::operator=(const TouchEvent& arOther)
{
    if (this != &arOther) {
        Assign(arOther);
    }
    return *this;
}

void TouchEvent::Assign(const TouchEvent &arOther)
{
    if (this == &arOther) {
        return;
    }
    mTime = arOther.mTime;
    mType = arOther.mType;
    mCurrent = arOther.mCurrent;
    if (arOther.mPressTime != std::chrono::steady_clock::time_point()) {
        mPress = arOther.mPress;
        mPressTime = arOther.mPressTime;
    }
}

} /* namespace rsp::graphics */
