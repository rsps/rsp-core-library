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
#include <locale>
#include <algorithm>
#include <graphics/primitives/Font.h>
#include <graphics/primitives/FontRawInterface.h>
#include <logging/Logger.h>

namespace rsp::graphics {

std::string Font::mDefaultFontName("Exo 2");


std::ostream& operator <<(std::ostream &os, const Glyph &arGlyph)
{
    os << "Symbol: ";
    if ((arGlyph.mSymbolUnicode > 31) && (arGlyph.mSymbolUnicode < 127)) {
        os << static_cast<char>(arGlyph.mSymbolUnicode);
    }
    else {
        os << "0x" << std::hex << static_cast<int>(arGlyph.mSymbolUnicode) << std::dec;
    }
    os << ", "
        << "Top: " << arGlyph.mTop << ", "
        << "Left: " << arGlyph.mLeft << ", "
        << "Height: " << arGlyph.mHeight << ", "
        << "Width: " << arGlyph.mWidth;

    return os;
}

void Font::RegisterFont(const char *apFileName)
{
    FontRawInterface::RegisterFont(apFileName);
}


Font::Font(FontStyles aStyle)
    : mColor(Color::White),
      mpImpl(MakePimpl(mDefaultFontName))
{
    SetStyle(aStyle);
    SetSize(16);
}

Font::Font(const std::string &arFontName, FontStyles aStyle)
    : mColor(Color::White),
      mpImpl(MakePimpl(arFontName))
{
    SetStyle(aStyle);
    SetSize(16);
}

Font::~Font()
{
}

Font& Font::SetSize(int aSizePx)
{
    return SetSize(aSizePx, aSizePx);
}

Font& Font::SetSize(int aWidthPx, int aHeightPx)
{
    mpImpl->SetSize(aWidthPx, aHeightPx);
    return *this;
}

int Font::GetSize() const
{
    return mpImpl->GetSize();
}

Font& Font::SetColor(const Color &arColor)
{
    mColor = arColor;
    return *this;
}

Font& Font::SetStyle(FontStyles aStyle)
{
    mpImpl->SetStyle(aStyle);
    return *this;
}

FontStyles Font::GetStyle() const
{
    return mpImpl->GetStyle();
}


std::vector<Glyph> Font::MakeGlyphs(const std::string &arText, int aLineSpacing) const
{
    return mpImpl->MakeGlyphs(arText, aLineSpacing);
}

std::string Font::GetFamilyName() const
{
    return mpImpl->GetFamilyName();
}

}
