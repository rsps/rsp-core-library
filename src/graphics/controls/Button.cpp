/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/controls/Button.h>

namespace rsp::graphics {

Button::Button()
    : TouchControl(rsp::utils::MakeTypeInfo<Button>())
{
    AddChild(&mImage);
}

Button::Button(const rsp::utils::TypeInfo &arInfo)
    : TouchControl(arInfo)
{
    AddChild(&mImage);
}

Button& Button::SetArea(const Rect &arRect)
{
    TouchControl::SetArea(arRect);
    mImage.SetArea(arRect);
    return *this;
}

Button& Button::SetBitmapPosition(const Point &arPoint)
{
    mBitmapPosition = arPoint + mArea.GetTopLeft();
    Invalidate();
    return *this;
}

Button& Button::SetTextPosition(const Point &arPoint)
{
    Rect r = mText.GetArea();
    r.SetOrigin(arPoint);
    mText.SetArea(r);
    Invalidate();
    return *this;
}

void Button::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);

    arCanvas.DrawText(mText, arStyle.mForegroundColor);
}

} /* namespace rsp::graphics */
