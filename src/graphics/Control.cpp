/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <rsp/graphics/Control.h>
#include <rsp/graphics/GfxEvents.h>

using namespace rsp::logging;

namespace rsp::graphics
{

Color Control::mTouchAreaColor = Color::None;

std::string to_string(const Control::States aState)
{
    return std::string(utils::EnumName<Control::States>(aState));
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

void Control::setId(const uint32_t aId)
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
        result = static_cast<States>(static_cast<uint32_t>(result) + static_cast<uint32_t>(States::CheckedDisabled));
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
    if (mArea == aRect) {
        return *this;
    }
    const Point difference = aRect.GetTopLeft() - mArea.GetTopLeft();
    if (difference != Point()) {
        for (const auto &style : mStyles) {
            for (const TexturePtr_t &texture : style.mTextures) {
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
    doSetArea(aRect, difference);
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

Control& Control::ExpandToParent()
{
    if (!mpParent) {
        return *this;
    }
    bool changed = false;
    auto r = mpParent->GetArea();
    if (mArea.GetWidth() < r.GetWidth()) {
        mArea.SetWidth(r.GetWidth());
        changed = true;
    }
    if (mArea.GetHeight() < r.GetHeight()) {
        mArea.SetHeight(r.GetHeight());
        changed = true;
    }

    if (changed) {
        mpParent->Invalidate();
        doSetArea(mArea, Point());
    }

    return *this;
}

Control& Control::SetOrigin(const Point &arPoint)
{
    Point difference = arPoint - mArea.GetTopLeft();
    if (difference == Point()) {
        return *this;
    }

    mArea.MoveTo(arPoint);
    mTouchArea.MoveTo(mTouchArea.GetTopLeft() + difference);
    for (const auto &style : mStyles) {
        for (const TexturePtr_t &texture : style.mTextures) {
            texture->SetDestination(texture->GetDestination() + difference);
        }
    }
    for (Control *child : mChildren) {
        child->SetOrigin(child->GetOrigin() + difference);
    }
    doSetArea(mArea, difference);
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
    if (!apChild) {
        return *this;
    }
    if (apChild->mpParent) {
        apChild->mpParent->RemoveChild(apChild); // Remove the child from its old parent
    }

    mChildren.push_back(apChild);
    apChild->mpParent = this;
    apChild->SetOrigin(apChild->GetOrigin() + GetOrigin());
    Invalidate();

    return *this;
}

Control& Control::RemoveChild(Control *apChild)
{
    if (apChild) {
        if (const auto it = std::ranges::find(mChildren, apChild) ; it != mChildren.end()) {
            apChild->mpParent = nullptr;
            mChildren.erase(it);
            Invalidate();
        }
    }
    return *this;
}

Control& Control::SetTexturePosition(const Point &arPoint)
{
    for (const auto &style : mStyles) {
        for (const TexturePtr_t &texture : style.mTextures) {
            texture->SetDestination(arPoint);
        }
    }
    Invalidate();
    return *this;
}

bool Control::UpdateData()
{
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

    auto &style = mStyles[GetState()];

    if (!mTransparent) {
        arRenderer.Fill(style.mBackgroundColor, mArea);
    }

    arRenderer.PushClipRect(mArea);

    for (const TexturePtr_t &texture : style.mTextures) {
        arRenderer.Blit(*texture);
    }

    if (!render(arRenderer) && !mChildren.empty()) {
        for (const Control *child : mChildren) {
            child->Render(arRenderer);
        }
    }

    arRenderer.PopClipRect();

    if ((mTouchAreaColor != Color::None) && !mTouchArea.empty()) {
        Rect r = mTouchArea;
        r.AddSize(1, 1);
        arRenderer.DrawRect(mTouchAreaColor, mTouchArea);
    }
}

Control& Control::SetDraggable(const bool aValue)
{
    if (mDraggable != aValue) {
        mDraggable = aValue;
        Invalidate();
    }
    return *this;
}

Control& Control::SetCheckable(const bool aValue)
{
    if (mCheckable != aValue) {
        mCheckable = aValue;
        Invalidate();
    }
    return *this;
}

Control& Control::SetChecked(const bool aValue)
{
    if (mChecked != aValue) {
        mChecked = aValue;
        Invalidate();
    }
    return *this;
}

Control& Control::SetVisible(const bool aVisible)
{
    if (mVisible != aVisible) {
        mVisible = aVisible;
        Invalidate();
    }
    return *this;
}

Control& Control::SetEnable(const bool aEnable)
{
    if (mEnabled != aEnable) {
        mEnabled = aEnable;
        Invalidate();
        for (Control *child : mChildren) {
            child->SetEnable(aEnable);
        }
    }
    return *this;
}

Control& Control::SetTransparent(const bool aValue)
{
    if (mTransparent != aValue) {
        mTransparent = aValue;
        Invalidate();
    }
    return *this;
}

bool Control::processChildren(messaging::Event &arEvent) const
{
    for (Control *child : mChildren) {
        if (child->ProcessEvent(arEvent)) {
            return true;
        }
    }
    return false;
}

bool Control::handleTouchEvent(messaging::Event &arEvent)
{
    switch (auto &touch = arEvent.CastTo<TouchEvent>() ; touch.mType) {
        case TouchTypes::Press:
            if (mArea.IsHit(touch.mCurrent) && processChildren(arEvent)) {
                return true;
            }
            if (mTouchArea.IsHit(touch.mCurrent)) {
                return doPress(touch);
            }
            break;

        case TouchTypes::Lift:
            if (mArea.IsHit(touch.mPress) && processChildren(arEvent)) {
                return true;
            }
            if (mTouchArea.IsHit(touch.mPress)) {
                const bool result = doLift(touch);
                if (mTouchArea.IsHit(touch.mCurrent)) {
                    mLogger.Debug() << GetName() << " was clicked by " << touch;
                    doClick(touch);
                }
                return result;
            }
            break;

        case TouchTypes::Drag:
            if (mArea.IsHit(touch.mPress) && processChildren(arEvent)) {
                return true;
            }
            if (mTouchArea.IsHit(touch.mPress)) {
                if (IsDraggable()) {
                    return doMove(touch);
                }
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

bool Control::ProcessEvent(rsp::messaging::Event &arEvent)
{
    switch (static_cast<GfxEvents>(arEvent.Type)) {
        case GfxEvents::Touch:
            if (!IsVisible()) {
                break;
            }
            if (!IsEnabled()) {
                return (mTouchArea.IsHit(arEvent.CastTo<TouchEvent>().mCurrent));
            }
            return handleTouchEvent(arEvent);

        case GfxEvents::Refresh:
            Invalidate();
            break;

        default:
            break;
    }

    return false;
}

Control& Control::SetPressed(const bool aValue)
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

Control& Control::SetDragged(const bool aValue)
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
    if (IsCheckable()) {
        SetChecked(!IsChecked());
    }
    mOnClick(arEvent, GetId());
    return true;
}

} // namespace rsp::graphics
