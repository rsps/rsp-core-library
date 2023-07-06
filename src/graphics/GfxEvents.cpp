/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/GfxEvent.h>
#include <magic_enum.hpp>

namespace rsp::graphics {

std::ostream &operator<<(std::ostream &os, const GfxEvent &arGfxEvent)
{
    os << arGfxEvent.mTime.time_since_epoch().count() << " "
        << std::string(magic_enum::enum_name<EventTypes>(arGfxEvent.mType)) << "(" << arGfxEvent.mCurrent << ")";
    return os;
}


GfxEvent::GfxEvent(int aOffset, EventTypes aType, const Point &arPoint)
    : mTime(std::chrono::steady_clock::now()),
      mType(aType),
      mCurrent(arPoint)
{
    mTime += std::chrono::milliseconds(aOffset);
    if (mType == EventTypes::Press) {
        mPress = arPoint;
        mPressTime = mTime;
    }
}

GfxEvent::GfxEvent(std::chrono::steady_clock::time_point aTime, EventTypes aType, const Point &arPoint)
    : mTime(aTime),
      mType(aType),
      mCurrent(arPoint)
{
    if (mType == EventTypes::Press) {
        mPress = arPoint;
        mPressTime = mTime;
    }
}

GfxEvent::GfxEvent(const GfxEvent& arOther)
{
    Assign(arOther);
}

GfxEvent& GfxEvent::operator=(const GfxEvent& arOther)
{
    if (this != &arOther) {
        Assign(arOther);
    }
    return *this;
}

void GfxEvent::Assign(const GfxEvent &arOther)
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
