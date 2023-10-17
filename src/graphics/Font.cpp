/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/Font.h>
#include <graphics/FontRawInterface.h>
#include <string>
#include <locale>
#include <codecvt>
#include <logging/Logger.h>

namespace rsp::graphics {

std::string Font::mDefaultFontName("Exo 2");


std::ostream& operator <<(std::ostream &os, const Glyph &arGlyph)
{
    os << "Symbol: '";
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    os << convert.to_bytes(&arGlyph.mSymbolUnicode, &arGlyph.mSymbolUnicode + 1);
    os << "' "
        << "Top: " << arGlyph.mTop << ", "
        << "Left: " << arGlyph.mLeft << ", "
        << "Height: " << arGlyph.mHeight << ", "
        << "Width: " << arGlyph.mWidth << ", "
        << "Advance: (" << arGlyph.mAdvanceX << "," << arGlyph.mAdvanceY << ")";
    return os;
}

std::ostream& operator <<(std::ostream &os, const Glyphs &arGlyphs)
{
    os << "Glyphs (" << arGlyphs.GetCount() << ")\n"
        << "  Underline: " << arGlyphs.mUnderlineYCenter << "\n"
        << "  Thickness: " << arGlyphs.mUnderlineThickness << "\n"
        << "  Line height: " << arGlyphs.mLineHeight << "\n"
        << "  BRect: " << arGlyphs.mBoundingRect;

    for (unsigned i=0 ; i < arGlyphs.GetCount() ; ++i) {
        os << "\n  " << arGlyphs.GetGlyph(i);
    }
    return os;
}

void Font::RegisterFont(const char *apFileName)
{
    FontRawInterface::RegisterFont(apFileName);
}


Font::Font(FontStyles aStyle)
    : mpImpl(MakePimpl(mDefaultFontName))
{
    SetStyle(aStyle);
    SetSize(16);
}

Font::Font(const std::string &arFontName, FontStyles aStyle)
    : mpImpl(MakePimpl(arFontName))
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
    mDirty = true;
    return *this;
}

int Font::GetSize() const
{
    return mpImpl->GetSize();
}

Font& Font::SetColor(const Color &arColor)
{
    if (mColor != arColor) {
        mColor = arColor;
        mDirty = true;
    }
    return *this;
}

//Font& Font::SetBackgroundColor(const Color &arColor)
//{
//    mBackgroundColor = arColor;
//    return *this;
//}

Font& Font::SetStyle(FontStyles aStyle)
{
    mpImpl->SetStyle(aStyle);
    mDirty = true;
    return *this;
}

FontStyles Font::GetStyle() const
{
    return mpImpl->GetStyle();
}


std::shared_ptr<Glyphs> Font::MakeGlyphs(const std::string &arText, int aLineSpacing, int aHAlignment)
{
    mDirty = false;
    return mpImpl->MakeGlyphs(arText, aLineSpacing, aHAlignment);
}

std::string Font::GetFamilyName() const
{
    return mpImpl->GetFamilyName();
}

}
