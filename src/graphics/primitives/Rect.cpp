/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Rect.h>
#include <utils/RSPCoreExceptions.h>

Rect::Rect(int aLeft, int aTop, int aWidth, int aHeight)
    : mLeftTop(aLeft, aTop), mRightBottom(aLeft + aWidth, aTop + aHeight)
{
    VerifyDimensions();
}

Rect::Rect(const Point &aLeftTop, const Point &aBottomRight)
    : mLeftTop(aLeftTop), mRightBottom(aBottomRight)
{
    VerifyDimensions();
}

Rect::Rect(const Point &aLeftTop, int aWidth, int aHeight)
    : mLeftTop(aLeftTop), mRightBottom(aLeftTop.mX + aWidth, aLeftTop.mY + aHeight)
{
    VerifyDimensions();
}

Rect::Rect(const Rect &aRect)
    : mLeftTop(aRect.mLeftTop), mRightBottom(aRect.mRightBottom)
{
    VerifyDimensions();
}

int Rect::GetTop()
{
    return mLeftTop.mY;
}

void Rect::SetTop(int aTopValue)
{
    ASSERT(aTopValue > mRightBottom.mY);
    mLeftTop.mY = aTopValue;
}

int Rect::GetBottom()
{
    return mRightBottom.mY;
}

void Rect::SetBottom(int aBotValue)
{
    ASSERT(aBotValue < mLeftTop.mY);
    mRightBottom.mY = aBotValue;
}

int Rect::GetLeft()
{
    return mLeftTop.mX;
}

void Rect::SetLeft(int aLeftValue)
{
    ASSERT(aLeftValue > mRightBottom.mX);
    mLeftTop.mX = aLeftValue;
}

int Rect::GetRight()
{
    return mRightBottom.mX;
}

void Rect::SetRight(int aRightValue)
{
    ASSERT(aRightValue < mLeftTop.mX);
    mRightBottom.mX = aRightValue;
}

int Rect::GetWidth()
{
    return mRightBottom.mX - mLeftTop.mX;
}

void Rect::SetWidth(int aWidth)
{
    ASSERT(aWidth < 0);
    mRightBottom.mX = mLeftTop.mX + aWidth;
}

int Rect::GetHeight()
{
    return mRightBottom.mY - mLeftTop.mY;
}

void Rect::SetHeight(int aHeight)
{
    ASSERT(aHeight < 0);
    mRightBottom.mY = mLeftTop.mY + aHeight;
}

bool Rect::IsHit(const Point &aPoint)
{
    throw NotImplementedException("IsHit is not yet implemeted");

    return false;
}

bool Rect::VerifyDimensions()
{
    ASSERT(GetWidth() < 0);
    ASSERT(GetHeight() < 0);

    return false;
}
