/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Colour.h>

Colour::Colour(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha) {
    colours.red = aRed;
    colours.green = aGreen;
    colours.blue = aBlue;
    colours.alpha = aAlpha;
}
Colour::Colour(uint32_t aRGBA) {
#ifdef LITTLE_ENDIAN
    colours.rgba = aRGBA;
#else
    colour.alpha = (aRGBA & 0xFF);
    colour.blue = (aRGBA >> 8) & 0xFF;
    colour.green = (aRGBA >> 16) & 0xFF;
    colour.red = (aRGBA >> 24) & 0xFF;
#endif
}
Colour::Colour(const Colour &aColour)
    : colours(aColour.colours) {
}

uint8_t Colour::GetRed() const {
    return colours.red;
}
void Colour::SetRed(uint8_t aValue) {
    colours.red = aValue;
}
uint8_t Colour::GetGreen() const {
    return colours.green;
}
void Colour::SetGreen(uint8_t aValue) {
    colours.green = aValue;
}
uint8_t Colour::GetBlue() const {
    return colours.blue;
}
void Colour::SetBlue(uint8_t aValue) {
    colours.blue = aValue;
}
uint8_t Colour::GetAlpha() const {
    return colours.alpha;
}
void Colour::SetAlpha(uint8_t aValue) {
    colours.alpha = aValue;
}
Colour::operator uint32_t() const {
#ifdef LITTLE_ENDIAN
    return colours.rgba;
#else
    return (((uint32_t)colours.red) << 24) |
           (((uint32_t)colours.green) << 16) |
           (((uint32_t)colours.blue) << 8) |
           ((uint32_t)colours.alpha);
#endif
}
Colour &Colour::operator=(const Colour &aColour) {
    colours = aColour.colours;
    return *this;
}
