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

TouchArea::TouchArea(Rect &aArea, ClickTopic aClickedTopic, std::string aClickInfo)
    : mTouchArea(aArea), mClickedTopic(aClickedTopic), mclickEvent(aClickInfo)
{
}

TouchArea::~TouchArea()
{
}

void TouchArea::ProcessInput(Input &aInput)
{
    std::cout << "TouchArea Processing Input" << std::endl;
    switch (aInput.type) {
    case InputType::Press:
        mCurrentPress = Point(aInput.x, aInput.y);
        mOriginalPress = Point(aInput.x, aInput.y);
        if (IsHit(mCurrentPress)) {
            std::cout << "TouchArea Setting pressed state" << std::endl;
            mPressed(Control::States::pressed);
        }
        break;
    case InputType::Lift:
        std::cout << "TouchArea Setting normal state" << std::endl;
        mPressed(Control::States::normal);
        if (IsHit(mCurrentPress) && IsHit(mOriginalPress)) {
            std::cout << "Calling Clicked Callback" << std::endl;
            mClicked(mClickedTopic, mclickEvent);
        }
        // Will this return true for IsHit on things outside the scene?
        mCurrentPress = Point(-1, -1);
        break;
    case InputType::Drag:
        mCurrentPress = Point(aInput.x, aInput.y);
        if (!IsHit(mCurrentPress)) {
            mPressed(Control::States::normal);
        }
        break;
    default:
        break;
    }
}

bool TouchArea::IsHit(const Point &aPoint) const
{
    return mTouchArea.IsHit(aPoint);
}

void TouchArea::RegisterOnPressed(std::function<void(Control::States)> aFunc)
{
    mPressed = aFunc;
}

void TouchArea::RegisterOnClicked(std::function<void(ClickTopic, ClickedEvent &)> aFunc)
{
    mClicked = aFunc;
}
} // namespace rsp::graphics
