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
#include <graphics/controls/TouchControl.h>

namespace rsp::graphics
{

std::string to_string(Control::States aState)
{
    const char *names[] = {
        "disabled",
        "normal",
        "pressed",
        "dragged",
        "checked"
    };
    return names[static_cast<int>(aState)];
}

std::ostream& operator <<(std::ostream &os, const Control::States aState)
{
    os << to_string(aState);
    return os;
}

void Control::SetState(States aState)
{
    if (mState != aState) {
        mState = aState;
        Invalidate();
        for (Control* child : mChildren) {
            child->SetState(aState);
        }
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

Control& Control::SetArea(const Rect &arRect)
{
    if (mArea != arRect) {
        mArea = arRect;
        Invalidate();
        if (mpParent) {
            // Force repaint of parent, this is resizing
            mpParent->Invalidate();
        }
    }
    return *this;
}

Control& Control::AddChild(Control *apChild)
{
    if (apChild) {
        mChildren.push_back(apChild);
        apChild->mpParent = this;
        auto tc = dynamic_cast<TouchControl*>(apChild);
        if (tc != nullptr) {
            addTouchable(tc);
        }
    }
    return *this;
}

bool Control::Render(Canvas &arCanvas)
{
    GFXLOG("Rendering: " << GetName() << " (" << this << ")");
    bool result = mIsInvalid;

    if (mIsInvalid) {
        GFXLOG("Painting: " << GetName());
        paint(arCanvas, mStyles[mState]);
    }

    for (Control* child : mChildren) {
        GFXLOG("Rendering "<< GetName() << "'s child: " << child->GetName());
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
