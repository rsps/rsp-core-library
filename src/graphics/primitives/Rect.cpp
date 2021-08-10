/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Rect.h"

Rect::Rect(int aLeft, int aTop, int aWidth, int aHeight)
    : LeftTop(aLeft, aTop), RightBottom(aLeft + aWidth, aTop + aHeight) {
}
Rect::Rect(const Point &aLeftTop, const Point &aBottomRight)
    : LeftTop(aLeftTop), RightBottom(aBottomRight) {
}
Rect::Rect(const Point &aLeftTop, int aWidth, int aHeight)
    : LeftTop(aLeftTop), RightBottom(aLeftTop.x + aWidth, aLeftTop.y + aHeight) {
}
Rect::Rect(const Rect &aRect)
    : LeftTop(aRect.LeftTop), RightBottom(aRect.RightBottom) {
}

int Rect::GetTop() {
    return LeftTop.y;
}

void Rect::SetTop(int aValue) {
    if (aValue > RightBottom.y) {
        LeftTop.y = RightBottom.y;
        RightBottom.y = aValue;
    } else {
        LeftTop.y = aValue;
    }
}

int Rect::GetBottom() {
    return RightBottom.y;
}

void Rect::SetBottom(int aValue) {
    if (aValue < LeftTop.y) {
        RightBottom.y = LeftTop.y;
        LeftTop.y = aValue;
    } else {
        RightBottom.y = aValue;
    }
}

int Rect::GetLeft() {
    return LeftTop.x;
}

void Rect::SetLeft(int aValue) {
    if (aValue > RightBottom.x) {
        LeftTop.x = RightBottom.x;
        RightBottom.x = aValue;
    } else {
        LeftTop.x = aValue;
    }
}

int Rect::GetRight() {
    return RightBottom.x;
}

void Rect::SetRight(int aValue) {
    if (aValue < LeftTop.x) {
        RightBottom.x = LeftTop.x;
        LeftTop.x = aValue;
    } else {
        RightBottom.x = aValue;
    }
}

int Rect::GetWidth() {
    return RightBottom.x - LeftTop.x;
}

void Rect::SetWidth(int aValue) {
    if (aValue < 0) {
        RightBottom.x = LeftTop.x;
        LeftTop.x += aValue;
    } else {
        RightBottom.x = LeftTop.x + aValue;
    }
}

int Rect::GetHeight() {
    return RightBottom.y - LeftTop.y;
}

void Rect::SetHeight(int aValue) {
    if (aValue < 0) {
        RightBottom.y = LeftTop.y;
        LeftTop.y += aValue;
    } else {
        RightBottom.y = LeftTop.y + aValue;
    }
}

bool Rect::IsHit(const Point &aPoint) {
    throw NotImplementedException("IsHit is not yet implemeted");
}