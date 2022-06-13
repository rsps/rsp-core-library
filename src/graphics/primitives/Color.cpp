/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Color.h>

namespace rsp::graphics {

Color::Color(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha)
{
    mValue.item.red = aRed;
    mValue.item.green = aGreen;
    mValue.item.blue = aBlue;
    mValue.item.alpha = aAlpha;
}

Color::Color(ARGB_t aRGBA)
{
#ifdef LITTLE_ENDIAN
    mValue.rgba = aRGBA;
#else
    mValue.item.alpha = (aRGBA & 0xFF);
    mValue.item.blue = (aRGBA >> 8) & 0xFF;
    mValue.item.green = (aRGBA >> 16) & 0xFF;
    mValue.item.red = (aRGBA >> 24) & 0xFF;
#endif
}

Color::Color(const Color &aColor)
    : mValue(aColor.mValue)
{
}

uint8_t Color::GetRed() const
{
    return mValue.item.red;
}

void Color::SetRed(uint8_t aValue)
{
    mValue.item.red = aValue;
}

uint8_t Color::GetGreen() const
{
    return mValue.item.green;
}

void Color::SetGreen(uint8_t aValue)
{
    mValue.item.green = aValue;
}

uint8_t Color::GetBlue() const
{
    return mValue.item.blue;
}

void Color::SetBlue(uint8_t aValue)
{
    mValue.item.blue = aValue;
}

uint8_t Color::GetAlpha() const
{
    return mValue.item.alpha;
}

void Color::SetAlpha(uint8_t aValue)
{
    mValue.item.alpha = aValue;
}

Color::operator Color::ARGB_t() const
{
#ifdef LITTLE_ENDIAN
    return mValue.rgba;
#else
    std::err << "UPPPSSSSS" << std::endl;
    return (((ARGB_t)mValue.item.red) << 24) |
           (((ARGB_t)mValue.item.green) << 16) |
           (((ARGB_t)mValue.item.blue) << 8) |
           ((ARGB_t)mValue.item.alpha);
#endif
}

Color &Color::operator=(const Color &arColor)
{
    if (this != &arColor) {
        mValue = arColor.mValue;
    }
    return *this;
}

Color Color::Blend(Color a, Color b)
{
    std::uint32_t alpha_b = b.GetAlpha();
    if (alpha_b == 255) {
        return b;
    }

    Color result(b);
    result.SetAlpha(255);

    std::uint32_t alpha_a = a.GetAlpha();
    switch(a) {
        case 0:
            return result;
            break;

        case 255:
            result.SetRed(static_cast<std::uint8_t>( (a.GetRed() + ((alpha_b * b.GetRed()) / 255)) / 2));
            result.SetGreen(static_cast<std::uint8_t>( (a.GetGreen() + ((alpha_b * b.GetGreen()) / 255)) / 2));
            result.SetBlue(static_cast<std::uint8_t>( (a.GetBlue() + ((alpha_b * b.GetBlue()) / 255)) / 2));
            break;

        default:
            result.SetRed(static_cast<std::uint8_t>( (((alpha_a * a.GetRed()) / 255) + ((alpha_b * b.GetRed()) / 255)) / 2));
            result.SetGreen(static_cast<std::uint8_t>( (((alpha_a * a.GetGreen()) / 255) + ((alpha_b * b.GetGreen()) / 255)) / 2));
            result.SetBlue(static_cast<std::uint8_t>( (((alpha_a * a.GetBlue()) / 255) + ((alpha_b * b.GetBlue()) / 255)) / 2));
            break;
    }

    return result;
}

Pixel AlphaBlendPixels(Pixel p1, Pixel p2)
{
    static const int AMASK = 0xFF000000;
    static const int RBMASK = 0x00FF00FF;
    static const int GMASK = 0x0000FF00;
    static const int AGMASK = AMASK | GMASK;
    static const int ONEALPHA = 0x01000000;
    unsigned int a = (p2 & AMASK) >> 24;
    unsigned int na = 255 - a;
    unsigned int rb = ((na * (p1 & RBMASK)) + (a * (p2 & RBMASK))) >> 8;
    unsigned int ag = (na * ((p1 & AGMASK) >> 8)) + (a * (ONEALPHA | ((p2 & GMASK) >> 8)));
    return ((rb & RBMASK) | (ag & AGMASK));
}

}
