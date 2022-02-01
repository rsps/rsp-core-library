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

Text::Text(const std::string &arText)
{
    SetValue(arText);
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

    loadGlyphs();
    return *this;
}

void Text::scaleToFit()
{
    int width = ((mArea.GetWidth() + (mLineMaxChar/2)) / mLineMaxChar) * 2; // Texts seems to be about 1/3 of desired width
    int height = mArea.GetHeight() / mLineCount;
    int done;
    int attempts = 5;
    int w_limit = mArea.GetWidth() * 90 / 100; // >90%
    int h_limit = mArea.GetHeight() * 90 / 100; // > 90%
    DLOG("w_limit: " << w_limit << " h_limit: " << h_limit);

    do {
        mFont.SetSize(width, height);

        mGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
        auto r = calcBoundingRect(mGlyphs);
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

Rect Text::calcBoundingRect(const std::vector<Glyph> &arGlyphs)
{
    int w = 0;
    int h = 0;
    int line_count = 1;
    int line_width = 0;
    for (auto tm : arGlyphs) {
        switch (tm.mSymbolUnicode) {
            case '\n':
                line_count++;
                if (w > line_width) {
                    line_width = w;
                }
                w = 0;
                break;
            default:
                w += tm.mWidth;
                if (h < tm.mHeight) {
                    h = tm.mHeight;
                }
                break;
        }
    }
    if (w > line_width) {
        line_width = w;
    }

    DLOG("Line Width: " << line_width << " Line Height: " << h  << " Line Count: " << line_count);
    return Rect(0, 0, line_width, h * line_count);

}

void Text::loadGlyphs()
{
    if (mScaleToFit) {
        scaleToFit();
    }
    else {
        mGlyphs = mFont.MakeGlyphs(mValue, mLineSpacing);
    }
}

}

