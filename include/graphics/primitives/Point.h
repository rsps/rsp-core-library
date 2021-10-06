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

namespace rsp::graphics
{

class Rect;
class Framebuffer;

/**
 * Point class
 *
 * A point is a 2D coordinate (x,y)
 */
class Point
{
public:
    /**
     * Construct an empty point
     */
    inline Point()
        : mX(0), mY(0)
    {
    }

    /**
     * Construct a point at aX, aY.
     *
     * \param aX
     * \param aY
     */
    inline Point(int aX, int aY)
        : mX(aX), mY(aY)
    {
    }

    /**
     * Copy constructor
     *
     * \param aPoint
     */
    inline Point(const Point &aPoint)
        : mX(aPoint.mX), mY(aPoint.mY)
    {
    }

    /**
     * Assignment operator.
     *
     * \param aPoint
     * \return self
     */
    inline Point& operator=(const Point &aPoint)
    {
        mX = aPoint.mX;
        mY = aPoint.mY;
        return *this;
    }

    inline int GetX() const {
        return mX;
    }

    inline int GetY() const {
        return mY;
    }

protected:
    // Allow friends to access members for speed optimizations.
    friend Framebuffer;
    friend Rect;

    int mX;
    int mY;
};

}
#endif // POINT_H
