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
    : Control(rsp::utils::MakeTypeInfo<Button>())
{
}

Button::Button(const rsp::utils::TypeInfo &arInfo)
    : Control(arInfo)
{
}

Button& Button::SetBitmapPosition(const Point &arPoint)
{
    for (auto &tuple : mStyles) {
        tuple.second.mBitmapView.SetDestination(arPoint);
    }
    Invalidate();
    return *this;
}

Button& Button::SetTextPosition(const Point &arPoint)
{
    Rect r = mText.GetArea();
    r.MoveTo(arPoint);
    mText.SetArea(r);
    Invalidate();
    return *this;
}

void Button::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);
    arStyle.mBitmapView.Paint(GetOrigin(), arCanvas);
    arCanvas.DrawText(mText, arStyle.mForegroundColor);
}

Button& Button::SetCaption(const std::string &arText)
{
    GetText().SetValue(arText);
    Invalidate();
    return *this;
}

} /* namespace rsp::graphics */
