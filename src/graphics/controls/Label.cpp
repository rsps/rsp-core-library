/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/controls/Label.h>

namespace rsp::graphics
{

Label& Label::SetCaption(const std::string &arCaption)
{
    mText.SetValue(arCaption).Reload();
    Invalidate();
    return *this;
}

Label& Label::SetTextPosition(const Point &arPoint)
{
    Rect r = mText.GetArea();
    r.MoveTo(arPoint);
    mText.SetArea(r);
    Invalidate();
    return *this;
}

Control& Label::SetOrigin(const Point &arPoint)
{
    Point difference = arPoint - mArea.GetTopLeft();
    Control::SetOrigin(arPoint);
    Rect r = mText.GetArea();
    r.MoveTo(r.GetTopLeft() + difference);
    mText.SetArea(r);
    return *this;
}

Control& Label::SetArea(Rect aRect)
{
    mText.SetArea(aRect);
    return Control::SetArea(aRect);
}


void Label::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);
    arCanvas.DrawText(mText, arStyle.mForegroundColor);
}

} //namespace rsp::graphics
