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

namespace rsp::graphics
{
TouchArea::TouchArea(Rect &aArea)
    : mTouchArea(aArea)
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
        std::cout << "TouchArea Setting pressed state" << std::endl;
        if (IsHit(Point(aInput.x, aInput.y))) {
            mPressed(Control::States::pressed);
        }
        break;
    case InputType::Lift:
        std::cout << "TouchArea Setting normal state" << std::endl;
        if (IsHit(Point(aInput.x, aInput.y))) {
            mClicked();
        }
        mPressed(Control::States::normal);
        break;
    case InputType::Drag:
        if (!IsHit(Point(aInput.x, aInput.y))) {
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
void TouchArea::RegisterOnClicked(std::function<void()> aFunc)
{
    mClicked = aFunc;
}
} // namespace rsp::graphics