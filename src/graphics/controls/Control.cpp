/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Control.h"

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

bool Control::IsHit()
{
}

} // namespace rsp::graphics
