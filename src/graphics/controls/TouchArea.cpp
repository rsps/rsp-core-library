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

TouchArea::TouchArea(Rect &aArea, ClickTopics aClickedTopic, std::string aClickInfo)
    : mTouchArea(aArea), mClickedTopic(aClickedTopic), mClickEvent(aClickInfo)
{
}

TouchArea::~TouchArea()
{
}

void TouchArea::ProcessInput(Input &arInput)
{
    switch (arInput.type) {
    case InputType::Press:
        mCurrentPress = Point(arInput.x, arInput.y);
        mOriginalPress = Point(arInput.x, arInput.y);
        if (IsHit(mCurrentPress)) {
            if (mPressed) {
                mPressed(Control::States::pressed);
            }
        }
        break;
    case InputType::Lift:
        if (mPressed) {
            mPressed(Control::States::normal);
        }
        if (IsHit(mCurrentPress) && IsHit(mOriginalPress)) {
            if (mClicked) {
                mClicked(mClickedTopic, mClickEvent);
            }
        }
        // Will this return true for IsHit on things outside the scene?
        mCurrentPress = Point(-1, -1);
        break;
    case InputType::Drag:
        mCurrentPress = Point(arInput.x, arInput.y);
        if (!IsHit(mCurrentPress)) {
            if (mPressed) {
                mPressed(Control::States::normal);
            }
        }
        break;
    default:
        break;
    }
}

bool TouchArea::IsHit(const Point &arPoint) const
{
    return mTouchArea.IsHit(arPoint);
}

void TouchArea::RegisterOnPressed(std::function<void(Control::States)> aFunc)
{
    mPressed = aFunc;
}

void TouchArea::RegisterOnClicked(std::function<void(ClickTopics, ClickedEvent &)> aFunc)
{
    mClicked = aFunc;
}
} // namespace rsp::graphics
