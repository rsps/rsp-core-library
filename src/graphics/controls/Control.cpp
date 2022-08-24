/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/controls/Control.h>

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

bool Control::Render(Canvas &arCanvas)
{
    bool result = mIsInvalid;

    if (mIsInvalid) {
        paint(arCanvas, mStyles[mState]);
    }

    for (Control* child : mChildren) {
        if (child->Render(arCanvas)) {
            result = true;
        }
    }

    mIsInvalid = false;

    return result;
}

void Control::paint(Canvas &arCanvas, const Style &arStyle)
{
    if (!mTransparent) {
        arCanvas.DrawRectangle(mArea, arStyle.mBackgroundColor, true);
    }
}

void Control::addTouchable(TouchControl *apTouchControl)
{
    if (mpParent) {
        mpParent->addTouchable(apTouchControl);
    }
}

void Control::removeTouchable(TouchControl *apTouchControl)
{
    if (mpParent) {
        mpParent->removeTouchable(apTouchControl);
    }
}

} // namespace rsp::graphics
