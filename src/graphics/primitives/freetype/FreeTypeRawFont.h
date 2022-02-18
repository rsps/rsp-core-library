/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef SRC_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPERAWFONT_H_
#define SRC_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPERAWFONT_H_

#include <map>
#include <string>
#include <graphics/primitives/FontRawInterface.h>
#include "FreeTypeLibrary.h"

namespace rsp::graphics {

/**
 * \class FreeTypeRawFont
 * \brief A FontRawInterface implementation for the FreeType library.
 */
class FreeTypeRawFont : public FontRawInterface
{
public:
    FreeTypeRawFont(const std::string &arFontName, int aFaceIndex = 0);
    ~FreeTypeRawFont();

    std::vector<Glyph> MakeGlyphs(const std::string &arText, int aLineSpacing) override;
    std::string GetFamilyName() const override;
    void SetSize(int aWidthPx, int aHeightPx) override;
    void SetStyle(Font::Styles aStyle) override;

protected:
    FT_Face mpFace = nullptr;
    std::string mFontName{};
    FreeTypeRawFont(const FreeTypeRawFont&) = delete;
    FreeTypeRawFont& operator=(const FreeTypeRawFont&) = delete;

    void createFace();
    void freeFace();
    Glyph getSymbol(uint32_t aSymbolCode, Font::Styles aStyle) const;
    int getKerning(uint aFirst, uint aSecond, uint aKerningMode = 0) const;
    std::u32string stringToU32(const std::string &arText) const;
};

}


#endif /* SRC_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPERAWFONT_H_ */
