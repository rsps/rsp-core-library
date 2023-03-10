/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/Color.h>
#include <logging/Logger.h>

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

Color::Color(const Color &arColor)
    : mValue(arColor.mValue)
{
}

Color::Color(const Color &&arColor)
    : mValue(std::move(arColor.mValue))
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

Color& Color::operator =(const Color &&arColor)
{
    if (this != &arColor) {
        mValue = std::move(arColor.mValue);
    }
    return *this;
}

/*
 * \see http://gimpchat.com/viewtopic.php?f=8&t=8405
 * \see https://en.wikipedia.org/wiki/Blend_modes
 * \see GIMP mode: GIMP_LAYER_MODE_NORMAL
 */
Color Color::Blend(Color aBg, Color aFg)
{
    uint32_t a = aFg >> 24;

    // If source pixel is fully transparent, just return the background
    if (0 == a) {
      return aBg;
    }

    // If source pixel is not transparent, just return the foreground
    if (255 == a) {
        return aFg;
    }

    uint32_t rb = ((((aFg & 0x00ff00ff) * a) + 0x00007F007F) + ((aBg & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
    uint32_t g  = ((((aFg & 0x0000ff00) * a) + 0x00007f00) + ((aBg & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
    uint32_t result = 0xff000000 | ((rb | g) >> 8);

    return result;
}

}
