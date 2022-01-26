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
#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <utils/CoreException.h>

namespace rsp::graphics {

/**
 * Color class
 *
 * A color consist of tree base colors: Red, green and blue,
 * and an optional alpha channel that determines transparency.
 *
 * Note: Not all hardware supports alpha channel transparency.
 */
class Color
{
public:
    typedef uint32_t RGB_t;

    /**
     * Predefined basic colors
     */
    enum :RGB_t {
        White   = 0xFFFFFF,
        Silver  = 0xC0C0C0,
        Grey    = 0x808080,
        Black   = 0x000000,
        Red     = 0xFF0000,
        Maroon  = 0x800000,
        Yellow  = 0xFFFF00,
        Olive   = 0x808000,
        Lime    = 0x00FF00,
        Green   = 0x008000,
        Aqua    = 0x00FFFF,
        Teal    = 0x008080,
        Blue    = 0x0000FF,
        Navy    = 0x000080,
        Fuchsia = 0xFF00FF,
        Purple  = 0x800080
    };

    /**
     * Construct with given base colors.
     *
     * \param aRed
     * \param aGreen
     * \param aBlue
     * \param aAlpha
     */
    Color(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha);

    /**
     * Construct from ARGB value.
     *
     * \param aARGB
     */
    Color(RGB_t aARGB);

    /**
     * Copy constructor.
     *
     * \param aColor
     */
    Color(const Color &aColor);

    /**
     * Get the red base color value.
     *
     * \return Red value
     */
    uint8_t GetRed() const;
    /**
     * Set the red base color value.
     *
     * \param aValue
     */
    void SetRed(uint8_t aValue);

    /**
     * Get the green base color value.
     *
     * \return Green value
     */
    uint8_t GetGreen() const;
    /**
     * Set the green base color value.
     *
     * \param aValue
     */
    void SetGreen(uint8_t aValue);

    /**
     * Get the blue base color value.
     *
     * \return Blue value
     */
    uint8_t GetBlue() const;
    /**
     * Set the blue base color value.
     *
     * \param aValue
     */
    void SetBlue(uint8_t aValue);

    /**
     * Get the alpha channel value.
     *
     * \return Alpha value
     */
    uint8_t GetAlpha() const;
    /**
     * Set the alpha channel value.
     *
     * \param aValue
     */
    void SetAlpha(uint8_t aValue);

    /**
     * Get the ARGB value.
     * \return ARGB
     */
    operator RGB_t() const;
    /**
     * Assign the value from the given Color object.
     *
     * \param aValue
     */
    Color& operator=(const Color &aColor);

protected:
    /**
     * Color value type
     */
    typedef union
    {
        RGB_t rgba;
    #ifdef LITTLE_ENDIAN
        struct __item_type
        {
            uint32_t alpha :8;
            uint32_t blue :8;
            uint32_t green :8;
            uint32_t red :8;
        } item;
    #else
        struct __item_type {
            uint32_t alpha : 8;
            uint32_t red : 8;
            uint32_t green : 8;
            uint32_t blue : 8;
        } item;
    #endif
    } ColorValue_t;

    ColorValue_t mValue;

};

}
#endif // COLOR_H
