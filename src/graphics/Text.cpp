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

Text& Text::SetValue(const std::string &arValue)
{
    if (mValue != arValue) {
        mValue = arValue;
        mDirty = true;
    }
    return *this;
}

const Rect& Text::GetArea() const
{
    return mRect;
}

Text& Text::SetArea(const Rect &arRect)
{
    if (mRect != arRect) {
        mDirty = true;
        Init(arRect.GetWidth(), arRect.GetHeight(), ColorDepth::Alpha, nullptr);
    }
    return *this;
}

Text& Text::Reload()
{
    if (!mDirty) {
        return *this;
    }
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

    loadGlyphs();
    draw();
    mDirty = false;
    return *this;
}

Text& Text::SetScaleToFit(bool aValue)
{
    if (mScaleToFit != aValue) {
        mScaleToFit = aValue;
        mDirty = true;
    }
    return *this;
}

void Text::scaleToFit()
{
//    int width = ((mArea.GetWidth() + (mLineMaxChar/2)) / mLineMaxChar); // Texts seems to be about 1/3 of desired width
    int width = GetWidth() / mLineMaxChar; // Texts seems to be about 1/3 of desired width
    int height = GetHeight() / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = GetWidth() * 90 / 100; // >90%
    int h_limit = GetHeight() * 90 / 100; // > 90%
    Logger::GetDefault().Debug() << "scaleToFit w_limit: " << w_limit << " h_limit: " << h_limit << " line count: " << mLineCount;

    do {
        mFont.SetSize(width, height);

        mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        mBoundingRect = mpGlyphs->mBoundingRect;

        Logger::GetDefault().Debug() << "Area: " << mRect << ", Bounding Rect: " << mBoundingRect;

        done = 0;
        if ((mBoundingRect.GetWidth() < w_limit) || (mBoundingRect.GetWidth() > GetWidth())) {
            width += ((GetWidth() - mBoundingRect.GetWidth()) + (mLineMaxChar/2)) / mLineMaxChar;
        }
        else {
            done++;
        }
        if ((mBoundingRect.GetHeight() < h_limit) || (mBoundingRect.GetHeight() > GetHeight())) {
            height += (GetHeight() - mBoundingRect.GetHeight()) / mLineCount;
        }
        else {
            done++;
        }
    }
    while( (done != 2) && --attempts);
    Logger::GetDefault().Debug() << "scaleToFit done: " << done << " attempts: " << attempts;
}

void Text::loadGlyphs()
{
    Logger::GetDefault().Debug() << "Loading glyphs. ScaleToFit=" << mScaleToFit;
    if (mScaleToFit) {
        scaleToFit();
    }
    else {
        mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        mBoundingRect = mpGlyphs->mBoundingRect;
        SetArea(mBoundingRect);
    }
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
            hoffset = (arArea.GetWidth() - GetWidth()) / 2;
            break;
        case HAlign::Right:
            hoffset = (arArea.GetWidth()- GetWidth());
            break;
    }
    return Point(hoffset, voffset) + arArea.GetTopLeft();
}

void Text::draw()
{
    auto &glyphs = GetGlyphs();
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
