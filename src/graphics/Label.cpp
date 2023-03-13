/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <graphics/Label.h>

namespace rsp::graphics {

Label& Label::SetCaption(const std::string &arCaption)
{
    mText.SetValue(arCaption);
    return *this;
}

Label& Label::SetTextPosition(const Point &arPoint)
{
    Rect r = mText.GetArea();
    r.MoveTo(arPoint);
    mText.SetArea(r);
    return *this;
}

Label& Label::SetVAlignment(Text::VAlign aVAlign)
{
    mText.SetVAlignment(aVAlign);
    return *this;
}

Label& Label::SetHAlignment(Text::HAlign aHAlign)
{
    mText.SetHAlignment(aHAlign);
    return *this;
}

Control& Label::SetOrigin(const Point &arPoint)
{
    Point difference = arPoint - mArea.GetTopLeft();
    Control::SetOrigin(arPoint);
//    Rect r = mText.GetArea();
//    r.MoveTo(r.GetTopLeft() + difference);
//    mText.SetArea(r);
    return *this;
}

void Label::doSetArea(const Rect &arRect)
{
    Rect r = arRect;
    r.MoveTo(Point());
    mText.SetArea(r);
}

Label& Label::SetFontSize(int aSizePx)
{
    mText.SetFontSize(aSizePx);
    return *this;
}

void Label::refresh()
{
    if (mText.IsDirty()) {
        mText.Reload();
        Invalidate();
    }
}

void Label::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);
    arCanvas.DrawPixelData(mText.GetArea().GetTopLeft(), mText);
}

} /* namespace rsp::graphics */
