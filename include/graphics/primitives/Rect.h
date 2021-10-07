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
#ifndef RECT_H
#define RECT_H

#include <utils/RSPCoreExceptions.h>
#include "Point.h"

namespace rsp::graphics
{

class Framebuffer;

/**
 * Rect class
 *
 * A Rect is made from two 2D coordinates (LeftTop and RightBottom Points).
 * It can also be expressed as one point of origin with width and height properties.
 */
class Rect
{
public:
    /**
     * Constructs a Rect from coordinate and size.
     *
     * \param aLeft
     * \param aTop
     * \param aWidth
     * \param aHeight
     */
    Rect(int aLeft, int aTop, int aWidth, int aHeight);

    /**
     * Construct a Rect from two points.
     *
     * \param aLeftTop
     * \param aRightBottom
     */
    Rect(const Point &aLeftTop, const Point &aRightBottom);

    /**
     * Construct a Rect from origin point and size.
     *
     * \param aLeftTop
     * \param aWidth
     * \param aHeight
     */
    Rect(const Point &aLeftTop, int aWidth, int aHeight);

    /**
     * Copy constructor.
     *
     * \param aRect
     */
    Rect(const Rect &aRect);

    /**
     * Get the top value
     *
     * \return int
     */
    int GetTop() const;
    /**
     * Set the top value of the Rect.
     *
     * \param aTopValue
     */
    void SetTop(int aTopValue);

    /**
     * Get the bottom value
     *
     * \return int
     */
    int GetBottom() const;
    /**
     * Set the bottom value of the Rect.
     *
     * \param aBotValue
     */
    void SetBottom(int aBotValue);

    /**
     * Get the left value
     *
     * \return int
     */
    int GetLeft() const;
    /**
     * Set the left value of the Rect.
     *
     * \param aLeftValue
     */
    void SetLeft(int aLeftValue);

    /**
     * Get the right value.
     *
     * \return int
     */
    int GetRight() const;
    /**
     * Set the right value of the Rect.
     *
     * \param aRightValue
     */
    void SetRight(int aRightValue);

    /**
     * Get the top left coordinate.
     *
     * \return Point
     */
    const Point& GetTopLeft() const {
        return mLeftTop;
    }

    /**
     * Get the bottom right coordinate.
     *
     * \return Point
     */
    const Point& GetBottomRight() const {
        return mRightBottom;
    }

    /**
     * Get the width of the Rect.
     *
     * \return int
     */
    int GetWidth() const;
    /**
     * Set the width of the Rect.
     *
     * \param aWidth
     */
    void SetWidth(int aWidth);

    /**
     * Get the height of the Rect.
     *
     * \return int
     */
    int GetHeight() const;
    /**
     * Set the height of the Rect.
     *
     * \param aHeight
     */
    void SetHeight(int aHeight);

    /**
     * Determines if a point is inside area covered by the Rect.
     *
     * \param aPoint
     * \return bool
     */
    bool IsHit(const Point &aPoint) const;


    bool VerifyDimensions() const;

protected:
    // Allow friends to access members for speed optimizations.
    friend Framebuffer;

    Point mLeftTop;
    Point mRightBottom;
};

}
#endif // RECT_H
