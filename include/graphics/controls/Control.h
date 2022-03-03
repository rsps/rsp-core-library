/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef CONTROL_H
#define CONTROL_H

#include "graphics/primitives/Color.h"
#include "graphics/primitives/Rect.h"
#include <vector>

namespace rsp::graphics
{

class Control
{
  public:
    enum class States : int {
        disabled,
        normal,
        pressed,
        dragged,
        checked
    };

    Control() {}
    Control(const Rect &arRect);
    virtual ~Control() {}

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

    void SetState(States aState);
    States GetState() const { return mState; }

    void Invalidate();
    bool IsInvalid() const { return mIsInvalid; }

    Control &SetTransparent(bool aValue)
    {
        mTransparent = aValue;
        return *this;
    }
    bool IsTransparent() const { return mTransparent; }

    virtual void Render(Canvas &aCanvas) = 0;

    void SetArea(const Rect &aRect);
    const Rect &GetArea() const { return mArea; }

    Control &AddChild(Control *apChild)
    {
        mChildren.push_back(apChild);
        apChild->mpParent = this;
        return *this;
    }

  protected:
    Rect mArea{};
    Color mBackground{0};
    bool mTransparent = false;
    Control *mpParent = nullptr;
    std::vector<Control *> mChildren{};
    bool mIsInvalid = true;
    States mState = States::normal;
};

} // namespace rsp::graphics

#endif // CONTROL_H
