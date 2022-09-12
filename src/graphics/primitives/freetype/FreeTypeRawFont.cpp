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
#include <graphics/primitives/Font.h>
#include <logging/Logger.h>

#include <ft2build.h>
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
    return "Unknown Error";
}

FontException::FontException(const char *aMsg, FT_Error aCode)
    : rsp::utils::CoreException(aMsg)
{
    mMsg += ": (" + std::to_string(aCode) + ") ";
    const char *err = FreeTypeErrorToString(aCode);
    mMsg.append((err) ? err : "N");
}

inline static uint unsign(int aValue)
{
    if (aValue < 0) {
        return 0;
    }

    return static_cast<uint>(aValue);
}

Glyph::Glyph(void* apFace)
{
    FT_Face arFace = reinterpret_cast<FT_Face>(apFace);

    mWidth = static_cast<int>(arFace->glyph->bitmap.width);
    mHeight = static_cast<int>(arFace->glyph->bitmap.rows);
    mTop = arFace->glyph->bitmap_top;
    mLeft = arFace->glyph->bitmap_left;

    mPixels.resize(static_cast<std::vector<uint8_t>::size_type>(unsign(mWidth)) * unsign(mHeight), 0xFF);
    memcpy(mPixels.data(), arFace->glyph->bitmap.buffer, mPixels.size());
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

std::vector<Glyph> FreeTypeRawFont::MakeGlyphs(const std::string &arText, int aLineSpacing)
{
    createFace();

    std::u32string unicode = stringToU32(arText);
    int line_height = 0;

    std::vector<Glyph> result;
    for (char32_t c : unicode) {
        result.push_back(getSymbol(c, mStyle));
        line_height = std::max(line_height, result.back().mHeight);
        auto rs = result.size();
        if (rs > 1) {
            result[rs - 2].mWidth += getKerning(result[rs - 2].mSymbolUnicode, result[rs - 1].mSymbolUnicode);

            // Space ' ' has no width, add width of previous character to space character
            if (result[rs - 2].mWidth == 0) {
                result[rs - 2].mWidth = (rs > 2) ? result[rs - 3].mWidth : mSizePx;
            }
        }
    }

//    DLOG("Line Height: " << line_height);

    int top = line_height;
    int left = 0;
    for (Glyph &glyph : result) {
        if (glyph.mSymbolUnicode == static_cast<uint32_t>('\n')) {
            top += line_height + aLineSpacing;
            left = 0;
        }
        else {
            glyph.mTop += top - glyph.mHeight - glyph.mTop;
            glyph.mLeft += left;
            left += glyph.mWidth;
        }
    }
    return result;
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
    Logger::GetDefault().Debug() << "Font.SetSize(" << aWidthPx << ", " << aHeightPx << ") -> " << mSizePx << std::endl;
}

void FreeTypeRawFont::SetStyle(FontStyles aStyle)
{
    mStyle = aStyle;
    freeFace();
}


Glyph FreeTypeRawFont::getSymbol(uint32_t aSymbolCode, FontStyles aStyle) const
{
#undef FT_LOAD_TARGET_
#define FT_LOAD_TARGET_( x )   ( static_cast<FT_Int32>(( (x) & 15 ) << 16 ) )

    if (aSymbolCode == '\n') {
        Glyph nl { };
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

    Glyph Result { mpFace };
    Result.mSymbolUnicode = aSymbolCode;
    return Result;
}

int FreeTypeRawFont::getKerning(uint aFirst, uint aSecond, uint aKerningMode) const
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

std::u32string FreeTypeRawFont::stringToU32(const std::string &arText) const
{
    std::u32string result;

    for (size_t i = 0 ; i < arText.size() ;) {
        int a = arText[i++];

        if ((a & 0xE0) == 0xC0) {
            a = (a & 0x1F) << 6;
            a |= arText[i++] & 0x3F;
        }
        else if ((a & 0xF0) == 0xE0) {
            a = (a & 0xF) << 12;
            a |= (arText[i++] & 0x3F) << 6;
            a |= arText[i++] & 0x3F;
        }
        else if ((a & 0xF8) == 0xF0) {
            a = (a & 0x7) << 18;
            a |= (a & 0x3F) << 12;
            a |= (arText[i++] & 0x3F) << 6;
            a |= arText[i++] & 0x3F;
        }
        result.push_back(static_cast<char32_t>(a));
    }

    return result;
}

}

#endif /* USE_FREETYPE */

