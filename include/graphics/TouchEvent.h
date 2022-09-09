/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <chrono>
#include <graphics/primitives/Point.h>

namespace rsp::graphics
{

/**
 * \brief Object tracking touch interface signals
 */
class TouchEvent
{
  public:
    /**
     * \brief Enum type specifying the different types of touch events
     * All coordinates are in absolute screen pixel values.
     */
    enum class Types {
        None,
        Press,
        Drag,
        Lift
    };

    TouchEvent() {}

    TouchEvent(int aOffset, Types aType, const Point &arPoint)
        : mTime(std::chrono::steady_clock::now()),
          mType(aType),
          mCurrent(arPoint)
    {
        mTime += std::chrono::milliseconds(aOffset);
        if (mType == Types::Press) {
            mPress = arPoint;
        }
    }

    TouchEvent(std::chrono::steady_clock::time_point aTime, Types aType, const Point &arPoint)
        : mTime(aTime),
          mType(aType),
          mCurrent(arPoint)
    {
        if (mType == Types::Press) {
            mPress = arPoint;
        }
    }

    void Assign(const TouchEvent &arOther)
    {
        mTime = arOther.mTime;
        mType = arOther.mType;
        mCurrent = arOther.mCurrent;
        if (mType == Types::Press) {
            mPress = arOther.mCurrent;
        }
    }

    std::chrono::steady_clock::time_point mTime{};
    Types mType = Types::None;
    Point mCurrent{};  // Value of the latest absolute coordinate from touch controller
    Point mPress{}; // Absolute coordinate of latest press
};

struct TouchEvents
{
    const TouchEvent *mpEvents;
    std::size_t mCount;
};

std::ostream &operator<<(std::ostream &os, const TouchEvent &arTouchEvent);

} // namespace rsp::graphics

#endif // INPUT_H
