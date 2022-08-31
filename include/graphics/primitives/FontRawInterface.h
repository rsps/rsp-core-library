/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_

#include <string>
#include <memory>
#include <vector>

namespace rsp::graphics {


/**
 * \class Glyph
 * \brief A data class holding all the pixel data of a unicode glyph.
 */
class Glyph
{
public:
    Glyph() {}
    Glyph(void* apFace);

    std::vector<uint8_t> mPixels { };
    uint32_t mSymbolUnicode = 0;

    int mTop = 0;
    int mLeft = 0;
    int mWidth = 0;
    int mHeight = 0;
};

/**
 * Stream overloading for the Glyph class. Can be used for debugging.
 *
 * \param os
 * \param arGlyph
 * \return
 */
std::ostream& operator <<(std::ostream &os, const Glyph &arGlyph);

enum class FontStyles {
    Normal = 0,
    Italic = 1,
    Bold = 2,
    BoldItalic = 3
};

/**
 * \class FontRawInterface
 * \brief An abstract interface used to separate different font engines.
 */
class FontRawInterface
{
public:
    static void RegisterFont(std::string_view aFileName);

    virtual ~FontRawInterface() {};

    virtual std::vector<Glyph> MakeGlyphs(const std::string &arText, int aLineSpacing) = 0;
    virtual std::string GetFamilyName() const = 0;
    virtual void SetSize(int aWidthPx, int aHeightPx) = 0;

    int GetSize() const { return mSizePx; }

    virtual void SetStyle(FontStyles aStyle) { mStyle = aStyle; }
    FontStyles GetStyle() const { return mStyle; }

protected:
    FontStyles mStyle = FontStyles::Normal;
    int mSizePx = 0;
};

}

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_ */
