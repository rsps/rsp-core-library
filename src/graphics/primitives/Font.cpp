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
#include <graphics/primitives/Font.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace rsp::graphics
{

inline uint unsign(int aValue)
{
    if (aValue < 0) {
        return 0;
    }

    return static_cast<uint>(aValue);
}

Font::Font(const char *apFilename, int aFaceIndex)
    : mSize(0),
      mWeigth(0),
      mColor(0)
{
    FT_Error error = FT_New_Face(mLib, apFilename, aFaceIndex, &mFace);

    if (error) std::cerr << "FT_New_Face() failed" << std::endl;
}

Font::~Font()
{
}

void Font::SetSize(uint32_t aWidth, uint32_t aHeight)
{
}

TextMask Font::GetSymbol(uint32_t aSymbolCode)
{
    return TextMask();
}

TextMask Font::MakeTextMask(const std::string &arText)
{
    return TextMask();
}

int Font::getKerning(uint aFirst, uint aSecond, uint aKerningMode)
{
    return 0;
}

void Font::getBbox(TextMask &arTextMask)
{
}

std::u32string Font::stringToU32(const std::string &arText)
{
    std::u32string result;

    for (size_t i = 0 ; i < arText.size() ; ) {
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
    mWidth  = static_cast<int>(arFace->glyph->bitmap.width);
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
