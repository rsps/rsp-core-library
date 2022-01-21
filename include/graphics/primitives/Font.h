/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef FONT_H
#define FONT_H

#include <vector>
#include <utils/CoreException.h>
#include <graphics/primitives/Color.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace rsp::graphics
{

typedef FT_Face FontFace;

class TextMask
{
public:
    TextMask() {}
    TextMask(const FontFace &arFace);

    std::vector<uint8_t> mBits { };
    uint32_t mSymbolUnicode = 0;

    int mTop = 0;
    int mLeft = 0;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
};

/**
 * \class FreeTypeLibrary
 * \brief Simple object to load the freetype library
 *
 */
class FreeTypeLibrary
{
public:
    FreeTypeLibrary(void);
    ~FreeTypeLibrary(void);

    operator FT_Library() const {
        return mFtLib;
    }

private:
    FreeTypeLibrary(const FreeTypeLibrary&) = delete;
    FreeTypeLibrary& operator =(const FreeTypeLibrary&) = delete;

    FT_Library mFtLib { };
};


class Font
{
public:
    Font(const char *apFilename, int aFaceIndex = 0);
    ~Font();

    operator FontFace() const { return mFace; }

    void SetSize(uint32_t aWidth, uint32_t aHeight);

    TextMask GetSymbol(uint32_t aSymbolCode);
    TextMask MakeTextMask(const std::string& arText);

protected:
    std::string mName;
    std::string mStyle;
    Color mColor;
    int mWeigth;
    int mSize;

    FreeTypeLibrary mLib {};
    FontFace mFace {};

    int getKerning(uint aFirst, uint aSecond, uint aKerningMode = 0);
    void getBbox(TextMask& arTextMask);
    std::u32string stringToU32(const std::string &arText);

private:
    Font(const Font &) = delete;
    Font &operator =(const Font &) = delete;
};

}

#endif //FONT_H
