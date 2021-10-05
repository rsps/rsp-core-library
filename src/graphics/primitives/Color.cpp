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
    mValue.red = aRed;
    mValue.green = aGreen;
    mValue.blue = aBlue;
    mValue.alpha = aAlpha;
}

Color::Color(uint32_t aRGBA)
{
#ifdef LITTLE_ENDIAN
    mValue.rgba = aRGBA;
#else
    mValue.alpha = (aRGBA & 0xFF);
    mValue.blue = (aRGBA >> 8) & 0xFF;
    mValue.green = (aRGBA >> 16) & 0xFF;
    mValue.red = (aRGBA >> 24) & 0xFF;
#endif
}

Color::Color(const Color &aColor)
    : mValue(aColor.mValue)
{
}

uint8_t Color::GetRed() const
{
    return mValue.red;
}

void Color::SetRed(uint8_t aValue)
{
    mValue.red = aValue;
}

uint8_t Color::GetGreen() const
{
    return mValue.green;
}

void Color::SetGreen(uint8_t aValue)
{
    mValue.green = aValue;
}

uint8_t Color::GetBlue() const
{
    return mValue.blue;
}

void Color::SetBlue(uint8_t aValue)
{
    mValue.blue = aValue;
}

uint8_t Color::GetAlpha() const
{
    return mValue.alpha;
}

void Color::SetAlpha(uint8_t aValue)
{
    mValue.alpha = aValue;
}

Color::operator uint32_t() const
{
#ifdef LITTLE_ENDIAN
    return mValue.rgba;
#else
    return (((uint32_t)mValue.red) << 24) |
           (((uint32_t)mValue.green) << 16) |
           (((uint32_t)mValue.blue) << 8) |
           ((uint32_t)mValue.alpha);
#endif
}

Color &Color::operator=(const Color &aColor)
{
    mValue = aColor.mValue;
    return *this;
}

}
