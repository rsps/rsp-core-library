/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/Control.h>
#include <logging/Logger.h>

using namespace rsp::logging;

namespace rsp::graphics
{

Color Control::mTouchAreaColor = Color::None;

std::string to_string(Control::States aState)
{
    return std::string(magic_enum::enum_name<Control::States>(aState));
}

std::ostream& operator <<(std::ostream &os, const Control::States aState)
{
    os << to_string(aState);
    return os;
}

Control::~Control()
{
    if (mpParent) {
        mpParent->RemoveChild(this);
    }
}

void Control::setName(const std::string &arName)
{
    TypeInfo::setName(arName);
    Invalidate();
}

void Control::setId(uint32_t aId)
{
    TypeInfo::setId(aId);
    Invalidate();
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
    mDirty = true;
}

Control& Control::SetArea(Rect aRect)
{
    if (mpParent) {
        aRect.MoveTo(aRect.GetTopLeft() + mpParent->GetOrigin());
    }
    if (mArea != aRect) {
        for (Control* child : mChildren) {
            child->SetOrigin((child->GetOrigin() - mArea.GetTopLeft()) + aRect.GetTopLeft());
        }
        mArea = aRect;
        Invalidate();
        if (mpParent) {
            // Force repaint of parent, this is resizing
            mpParent->Invalidate();
        }
    }
    doSetArea(aRect);
    return *this;
}

void Control::doSetArea(const Rect &arRect)
{
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
    Point difference = arPoint - mArea.GetTopLeft();
    mArea.MoveTo(arPoint);
    mTouchArea.MoveTo(mTouchArea.GetTopLeft() + difference);
    for (Control* child : mChildren) {
        child->SetOrigin(child->GetOrigin() + difference);
    }
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

Control& Control::RemoveChild(Control *apChild)
{
    if (apChild) {
        auto it = std::find(mChildren.begin(), mChildren.end(), apChild);
        if (it != mChildren.end()) {
            mChildren.erase(it);
            Invalidate();
        }
    }
    return *this;
}

Control& Control::SetTexturePosition(const Point &arPoint)
{
    for (auto &style : mStyles) {
        for (TexturePtr_t &texture : style.mTextures) {
            texture->SetDestination(arPoint);
        }
    }
    Invalidate();
    return *this;
}

bool Control::UpdateData()
{
    GFXLOG("Updating Data: " << GetName() << " " << mArea);
    bool result = false;
    refresh();
    if (mDirty) {
        update();
        mDirty = false;
        result = true;
    }
    for (Control* child : mChildren) {
        if (child->UpdateData()) {
            result = true;
        }
    }
    return result;
}

void Control::Render(Renderer &arRenderer) const
{
    if (!mVisible) {
        return;
    }

    GFXLOG("Rendering: " << GetName() << " " << mArea);

    auto &style = mStyles[mState];

    if (!mTransparent) {
        arRenderer.Fill(style.mBackgroundColor, mArea);
    }

    for (const TexturePtr_t& texture : style.mTextures) {
        arRenderer.Blit(*texture);
    }

    for (Control* child : mChildren) {
        child->Render(arRenderer);
    }

    if ((mTouchAreaColor != Color::None) && !mTouchArea.empty()) {
        Rect r = mTouchArea;
        r.AddSize(1, 1);
        arRenderer.DrawRect(mTouchAreaColor, mTouchArea);
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

Control& Control::SetTransparent(bool aValue)
{
    if (mTransparent != aValue) {
        mTransparent = aValue;
        Invalidate();
    }
    return *this;
}

bool Control::ProcessInput(GfxEvent &arInput)
{
    if (!IsVisible()) {
        return false;
    }
    if ((GetState() == Control::States::Disabled) && mTouchArea.IsHit(arInput.mCurrent)) {
        return true;
    }

    switch (arInput.mType) {
        case EventTypes::Press:
            if (mArea.IsHit(arInput.mCurrent)) {
                for(Control *child : mChildren) {
                    if (child->ProcessInput(arInput)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(arInput.mCurrent)) {
                doPress(arInput.mCurrent);
                if (GetState() == Control::States::Checked) {
                    SetState(Control::States::CheckedPressed);
                }
                else {
                    SetState(Control::States::Pressed);
                }
                return true;
            }
            break;

        case EventTypes::Lift:
            if (mArea.IsHit(arInput.mPress)) {
                for(Control *child : mChildren) {
                    if (child->ProcessInput(arInput)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(arInput.mPress)) {
                doLift(arInput.mCurrent);
                if (mTouchArea.IsHit(arInput.mCurrent)) {
                    Logger::GetDefault().Debug() << GetName() << " was clicked by " << arInput;
                    if (IsCheckable()) {
                        if (IsChecked()) {
                            mState = States::Normal;
                        }
                        else {
                            mState = States::Checked;
                        }
                        Invalidate();
                    }
                    else {
                        SetState(Control::States::Normal);
                    }
                    doClick(arInput.mCurrent);
                }
                else {
                    SetState(Control::States::Normal);
                }
                return true;
            }
            break;

        case EventTypes::Drag:
            if (mArea.IsHit(arInput.mPress)) {
                for(Control *child : mChildren) {
                    if (child->ProcessInput(arInput)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(arInput.mPress)) {
                if (IsDraggable()) {
                    doMove(arInput.mCurrent);
                    SetState(Control::States::Dragged);
                }
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

void Control::doPress(const Point &arPoint)
{
    mOnPress(arPoint, GetId());
}

void Control::doMove(const Point &arPoint)
{
    mOnMove(arPoint, GetId());
}

void Control::doLift(const Point &arPoint)
{
    mOnLift(arPoint, GetId());
}

void Control::doClick(const Point &arPoint)
{
    mOnClick(arPoint, GetId());
}

} // namespace rsp::graphics
