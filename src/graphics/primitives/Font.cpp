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

inline uint unsign(int aValue)
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
        os << std::hex << static_cast<int>(tm.mSymbolUnicode) << std::dec;
    }
    os << ", "
        << "Top: " << tm.mTop << ", "
        << "Left: " << tm.mLeft << ", "
        << "Height: " << tm.mHeight << ", "
        << "Width: " << tm.mWidth;

    return os;
}


Font::Font(const char *apFilename, int aFaceIndex)
    : mSize(0), mWeigth(0), mColor(0), mFace(nullptr)
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

void Font::SetSize(uint32_t aPx)
{
    FT_Error error = FT_Set_Pixel_Sizes(mFace, aPx, aPx);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Set_Pixel_Sizes() failed", error);
    }
}

void Font::ScaleToFit(const std::string &arText, uint32_t aWidthPx, uint32_t aHeightPx)
{
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

    std::cout << "ScaleToFit( w:" << ((chars) ? ((aWidthPx + (chars/2)) / chars) : 0) << ", h:" << (aHeightPx / lines) << ")" << std::endl;

    FT_Error error = FT_Set_Pixel_Sizes(mFace, (chars) ? ((aWidthPx + (chars/2)) / chars) : 0, aHeightPx / lines);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Set_Pixel_Sizes() failed", error);
    }
}

TextMask Font::GetSymbol(uint32_t aSymbolCode) const
{
#undef FT_LOAD_TARGET_
#define FT_LOAD_TARGET_( x )   ( static_cast<FT_Int32>(( (x) & 15 ) << 16 ) )

    FT_Error error = FT_Load_Char(mFace, aSymbolCode, FT_LOAD_RENDER /*| FT_LOAD_TARGET_LCD_V*/);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_Load_Char() failed", error);
    }

    TextMask Result { mFace };
    Result.mSymbolUnicode = aSymbolCode;
    return Result;
}

void Font::paintOver(const TextMask &aSrc, TextMask &aDst, int aX, int aY) const
{
    if ((aSrc.mWidth + aX > aDst.mWidth) || (aSrc.mHeight + aY > aDst.mHeight)) {
        THROW_WITH_BACKTRACE1(rsp::utils::CoreException,
            "ERROR: (Src.width + x  > Dst.width ) || (Src.height + y > Dst.height )");
    }

    int i = 0;
    int i_max = aSrc.mHeight * aSrc.mWidth;
    int src_row_start = 0;
    int dst_row_start = aX + aY * aDst.mWidth;

    while (i < i_max) {
        int row_n = aSrc.mWidth;
        unsigned int dst = unsign(dst_row_start);
        unsigned int src = unsign(src_row_start);
        while (row_n > 0) {
            aDst.mBits[dst] = aSrc.mBits[src];
            dst += 1;
            src += 1;
            row_n -= 1;
            i += 1;
        }
        src_row_start += aSrc.mWidth;
        dst_row_start += aDst.mWidth;
    }
}

std::vector<TextMask> Font::MakeTextMasks(const std::string &arText) const
{
    std::u32string unicode = stringToU32(arText);
    uint32_t line_height = 0;

    std::vector<TextMask> result;
    for (auto s : unicode) {
        switch (s) {
            case '\n':
            case '\r':
                break;

            default:
                result.push_back(GetSymbol(s));
                line_height = std::max(line_height, result.back().mHeight);
                auto rs = result.size();
                if (rs > 1) {
                    result[rs - 2].mWidth += getKerning(result[rs - 2].mSymbolUnicode, result[rs - 1].mSymbolUnicode);

                    // Space ' ' has no width, add width of next character to space character
                    if (result[rs - 2].mWidth == 0) {
                        result[rs - 2].mWidth = result[rs - 1].mWidth;
                    }
                }
                break;
        }
    }

    std::cout << "Line Height: " << line_height << std::endl;

    uint32_t top = line_height;
    uint32_t left = 0;
    for (auto &tm : result) {
        std::cout << "Before: " << tm << std::endl;
        switch (tm.mSymbolUnicode) {
            case '\n':
                std::cout << "newline " << top << ", " << line_height << std::endl;
                top += line_height;
                left = 0;
                break;

            default:
                tm.mTop += top - tm.mHeight - tm.mTop;
                tm.mLeft += left;
                left += tm.mWidth;
                break;
        }
        std::cout << "After: " << tm << std::endl;
    }
    return result;
}

TextMask Font::MakeTextMask(const std::string &arText) const
{
    std::u32string unicode = stringToU32(arText);

    TextMask Result { };
    std::vector<TextMask> TmpArray { };

    for (auto s : unicode) {
        TmpArray.push_back(GetSymbol(s));

        std::cout << TmpArray.back() << std::endl;

        Result.mHeight = std::max(Result.mHeight, TmpArray.back().mHeight);
        Result.mWidth += TmpArray.back().mWidth + TmpArray.back().mLeft;

        auto ts = TmpArray.size();
        if (ts > 1) {
            Result.mWidth += getKerning(TmpArray[ts - 2].mSymbolUnicode, TmpArray[ts - 1].mSymbolUnicode);

            // make "Space"
            if (TmpArray[ts - 2].mWidth == 0) {
                TmpArray.back().mLeft += static_cast<int>(TmpArray.back().mWidth);
                Result.mWidth += TmpArray.back().mLeft;
            }
        }

        Result.mTop = std::max(Result.mTop, static_cast<int>(TmpArray.back().mHeight) - TmpArray.back().mTop);
    }

    Result.mHeight += Result.mTop;

    Result.mBits.resize(unsign(Result.mWidth) * unsign(Result.mHeight), 0x00);

    int x = 0;
    uint32_t prev_symbol_unicode = 0;
    for (auto &M : TmpArray) {
        x += M.mLeft + getKerning(prev_symbol_unicode, M.mSymbolUnicode);
        prev_symbol_unicode = M.mSymbolUnicode;
        paintOver(M, Result, x, Result.mHeight - M.mTop - Result.mTop);
        x += static_cast<int>(M.mWidth);
    }

    return Result;
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
