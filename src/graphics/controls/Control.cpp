/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/Control.h"

namespace rsp::graphics
{

Control::Control(Rect &aRect)
    : mArea(aRect)
{
}

Control::~Control()
{
}

void Control::Invalidate()
{
    mIsInvalid = true;
}

bool Control::IsInvalid() const
{
    return mIsInvalid;
}

bool Control::IsHit(const Point &aPoint)
{
    // Only works with non-rotated rectangles
    if (aPoint.GetX() > mArea.GetTopLeft().GetX() &&
        aPoint.GetX() < mArea.GetBottomRight().GetX() &&
        aPoint.GetY() > mArea.GetTopLeft().GetY() &&
        aPoint.GetY() < mArea.GetBottomRight().GetY())
        return true;

    return false;
}

} // namespace rsp::graphics
