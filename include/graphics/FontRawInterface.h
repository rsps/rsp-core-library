/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_GRAPHICS_FONTRAWINTERFACE_H_
#define INCLUDE_GRAPHICS_FONTRAWINTERFACE_H_

#include <graphics/Rect.h>
#include <string>
#include <memory>
#include <vector>

namespace rsp::graphics {


/**
 * \class GlyphInterface
 * \brief A data class holding all the pixel data of a unicode glyph.
 */
class Glyph
{
public:
    virtual ~Glyph() {};

    char32_t mSymbolUnicode = 0;
    int mTop = 0;
    int mLeft = 0;
    int mWidth = 0;
    int mHeight = 0;
    int mAdvanceX = 0;
    int mAdvanceY = 0;

    /**
     * \brief Get a pointer to a row of alpha values, with a length equal to the glyph width.
     *
     * \param aY Row index to return
     * \return Pointer to alpha values
     */
    virtual const uint8_t* GetPixelRow(int aY) const = 0;
};

class Glyphs
{
public:
    virtual ~Glyphs() {}
    virtual unsigned GetCount() const = 0;
    virtual Glyph& GetGlyph(unsigned int aIndex) = 0;
    virtual const Glyph& GetGlyph(unsigned int aIndex) const = 0;

    int mUnderlineYCenter = 0;
    int mUnderlineThickness = 0;
    int mLineHeight = 0;
    int mBaseLine = 0;
    Rect mBoundingRect{};
    std::vector<int> mLineWidths{};
};

/**
 * Stream overloading for the Glyph class. Can be used for debugging.
 *
 * \param os
 * \param arGlyph
 * \return
 */
std::ostream& operator <<(std::ostream &os, const Glyph &arGlyph);

/**
 * \brief Stream overloading for the Glyphs container. Can be used for debugging.
 *
 * \param os
 * \param arGlyphs
 * \return
 */
std::ostream& operator <<(std::ostream &os, const Glyphs &arGlyphs);

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

    virtual std::unique_ptr<Glyphs> MakeGlyphs(const std::string &arText, int aLineSpacing) = 0;
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

#endif /* INCLUDE_GRAPHICS_FONTRAWINTERFACE_H_ */
