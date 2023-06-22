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

#include <exceptions/CoreException.h>
#include <graphics/Color.h>
#include <graphics/FontRawInterface.h>
#include <graphics/Rect.h>
#include <vector>
#include <string>
#include <memory>

namespace rsp::graphics {

/**
 * \class FontException
 * \brief Exception type used to throw exceptions from the font classes.
 */
class FontException : public exceptions::CoreException
{
public:
    explicit FontException(const char *aMsg, int aCode);
    FontException(const char *aMsg) : CoreException(aMsg) {};
    FontException(const std::string &arMsg) : CoreException(arMsg.c_str()) {};

    std::string formatCode(int aCode);
};

/**
 * \class Font
 * \brief Class for fonts and their properties.
 */
class Font
{
public:
    /**
     * Static gate to register a font file with the application.
     * The font is later accessed by the family name inside the font.
     *
     * \param apFileName
     */
    static void RegisterFont(const char *apFileName);
    static void SetDefaultFont(const std::string &arFontName) { mDefaultFontName = arFontName; }
    static const std::string& GetDefaultFont() { return mDefaultFontName; }

    Font(FontStyles aStyle = FontStyles::Normal);

    /**
     * Constructs a Font object based on the given font name (font family) in the given style.
     *
     * \param arFontName
     * \param aStyle
     */
    Font(const std::string &arFontName, FontStyles aStyle = FontStyles::Normal);

    Font(const Font&) = default;
    Font(Font&&) = default;
    Font& operator=(const Font&) = default;
    Font& operator=(Font&&) = default;

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
    std::unique_ptr<Glyphs> MakeGlyphs(const std::string &arText, int aLineSpacing = 0);

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
     * \brief Set the background color for the characters in the font
     * \param arColor
     * \return self
     */
//    Font& SetBackgroundColor(const Color &arColor);
    /**
     * \brief Get the current background color for the characters in the font.
     * \return Color
     */
//    Color GetBackgroundColor() const { return mBackgroundColor; }

    /**
     * Set the style of the font.
     *
     * \param aStyle
     * \return Reference to this for fluent calls.
     */
    Font& SetStyle(FontStyles aStyle);
    /**
     * Get the current style of the font.
     *
     * \return Font::Styles
     */
    FontStyles GetStyle() const;

    /**
     * \brief Get if the font attributes has changed since last creation
     *
     * \return True if font needs updating
     */
    bool IsDirty() const { return mDirty; }

protected:
    static std::string mDefaultFontName;
    Color mColor;
//    Color mBackgroundColor = Color::None;
    bool mDirty = false;
    std::shared_ptr<FontRawInterface> mpImpl;

    static std::shared_ptr<FontRawInterface> MakePimpl(const std::string &arFontName);
};

}

#endif //FONT_H
