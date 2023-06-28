/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/Text.h>
#include <logging/Logger.h>
#include <utils/StrUtils.h>

//#define DEBUG(a) { Logger::GetDefault().Debug() << a; }
#define DEBUG(a)

using namespace rsp::logging;

namespace rsp::graphics {

Text::Text(const std::string &arFontName)
    : mFont(arFontName)
{
}

Text::Text(const std::string &arFontName, const std::string &arText)
    : mFont(arFontName)
{
    SetValue(arText);
}

Text& Text::operator <<(const Typography &arTypography)
{
    ForceUpperCase(arTypography.UpperCase);
    mFont.SetStyle(arTypography.Style);
    mFont.SetColor(arTypography.Color);
    mFont.SetSize(arTypography.Size);
    SetLineSpacing(arTypography.Spacing);
    SetHAlignment(arTypography.HAlignment);
    SetVAlignment(arTypography.VAlignment);
    return *this;
}

Text& Text::SetValue(std::string aValue)
{
    if (mUpperCase) {
        utils::StrUtils::ToUpper(aValue);
    }
    if (mValue != aValue) {
        mValue = aValue;
        mDirty = true;
    }
    return *this;
}

Text& Text::ForceUpperCase(bool aUpperCase)
{
    if (mUpperCase != aUpperCase) {
        mUpperCase = aUpperCase;
        mDirty = true;
    }
    return *this;
}

Text& Text::Reload(utils::OptionalPtr<const Rect> aRect)
{
    if (!mDirty && !mFont.IsDirty() && !aRect) {
        return *this;
    }

    DEBUG("Reloading glyphs for '" << mValue << "'")

    mLineCount = 1;
    mLineMaxChar = 1; // Avoid division by zero
    int count = 0;

    for (long unsigned int i = 0; i < mValue.size(); i++) {
        if (mValue[i] == '\n') {
            mLineCount++;
            count = 0;
        }
        else if (++count > mLineMaxChar) {
            mLineMaxChar = count;
        }
    }

    if (aRect) {
        scaleToFit(int(aRect.Get().GetWidth()), int(aRect.Get().GetHeight()));
    }
    else {
        loadGlyphs();
    }
    Init(GetId(), mBoundingRect.GetWidth(), mBoundingRect.GetHeight(), ColorDepth::Alpha, nullptr);
    draw();
    mDirty = false;
    return *this;
}

void Text::scaleToFit(int aWidth, int aHeight)
{
    int width = aWidth / mLineMaxChar; // Texts seems to be about 1/3 of desired width
    int height = aHeight / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = aWidth * 90 / 100; // >90%
    int h_limit = aHeight * 90 / 100; // > 90%
    DEBUG("scaleToFit(" << aWidth << ", " << aHeight << "), line count: " << mLineCount)

    do {
        mFont.SetSize(width, height);

        mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        mBoundingRect = mpGlyphs->mBoundingRect;

//        DEBUG("ScaleToFit(" << aWidth << ", " << aHeight << ") -> Bounding Rect: " << mBoundingRect)

        done = 0;
        if ((mBoundingRect.GetWidth() < w_limit) || (mBoundingRect.GetWidth() > aWidth)) {
            width += ((aWidth - mBoundingRect.GetWidth()) + (mLineMaxChar/2)) / mLineMaxChar;
        }
        else {
            done++;
        }
        if ((mBoundingRect.GetHeight() < h_limit) || (mBoundingRect.GetHeight() > aHeight)) {
            height += (aHeight - mBoundingRect.GetHeight()) / mLineCount;
        }
        else {
            done++;
        }
    }
    while( (done != 2) && --attempts);
    DEBUG("scaleToFit done: " << done << " attempts: " << attempts << " Bounding Rect: " << mBoundingRect)
}

void Text::loadGlyphs()
{
    DEBUG("Loading glyphs")
    mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
    mBoundingRect = mpGlyphs->mBoundingRect;
}

Point Text::GetPosition(const Rect &arArea) const
{
    int voffset = 0;
    int hoffset = 0;
    switch(mVAlign) {
        default:
        case VAlign::Top:
            voffset = 0;
            break;
        case VAlign::Center:
            voffset = (arArea.GetHeight() - GetHeight()) / 2;
            break;
        case VAlign::Bottom:
            voffset = (arArea.GetHeight() - GetHeight());
            break;
    }
    switch(mHAlign) {
        default:
        case HAlign::Left:
            hoffset = 0;
            break;
        case HAlign::Center:
            if (arArea.GetWidth() < GetWidth()) {
                hoffset = 0;
            }
            else {
                hoffset = (arArea.GetWidth() - GetWidth()) / 2;
            }
            break;
        case HAlign::Right:
            hoffset = (arArea.GetWidth()- GetWidth());
            break;
    }
    Point p(hoffset, voffset);
    p += arArea.GetTopLeft();
    DEBUG("GetPosition(" << arArea << ") -> " << p)
    return p;
}

void Text::draw()
{
    auto &glyphs = mpGlyphs;
    if (!glyphs) {
        std::cout << "No glyphs to paint!!" << std::endl;
        return;
    }
    Color aColor = Color::None;
    Fill(aColor); // TODO: Test if this is faster than writing alpha=0 via SetPixel below.
    for (unsigned i=0; i < glyphs->GetCount() ; ++i) {
        Glyph &glyph = glyphs->GetGlyph(i);
        GuiUnit_t py = GuiUnit_t(glyph.mTop);
        for (int y = 0; y < glyph.mHeight; y++) {
            const uint8_t* p_row = glyph.GetPixelRow(y);
            GuiUnit_t px = GuiUnit_t(glyph.mLeft);
            for (int x = 0; x < glyph.mWidth; x++) {
                auto c = *p_row++;
                if (!c) {
                    px++;
                    continue;
                }
                aColor.SetAlpha(c);
                SetPixelAt(px, py, aColor);
                px++;
            }
            py++;
        }
    }
}

}
