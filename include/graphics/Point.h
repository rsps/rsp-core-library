/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_POINT_H
#define RSP_CORE_LIB_GRAPHICS_POINT_H

#include <graphics/GuiUnit.h>
#include <ostream>

namespace rsp::graphics
{

class Rect;
class Canvas;
class Bitmap;
class PixelData;

/**
 * \brief Point class
 *
 * A point is a 2D coordinate (x,y)
 */
class Point
{
  public:
    /**
     * \brief Construct an empty point
     */
    Point() noexcept
        : mX(0), mY(0)
    {
    }

    /**
     * \brief Construct a point at aX, aY.
     *
     * \param aX
     * \param aY
     */
    Point(int aX, int aY) noexcept
        : mX(static_cast<GuiUnit_t>(aX)), mY(static_cast<GuiUnit_t>(aY))
    {
    }

    Point(unsigned int aX, unsigned int aY) noexcept
        : mX(static_cast<GuiUnit_t>(aX)), mY(static_cast<GuiUnit_t>(aY))
    {
    }

    /**
     * \brief Copy constructor
     *
     * \param aPoint
     */
    Point(const Point &) = default;
    Point(Point &&) = default;

    /**
     * \brief Assignment operator.
     *
     * \param aPoint
     * \return self
     */
    Point &operator=(const Point&) = default;
    Point &operator=(Point&&) = default;

    /**
     * \brief Add the values from other point to this
     *
     * \param arPoint
     * \return self
     */
    Point& operator+=(const Point &arPoint) noexcept
    {
        mX += arPoint.mX;
        mY += arPoint.mY;
        return *this;
    }

    Point operator+(const Point &arPoint) const noexcept
    {
        Point result(mX, mY);
        result += arPoint;
        return result;
    }

    Point operator-(const Point &arPoint) const noexcept
    {
        Point result(mX, mY);
        result -= arPoint;
        return result;
    }

    /**
     * \brief Subtract the values from other point from this
     *
     * \param arPoint
     * \return self
     */
    Point& operator-=(const Point &arPoint) noexcept
    {
        mX -= arPoint.mX;
        mY -= arPoint.mY;
        return *this;
    }

    /**
     * \brief Equal to operator
     * \param aPoint
     * \return True if equal
     */
    bool operator==(const Point &aPoint) const noexcept
    {
        return (mX == aPoint.mX) && (mY == aPoint.mY);
    }

    /**
     * \brief Not equal to operator
     * \param aPoint
     * \return True if not equal
     */
    bool operator!=(const Point &aPoint) const noexcept
    {
        return (mX != aPoint.mX) || (mY != aPoint.mY);
    }

    /**
     * \brief Gets the value of the X coordinate
     * \return Integer
     */
    [[nodiscard]] GuiUnit_t GetX() const noexcept
    {
        return mX;
    }

    /**
     * \brief Set the value of the X coordinate
     * \param aValue
     */
    void SetX(GuiUnit_t aValue) noexcept
    {
        mX = aValue;
    }

    /**
     * \brief Gets the value of the Y coordinate
     * \return Integer
     */
    [[nodiscard]] GuiUnit_t GetY() const noexcept
    {
        return mY;
    }

    /**
     * \brief Set the value of the Y coordinate
     * \param aValue
     */
    void SetY(GuiUnit_t aValue) noexcept
    {
        mY = aValue;
    }

    [[nodiscard]] int Distance(const Point &arOther) const;

  protected:
    // Allow friends to access members for speed optimizations.
    friend PixelData;
    friend Canvas;
    friend Bitmap;
    friend Rect;

    GuiUnit_t mX;
    GuiUnit_t mY;
};

std::ostream &operator<<(std::ostream &os, const Point &p);

} // namespace rsp::graphics
#endif // RSP_CORE_LIB_GRAPHICS_POINT_H
