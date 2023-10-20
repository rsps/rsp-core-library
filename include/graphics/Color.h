/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_COLOR_H
#define RSP_CORE_LIB_GRAPHICS_COLOR_H

#include <exceptions/CoreException.h>
#include <cstdint>

namespace rsp::graphics
{

/**
 * \class Color
 *
 * A color consist of tree base colors: Red, green and blue,
 * and an optional alpha channel that determines transparency.
 *
 * Note: Not all hardware supports alpha channel transparency.
 */
class Color
{
public:
    using ARGB_t = uint32_t;

/**
 * \brief Predefined basic colors
 */
    enum BasicColors : uint32_t {
        None    = 0,
        White   = 0xFFFFFFFF,
        Silver  = 0xFFC0C0C0,
        Grey    = 0xFF808080,
        Black   = 0xFF000000,
        Red     = 0xFFFF0000,
        Maroon  = 0xFF800000,
        Yellow  = 0xFFFFFF00,
        Olive   = 0xFF808000,
        Lime    = 0xFF00FF00,
        Green   = 0xFF008000,
        Aqua    = 0xFF00FFFF,
        Teal    = 0xFF008080,
        Blue    = 0xFF0000FF,
        Navy    = 0xFF000080,
        Fuchsia = 0xFFFF00FF,
        Purple  = 0xFF800080,
    };

    Color() noexcept : mValue{} {}

    /**
     * \brief Construct with given base colors.
     *
     * \param aRed
     * \param aGreen
     * \param aBlue
     * \param aAlpha
     */
    Color(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha);

    /**
     * \brief Construct from ARGB value. This is a conversion constructor.
     *
     * \param aARGB
     */
    Color(ARGB_t aARGB); // NOLINT

    /**
     * \brief Copy constructor.
     *
     * \param aColor
     */
    Color(const Color &arColor) = default;

    /**
     * \brief Move constructor.
     *
     * \param aColor
     */
    Color(Color &&arColor) noexcept = default;

    /**
     * \brief Get the red base color value.
     *
     * \return Red value
     */
    [[nodiscard]] uint8_t GetRed() const;
    /**
     * \brief Set the red base color value.
     *
     * \param aValue
     */
    void SetRed(uint8_t aValue);

    /**
     * \brief Get the green base color value.
     *
     * \return Green value
     */
    [[nodiscard]] uint8_t GetGreen() const;
    /**
     * \brief Set the green base color value.
     *
     * \param aValue
     */
    void SetGreen(uint8_t aValue);

    /**
     * \brief Get the blue base color value.
     *
     * \return Blue value
     */
    [[nodiscard]] uint8_t GetBlue() const;
    /**
     * \brief Set the blue base color value.
     *
     * \param aValue
     */
    void SetBlue(uint8_t aValue);

    /**
     * \brief Get the alpha channel value.
     *
     * \return Alpha value
     */
    [[nodiscard]] uint8_t GetAlpha() const;
    /**
     * \brief Set the alpha channel value.
     *
     * \param aValue
     */
    void SetAlpha(uint8_t aValue);

    /**
     * \brief Get the ARGB value. This is a conversion function.
     * \return ARGB
     */
    operator uint32_t() const; // NOLINT
    [[nodiscard]] uint32_t AsUint() const { return static_cast<ARGB_t>(*this); }

    /**
     * \brief For fast color value in native 32-bit format
     *
     * \return uint32
     */

    [[nodiscard]] uint32_t AsRaw() const { return mValue.rgba; }
    Color& FromRaw(uint32_t aValue) { mValue.rgba = aValue; return *this; }

    /**
     * \brief Assignment operator.
     *
     * \param aValue
     */
    Color& operator=(const Color &arColor);

    /**
     * \brief Move operator.
     *
     * \param aValue
     */
    Color& operator=(Color &&arColor) noexcept;

    Color& operator=(ARGB_t aValue);

    bool operator!=(ARGB_t aValue) const;
    bool operator!=(const Color &arOther) const;

    bool operator==(ARGB_t aValue) const;
    bool operator==(const Color &arOther) const;


    /**
     * \fn Color Blend(Color&, Color&)
     * \brief Perform alpha blending of the two given colors.
     *
     * \param a
     * \param b
     * \return Combined color
     */
    static Color Blend(const Color &a, const Color &b);

  protected:
    struct color_components {
        uint32_t red : 8;
        uint32_t green : 8;
        uint32_t blue : 8;
        uint32_t alpha : 8;
    };

    /**
     * \brief Color value type
     */
    union ColorValue_t {
        uint32_t rgba;
        color_components item;
    };

    ColorValue_t mValue{};
};

} // namespace rsp::graphics
#endif // RSP_CORE_LIB_GRAPHICS_COLOR_H
