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
#include <magic_enum.hpp>

using namespace rsp::logging;

namespace rsp::graphics
{

Color Control::mTouchAreaColor = Color::None;
bool Control::mMustRender = true;

std::string to_string(Control::States aState)
{
    return std::string(magic_enum::enum_name<Control::States>(aState));
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
//    for (Control* child : mChildren) {
//        child->Invalidate();
//    }
//    if (mTransparent && mpParent) {
//        mpParent->Invalidate();
//    }
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

Control& Control::SetBitmapPosition(const Point &arPoint)
{
    for (auto &tuple : mStyles) {
        tuple.second.mBackground.SetDestination(arPoint);
    }
    Invalidate();
    return *this;
}

bool Control::UpdateData()
{
    bool result = false;
    refresh();
    if (mDirty) {
        Canvas canvas(mArea.GetWidth(), mArea.GetHeight());
        paint(canvas, mStyles.at(mState));
        mTextures.at(mState)->Update(canvas.GetPixelData());
//        for (auto &tuple : mStyles) {
//            canvas.Fill(Color::None);
//            paint(canvas, tuple.second);
//            mTextures[tuple.first]->Update(canvas.GetPixelData());
//        }
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

void Control::MakeTextures(Renderer &arRenderer)
{
    mTextures.clear();
    for (auto &tuple : mStyles) {
        mTextures.emplace(tuple.first, arRenderer.CreateTexture(mArea.GetWidth(), mArea.GetHeight()));
    }
}

void Control::Render(Renderer &arRenderer)
{
    GFXLOG("Rendering: " << GetName() << " (" << this << ")");

    if (!mVisible) {
        return;
    }

    if (mTextures.find(mState) != mTextures.end()) {
        arRenderer.RenderTexture(*mTextures[mState], mArea);
    }

    for (Control* child : mChildren) {
        GFXLOG("Rendering "<< GetName() << "'s child: " << child->GetName());
        child->Render(arRenderer);
    }

    if ((mTouchAreaColor != Color::None) && !mTouchArea.empty()) {
        Rect r = mTouchArea;
        r.AddSize(1, 1);
        arRenderer.DrawRect(mTouchArea, mTouchAreaColor);
    }
}

void Control::paint(Canvas &arCanvas, const Style &arStyle)
{
    if (!mTransparent && (arStyle.mBackgroundColor != Color::None)) {
        arCanvas.Fill(arStyle.mBackgroundColor);
    }
    else {
        arCanvas.Fill(arStyle.mBackgroundColor);
    }

    arStyle.mBackground.Paint(Point(), arCanvas);
    arStyle.mForeground.Paint(Point(), arCanvas);
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

bool Control::ProcessInput(TouchEvent &arInput)
{
    if (!IsVisible()) {
        return false;
    }
    if ((GetState() == Control::States::disabled) && mTouchArea.IsHit(arInput.mCurrent)) {
        return true;
    }

    switch (arInput.mType) {
        case TouchEvent::Types::Press:
            if (mArea.IsHit(arInput.mCurrent)) {
                for(Control *child : mChildren) {
                    if (child->ProcessInput(arInput)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(arInput.mCurrent)) {
                doPress(arInput.mCurrent);
                if (GetState() == Control::States::checked) {
                    SetState(Control::States::checkedPressed);
                }
                else {
                    SetState(Control::States::pressed);
                }
                return true;
            }
            break;

        case TouchEvent::Types::Lift:
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
                            mState = States::normal;
                        }
                        else {
                            mState = States::checked;
                        }
                        Invalidate();
                    }
                    else {
                        SetState(Control::States::normal);
                    }
                    doClick(arInput.mCurrent);
                }
                else {
                    SetState(Control::States::normal);
                }
                return true;
            }
            break;

        case TouchEvent::Types::Drag:
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
                    SetState(Control::States::dragged);
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
