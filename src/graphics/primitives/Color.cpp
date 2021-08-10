/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Color.h"

Color::Color(uint8_t aAlpha, uint8_t aRed, uint8_t aGreen, uint8_t aBlue) {
    color.alpha = aAlpha;
    color.red = aRed;
    color.green = aGreen;
    color.blue = aBlue;
}
Color::Color(uint32_t aARGB) {
    color.argb = aARGB;
    /*color.alpha = (aARGB & 0xFF);
    color.blue = (aARGB >> 8) & 0xFF;
    color.green = (aARGB >> 16) & 0xFF;
    color.red = (aARGB >> 24) & 0xFF;*/
}
Color::Color(const Color &aColor)
    : color(aColor.color) {
}

uint8_t Color::GetRed() {
    return color.red;
}
void Color::SetRed(uint8_t aValue) {
    color.red = aValue;
}
uint8_t Color::GetGreen() {
    return color.green;
}
void Color::SetGreen(uint8_t aValue) {
    color.green = aValue;
}
uint8_t Color::GetBlue() {
    return color.blue;
}
void Color::SetBlue(uint8_t aValue) {
    color.blue = aValue;
}
uint8_t Color::GetAlpha() {
    return color.alpha;
}
void Color::SetAlpha(uint8_t aValue) {
    color.alpha = aValue;
}
Color::operator uint32_t() {
    throw NotImplementedException("");
}
void Color::operator=(const Color &aColor) {
    throw NotImplementedException("");
}