/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_

#include <string>
#include "Font.h"
#include "Rect.h"

namespace rsp::graphics {

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
class Text
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
    Text() : mFont(), mArea() {}

    /**
     * \brief Constructs an empty text element with the given font.
     *
     * \param arFontName
     */
    Text(const std::string &arFontName);

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
     * Simple destructor.
     */
    virtual ~Text() {};


    operator std::string() const { return mValue; }

    /**
     * Getter for the string content
     *
     * \return std::string
     */
    const std::string& GetValue() const { return mValue; }
    /**
     * Setter for the string content.
     *
     * \param arValue
     * \return Reference to this for fluent calls.
     */
    Text& SetValue(const std::string &arValue) { mValue = arValue; return *this; }

    /**
     * Getter for the area where the text is drawn on a canvas.
     *
     * \return
     */
    const Rect& GetArea() const { return mArea; }
    /**
     * Setter for the text area.
     *
     * \param arRect
     * \return Reference to this for fluent calls.
     */
    Text& SetArea(const Rect &arRect) { mArea = arRect; return *this; }

    /**
     * Returns a reference to the internal font object.
     * It must be used to change font properties of the text.
     *
     * \return Reference to Font.
     */
    Font& GetFont() { return mFont; }

    /**
     * Get the current value of the ScaleToFit member.
     *
     * \return bool
     */
    bool GetScaleToFit() const { return mScaleToFit; }
    /**
     * Set the current ScaleToFit member. If set the text is scaled to fit the Area of the Text object
     *
     * \param aValue
     * \return Reference to this for fluent calls.
     */
    Text& SetScaleToFit(bool aValue = true) { mScaleToFit = aValue; return *this; }

    /**
     * Get the current value of the line spacing.
     *
     * \param aSpacing
     * \return
     */
    int GetLineSpacing() const { return mLineSpacing; }
    /**
     * Set the current line spacing.
     *
     * \param aSpacing
     * \return Reference to this for fluent calls.
     */
    Text& SetLineSpacing(int aSpacing) { mLineSpacing = aSpacing; return *this; }

    /**
     * Get the number of lines in the text content.
     * Invalidate must have been called before this is valid.
     *
     * \return number of lines in the text content.
     */
    int GetLineCount() const { return mLineCount; }

    /**
     * Get the current vertical alignment setting.
     *
     * \return a VAlign type.
     */
    VAlign GetVAlignment() { return mVAlign; }
    /**
     * Set the vertical alignment setting.
     *
     * \param aVAlign
     * \return Reference to this for fluent calls.
     */
    Text& SetVAlignment(VAlign aVAlign) { mVAlign = aVAlign; return *this; }

    /**
     * Get the current horizontal alignment setting.
     *
     * \return a HAlign type.
     */
    HAlign GetHAlignment() { return mHAlign; }
    /**
     * Set the horizontal alignment setting.
     *
     * \param aHAlign
     * \return Reference to this for fluent calls.
     */
    Text& SetHAlignment(HAlign aHAlign) { mHAlign = aHAlign; return *this; }

    /**
     * Reload all glyphs based on the current settings.
     *
     * \return Reference to this for fluent calls.
     */
    Text& Reload();

    /**
     * Get a reference to the glyphs for the current text.
     *
     * \return Vector of Glyph objects.
     */
    Glyphs& GetGlyphs() const { return *mpGlyphs; }

    /**
     * Calculate the minimum bounding rectangle containing all the glyphs.
     *
     * \param apGlyphs
     * \return Rect
     */
    Rect CalcBoundingRect(std::unique_ptr<Glyphs>& apGlyphs) const;

protected:
    Font mFont;
    Rect mArea;
    std::string mValue{};
    bool mScaleToFit = false;
    std::unique_ptr<Glyphs> mpGlyphs{};
    int mLineCount = 0;
    int mLineMaxChar = 0;
    int mLineSpacing = 1;
    HAlign mHAlign = HAlign::Center;
    VAlign mVAlign = VAlign::Center;

    void scaleToFit();
    void loadGlyphs();
    void alignGlyphs();
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_TEXT_H_ */
