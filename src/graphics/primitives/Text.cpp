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

Text::Text()
{
}

Text::Text(const Font &arFont)
    : mFont(arFont)
{
}

Text::Text(const std::string &arText, const Font &arFont)
    : mValue(arText), mFont(arFont)
{
}

Text& Text::SetValue(const std::string &arValue)
{
    mValue = arValue;
    return *this;
}

Text& Text::SetArea(const Rect &arRect)
{
    mArea = arRect;
    return *this;
}

Text& Text::SetFont(const Font &arFont)
{
    mFont = arFont;
    return *this;
}

Text& Text::Invalidate()
{
    mLineCount = 1;
    mLineMaxChar = 1; // Avoid division by zero
    int count = 0;

    for (int i = 0; i < mValue.size(); i++) {
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

    scaleToFit();
    loadGlyphs();
    return *this;
}

Text& Text::SetScaleToFit(bool aValue)
{
    mScaleToFit = aValue;
    return *this;
}

void Text::scaleToFit()
{
    if (!mScaleToFit) {
        return;
    }

    int width = ((mArea.GetWidth() + (mLineMaxChar/2)) / mLineMaxChar) * 2; // Texts seems to be about 1/3 of desired width
    int height = mArea.GetHeight() / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = mArea.GetWidth() * 90 / 100; // >90%
    int h_limit = mArea.GetHeight() * 90 / 100; // > 90%
    DLOG("w_limit: " << w_limit << " h_limit: " << h_limit);

    do {
        mFont.SetSize(width, height);

        mMasks = mFont.MakeTextMasks(mValue);
        auto r = calcBoundingRect();
        DLOG("Bounding Rect: " << r);

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
        DLOG("done: " << done << " attempts: " << attempts);
    }
    while( (done != 2) && --attempts);
}

void Text::loadGlyphs()
{
    mMasks = mFont.MakeTextMasks(mValue);
}

}

