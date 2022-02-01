/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "FreeTypeRawFont.h"
#include <graphics/primitives/Font.h>
#include <logging/Logger.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace rsp::graphics {


const char* FreeTypeErrorToString(int aCode)
{
    #undef FTERRORS_H_
    #define FT_ERROR_START_LIST     switch ( aCode ) {
    #define FT_ERRORDEF( e, v, s )    case v: return s;
    #define FT_ERROR_END_LIST       }
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
    FT_Face arFace = apFace;

    mWidth = static_cast<int>(arFace->glyph->bitmap.width);
    mHeight = static_cast<int>(arFace->glyph->bitmap.rows);
    mTop = arFace->glyph->bitmap_top;
    mLeft = arFace->glyph->bitmap_left;

    mPixels.resize(unsign(mWidth) * unsign(mHeight), 0xFF);
    memcpy(mPixels.data(), arFace->glyph->bitmap.buffer, mPixels.size());
}


FreeTypeRawFont::FreeTypeRawFont(const std::string &arFontName, int aFaceIndex)
    : mSizePx(0), mStyle(Font::Styles::Normal)
{
    FT_Error error = FT_New_Face(FreeTypeLibrary::Get(), apFilename, aFaceIndex, &mpFace);

    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
    }
}

FreeTypeRawFont::~FreeTypeRawFont()
{
    if (mpFace) {
        FT_Done_Face(mpFace);
    }
}

std::vector<Glyph> FreeTypeRawFont::MakeGlyphs(const std::string &arText, int aLineSpacing, Font::Styles aStyle, int aSizePx) const
{
    std::u32string unicode = stringToU32(arText);
    int line_height = 0;

    std::vector<Glyph> result;
    for (auto s : unicode) {
        result.push_back(getSymbol(s, aStyle));
        line_height = std::max(line_height, result.back().mHeight);
        auto rs = result.size();
        if (rs > 1) {
            result[rs - 2].mWidth += getKerning(result[rs - 2].mSymbolUnicode, result[rs - 1].mSymbolUnicode);

            // Space ' ' has no width, add width of next character to space character
            if (result[rs - 2].mWidth == 0) {
                result[rs - 2].mWidth = aSizePx;
//                result[rs - 2].mWidth = result[rs - 1].mWidth;
            }
        }
    }

//    DLOG("Line Height: " << line_height);

    uint32_t top = line_height;
    uint32_t left = 0;
    for (auto &tm : result) {
        switch (tm.mSymbolUnicode) {
            case '\n':
                top += line_height + aLineSpacing;
                left = 0;
                break;

            default:
                tm.mTop += top - tm.mHeight - tm.mTop;
                tm.mLeft += left;
                left += tm.mWidth;
                break;
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
    FT_Error error = FT_Set_Pixel_Sizes(mpFace, static_cast<uint32_t>(aWidthPx), static_cast<uint32_t>(aHeightPx));
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Set_Pixel_Sizes() failed", error);
    }
    mSizePx = std::min(aWidthPx, aHeightPx);
    DLOG("Font.SetSize(" << aWidthPx << ", " << aHeightPx << ") -> " << result);
}

Glyph FreeTypeRawFont::getSymbol(uint32_t aSymbolCode, Font::Styles aStyle) const
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
        THROW_WITH_BACKTRACE2(FontException, "FT_Load_Char() failed", error);
    }

    if ((aStyle & Font::Styles::Bold) && (mpFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)) {
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
    mpFace
}

void FreeTypeRawFont::freeFace()
{
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


