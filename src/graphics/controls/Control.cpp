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

Control::Control(const Rect &arRect)
    : mArea(arRect)
{
    std::cout << "Constructor of Control" << std::endl;
}

Control::~Control()
{
}

void Control::SetState(States aState)
{
    std::cout << "Control Image setting State" << std::endl;
    if (mState != aState) {
        mState = aState;
        Invalidate();
    }
}

void Control::Invalidate()
{
    if (mIsInvalid) {
        return;
    }
    mIsInvalid = true;
    std::cout << "Invalidating Ctrl object" << std::endl;
    std::cout << this << std::endl;
    for (auto child : mChildren) {
        child->Invalidate();
    }
    if (mTransparent && mpParent) {
        mpParent->Invalidate();
    }
}

} // namespace rsp::graphics
