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
}

void Control::SetState(States aState)
{
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
    for (Control* child : mChildren) {
        child->Invalidate();
    }
    if (mTransparent && mpParent) {
        mpParent->Invalidate();
    }
}

void Control::SetArea(const Rect &arRect)
{
    if (mArea != arRect) {
        mArea = arRect;
        Invalidate();
        if (mpParent) {
            // Force repaint of parent, this is resizing
            mpParent->Invalidate();
        }
    }
}

Control& Control::AddChild(Control *apChild)
{
    if (apChild) {
        mChildren.push_back(apChild);
        apChild->mpParent = this;
    }
    return *this;
}

} // namespace rsp::graphics
