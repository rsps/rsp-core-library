/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Colour.h"

Colour::Colour(uint8_t aAlpha, uint8_t aRed, uint8_t aGreen, uint8_t aBlue) {
    colour.alpha = aAlpha;
    colour.red = aRed;
    colour.green = aGreen;
    colour.blue = aBlue;
}
Colour::Colour(uint32_t aARGB) {
    colour.argb = aARGB;
    /*colour.alpha = (aARGB & 0xFF);
    colour.blue = (aARGB >> 8) & 0xFF;
    colour.green = (aARGB >> 16) & 0xFF;
    colour.red = (aARGB >> 24) & 0xFF;*/
}
Colour::Colour(const Colour &aColour)
    : colour(aColour.colour) {
}

uint8_t Colour::GetRed() {
    return colour.red;
}
void Colour::SetRed(uint8_t aValue) {
    colour.red = aValue;
}
uint8_t Colour::GetGreen() {
    return colour.green;
}
void Colour::SetGreen(uint8_t aValue) {
    colour.green = aValue;
}
uint8_t Colour::GetBlue() {
    return colour.blue;
}
void Colour::SetBlue(uint8_t aValue) {
    colour.blue = aValue;
}
uint8_t Colour::GetAlpha() {
    return colour.alpha;
}
void Colour::SetAlpha(uint8_t aValue) {
    colour.alpha = aValue;
}
Colour::operator uint32_t() {
    throw NotImplementedException("");
}
void Colour::operator=(const Colour &aColour) {
    throw NotImplementedException("");
}