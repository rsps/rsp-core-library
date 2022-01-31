/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_

#include <string>
#include "Font.h"
#include "Rect.h"

namespace rsp::graphics {

class Text
{
public:
    Text();
    Text(const Font &arFont);
    Text(const std::string &arText, const Font &arFont);
    virtual ~Text() {};

    Text& SetValue(const std::string &arValue);
    std::string& GetValue() { return mValue; }

    int GetLineCount() { return mLineCount; }

    Rect& GetArea() { return mArea; }
    Text& SetArea(const Rect &arRect);

    Font& GetFont() { return mFont; }
    Text& SetFont(const Font &arFont);

    Text& SetScaleToFit(bool aValue = true);
    bool GetScaleToFit() { return mScaleToFit; }

    Text& Invalidate();

    std::vector<TextMask>& GetMasks() { return mMasks; }

protected:
    Font mFont;
    Rect mArea;
    std::string mValue;
    bool mScaleToFit = false;
    std::vector<TextMask> mMasks;
    int mLineCount = 0;
    int mLineMaxChar = 0;

    void scaleToFit();
    Rect calcBoundingRect();
    void loadGlyphs();
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_ */
