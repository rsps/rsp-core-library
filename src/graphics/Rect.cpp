/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <sstream>
#include <exceptions/CoreException.h>
#include <graphics/Rect.h>

using namespace rsp::exceptions;

namespace rsp::graphics
{

std::ostream& operator <<(std::ostream &aStream, const Rect &arRect)
{
    aStream << "{x: " << arRect.GetLeft() << ", "
        << "y: " << arRect.GetTop() << ", "
        << "w: " << arRect.GetWidth() << ", "
        << "h: " << arRect.GetHeight() << "}";
    return aStream;
}

std::string to_string(const Rect &arRect)
{
    std::stringstream ss;
    ss << arRect;
    return ss.str();
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
      mWidth(aBottomRight.mX - aLeftTop.mX),
      mHeight(aBottomRight.mY - aLeftTop.mY)
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
    if (mLeftTop.mX > (arOther.mLeftTop.mX + arOther.mWidth)) {
        return Rect();
    }
    if ((mLeftTop.mX + mWidth) < arOther.mLeftTop.mX) {
        return Rect();
    }
    if (mLeftTop.mY > (arOther.mLeftTop.mY + arOther.mHeight)) {
        return Rect();
    }
    if ((mLeftTop.mY + mHeight) < arOther.mLeftTop.mY) {
        return Rect();
    }

    int minX = std::max(mLeftTop.mX, arOther.mLeftTop.mX);
    int minY = std::max(mLeftTop.mY, arOther.mLeftTop.mY);
    int maxX = std::min(mLeftTop.mX + mWidth, arOther.mLeftTop.mX + arOther.mWidth);
    int maxY = std::min(mLeftTop.mY + mHeight, arOther.mLeftTop.mY + arOther.mHeight);

    return Rect(minX, minY, maxX - minX, maxY - minY);
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
    int minX = std::min(mLeftTop.mX, arOther.mLeftTop.mX);
    int minY = std::min(mLeftTop.mY, arOther.mLeftTop.mY);
    int maxX = std::max(mLeftTop.mX + mWidth, arOther.mLeftTop.mX + arOther.mWidth);
    int maxY = std::max(mLeftTop.mY + mHeight, arOther.mLeftTop.mY + arOther.mHeight);
    mLeftTop.mX = minX;
    mLeftTop.mY = minY;
    mWidth = maxX - minX;
    mHeight = maxY - minY;
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
    return mLeftTop.mY + mHeight;
}

Rect& Rect::SetBottom(GuiUnit_t aBotValue)
{
    mHeight = aBotValue - mLeftTop.mY;
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
    return mLeftTop.mX + mWidth;
}

Rect& Rect::SetRight(GuiUnit_t aRightValue)
{
    mWidth = aRightValue - mLeftTop.mX;
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

bool Rect::IsHit(GuiUnit_t aX, GuiUnit_t aY) const
{
    // Only works with non-rotated rectangles
    if (aX >= mLeftTop.mX &&
        aX <  (mLeftTop.mX + mWidth) &&
        aY >= mLeftTop.mY &&
        aY <  (mLeftTop.mY + mHeight)) {
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
