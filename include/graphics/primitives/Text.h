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
    Text(const std::string &arText);
    virtual ~Text() {};

    const std::string& GetValue() const { return mValue; }
    Text& SetValue(const std::string &arValue) { mValue = arValue; return *this; }

    const Rect& GetArea() const { return mArea; }
    Text& SetArea(const Rect &arRect) { mArea = arRect; return *this; }

    Font& GetFont() { return mFont; }
//    Text& SetFont(const Font &arFont) { mFont = arFont; return *this; }

    bool GetScaleToFit() const { return mScaleToFit; }
    Text& SetScaleToFit(bool aValue = true) { mScaleToFit = aValue; return *this; }

    Text& SetLineSpacing(int aSpacing) { mLineSpacing = aSpacing; return *this; }
    int GetLineSpacing() const { return mLineSpacing; }

    int GetLineCount() const { return mLineCount; }

    Text& Invalidate();

    const std::vector<Glyph>& GetGlyphs() const { return mGlyphs; }

protected:
    Font mFont;
    Rect mArea;
    std::string mValue{};
    bool mScaleToFit = false;
    std::vector<Glyph> mGlyphs{};
    int mLineCount = 0;
    int mLineMaxChar = 0;
    int mLineSpacing = 1;

    void scaleToFit();
    Rect calcBoundingRect(const std::vector<Glyph> &arGlyphs);
    void loadGlyphs();
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_ */
