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

#ifdef USE_FREETYPE

#include <map>
#include <vector>
#include <string>
#include <graphics/primitives/FontRawInterface.h>
#include "FreeTypeLibrary.h"

namespace rsp::graphics {

class FTGlyph : public Glyph
{
public:
    FTGlyph() {}
    FTGlyph(FT_Face apFace);

    const uint8_t* GetPixelRow(int aY) const override {
        return &mpPixels[mPitch * aY];
    }
protected:
    uint8_t *mpPixels = nullptr;
    int mPitch = 0;
};

class FTGlyphs : public Glyphs
{
public:
    int GetCount() const override { return mGlyphs.size(); }
    Glyph& GetGlyph(int aIndex) override { return *static_cast<Glyph*>(&mGlyphs.at(aIndex)); };

protected:
    std::vector<FTGlyph> mGlyphs;
};

/**
 * \class FreeTypeRawFont
 * \brief A FontRawInterface implementation for the FreeType library.
 */
class FreeTypeRawFont : public FontRawInterface
{
public:
    FreeTypeRawFont(const std::string &arFontName, int aFaceIndex = 0);
    ~FreeTypeRawFont();

    std::unique_ptr<Glyphs> MakeGlyphs(const std::string &arText, int aLineSpacing) override;
    std::string GetFamilyName() const override;
    void SetSize(int aWidthPx, int aHeightPx) override;
    void SetStyle(FontStyles aStyle) override;

protected:
    FT_Face mpFace = nullptr;
    std::string mFontName{};
    FreeTypeRawFont(const FreeTypeRawFont&) = delete;
    FreeTypeRawFont& operator=(const FreeTypeRawFont&) = delete;

    void createFace();
    void freeFace();
    FTGlyph getSymbol(uint32_t aSymbolCode, FontStyles aStyle) const;
    int getKerning(uint aFirst, uint aSecond, uint aKerningMode = 0) const;
    std::u32string stringToU32(const std::string &arText) const;
};

}

#endif /* USE_FREETYPE */

#endif /* SRC_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPERAWFONT_H_ */

