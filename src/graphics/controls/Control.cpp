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
        "Disabled",
        "Normal",
        "Pressed",
        "Dragged",
        "Checked",
        "CheckedPressed"
    };
    return names[static_cast<int>(aState)];
}

std::ostream& operator <<(std::ostream &os, const Control::States aState)
{
    os << to_string(aState);
    return os;
}

Control& Control::SetId(uint32_t aId)
{
    GetInfo().mId = aId;
    Invalidate();
    return *this;
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

Control& Control::SetArea(Rect aRect)
{
    if (mpParent) {
        aRect.MoveTo(aRect.GetTopLeft() + mpParent->GetOrigin());
    }
    if (mArea != aRect) {
        mArea = aRect;
        mTouchArea = aRect;
        Invalidate();
        if (mpParent) {
            // Force repaint of parent, this is resizing
            mpParent->Invalidate();
        }
    }
    return *this;
}

Rect Control::GetArea() const
{
    Rect r = mArea;
    if (mpParent) {
        r.MoveTo(r.GetTopLeft() - mpParent->GetOrigin());
    }
    return r;
}

Control& Control::SetOrigin(const Point &arPoint)
{
    mArea.MoveTo(arPoint);
    return *this;
}

Point Control::GetOrigin() const
{
    return mArea.GetTopLeft();
}

Rect Control::GetTouchArea()
{
    Rect r = mTouchArea;
    if (mpParent) {
        r.MoveTo(r.GetTopLeft() - mpParent->GetOrigin());
    }
    return r;
}

Control& Control::SetTouchArea(Rect aRect)
{
    if (mpParent) {
        aRect.MoveTo(aRect.GetTopLeft() + mpParent->GetOrigin());
    }
    mTouchArea = aRect;
    return *this;
}

Control& Control::AddChild(Control *apChild)
{
    if (apChild) {
        mChildren.push_back(apChild);
        Rect r = apChild->GetArea();
        apChild->mpParent = this;
        apChild->SetOrigin(r.GetTopLeft() + GetOrigin());
    }
    return *this;
}

bool Control::Render(Canvas &arCanvas)
{
    GFXLOG("Rendering: " << GetName() << " (" << this << ")");
    bool result = mDirty;

    if (mDirty && mVisible) {
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

Control& Control::SetCheckable(bool aValue)
{
    if (mCheckable != aValue) {
        mCheckable = aValue;
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

void Control::toggleChecked()
{
    if (IsCheckable()) {
        if (IsChecked()) {
            mState = States::normal;
        }
        else {
            mState = States::checked;
        }
        Invalidate();
    }
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
                        toggleChecked();
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
    if (GetState() == Control::States::checked) {
        SetState(Control::States::checkedPressed);
    }
    else {
        SetState(Control::States::pressed);
    }
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
