/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef FONT_H
#define FONT_H

#include <vector>
#include <string>
#include <utils/CoreException.h>
#include <graphics/primitives/Color.h>
#include <graphics/primitives/Rect.h>

namespace rsp::graphics {

class FontRawInterface;

class FontException : public rsp::utils::CoreException
{
public:
    explicit FontException(const char *aMsg, int aCode);
};

class Glyph
{
public:
    Glyph() {}
    Glyph(void* apFace);

    std::vector<uint8_t> mPixels { };
    uint32_t mSymbolUnicode = 0;

    int mTop = 0;
    int mLeft = 0;
    int mWidth = 0;
    int mHeight = 0;
};

std::ostream& operator <<(std::ostream &os, const Glyph &arGlyph);

class Font
{
public:
    enum class Styles {
        Normal = 0,
        Italic = 1,
        Bold = 2
    };

    static void RegisterFont(const char *apFileName);

    Font(const std::string &arFontName, Styles aStyle = Styles::Normal);
    virtual ~Font();

    std::vector<Glyph> MakeGlyphs(const std::string &arText, int aLineSpacing) const;

    std::string GetFamilyName() const;

    Font& SetSize(int aSizePx);
    virtual Font& SetSize(int aWidthPx, int aHeightPx);
    int GetSize() const;

    Font& SetColor(const Color &arColor);
    Color GetColor() const { return mColor; }

    Font& SetStyle(Styles aStyle);
    Styles GetStyle() const;

protected:
    Color mColor;
    FontRawInterface *mpImpl = nullptr;

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
};

}

#endif //FONT_H
