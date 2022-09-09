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

Rect::Rect(GuiUnit_t aLeft, GuiUnit_t aTop, GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mLeftTop(aLeft, aTop),
      mWidth(aWidth),
      mHeight(aHeight)
{
    LimitDimensions();
}

Rect::Rect(const Point &aLeftTop, const Point &aBottomRight)
    : mLeftTop(aLeftTop),
      mWidth(aBottomRight.mX - aLeftTop.mX + 1),
      mHeight(aBottomRight.mY - aLeftTop.mY + 1)
{
    LimitDimensions();
}

Rect::Rect(const Point &aLeftTop, GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mLeftTop(aLeftTop),
      mWidth(aWidth),
      mHeight(aHeight)
{
    LimitDimensions();
}


Rect Rect::operator &(const Rect &arOther) const
{
    Point lt = arOther.GetTopLeft();
    Point rb = arOther.GetBottomRight();
    Point right_bottom = GetBottomRight();

    if (mLeftTop.mY > lt.mY) {
        lt.mY = mLeftTop.mY;
    }
    if (mLeftTop.mX > lt.mX) {
        lt.mX = mLeftTop.mX;
    }
    if (right_bottom.mY < rb.mY) {
        rb.mY = right_bottom.mY;
    }
    if (right_bottom.mX < rb.mX) {
        rb.mX = right_bottom.mX;
    }
    if (    (rb.mY < lt.mY)
        ||  (rb.mX < lt.mX)) {
        rb = lt;
    }
    return Rect(lt, rb);
}

Rect& Rect::operator &=(const Rect &arRect)
{
    Rect r = *this & arRect;
    mLeftTop = r.GetTopLeft();
    mWidth = r.mWidth;
    mHeight = r.mHeight;
    return *this;
}

GuiUnit_t Rect::GetTop() const
{
    return mLeftTop.mY;
}

void Rect::SetTop(GuiUnit_t aTopValue)
{
    mLeftTop.mY = aTopValue;
}

GuiUnit_t Rect::GetBottom() const
{
    return mLeftTop.mY + mHeight - 1;
}

void Rect::SetBottom(GuiUnit_t aBotValue)
{
    mHeight = aBotValue - mLeftTop.mY + 1;
    LimitDimensions();
}

GuiUnit_t Rect::GetLeft() const
{
    return mLeftTop.mX;
}

void Rect::SetLeft(GuiUnit_t aLeftValue)
{
    mLeftTop.mX = aLeftValue;
}

GuiUnit_t Rect::GetRight() const
{
    return mLeftTop.mX + mWidth - 1;
}

void Rect::SetRight(GuiUnit_t aRightValue)
{
    mWidth = aRightValue - mLeftTop.mX + 1;
    LimitDimensions();
}

GuiUnit_t Rect::GetWidth() const
{
    return mWidth;
}

void Rect::SetWidth(GuiUnit_t aWidth)
{
    mWidth = aWidth;
    LimitDimensions();
}

GuiUnit_t Rect::GetHeight() const
{
    return mHeight;
}

void Rect::SetHeight(GuiUnit_t aHeight)
{
    mHeight = aHeight;
    LimitDimensions();
}

void Rect::MoveTo(const Point &arPoint)
{
    mLeftTop = arPoint;
}

bool Rect::IsHit(const Point &aPoint) const
{
    // Only works with non-rotated rectangles
    if (aPoint.GetX() >= mLeftTop.GetX() &&
        aPoint.GetX() <  (mLeftTop.GetX() + mWidth) &&
        aPoint.GetY() >= mLeftTop.GetY() &&
        aPoint.GetY() <  (mLeftTop.GetY() + mHeight)) {
        return true;
    }

    return false;
}

void Rect::LimitDimensions()
{
    if (static_cast<int>(mWidth) < 0) {
        mWidth = 0;
    }
    if (static_cast<int>(mHeight) < 0) {
        mHeight = 0;
    }
}

Rect Rect::operator +(const Point &arOffset) const
{
    auto r = *this;
    r.mLeftTop += arOffset;
    return r;
}

Rect Rect::operator +(int aValue)
{
    auto r = *this;
    r += aValue;
    return r;
}

Rect& Rect::operator +=(int aValue)
{
    mLeftTop.mX -= aValue;
    mLeftTop.mY -= aValue;
    mWidth += aValue + aValue;
    mHeight += aValue + aValue;
    LimitDimensions();
    return *this;
}

bool Rect::empty() const
{
    return (mWidth == 0) || (mHeight == 0);
}

} // namespace rsp::graphics
