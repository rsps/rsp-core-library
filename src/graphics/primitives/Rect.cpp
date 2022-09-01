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
#include <utils/CoreException.h>

using namespace rsp::utils;

namespace rsp::graphics
{

std::ostream& operator <<(std::ostream &aStream, const Rect &arRect)
{
    aStream << "Left: " << arRect.GetLeft() << ", "
        << "Top: " << arRect.GetTop() << ", "
        << "Width: " << arRect.GetWidth() << ", "
        << "Height: " << arRect.GetHeight();
    return aStream;
}

Rect::Rect(int aLeft, int aTop, int aWidth, int aHeight)
    : mLeftTop(aLeft, aTop), mRightBottom(aLeft + aWidth, aTop + aHeight)
{
    VerifyDimensions();
}

Rect::Rect(unsigned int aLeft, unsigned int aTop, unsigned int aWidth, unsigned int aHeight)
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

Rect::Rect(const Rect &arRect)
    : mLeftTop(arRect.mLeftTop), mRightBottom(arRect.mRightBottom)
{
    VerifyDimensions();
}

Rect& Rect::operator=(const Rect &arRect)
{
    if (&arRect != this) {
        mLeftTop = arRect.mLeftTop;
        mRightBottom = arRect.mRightBottom;
    }
    return *this;
}

Rect Rect::operator &(const Rect &arRect)
{
    Point lt = arRect.GetTopLeft();
    Point rb = arRect.GetBottomRight();

    if (mLeftTop.mY > lt.mY) {
        lt.mY = mLeftTop.mY;
    }
    if (mLeftTop.mX > lt.mX) {
        lt.mX = mLeftTop.mX;
    }
    if (mRightBottom.mY < rb.mY) {
        rb.mY = mRightBottom.mY;
    }
    if (mRightBottom.mX < rb.mX) {
        rb.mX = mRightBottom.mX;
    }
    if (    (rb.mY < lt.mY)
        ||  (rb.mX < lt.mX)) {
        rb = lt;
    }
    return Rect(lt, rb);
}

int Rect::GetTop() const
{
    return mLeftTop.mY;
}

void Rect::SetTop(int aTopValue)
{
    ASSERT(aTopValue <= mRightBottom.mY);
    mLeftTop.mY = aTopValue;
}

int Rect::GetBottom() const
{
    return mRightBottom.mY;
}

void Rect::SetBottom(int aBotValue)
{
    ASSERT(aBotValue >= mLeftTop.mY);
    mRightBottom.mY = aBotValue;
}

int Rect::GetLeft() const
{
    return mLeftTop.mX;
}

void Rect::SetLeft(int aLeftValue)
{
    ASSERT(aLeftValue <= mRightBottom.mX);
    mLeftTop.mX = aLeftValue;
}

int Rect::GetRight() const
{
    return mRightBottom.mX;
}

void Rect::SetRight(int aRightValue)
{
    ASSERT(aRightValue >= mLeftTop.mX);
    mRightBottom.mX = aRightValue;
}

int Rect::GetWidth() const
{
    return mRightBottom.mX - mLeftTop.mX;
}

void Rect::SetWidth(int aWidth)
{
    if (aWidth >= 0) {
        mRightBottom.mX = mLeftTop.mX + aWidth;
    }
    else {
        mLeftTop.mX = mRightBottom.mX + aWidth;
    }
}

int Rect::GetHeight() const
{
    return mRightBottom.mY - mLeftTop.mY;
}

void Rect::SetHeight(int aHeight)
{
    if (aHeight >= 0) {
        mRightBottom.mY = mLeftTop.mY + aHeight;
    }
    else {
        mLeftTop.mY = mRightBottom.mY + aHeight;
    }
}

void Rect::MoveTo(const Point &arPoint)
{
    auto w = GetWidth();
    auto h = GetHeight();
    GetTopLeft() = arPoint;
    SetWidth(w);
    SetHeight(h);
}

bool Rect::IsHit(const Point &aPoint) const
{
    // Only works with non-rotated rectangles
    if (aPoint.GetX() >= mLeftTop.GetX() &&
        aPoint.GetX() < mRightBottom.GetX() &&
        aPoint.GetY() >= mLeftTop.GetY() &&
        aPoint.GetY() < mRightBottom.GetY())
        return true;

    return false;
}

void Rect::VerifyDimensions()
{
    if (    (mRightBottom.mY < mLeftTop.mY)
        ||  (mRightBottom.mX < mLeftTop.mX)) {
        THROW_WITH_BACKTRACE1(rsp::utils::AssertException, "Rect size is negative");
    }
}

Rect Rect::operator +(const Point &arOffset)
{
    auto r = *this;
    r.MoveTo(arOffset);
    return r;
}

} // namespace rsp::graphics
