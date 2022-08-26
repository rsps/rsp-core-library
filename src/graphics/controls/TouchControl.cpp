/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/controls/TouchControl.h>

namespace rsp::graphics {

TouchControl::TouchControl()
    : Control(rsp::utils::MakeTypeInfo<TouchControl>())
{
    addTouchable(this);
}

TouchControl::TouchControl(const rsp::utils::TypeInfo &arInfo)
    : Control(arInfo)
{
    addTouchable(this);
}

TouchControl::~TouchControl()
{
    removeTouchable(this);
}

TouchControl::TouchControl(const TouchControl &arOther)
{
    operator=(arOther);
}

TouchControl::TouchControl(TouchControl &&arOther)
{
    *this = std::move(arOther);
}

TouchControl& TouchControl::operator =(const TouchControl &arOther)
{
    if (this != &arOther) {
        mOnPress = arOther.mOnPress;
        mOnMove = arOther.mOnMove;
        mOnLift = arOther.mOnLift;
        mOnClick = arOther.mOnClick;

        mTouchArea = arOther.mTouchArea;
        mCurrentPress = arOther.mCurrentPress;
        mOriginalPress = arOther.mOriginalPress;
    }
    return *this;
}

TouchControl& TouchControl::operator =(TouchControl &&arOther)
{
    if (this != &arOther) {
        mOnPress = std::move(arOther.mOnPress);
        mOnMove = std::move(arOther.mOnMove);
        mOnLift = std::move(arOther.mOnLift);
        mOnClick = std::move(arOther.mOnClick);

        mTouchArea = std::move(arOther.mTouchArea);
        mCurrentPress = std::move(arOther.mCurrentPress);
        mOriginalPress = std::move(arOther.mOriginalPress);
    }
    return *this;
}

TouchControl& TouchControl::SetArea(const Rect &arRect)
{
    Control::SetArea(arRect);
    mTouchArea = arRect;
    return *this;
}

void TouchControl::ProcessInput(TouchEvent &arInput)
{
    if (GetState() == Control::States::disabled) {
        return;
    }
    switch (arInput.mType) {
        case TouchEvent::Types::Press:
            mCurrentPress = arInput.mPoint;
            mOriginalPress = arInput.mPoint;
            if (IsHit(mCurrentPress)) {
                doPress(mCurrentPress);
            }
            break;

        case TouchEvent::Types::Lift:
            if (IsHit(mOriginalPress)) {
                mCurrentPress = arInput.mPoint;
                doLift(mCurrentPress);
                if (IsHit(mCurrentPress)) {
                    doClick(mCurrentPress);
                }
            }
            break;

        case TouchEvent::Types::Drag:
            if (IsHit(mOriginalPress)) {
                mCurrentPress = arInput.mPoint;
                doMove(mCurrentPress);
            }
            break;

        default:
            break;
    }
}

bool TouchControl::IsHit(const Point &arPoint) const
{
    return mTouchArea.IsHit(arPoint);
}

void TouchControl::doPress(const Point &arPoint)
{
    SetState(Control::States::pressed);
    mOnPress(arPoint, GetId());
}

void TouchControl::doMove(const Point &arPoint)
{
    SetState(Control::States::dragged);
    mOnMove(arPoint, GetId());
}

void TouchControl::doLift(const Point &arPoint)
{
    SetState(Control::States::normal);
    mOnLift(arPoint, GetId());
}

void TouchControl::doClick(const Point &arPoint)
{
    mOnClick(arPoint, GetId());
}

} // namespace rsp::graphics
