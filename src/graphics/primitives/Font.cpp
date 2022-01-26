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

std::ostream& operator <<(std::ostream &os, TextMask &tm)
{
    os << "Symbol: ";
    if ((tm.mSymbolUnicode > 31) && (tm.mSymbolUnicode < 127)) {
        os << static_cast<char>(tm.mSymbolUnicode);
    }
    else {
        os << "0x" << std::hex << static_cast<int>(tm.mSymbolUnicode) << std::dec;
    }
    os << ", "
        << "Top: " << tm.mTop << ", "
        << "Left: " << tm.mLeft << ", "
        << "Height: " << tm.mHeight << ", "
        << "Width: " << tm.mWidth;

    return os;
}


Font::Font(const char *apFilename, int aFaceIndex)
    : mSizePx(0), mStyle(Normal), mColor(Color::White), mFace(nullptr)
{
    FT_Error error = FT_New_Face(mLib, apFilename, aFaceIndex, &mFace);

    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
    }
}

Font::~Font()
{
    if (mFace) {
        FT_Done_Face(mFace);
    }
}

Font& Font::SetSize(int aSizePx)
{
    return SetSize(aSizePx, aSizePx);
}

Font& Font::SetSize(int aWidthPx, int aHeightPx)
{
    FT_Error error = FT_Set_Pixel_Sizes(mFace, static_cast<uint32_t>(aWidthPx), static_cast<uint32_t>(aHeightPx));
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Set_Pixel_Sizes() failed", error);
    }
    mSizePx = std::min(aWidthPx, aHeightPx);
    DLOG("Font.SetSize(" << aWidthPx << ", " << aHeightPx << ") -> " << mSizePx);
    return *this;
}

std::vector<TextMask> Font::ScaleToFit(const std::string &arText, int aWidthPx, int aHeightPx)
{
    DLOG("ScaleToFit(" << arText <<  ", " << aWidthPx << ", " << aHeightPx << ")");
    int lines = 1;
    int chars = 0;
    int count = 0;

    for (int i = 0; i < arText.size(); i++) {
        if (arText[i] == '\n') {
            lines++;
            count = 0;
        }
        else {
            if (++count > chars) {
                chars = count;
            }
        }
    }

    if (chars == 0) {
        ++chars;
    }
    int width = ((aWidthPx + (chars/2)) / chars) * 2; // Texts seems to be about 1/3 of desired width
    int height = aHeightPx / lines;
    int done;
    int attempts = 5;
    int w_limit = aWidthPx - (aWidthPx / 10);
    int h_limit = aHeightPx - (aHeightPx / 10);
    DLOG("w_limit: " << w_limit << " h_limit: " << h_limit);

    std::vector<TextMask> result;

    do {
        SetSize(width, height);

        result = MakeTextMasks(arText);
        auto r = GetTextBoundingRect(result);
        DLOG("Bounding Rect: " << r);

        done = 0;
        if ((r.GetWidth() < w_limit) || (r.GetWidth() > aWidthPx)) {
            width += ((aWidthPx - r.GetWidth()) + (chars/2)) / chars;
        }
        else {
            done++;
        }
        if ((r.GetHeight() < h_limit) || (r.GetHeight() > aHeightPx)) {
            height += (aHeightPx - r.GetHeight()) / lines;
        }
        else {
            done++;
        }
        DLOG("done: " << done << " attempts: " << attempts);
    }
    while( (done != 2) && --attempts);

    return result;
}

Rect Font::GetTextBoundingRect(const std::vector<TextMask> &arTms) const
{
    int w = 0;
    int h = 0;
    int line_count = 1;
    int line_width = 0;
    for (auto tm : arTms) {
        switch (tm.mSymbolUnicode) {
            case '\n':
                line_count++;
                if (w > line_width) {
                    line_width = w;
                }
                w = 0;
                break;
            default:
                w += tm.mWidth;
                if (h < tm.mHeight) {
                    h = tm.mHeight;
                }
                break;
        }
    }
    if (w > line_width) {
        line_width = w;
    }

    DLOG("Line Width: " << line_width << " Line Height: " << h  << " Line Count: " << line_count);
    return Rect(0, 0, line_width, h * line_count);
}

TextMask Font::GetSymbol(uint32_t aSymbolCode) const
{
#undef FT_LOAD_TARGET_
#define FT_LOAD_TARGET_( x )   ( static_cast<FT_Int32>(( (x) & 15 ) << 16 ) )

    if (aSymbolCode == '\n') {
        TextMask nl { };
        nl.mSymbolUnicode = aSymbolCode;
        return nl;
    }

    FT_Error error = FT_Load_Char(mFace, aSymbolCode, FT_LOAD_RENDER /*| FT_LOAD_TARGET_LCD_V*/);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Load_Char() failed", error);
    }

    TextMask Result { mFace };
    Result.mSymbolUnicode = aSymbolCode;
    return Result;
}

std::vector<TextMask> Font::MakeTextMasks(const std::string &arText) const
{
    std::u32string unicode = stringToU32(arText);
    int line_height = 0;

    std::vector<TextMask> result;
    for (auto s : unicode) {
        result.push_back(GetSymbol(s));
        line_height = std::max(line_height, result.back().mHeight);
        auto rs = result.size();
        if (rs > 1) {
            result[rs - 2].mWidth += getKerning(result[rs - 2].mSymbolUnicode, result[rs - 1].mSymbolUnicode);

            // Space ' ' has no width, add width of next character to space character
            if (result[rs - 2].mWidth == 0) {
                result[rs - 2].mWidth = mSizePx;
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
                top += line_height;
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

int Font::getKerning(uint aFirst, uint aSecond, uint aKerningMode) const
{
    if (aKerningMode == 0) {
        aKerningMode = FT_KERNING_DEFAULT;
    }

    if (aFirst == 0) {
        return 0;
    }

    FT_UInt IndexFirst = FT_Get_Char_Index(mFace, aFirst);
    FT_UInt IndexSecond = FT_Get_Char_Index(mFace, aSecond);
    FT_Vector delta { };
    FT_Error error = FT_Get_Kerning(mFace, IndexFirst, IndexSecond, aKerningMode, &delta);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Get_Kerning() failed", error);
    }

    return delta.x >> 6;
}

std::u32string Font::stringToU32(const std::string &arText) const
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

TextMask::TextMask(const FontFace &arFace)
{
    mWidth = static_cast<int>(arFace->glyph->bitmap.width);
    mHeight = static_cast<int>(arFace->glyph->bitmap.rows);
    mTop = arFace->glyph->bitmap_top;
    mLeft = arFace->glyph->bitmap_left;

    mBits.resize(unsign(mWidth) * unsign(mHeight), 0xFF);
    memcpy(mBits.data(), arFace->glyph->bitmap.buffer, mBits.size());
}

FreeTypeLibrary::FreeTypeLibrary(void)
{
    FT_Error error = FT_Init_FreeType(&mFtLib);
    if (error) {
        THROW_WITH_BACKTRACE3(std::system_error, error, std::generic_category(), "Failed to load library");
    }
}

FreeTypeLibrary::~FreeTypeLibrary(void)
{
    FT_Done_FreeType(mFtLib);
}

}
