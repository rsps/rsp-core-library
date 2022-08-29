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
    if (mDirty) {
        return;
    }
    mDirty = true;
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
        mTouchArea = arRect;
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
    }
    return *this;
}

bool Control::Render(Canvas &arCanvas)
{
    GFXLOG("Rendering: " << GetName() << " (" << this << ")");
    bool result = mDirty;

    if (mDirty) {
        GFXLOG("Painting: " << GetName());
        paint(arCanvas, mStyles[mState]);
    }

    for (Control* child : mChildren) {
        GFXLOG("Rendering "<< GetName() << "'s child: " << child->GetName());
        if (child->Render(arCanvas)) {
            result = true;
        }
    }

    mDirty = false;

    return result;
}

void Control::paint(Canvas &arCanvas, const Style &arStyle)
{
    if (!mTransparent) {
        arCanvas.DrawRectangle(mArea, arStyle.mBackgroundColor, true);
    }
}

Control& Control::SetDraggable(bool aValue)
{
    if (mDraggable != aValue) {
        mDraggable = aValue;
        Invalidate();
    }
    return *this;
}

Control& Control::Show(bool aVisible)
{
    if (mVisible != aVisible) {
        mVisible = aVisible;
        Invalidate();
    }
    return *this;
}

Control& Control::ToggleChecked()
{
    mChecked = !mChecked;
    Invalidate();
    return *this;
}

Control& Control::SetTransparent(bool aValue)
{
    if (mTransparent != aValue) {
        mTransparent = aValue;
        Invalidate();
    }
    return *this;
}

bool Control::ProcessInput(TouchEvent &arInput)
{
    if (!IsVisible()) {
        return false;
    }
    if ((GetState() == Control::States::disabled) && mTouchArea.IsHit(arInput.mCurrent)) {
        return true;
    }

    bool result = false;

    switch (arInput.mType) {
        case TouchEvent::Types::Press:
            if (mTouchArea.IsHit(arInput.mCurrent)) {
                for(Control *child : mChildren) {
                    result = child->ProcessInput(arInput);
                    if (result) {
                        break;
                    }
                }
                if (!result) {
                    result = true;
                    doPress(arInput.mCurrent);
                }
            }
            break;

        case TouchEvent::Types::Lift:
            if (mTouchArea.IsHit(arInput.mPress)) {
                for(Control *child : mChildren) {
                    result = child->ProcessInput(arInput);
                    if (result) {
                        break;
                    }
                }
                if (!result) {
                    doLift(arInput.mCurrent);
                    if (mTouchArea.IsHit(arInput.mCurrent)) {
                        ToggleChecked();
                        doClick(arInput.mCurrent);
                        result = true;
                    }
                }
            }
            break;

        case TouchEvent::Types::Drag:
            if (mTouchArea.IsHit(arInput.mPress)) {
                for(Control *child : mChildren) {
                    result = child->ProcessInput(arInput);
                    if (result) {
                        break;
                    }
                }
                if (!result) {
                    if (IsDraggable()) {
                        doMove(arInput.mCurrent);
                    }
                    result = true;
                }
            }
            break;

        default:
            break;
    }

    return result;
}

void Control::doPress(const Point &arPoint)
{
    SetState(Control::States::pressed);
    mOnPress(arPoint, GetId());
}

void Control::doMove(const Point &arPoint)
{
    SetState(Control::States::dragged);
    mOnMove(arPoint, GetId());
}

void Control::doLift(const Point &arPoint)
{
    SetState(Control::States::normal);
    mOnLift(arPoint, GetId());
}

void Control::doClick(const Point &arPoint)
{
    mOnClick(arPoint, GetId());
}

} // namespace rsp::graphics
