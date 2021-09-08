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

#include <cassert>

Rect::Rect(int aLeft, int aTop, int aWidth, int aHeight)
    : LeftTop(aLeft, aTop), RightBottom(aLeft + aWidth, aTop + aHeight) {
    VerifyDimensions();
}
Rect::Rect(const Point &aLeftTop, const Point &aBottomRight)
    : LeftTop(aLeftTop), RightBottom(aBottomRight) {
    VerifyDimensions();
}
Rect::Rect(const Point &aLeftTop, int aWidth, int aHeight)
    : LeftTop(aLeftTop), RightBottom(aLeftTop.x + aWidth, aLeftTop.y + aHeight) {
    VerifyDimensions();
}
Rect::Rect(const Rect &aRect)
    : LeftTop(aRect.LeftTop), RightBottom(aRect.RightBottom) {
    VerifyDimensions();
}

int Rect::GetTop() {
    return LeftTop.y;
}

void Rect::SetTop(int aTopValue) {
    ASSERT(aTopValue > RightBottom.y);
    LeftTop.y = aTopValue;
}

int Rect::GetBottom() {
    return RightBottom.y;
}

void Rect::SetBottom(int aBotValue) {
    ASSERT(aBotValue < LeftTop.y);
    RightBottom.y = aBotValue;
}

int Rect::GetLeft() {
    return LeftTop.x;
}

void Rect::SetLeft(int aLeftValue) {
    ASSERT(aLeftValue > RightBottom.x);
    LeftTop.x = aLeftValue;
}

int Rect::GetRight() {
    return RightBottom.x;
}

void Rect::SetRight(int aRightValue) {
    ASSERT(aRightValue < LeftTop.x);
    RightBottom.x = aRightValue;
}

int Rect::GetWidth() {
    return RightBottom.x - LeftTop.x;
}

void Rect::SetWidth(int aWidth) {
    ASSERT(aWidth < 0);
    RightBottom.x = LeftTop.x + aWidth;
}

int Rect::GetHeight() {
    return RightBottom.y - LeftTop.y;
}

void Rect::SetHeight(int aHeight) {
    ASSERT(aHeight < 0);
    RightBottom.y = LeftTop.y + aHeight;
}

bool Rect::IsHit(const Point &aPoint) {
    throw NotImplementedException("IsHit is not yet implemeted");
}
bool Rect::VerifyDimensions() {
    ASSERT(GetWidth() < 0);
    ASSERT(GetHeight() < 0);
}