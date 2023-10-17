/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_FREETYPE

#include "FreeTypeRawFont.h"
#include <graphics/Font.h>
#include <logging/Logger.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

using namespace rsp::logging;

namespace rsp::graphics {


std::shared_ptr<FontRawInterface> Font::MakePimpl(const std::string &arFontName)
{
    if (arFontName.empty()) {
        THROW_WITH_BACKTRACE1(FontException, "A pre-registered font name must be given on Font object creation.");
    }
    return std::make_shared<FreeTypeRawFont>(arFontName);
}


static const char* FreeTypeErrorToString(int aCode)
{
    #undef FTERRORS_H_
    #define FT_ERROR_START_LIST     switch ( aCode ) {
    #define FT_ERRORDEF( e, v, s )    case v: return s;
    #define FT_ERROR_END_LIST       default: break; }
    #include FT_ERRORS_H
    auto result = FT_Error_String(FT_Error(aCode));
    return (result) ? result : "Unknown Error";
}

std::string FontException::formatCode(int aCode)
{
    std::string result;
    result += ": (" + std::to_string(aCode) + ") ";
    result.append(FreeTypeErrorToString(aCode));
    return result;
}


inline static uint unSign(long aValue)
{
    if (aValue < 0) {
        return 0;
    }
    return static_cast<uint>(aValue);
}

FTGlyph::FTGlyph(FT_Face apFace)
{
    mWidth = static_cast<int>(apFace->glyph->bitmap.width);
    mHeight = static_cast<int>(apFace->glyph->bitmap.rows);
    mTop = apFace->glyph->bitmap_top;
    mLeft = apFace->glyph->bitmap_left;
    mPitch = apFace->glyph->bitmap.pitch;

    mAdvanceX = apFace->glyph->advance.x >> 6;
    mAdvanceY = apFace->glyph->advance.y >> 6;

    mPixels.resize(static_cast<std::vector<uint8_t>::size_type>(unSign(mPitch)) * unSign(mHeight), 0xFF);
    memcpy(mPixels.data(), apFace->glyph->bitmap.buffer, mPixels.size());

//    Rect r(apFace->bbox.xMin, apFace->bbox.yMin, apFace->bbox.xMax, apFace->bbox.yMax);
//    std::cout << "BBOX: " << r << std::endl;
//    mpPixels = apFace->glyph->bitmap.buffer;
}


FreeTypeRawFont::FreeTypeRawFont(const std::string &arFontName, int /*aFaceIndex*/)
{
    mFontName = arFontName;
    createFace();
}

FreeTypeRawFont::~FreeTypeRawFont()
{
    freeFace();
}

std::shared_ptr<Glyphs> FreeTypeRawFont::MakeGlyphs(const std::string &arText, int aLineSpacing, int aHAlignment)
{
    /**
     * For general font terms
     * @See https://wiki.inkscape.org/wiki/File:Glyph_metrics.png
     */
    createFace();

    auto glyphs = std::make_shared<FTGlyphs>();

    std::u32string unicode = stringToU32(arText);

    for (char32_t c : unicode) {
        glyphs->mGlyphs.push_back(getSymbol(c, mStyle));
        auto rs = glyphs->mGlyphs.size();
        if (rs > 1) {
            glyphs->mGlyphs[rs - 2].mWidth += getKerning(glyphs->mGlyphs[rs - 2].mSymbolUnicode, glyphs->mGlyphs[rs - 1].mSymbolUnicode);
        }
    }

    glyphs->mLineHeight = mpFace->size->metrics.height >> 6; // (max_y - min_y); // mpFace->size->metrics.y_ppem;
    glyphs->mUnderlineYCenter = (mpFace->underline_position - 32) >> 6;
    glyphs->mUnderlineThickness = (mpFace->underline_thickness + 63) >> 6;
//    int internal_leading = (mpFace->ascender - mpFace->descender - mpFace->units_per_EM);
//    glyphs->mBaseLine = (mpFace->size->metrics.ascender * glyphs->mLineHeight) / (mpFace->size->metrics.ascender + std::abs(mpFace->size->metrics.descender));
    glyphs->mBaseLine = (mpFace->ascender * glyphs->mLineHeight) / (mpFace->ascender + std::abs(mpFace->descender));

    long baseline = glyphs->mBaseLine;
    long min_left = 0;
    long max_left = 0;
    long line_left = 0;
    long max_height = glyphs->mLineHeight;
    for (Glyph &glyph : glyphs->mGlyphs) {
        if (glyph.mSymbolUnicode == static_cast<uint32_t>('\n')) {
            baseline += glyphs->mLineHeight + aLineSpacing;
            max_height += glyphs->mLineHeight + aLineSpacing;
            glyphs->mLineWidths.push_back(line_left);
            line_left = 0;
        }
        else {
            glyph.mTop = baseline - glyph.mTop;
            if (glyph.mLeft < min_left) {
                min_left = glyph.mLeft;
            }
            glyph.mLeft += line_left;
            line_left += glyph.mAdvanceX;
            if (line_left > max_left) {
                max_left = line_left;
            }
        }
    }
    glyphs->mLineWidths.push_back(line_left);

    if (!glyphs->mGlyphs.empty() && (glyphs->mGlyphs.back().mWidth > glyphs->mGlyphs.back().mAdvanceX)) {
        max_left++;
    }

    if (aHAlignment > 0) {
        size_t line = 0;
        long offset = (aHAlignment == 1) ? ((max_left - glyphs->mLineWidths[line]) / 2) : (max_left - glyphs->mLineWidths[line]);
        for (Glyph &glyph : glyphs->mGlyphs) {
            if (glyph.mSymbolUnicode == static_cast<uint32_t>('\n')) {
                line++;
                offset = (aHAlignment == 1) ? ((max_left - glyphs->mLineWidths[line]) / 2) : (max_left - glyphs->mLineWidths[line]);
            }
            else {
                glyph.mLeft += offset;
            }
        }
    }

    glyphs->mBoundingRect = Rect(GuiUnit_t(min_left), 0, GuiUnit_t(max_left), GuiUnit_t(max_height));
    return glyphs;
}

std::string FreeTypeRawFont::GetFamilyName() const
{
    return mpFace->family_name;
}

void FreeTypeRawFont::SetSize(int aWidthPx, int aHeightPx)
{
    createFace();

    FT_Error error = FT_Set_Pixel_Sizes(mpFace, static_cast<uint32_t>(aWidthPx), static_cast<uint32_t>(aHeightPx));
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Set_Pixel_Sizes() failed", error);
    }
    mSizePx = std::min(aWidthPx, aHeightPx);
    Logger::GetDefault().Debug() << "Font.SetSize(" << aWidthPx << ", " << aHeightPx << ") -> " << mSizePx;
}

void FreeTypeRawFont::SetStyle(FontStyles aStyle)
{
    mStyle = aStyle;
    freeFace();
}


FTGlyph FreeTypeRawFont::getSymbol(char32_t aSymbolCode, FontStyles aStyle) const
{
#undef FT_LOAD_TARGET_
#define FT_LOAD_TARGET_( x )   ( static_cast<FT_Int32>(( (x) & 15 ) << 16 ) )

    if (aSymbolCode == '\n') {
        FTGlyph nl { };
        nl.mSymbolUnicode = aSymbolCode;
        return nl;
    }

    FT_Error error = FT_Load_Char(mpFace, aSymbolCode, FT_LOAD_RENDER /*| FT_LOAD_TARGET_LCD_V*/);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, (std::string("FT_Load_Char() failed for symbol ") + static_cast<char>(aSymbolCode)).c_str(), error);
    }

    if ((static_cast<int>(aStyle) & static_cast<int>(FontStyles::Bold)) && (mpFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)) {
        FT_Outline_Embolden( &mpFace->glyph->outline,  (1 << 6));
    }

    FTGlyph result { mpFace };
    result.mSymbolUnicode = aSymbolCode;
    if (aSymbolCode == ' ') {
        result.mWidth = result.mAdvanceX;
    }
    return result;
}

long FreeTypeRawFont::getKerning(char32_t aFirst, char32_t aSecond, uint aKerningMode) const
{
    if (aKerningMode == 0) {
        aKerningMode = FT_KERNING_DEFAULT;
    }

    if (aFirst == 0) {
        return 0;
    }

    FT_UInt IndexFirst = FT_Get_Char_Index(mpFace, aFirst);
    FT_UInt IndexSecond = FT_Get_Char_Index(mpFace, aSecond);
    FT_Vector delta { };
    FT_Error error = FT_Get_Kerning(mpFace, IndexFirst, IndexSecond, aKerningMode, &delta);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Get_Kerning() failed", error);
    }

    return delta.x >> 6;
}

void FreeTypeRawFont::createFace()
{
    if (mpFace) {
        return;
    }

    mpFace = FreeTypeLibrary::Get().CreateFontFace(mFontName, mStyle);
}

void FreeTypeRawFont::freeFace()
{
    if (mpFace) {
        FT_Done_Face(mpFace);
        mpFace = nullptr;
    }
}

std::u32string FreeTypeRawFont::stringToU32(const std::string &arText)
{
    std::u32string result;

    for (size_t i = 0 ; i < arText.size() ;) {
        auto a = char32_t(arText[i++]);

        if ((a & 0xE0) == 0xC0) {
            a = (a & 0x1F) << 6;
            a |= char32_t(arText[i++]) & 0x3F;
        }
        else if ((a & 0xF0) == 0xE0) {
            a = (a & 0xF) << 12;
            a |= (char32_t(arText[i++]) & 0x3F) << 6;
            a |= char32_t(arText[i++]) & 0x3F;
        }
        else if ((a & 0xF8) == 0xF0) {
            a = (a & 0x7) << 18;
            a |= (a & 0x3F) << 12;
            a |= (char32_t(arText[i++]) & 0x3F) << 6;
            a |= char32_t(arText[i++]) & 0x3F;
        }
        result.push_back(static_cast<char32_t>(a));
    }

    return result;
}

}

#endif /* USE_FREETYPE */
