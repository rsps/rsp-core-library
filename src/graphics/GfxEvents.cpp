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
#include <rsp/graphics/GfxEvents.h>

namespace rsp::graphics {


TouchEvent::TouchEvent(const int aOffset, const TouchTypes aType, const Point &arPoint)
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
