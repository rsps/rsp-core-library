/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/Text.h>
#include <logging/Logger.h>

namespace rsp::graphics {

Text::Text(const std::string &arFontName)
    : mFont(arFontName), mArea()
{
}

Text::Text(const std::string &arFontName, const std::string &arText)
    : mFont(arFontName), mArea()
{
    SetValue(arText);
}

Text& Text::Reload()
{
    mLineCount = 1;
    mLineMaxChar = 1; // Avoid division by zero
    int count = 0;

    for (long unsigned int i = 0; i < mValue.size(); i++) {
        if (mValue[i] == '\n') {
            mLineCount++;
            count = 0;
        }
        else {
            if (++count > mLineMaxChar) {
                mLineMaxChar = count;
            }
        }
    }

    loadGlyphs();
    return *this;
}

void Text::scaleToFit()
{
    int width = ((mArea.GetWidth() + (mLineMaxChar/2)) / mLineMaxChar); // Texts seems to be about 1/3 of desired width
    int height = mArea.GetHeight() / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = mArea.GetWidth() * 90 / 100; // >90%
    int h_limit = mArea.GetHeight() * 90 / 100; // > 90%
    DLOG("scaleToFit w_limit: " << w_limit << " h_limit: " << h_limit);

    do {
        mFont.SetSize(width, height);

        mGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        auto r = CalcBoundingRect(mGlyphs);

        done = 0;
        if ((r.GetWidth() < w_limit) || (r.GetWidth() > mArea.GetWidth())) {
            width += ((mArea.GetWidth() - r.GetWidth()) + (mLineMaxChar/2)) / mLineMaxChar;
        }
        else {
            done++;
        }
        if ((r.GetHeight() < h_limit) || (r.GetHeight() > mArea.GetHeight())) {
            height += (mArea.GetHeight() - r.GetHeight()) / mLineCount;
        }
        else {
            done++;
        }
    }
    while( (done != 2) && --attempts);
    DLOG("scaleToFit done: " << done << " attempts: " << attempts);

    alignGlyphs();
}

Rect Text::CalcBoundingRect(const std::vector<Glyph> &arGlyphs) const
{
    int w = 0;
    int h = 0;
    int line_count = 1;
    int line_width = 0;
    for (const Glyph &glyph : arGlyphs) {
        if (glyph.mSymbolUnicode == static_cast<uint32_t>('\n')) {
            line_count++;
            if (w > line_width) {
                line_width = w;
            }
            w = 0;
        }
        else {
            w += glyph.mWidth;
            if (h < glyph.mHeight) {
                h = glyph.mHeight;
            }
        }
    }
    if (w > line_width) {
        line_width = w;
    }

    return Rect(0, 0, line_width, (h * line_count) + ((line_count - 1) * mLineSpacing));

}

void Text::loadGlyphs()
{
    if (mScaleToFit) {
        scaleToFit();
    }
    else {
        mGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        alignGlyphs();
    }
}

void Text::alignGlyphs()
{
    if (mVAlign == VAlign::Top && mHAlign == HAlign::Left) {
        return;
    }

    auto r = CalcBoundingRect(mGlyphs);

    int voffset = 0;
    int hoffset = 0;
    switch(mVAlign) {
        default:
        case VAlign::Top:
            voffset = 0;
            break;
        case VAlign::Center:
            voffset = (mArea.GetHeight() - r.GetHeight()) / 2;
            break;
        case VAlign::Bottom:
            voffset = (mArea.GetHeight() - r.GetHeight());
            break;
    }
    switch(mHAlign) {
        default:
        case HAlign::Left:
            hoffset = 0;
            break;
        case HAlign::Center:
            hoffset = (mArea.GetWidth() - r.GetWidth()) / 2;
            break;
        case HAlign::Right:
            hoffset = (mArea.GetWidth()- r.GetWidth());
            break;
    }
    for (Glyph &glyph : mGlyphs) {
        glyph.mTop += voffset;
        glyph.mLeft += hoffset;
    }
}

}
