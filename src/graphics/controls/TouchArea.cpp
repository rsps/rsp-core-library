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
TouchArea::TouchArea(Rect &aArea, Image &aImage)
    : mTouchArea(aArea), mImage(aImage)
{
}
TouchArea::~TouchArea()
{
}
void TouchArea::ProcessEvent(Event &aEvent)
{
    switch (aEvent.type) {
    case EventType::Press:
        mImage.SetState(Control::States::pressed);
        break;
    case EventType::Lift:
        mImage.SetState(Control::States::normal);
        break;
    default:
        break;
    }
}
bool TouchArea::IsHit(const Point &aPoint) const
{
    return mTouchArea.IsHit(aPoint);
}
} // namespace rsp::graphics