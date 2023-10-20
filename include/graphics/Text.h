/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_TEXT_H
#define RSP_CORE_LIB_GRAPHICS_TEXT_H

#include <string>
#include <memory>
#include "Canvas.h"
#include "Font.h"
#include "Rect.h"
#include "Style.h"
#include <utils/OptionalPtr.h>

namespace rsp::graphics {

struct Typography;

/**
 * \class Text
 *
 * The Text class contains a string content and various settings that can be applied to the text.
 * The main output is the glyphs needed to paint the text content on a canvas. Those glyphs are
 * generated by the Font member with all settings applied.
 * The object has a lot of getters and setters for text settings, but to speed up execution
 * the rebuild of the glyphs is a manual process.
 * To rebuild the glyphs the Reload method must be called.
 */
class Text : public Canvas
{
public:
    enum class HAlign : uint8_t {
        Left,
        Center,
        Right
    };
    enum class VAlign : uint8_t {
        Top,
        Center,
        Bottom
    };

    /**
     * \brief Constructs an empty text element with the default font.
     */
    Text() : mFont() {}

    /**
     * \brief Constructs an empty text element with the given font.
     *
     * \param arFontName
     */
    explicit Text(const std::string &arFontName);

    /**
     * \brief Constructs a Text object with the given font and content.
     *
     * \param arFontName
     * \param arText
     */
    Text(const std::string &arFontName, const std::string &arText);

    Text(const Text&) = default;
    Text(Text&&) = default;

    Text& operator=(const Text&) = default;
    Text& operator=(Text&&) = default;

    /**
     * \brief Load Text and font attributes from given Typography
     *
     * \param arTypography
     * \return self
     */
    Text& operator<<(const Typography &arTypography);

    explicit operator std::string() const { return mValue; }

    /**
     * Getter for the string content
     *
     * \return std::string
     */
    [[nodiscard]] const std::string& GetValue() const { return mValue; }
    /**
     * Setter for the string content.
     *
     * \param aValue
     * \return Reference to this for fluent calls.
     */
    Text& SetValue(std::string aValue);

    /**
     * \brief Force all text to upper case, useful for headings.
     *
     * \param aUpperCase
     * \return self
     */
    Text& ForceUpperCase(bool aUpperCase = true);

    /**
     * Returns a reference to the internal font object.
     * It must be used to change font properties of the text.
     *
     * \return Reference to Font.
     */
    Font& GetFont() { return mFont; }
    [[nodiscard]] const Font& GetFont() const { return mFont; }

    /**
     * Get the current value of the line spacing.
     *
     * \param aSpacing
     * \return
     */
    [[nodiscard]] int GetLineSpacing() const { return mLineSpacing; }
    /**
     * Set the current line spacing.
     *
     * \param aSpacing
     * \return Reference to this for fluent calls.
     */
    Text& SetLineSpacing(int aSpacing) { mLineSpacing = aSpacing; mDirty = true; return *this; }

    /**
     * Get the number of lines in the text content.
     * Invalidate must have been called before this is valid.
     *
     * \return number of lines in the text content.
     */
    [[nodiscard]] int GetLineCount() const { return mLineCount; }

    /**
     * Get the current vertical alignment setting.
     *
     * \return a VAlign type.
     */
    [[nodiscard]] VAlign GetVAlignment() const { return mVAlign; }
    /**
     * Set the vertical alignment setting.
     *
     * \param aVAlign
     * \return Reference to this for fluent calls.
     */
    Text& SetVAlignment(VAlign aVAlign) { mVAlign = aVAlign; mDirty = true; return *this; }

    /**
     * Get the current horizontal alignment setting.
     *
     * \return a HAlign type.
     */
    [[nodiscard]] HAlign GetHAlignment() const { return mHAlign; }
    /**
     * Set the horizontal alignment setting.
     *
     * \param aHAlign
     * \return Reference to this for fluent calls.
     */
    Text& SetHAlignment(HAlign aHAlign) { mHAlign = aHAlign; mDirty = true; return *this; }

    /**
     * \brief Get the position of this text inside the given area, respecting alignments settings.
     * \param arArea
     * \return Point
     */
    [[nodiscard]] Point GetPosition(const Rect &arArea) const;

    /**
     * \brief Set the font size and mark text for refreshing
     * \param aSizePx
     * \return self
     */
    Text& SetFontSize(int aSizePx) { mFont.SetSize(aSizePx); return *this; }

    /**
     * Reload all glyphs based on the current settings.
     * Optional scale this text to fit the given rect.
     *
     * \param aRect Optional Rect that text should fit inside
     * \return Reference to this for fluent calls.
     */
    Text& Reload(utils::OptionalPtr<const Rect> aRect = nullptr);

    /**
     * \brief Check if the text object has changed and needs glyph reloading
     *
     * \return True if glyphs needs to be reloaded.
     */
    [[nodiscard]] bool IsDirty() const { return mDirty | mFont.IsDirty(); }

    /**
     * Get the minimum bounding rectangle containing all the glyphs.
     *
     * \return Rect
     */
    [[nodiscard]] const Rect& GetBoundingRect() const { return mBoundingRect; }

protected:
    Font mFont;
    Rect mBoundingRect{};
    std::string mValue{};
    std::shared_ptr<Glyphs> mpGlyphs{};
    int mLineCount = 0;
    int mLineMaxChar = 0;
    int mLineSpacing = 1;
    HAlign mHAlign = HAlign::Center;
    VAlign mVAlign = VAlign::Center;
    bool mDirty = false;
    bool mUpperCase = false;

    void scaleToFit(int aWidth, int aHeight);
    void loadGlyphs();
    void draw();
};


struct Typography {
    bool UpperCase = false;
    FontStyles Style = FontStyles::Normal;
    uint32_t Color = Color::Black;
    int Size = 16;
    int Spacing = 1;
    Text::HAlign HAlignment = Text::HAlign::Left;
    Text::VAlign VAlignment = Text::VAlign::Center;
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_TEXT_H
