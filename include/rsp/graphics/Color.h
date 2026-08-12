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

#include <bit>
#include <cstdint>

namespace rsp::graphics {

/**
 * \class Color
 *
 * A Color consists of three base colors: Red, green, and blue,
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
    enum BasicColors : ARGB_t {
        None = 0,
        White = 0xFFFFFFFF,
        Silver = 0xFFC0C0C0,
        Grey = 0xFF808080,
        Black = 0xFF000000,
        Red = 0xFFFF0000,
        Maroon = 0xFF800000,
        Yellow = 0xFFFFFF00,
        Olive = 0xFF808000,
        Lime = 0xFF00FF00,
        Green = 0xFF008000,
        Aqua = 0xFF00FFFF,
        Teal = 0xFF008080,
        Blue = 0xFF0000FF,
        Navy = 0xFF000080,
        Fuchsia = 0xFFFF00FF,
        Purple = 0xFF800080,
    };

    constexpr Color() noexcept = default;

    /**
     * \brief Construct with given base colors.
     *
     * \param aRed
     * \param aGreen
     * \param aBlue
     * \param aAlpha
     */
    constexpr Color(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha) noexcept
        : _rgba{aRed, aGreen, aBlue, aAlpha}
    {
    }

    /**
     * \brief Construct from ARGB value. This is a conversion constructor.
     *
     * \param aARGB
     */
    constexpr Color(ARGB_t aARGB) noexcept
        : _rgba{static_cast<uint8_t>((aARGB >> 16) & 0xFF),
                  static_cast<uint8_t>((aARGB >> 8) & 0xFF),
                  static_cast<uint8_t>(aARGB & 0xFF),
                  static_cast<uint8_t>((aARGB >> 24) & 0xFF)}
    {
    }

    /**
     * \brief Get the red base color value.
     *
     * \return Red value
     */
    [[nodiscard]] constexpr uint8_t GetRed() const noexcept
    {
        return _rgba.red;
    }

    /**
     * \brief Set the red base color value.
     *
     * \param aValue
     * \return self
     */
    constexpr Color& SetRed(uint8_t aValue)
    {
        _rgba.red = aValue;
        return *this;
    }

    /**
     * \brief Get the green base color value.
     *
     * \return Green value
     */
    [[nodiscard]] constexpr uint8_t GetGreen() const noexcept
    {
        return _rgba.green;
    }

    /**
     * \brief Set the green base color value.
     *
     * \param aValue
     * \return self
     */
    constexpr Color& SetGreen(uint8_t aValue) noexcept
    {
        _rgba.green = aValue;
        return *this;
    }

    /**
     * \brief Get the blue base color value.
     *
     * \return Blue value
     */
    [[nodiscard]] constexpr uint8_t GetBlue() const noexcept
    {
        return _rgba.blue;
    }

    /**
     * \brief Set the blue base color value.
     *
     * \param aValue
     * \return self
     */
    constexpr Color& SetBlue(uint8_t aValue) noexcept
    {
        _rgba.blue = aValue;
        return *this;
    }

    /**
     * \brief Get the alpha channel value.
     *
     * \return Alpha value
     */
    [[nodiscard]] constexpr uint8_t GetAlpha() const noexcept
    {
        return _rgba.alpha;
    }

    /**
     * \brief Set the alpha channel value.
     *
     * \param aValue
     * \return self
     */
    constexpr Color& SetAlpha(uint8_t aValue) noexcept
    {
        _rgba.alpha = aValue;
        return *this;
    }

    /**
     * \brief Get the ARGB value. This is a conversion function.
     * \return ARGB
     */
    [[nodiscard]] constexpr ARGB_t AsUint() const noexcept
    {
        return uint32_t{_rgba.alpha} << 24 | uint32_t{_rgba.red} << 16 | uint32_t{_rgba.green} << 8 | uint32_t{_rgba.blue};
    }

    /**
     * \brief For fast color value in native 32-bit RGBA format
     *
     * \return uint32
     */

    [[nodiscard]] constexpr uint32_t AsRaw() const noexcept
    {
        return std::bit_cast<uint32_t>(_rgba);
    }

    /**
     * @brief Set color from native 32-bit RGBA format
     *
     * @param aValue
     * @return constexpr Color&
     */
    constexpr Color& FromRaw(uint32_t aValue) noexcept
    {
        _rgba = std::bit_cast<Components>(aValue);
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Color& aColor) const noexcept
    {
        return std::bit_cast<uint32_t>(_rgba) == std::bit_cast<uint32_t>(aColor._rgba);
    }

    [[nodiscard]] constexpr bool operator==(const ARGB_t& aARGB) const noexcept
    {
        return AsUint() == aARGB;
    }

    /**
     * \fn Color Blend(Color&, Color&)
     * \brief Perform alpha blending of the two given colors.
     *
     * \param a
     * \param b
     * \return Combined color
     */
    [[nodiscard]] static constexpr Color Blend(const Color& arBg, const Color& arFg) noexcept
    {
        uint32_t fg = arFg.AsUint();
        uint32_t bg = arBg.AsUint();
        uint32_t a = fg >> 24;

        // If source pixel is fully transparent, just return the background
        if (0 == a) {
            return arBg;
        }

        // If source pixel is not transparent, just return the foreground
        if (255 == a) {
            return arFg;
        }

        uint32_t rb = ((((fg & 0x00ff00ff) * a) + 0x00007F007F) + ((bg & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
        uint32_t g = ((((fg & 0x0000ff00) * a) + 0x00007f00) + ((bg & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
        uint32_t result = 0xff000000 | ((rb | g) >> 8);

        return Color{result};
    }

private:
    struct Components
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    };

    Components _rgba{};
};

} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_COLOR_H
