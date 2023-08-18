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
#include <graphics/GfxEvents.h>

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

Control::States Control::GetState() const
{
    States result = States::Normal;

    if (!mEnabled) {
        result = States::Disabled;
    }
    else if (mPressed) {
        result = States::Pressed;
        if (mDragged) {
            result = States::Dragged;
        }
    }
    if (mChecked) {
        result = States(uint32_t(result) + uint32_t(States::CheckedDisabled));
    }

    return result;
}

void Control::Invalidate()
{
    if (!mDirty) {
        mDirty = true;
        for (Control *child : mChildren) {
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
        for (Control *child : mChildren) {
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
    for (Control *child : mChildren) {
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
        Invalidate();
    }
    return *this;
}

Control& Control::RemoveChild(Control *apChild)
{
    if (apChild) {
        auto it = std::find(mChildren.begin(), mChildren.end(), apChild);
        if (it != mChildren.end()) {
            apChild->mpParent = nullptr;
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
    for (Control *child : mChildren) {
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

    auto &style = mStyles[GetState()];

    if (!mTransparent) {
        arRenderer.Fill(style.mBackgroundColor, mArea);
    }

    arRenderer.PushClipRect(mArea);

    for (const TexturePtr_t &texture : style.mTextures) {
        arRenderer.Blit(*texture);
    }

    if (!render(arRenderer)) {
        if (mChildren.size() > 0) {
            for (Control *child : mChildren) {
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

Control& Control::Enable(bool aEnable)
{
    if (mEnabled != aEnable) {
        mEnabled = aEnable;
        Invalidate();
        for (Control *child : mChildren) {
            child->Enable(aEnable);
        }
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
    TouchEvent &touch = arEvent.CastTo<TouchEvent>();

    switch (touch.mType) {
        case TouchTypes::Press:
            if (mArea.IsHit(touch.mCurrent)) {
                for (Control *child : mChildren) {
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
                for (Control *child : mChildren) {
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
                    Logger::GetDefault().Debug() << GetName() << " was clicked by " << touch;
                    doClick(touch);
                }
                return result;
            }
            break;

        case TouchTypes::Drag:
            if (mArea.IsHit(touch.mPress)) {
                for (Control *child : mChildren) {
                    if (child->ProcessEvent(arEvent)) {
                        return true;
                    }
                }
            }
            if (mTouchArea.IsHit(touch.mPress)) {
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
    switch (arEvent.Type) {
        case TouchEvent::ClassType:
            if (!IsVisible()) {
                break;
            }
            if (!IsEnabled()) {
                return (mTouchArea.IsHit(arEvent.CastTo<TouchEvent>().mCurrent));
            }
            return handleTouchEvent(arEvent);

        case RefreshEvent::ClassType:
            Invalidate();
            break;

        default:
            break;
    }

    return false;
}

Control& Control::SetPressed(bool aValue)
{
    if (mPressed != aValue) {
        mPressed = aValue;
        Invalidate();
        for (Control *child : mChildren) {
            child->SetPressed(aValue);
        }
    }
    return *this;
}

Control& Control::SetDragged(bool aValue)
{
    if (mDragged != aValue) {
        mDragged = aValue;
        Invalidate();
        for (Control *child : mChildren) {
            child->SetDragged(aValue);
        }
    }
    return *this;
}

bool Control::doPress(const TouchEvent &arEvent)
{
    SetPressed(true);
    mOnPress(arEvent, GetId());
    return true;
}

bool Control::doMove(const TouchEvent &arEvent)
{
    SetDragged(true);
    mOnMove(arEvent, GetId());
    return true;
}

bool Control::doLift(const TouchEvent &arEvent)
{
    SetPressed(false);
    SetDragged(false);
    mOnLift(arEvent, GetId());
    return true;
}

bool Control::doClick(const TouchEvent &arEvent)
{
    mOnClick(arEvent, GetId());
    return true;
}

} // namespace rsp::graphics
