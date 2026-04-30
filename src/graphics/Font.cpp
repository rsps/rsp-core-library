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
#include <logging/Logger.h>

namespace rsp::graphics {

std::string Font::mDefaultFontName("Exo 2");


void Font::RegisterFont(const char *apFileName)
{
    FontRawInterface::RegisterFont(apFileName);
}


Font::Font(const FontStyles aStyle)
    : Font(mDefaultFontName, aStyle)
{
}

Font::Font(const std::string &arFontName, const FontStyles aStyle)
    : mpImpl(MakePimpl(arFontName))
{
    SetStyle(aStyle);
    mpImpl->SetSize(16, 16);
    mDirty = true;
}

Font& Font::SetSize(const int aSizePx)
{
    return SetSize(aSizePx, aSizePx);
}

Font& Font::SetSize(const int aWidthPx, const int aHeightPx)
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

Font& Font::SetStyle(const FontStyles aStyle)
{
    mpImpl->SetStyle(aStyle);
    mDirty = true;
    return *this;
}

FontStyles Font::GetStyle() const
{
    return mpImpl->GetStyle();
}


std::shared_ptr<Glyphs> Font::MakeGlyphs(const std::string &arText, const int aLineSpacing, const int aHAlignment)
{
    mDirty = false;
    return mpImpl->MakeGlyphs(arText, aLineSpacing, aHAlignment);
}

std::string Font::GetFamilyName() const
{
    return mpImpl->GetFamilyName();
}

}
