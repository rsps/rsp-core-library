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
    if (aState > States::Dragged) {
        aState = States(uint32_t(aState) % uint32_t(States::CheckedNormal));
    }
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
    if (!mDirty) {
        mDirty = true;
        for (Control* child : mChildren) {
            child->Invalidate();
        }
    }
}

Control& Control::SetArea(Rect aRect)
{
    if (mpParent) {
        aRect.MoveTo(aRect.GetTopLeft() + mpParent->GetOrigin());
    }
    if (mArea != aRect) {
        Point difference = aRect.GetTopLeft() - mArea.GetTopLeft();
        if (difference != Point()) {
            for (auto &style : mStyles) {
                for (TexturePtr_t &texture : style.mTextures) {
                    texture->SetDestination(texture->GetDestination() + difference);
                }
            }
        }
        for (Control* child : mChildren) {
            child->SetOrigin((child->GetOrigin() - mArea.GetTopLeft()) + aRect.GetTopLeft());
        }
        mArea = aRect;
        if (mpParent) {
            // Force repaint of parent, this is resizing
            mpParent->Invalidate();
        }
        else {
            Invalidate();
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
    if (difference == Point()) {
        return *this;
    }

    mArea.MoveTo(arPoint);
    mTouchArea.MoveTo(mTouchArea.GetTopLeft() + difference);
    for (auto &style : mStyles) {
        for (TexturePtr_t &texture : style.mTextures) {
            texture->SetDestination(texture->GetDestination() + difference);
        }
    }
    for (Control* child : mChildren) {
        child->SetOrigin(child->GetOrigin() + difference);
    }
    doSetArea(mArea);
    return *this;
}

Point Control::GetOrigin() const
{
    return mArea.GetTopLeft();
}

Rect Control::GetTouchArea() const
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
    if (apChild && (apChild->mpParent != this)) {
        mChildren.push_back(apChild);
        apChild->mpParent = this;
        apChild->SetOrigin(apChild->GetOrigin() + GetOrigin());
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
//    GFXLOG("Updating Data: " << GetName() << " " << mArea);
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

//    GFXLOG("Rendering: " << GetName() << " " << mArea);

    auto &style = mStyles[mState];

    if (!mTransparent) {
        arRenderer.Fill(style.mBackgroundColor, mArea);
    }

    arRenderer.PushClipRect(mArea);

    for (const TexturePtr_t& texture : style.mTextures) {
        arRenderer.Blit(*texture);
    }

    if (!render(arRenderer)) {
        if (mChildren.size() > 0) {
            for (Control* child : mChildren) {
                child->Render(arRenderer);
            }
        }
    }

    arRenderer.PopClipRect();

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

Control& Control::SetChecked(bool aValue)
{
    if (mChecked != aValue) {
        mChecked = aValue;
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

bool Control::handleTouchEvent(rsp::messaging::Event &arEvent)
{
    const TouchEvent &touch = arEvent.CastTo<const TouchEvent>();

    switch (touch.mType) {
        case TouchTypes::Press:
            if (mArea.IsHit(touch.mCurrent)) {
                for(Control *child : mChildren) {
                    if (child->ProcessEvent(arEvent)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(touch.mCurrent)) {
                return doPress(touch);
            }
            break;

        case TouchTypes::Lift:
            if (mArea.IsHit(touch.mPress)) {
                for(Control *child : mChildren) {
                    if (child->ProcessEvent(arEvent)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(touch.mPress)) {
                bool result = doLift(touch);
                if (mTouchArea.IsHit(touch.mCurrent)) {
                    if (IsCheckable()) {
                        SetChecked(!IsChecked());
                    }
                    if ((touch.mTime - touch.mPressTime) < std::chrono::milliseconds(800)) {
                        Logger::GetDefault().Debug() << GetName() << " was clicked by " << touch;
                        doClick(touch);
                    }
                }
                return result;
            }
            break;

        case TouchTypes::Drag:
            if (mArea.IsHit(touch.mPress)) {
                for(Control *child : mChildren) {
                    if (child->ProcessEvent(arEvent)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(touch.mPress)) {
//                if ((touch.mPressTime != std::chrono::steady_clock::time_point()) && (touch.mPress.Distance(touch.mCurrent) > 30)) {
//                    touch.mPressTime = std::chrono::steady_clock::time_point();
//                }
                if (IsDraggable()) {
                    return doMove(touch);
                }
            }
            break;

        default:
            break;
    }
    return false;
}

bool Control::ProcessEvent(rsp::messaging::Event &arEvent)
{
    if (!IsVisible()) {
        return false;
    }
    if (!IsEnabled()) {
        return (arEvent.Type == TouchEvent::ClassType && mTouchArea.IsHit(arEvent.CastTo<TouchEvent>().mCurrent));
    }

    switch (arEvent.Type) {
        case TouchEvent::ClassType:
            return handleTouchEvent(arEvent);

        case RefreshEvent::ClassType:
            Invalidate();
            break;

        default:
            break;
    }

    return false;
}

bool Control::doPress(const TouchEvent &arEvent)
{
    SetState(Control::States::Pressed);
    mOnPress(arEvent, GetId());
    return true;
}

bool Control::doMove(const TouchEvent &arEvent)
{
    SetState(Control::States::Dragged);
    mOnMove(arEvent, GetId());
    return true;
}

bool Control::doLift(const TouchEvent &arEvent)
{
    SetState(Control::States::Normal);
    mOnLift(arEvent, GetId());
    return true;
}

bool Control::doClick(const TouchEvent &arEvent)
{
    mOnClick(arEvent, GetId());
    return true;
}

} // namespace rsp::graphics
