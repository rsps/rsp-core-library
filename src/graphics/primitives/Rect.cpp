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

Rect& Rect::operator &=(const Rect &arOther)
{
    Rect r = *this & arOther;
    mLeftTop = r.GetTopLeft();
    mWidth = r.mWidth;
    mHeight = r.mHeight;
    return *this;
}

Rect Rect::operator |(const Rect &arOther) const
{
    Rect r = *this;
    r |= arOther;
    return r;
}

Rect& Rect::operator |=(const Rect &arOther)
{
    mLeftTop.mX = std::min(mLeftTop.mX, arOther.mLeftTop.mX);
    mLeftTop.mY = std::min(mLeftTop.mY, arOther.mLeftTop.mY);
    SetRight(std::max(GetRight(), arOther.GetRight()));
    SetBottom(std::max(GetBottom(), arOther.GetBottom()));
    return *this;
}


GuiUnit_t Rect::GetTop() const
{
    return mLeftTop.mY;
}

Rect& Rect::SetTop(GuiUnit_t aTopValue)
{
    mLeftTop.mY = aTopValue;
    return *this;
}

GuiUnit_t Rect::GetBottom() const
{
    return mLeftTop.mY + mHeight - 1;
}

Rect& Rect::SetBottom(GuiUnit_t aBotValue)
{
    mHeight = aBotValue - mLeftTop.mY + 1;
    LimitDimensions();
    return *this;
}

GuiUnit_t Rect::GetLeft() const
{
    return mLeftTop.mX;
}

Rect& Rect::SetLeft(GuiUnit_t aLeftValue)
{
    mLeftTop.mX = aLeftValue;
    return *this;
}

GuiUnit_t Rect::GetRight() const
{
    return mLeftTop.mX + mWidth - 1;
}

Rect& Rect::SetRight(GuiUnit_t aRightValue)
{
    mWidth = aRightValue - mLeftTop.mX + 1;
    LimitDimensions();
    return *this;
}

GuiUnit_t Rect::GetWidth() const
{
    return mWidth;
}

Rect& Rect::SetWidth(GuiUnit_t aWidth)
{
    mWidth = aWidth;
    LimitDimensions();
    return *this;
}

GuiUnit_t Rect::GetHeight() const
{
    return mHeight;
}

Rect& Rect::SetHeight(GuiUnit_t aHeight)
{
    mHeight = aHeight;
    LimitDimensions();
    return *this;
}

Rect& Rect::MoveTo(const Point &arPoint)
{
    mLeftTop = arPoint;
    return *this;
}

Rect& Rect::Move(int aX, int aY)
{
    mLeftTop.mX += aX;
    mLeftTop.mY += aY;
    return *this;
}

bool Rect::IsHit(const Point &aPoint) const
{
    // Only works with non-rotated rectangles
    if (aPoint.mX >= mLeftTop.mX &&
        aPoint.mX <  (mLeftTop.mX + mWidth) &&
        aPoint.mY >= mLeftTop.mY &&
        aPoint.mY <  (mLeftTop.mY + mHeight)) {
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

bool Rect::empty() const
{
    return (mWidth == 0) || (mHeight == 0);
}

Rect& Rect::SetSize(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    mWidth = aWidth;
    mHeight = aHeight;
    LimitDimensions();
    return *this;
}

Rect& Rect::AddSize(int aWidth, int aHeight)
{
    mWidth += aWidth;
    mHeight += aHeight;
    LimitDimensions();
    return *this;
}

} // namespace rsp::graphics
