/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/TouchArea.h"

using namespace rsp::messaging;

namespace rsp::graphics
{

TouchArea::TouchArea()
{
    std::cout << "TouchArea Construct" << std::endl;
}

TouchArea::~TouchArea()
{
}

TouchArea& TouchArea::SetArea(const Rect &arRect)
{
    std::cout << "TouchArea SetArea" << std::endl;
    mTouchArea = arRect;
    return *this;
}

void TouchArea::ProcessInput(Input &arInput)
{
    switch (arInput.type) {
        case InputType::Press:
            mCurrentPress = Point(arInput.x, arInput.y);
            mOriginalPress = Point(arInput.x, arInput.y);
            if (IsHit(mCurrentPress)) {
                mOnPress(mCurrentPress);
            }
            break;

        case InputType::Lift:
            mOnLift(mOriginalPress);
            if (IsHit(mCurrentPress) && IsHit(mOriginalPress)) {
                mOnClick(mCurrentPress);
            }
            break;

        case InputType::Drag:
            mCurrentPress = Point(arInput.x, arInput.y);
            mOnMove(mCurrentPress);
            break;

        default:
            break;
    }
}

bool TouchArea::IsHit(const Point &arPoint) const
{
    return mTouchArea.IsHit(arPoint);
}

} // namespace rsp::graphics
