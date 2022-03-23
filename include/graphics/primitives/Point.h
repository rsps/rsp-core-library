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
#ifndef POINT_H
#define POINT_H

#include <ostream>

namespace rsp::graphics
{

class Rect;
class Canvas;
class Bitmap;
class Framebuffer;

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
    inline Point()
        : mX(0), mY(0)
    {
    }

    /**
     * \brief Construct a point at aX, aY.
     *
     * \param aX
     * \param aY
     */
    inline Point(int aX, int aY)
        : mX(aX), mY(aY)
    {
    }

    /**
     * \brief Copy constructor
     *
     * \param aPoint
     */
    inline Point(const Point &aPoint)
        : mX(aPoint.mX), mY(aPoint.mY)
    {
    }

    /**
     * \brief Assignment operator.
     *
     * \param aPoint
     * \return self
     */
    inline Point &operator=(const Point &aPoint)
    {
        mX = aPoint.mX;
        mY = aPoint.mY;
        return *this;
    }

    /**
     * \brief Equal to operator
     * \param aPoint
     * \return True if equal
     */
    inline bool operator==(const Point &aPoint)
    {
        return (mX == aPoint.mX) && (mY == aPoint.mY);
    }

    /**
     * \brief Not equal to operator
     * \param aPoint
     * \return True if not equal
     */
    inline bool operator!=(const Point &aPoint)
    {
        return (mX != aPoint.mX) || (mY != aPoint.mY);
    }

    /**
     * \brief Gets the value of the X coordinate
     * \return Integer
     */
    inline int GetX() const
    {
        return mX;
    }

    /**
     * \brief Gets the value of the Y coordinate
     * \return Integer
     */
    inline int GetY() const
    {
        return mY;
    }

  protected:
    // Allow friends to access members for speed optimizations.
    friend Framebuffer;
    friend Canvas;
    friend Bitmap;
    friend Rect;

    int mX;
    int mY;
};

std::ostream &operator<<(std::ostream &os, const Point &p);

} // namespace rsp::graphics
#endif // POINT_H
