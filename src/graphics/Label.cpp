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

Label& Label::SetFontSize(int aSizePx)
{
    mText.SetFontSize(aSizePx);
    return *this;
}

Label& Label::ScaleToFit(bool aValue)
{
    if (mScaleToFit != aValue) {
        mScaleToFit = aValue;
        Invalidate();
    }
    return *this;
}

void Label::refresh()
{
    if (mDirty || mText.IsDirty()) {
        if (mScaleToFit) {
            mText.Reload(mArea);
        }
        else {
            mText.Reload();
        }
        Invalidate();
    }
}

void Label::update()
{
    auto tr = mText.GetRect();
    if (tr.empty()) {
        mpTexture = nullptr;
        return;
    }

    if (!mpTexture || (tr != Rect(0, 0, mpTexture->GetWidth(), mpTexture->GetHeight()))) {
        mpTexture = Texture::Create(tr.GetWidth(), tr.GetHeight());
        mpTexture->SetBlendOperation(Texture::BlendOperation::SourceAlpha);
    }

    Color &color = GetStyle(GetState()).mForegroundColor;

    mpTexture->Fill(Color::None).Update(mText.GetPixelData(), (color == Color::None) ? mText.GetFont().GetColor() : color);

    Point pos = mText.GetPosition(mArea);
    mpTexture->SetDestination(pos);

    if (tr.GetWidth() > mArea.GetWidth()) {
        Rect sr(tr.GetWidth() - mArea.GetWidth(), 0, mArea.GetWidth(), mArea.GetHeight());
        mpTexture->SetSourceRect(sr);
    }
}

bool Label::render(Renderer &arRenderer) const
{
    if (mpTexture) {
        arRenderer.Blit(*mpTexture);
    }
    return false;
}

Label::Label(const Label &arOther)
    : Control(arOther)
{
    if (this != &arOther) {
        mText = arOther.mText;
        mpTexture = arOther.mpTexture->Clone();
        mScaleToFit = arOther.mScaleToFit;
    }
}

Label& Label::operator =(const Label &arOther)
{
    if (this != &arOther) {
        Control::operator=(arOther);
        mText = arOther.mText;
        mpTexture = arOther.mpTexture->Clone();
        mScaleToFit = arOther.mScaleToFit;
    }
    return *this;
}

} /* namespace rsp::graphics */
