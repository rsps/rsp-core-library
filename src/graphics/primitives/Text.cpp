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

using namespace rsp::logging;

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
//    int width = ((mArea.GetWidth() + (mLineMaxChar/2)) / mLineMaxChar); // Texts seems to be about 1/3 of desired width
    int width = mArea.GetWidth() / mLineMaxChar; // Texts seems to be about 1/3 of desired width
    int height = mArea.GetHeight() / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = mArea.GetWidth() * 90 / 100; // >90%
    int h_limit = mArea.GetHeight() * 90 / 100; // > 90%
    Logger::GetDefault().Debug() << "scaleToFit w_limit: " << w_limit << " h_limit: " << h_limit << " line count: " << mLineCount << std::endl;

    do {
        mFont.SetSize(width, height);

        mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        calcBoundingRect(mpGlyphs);

        Logger::GetDefault().Debug() << "Area: " << mArea << ", Bounding Rect: " << mBoundingRect << std::endl;

        done = 0;
        if ((mBoundingRect.GetWidth() < w_limit) || (mBoundingRect.GetWidth() > mArea.GetWidth())) {
            width += ((mArea.GetWidth() - mBoundingRect.GetWidth()) + (mLineMaxChar/2)) / mLineMaxChar;
        }
        else {
            done++;
        }
        if ((mBoundingRect.GetHeight() < h_limit) || (mBoundingRect.GetHeight() > mArea.GetHeight())) {
            height += (mArea.GetHeight() - mBoundingRect.GetHeight()) / mLineCount;
        }
        else {
            done++;
        }
    }
    while( (done != 2) && --attempts);
    Logger::GetDefault().Debug() << "scaleToFit done: " << done << " attempts: " << attempts << std::endl;
}

void Text::calcBoundingRect(const std::unique_ptr<Glyphs>& apGlyphs)
{
    mBoundingRect = apGlyphs->mBoundingRect;
    alignGlyphs();
    return;

    int w = 0;
    int h = apGlyphs->mLineHeight;
    int line_count = 1;
    int line_width = 0;
    for (unsigned i=0 ; i < apGlyphs->GetCount(); ++i) {
        const Glyph &glyph = apGlyphs->GetGlyph(i);
        if (glyph.mSymbolUnicode == static_cast<uint32_t>('\n')) {
            line_count++;
            if (w > line_width) {
                line_width = w;
            }
            w = 0;
        }
        else {
            w += glyph.mAdvanceX; //.mWidth;
        }
    }
    if (w > line_width) {
        line_width = w;
    }

    mBoundingRect = Rect(0, 0, line_width, (h * line_count) + ((line_count - 1) * mLineSpacing));
    alignGlyphs();

    if (apGlyphs->GetCount()) {
        const Glyph &glyph = apGlyphs->GetGlyph(0);
        mBoundingRect.MoveTo(Point(glyph.mLeft, glyph.mTop));
    }
}

void Text::loadGlyphs()
{
    if (mScaleToFit) {
        scaleToFit();
    }
    else {
        mpGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        calcBoundingRect(mpGlyphs);
    }
    mDirty = false;
}

void Text::alignGlyphs()
{
    if (mVAlign == VAlign::Top && mHAlign == HAlign::Left) {
        mBoundingRect.MoveTo(mArea.GetTopLeft());
        return;
    }

    int voffset = 0;
    int hoffset = 0;
    switch(mVAlign) {
        default:
        case VAlign::Top:
            voffset = 0;
            break;
        case VAlign::Center:
            voffset = (mArea.GetHeight() - mBoundingRect.GetHeight()) / 2;
            break;
        case VAlign::Bottom:
            voffset = (mArea.GetHeight() - mBoundingRect.GetHeight());
            break;
    }
    switch(mHAlign) {
        default:
        case HAlign::Left:
            hoffset = 0;
            break;
        case HAlign::Center:
            hoffset = (mArea.GetWidth() - mBoundingRect.GetWidth()) / 2;
            break;
        case HAlign::Right:
            hoffset = (mArea.GetWidth()- mBoundingRect.GetWidth());
            break;
    }
    for (unsigned i=0 ; i < mpGlyphs->GetCount() ; ++i) {
        Glyph &glyph = mpGlyphs->GetGlyph(i);
        glyph.mTop += voffset;
        glyph.mLeft += hoffset;
    }
    mBoundingRect.MoveTo(mArea.GetTopLeft() + Point(hoffset, voffset));
}

}
