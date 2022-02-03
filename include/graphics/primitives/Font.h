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
#include <string>
#include <memory>
#include <utils/CoreException.h>
#include <graphics/primitives/Color.h>
#include <graphics/primitives/Rect.h>

namespace rsp::graphics {

class FontRawInterface;

/**
 * \class FontException
 * \brief Exception type used to throw exceptions from the font classes.
 */
class FontException : public rsp::utils::CoreException
{
public:
    explicit FontException(const char *aMsg, int aCode);
    FontException(const char *aMsg) : rsp::utils::CoreException(aMsg) {};
    FontException(const std::string &arMsg) : rsp::utils::CoreException(arMsg.c_str()) {};
};

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

/**
 * \class Font
 * \brief Class for fonts and their properties.
 *
 * TODO: Font::Styles support is not implemented.
 */
class Font
{
public:
    enum class Styles {
        Normal = 0,
        Italic = 1,
        Bold = 2
    };

    /**
     * Static gate to register a font file with the application.
     * The font is later accessed by the family name inside the font.
     *
     * \param apFileName
     */
    static void RegisterFont(const char *apFileName);

    /**
     * Constructs a Font object based on the given font name (font family) in the given style.
     *
     * \param arFontName
     * \param aStyle
     */
    Font(const std::string &arFontName, Styles aStyle = Styles::Normal);
    /**
     * Destructor
     */
    virtual ~Font();

    /**
     * Generate a set of glyphs for the given string.
     *
     * \param arText
     * \param aLineSpacing
     * \return
     */
    std::vector<Glyph> MakeGlyphs(const std::string &arText, int aLineSpacing = 0) const;

    /**
     * Get the family name of the font.
     *
     * \return string
     */
    std::string GetFamilyName() const;

    /**
     * Set the size of the font in pixels.
     *
     * \param aSizePx
     * \return Reference to this for fluent calls.
     */
    Font& SetSize(int aSizePx);
    /**
     * Set the width and height of the font in pixels.
     *
     * \param aWidthPx
     * \param aHeightPx
     * \return Reference to this for fluent calls.
     */
    virtual Font& SetSize(int aWidthPx, int aHeightPx);
    /**
     * Get the pixel size of the font
     * \return integer
     */
    int GetSize() const;

    /**
     * Store a color inside the Font object.
     *
     * \param arColor
     * \return Reference to this for fluent calls.
     */
    Font& SetColor(const Color &arColor);
    /**
     * Get the current color of the font.
     *
     * \return Color
     */
    Color GetColor() const { return mColor; }

    /**
     * Set the style of the font.
     *
     * \param aStyle
     * \return Reference to this for fluent calls.
     */
    Font& SetStyle(Styles aStyle);
    /**
     * Get the current style of the font.
     *
     * \return Font::Styles
     */
    Styles GetStyle() const;

protected:
    Color mColor;
    std::unique_ptr<FontRawInterface> mpImpl{};

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
};

}

#endif //FONT_H
