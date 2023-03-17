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

//Label& Label::SetTextArea(const Rect &arArea)
//{
//    mText.SetArea(arArea);
//    return *this;
//}

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

void Label::update()
{
    auto tr = mText.GetRect();
    if (!mpTexture || (tr != Rect(0, 0, mpTexture->GetWidth(), mpTexture->GetHeight()))) {
        mpTexture = Texture::Create(tr.GetWidth(), tr.GetHeight());
        mpTexture->SetBlendOperation(GfxBlendOperation::ColorKey);
    }

    mpTexture->Fill(Color::None).Update(mText.GetPixelData(), mStyles[mState].mForegroundColor);

    tr &= GetArea().MoveTo({0,0});
    mpTexture->SetDestination(mText.GetPosition(mArea))
        .SetSourceRect(tr);
}

void Label::Render(Renderer &arRenderer) const
{
    Control::Render(arRenderer);
    if (mpTexture) {
        arRenderer.Blit(*mpTexture);
    }
}

} /* namespace rsp::graphics */
