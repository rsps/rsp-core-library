/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_RAW_FONT_H
#define RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_RAW_FONT_H

#ifdef USE_FREETYPE

#include <map>
#include <vector>
#include <string>
#include <graphics/FontRawInterface.h>
#include "FreeTypeLibrary.h"
#include <logging/LogChannel.h>

namespace rsp::graphics {

class FTGlyph : public Glyph
{
public:
    FTGlyph() = default;
    explicit FTGlyph(FT_Face apFace);

    [[nodiscard]] const uint8_t* GetPixelRow(size_t aY) const override
    {
        return &mPixels[size_t(mPitch) * aY];
    }
protected:
    std::vector<uint8_t> mPixels{};
    int mPitch = 0;
};

class FTGlyphs : public Glyphs
{
public:
    FTGlyphs() noexcept = default;

    [[nodiscard]] unsigned int GetCount() const override { return mGlyphs.size(); }
    Glyph& GetGlyph(unsigned aIndex) override { return *static_cast<Glyph*>(&mGlyphs.at(aIndex)); };
    [[nodiscard]] const Glyph& GetGlyph(unsigned aIndex) const override { return *static_cast<const Glyph*>(&mGlyphs.at(aIndex)); };

    std::vector<FTGlyph> mGlyphs{};
};

/**
 * \class FreeTypeRawFont
 * \brief A FontRawInterface implementation for the FreeType library.
 */
class FreeTypeRawFont : public FontRawInterface, public logging::NamedLogger<FreeTypeRawFont>
{
public:
    explicit FreeTypeRawFont(const std::string &arFontName, int aFaceIndex = 0);
    ~FreeTypeRawFont() override;

    FreeTypeRawFont(const FreeTypeRawFont&) = delete;
    FreeTypeRawFont& operator=(const FreeTypeRawFont&) = delete;

    std::shared_ptr<Glyphs> MakeGlyphs(const std::string &arText, int aLineSpacing, int aHAlignment) override;
    [[nodiscard]] std::string GetFamilyName() const override;
    void SetSize(int aWidthPx, int aHeightPx) override;
    void SetStyle(FontStyles aStyle) override;

protected:
    FT_Face mpFace = nullptr;
    std::string mFontName{};

    void createFace();
    void freeFace();
    [[nodiscard]] FTGlyph getSymbol(char32_t aSymbolCode, FontStyles aStyle) const;
    [[nodiscard]] long getKerning(char32_t aFirst, char32_t aSecond, uint aKerningMode = 0) const;
    [[nodiscard]] static std::u32string stringToU32(const std::string &arText);
};

}

#endif /* USE_FREETYPE */

#endif // RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_RAW_FONT_H
