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

#include "Point.h"

namespace rsp::graphics
{

class Canvas;
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
    Rect() : mLeftTop(0, 0), mRightBottom(0, 0){};
    /**
     * \brief Constructs a Rect from coordinate and size.
     *
     * \param aLeft
     * \param aTop
     * \param aWidth
     * \param aHeight
     */
    Rect(int aLeft, int aTop, int aWidth, int aHeight);

    /**
     * \brief Construct a Rect from two points.
     *
     * \param aLeftTop
     * \param aRightBottom
     */
    Rect(const Point &aLeftTop, const Point &aRightBottom);

    /**
     * \brief Construct a Rect from origin point and size.
     *
     * \param aLeftTop
     * \param aWidth
     * \param aHeight
     */
    Rect(const Point &aLeftTop, int aWidth, int aHeight);

    /**
     * \brief Copy constructor.
     *
     * \param aRect
     */
    Rect(const Rect &aRect);

    /**
     * \brief Assignment operator
     *
     * \param arRect
     * \return
     */
    Rect &operator=(const Rect &arRect);

    /**
     * \brief Get the top value
     *
     * \return int
     */
    int GetTop() const;
    /**
     * \brief Set the top value of the Rect.
     *
     * \param aTopValue
     */
    void SetTop(int aTopValue);

    /**
     * \brief Get the bottom value
     *
     * \return int
     */
    int GetBottom() const;
    /**
     * \brief Set the bottom value of the Rect.
     *
     * \param aBotValue
     */
    void SetBottom(int aBotValue);

    /**
     * \brief Get the left value
     *
     * \return int
     */
    int GetLeft() const;
    /**
     * \brief Set the left value of the Rect.
     *
     * \param aLeftValue
     */
    void SetLeft(int aLeftValue);

    /**
     * \brief Get the right value.
     *
     * \return int
     */
    int GetRight() const;
    /**
     * \brief Set the right value of the Rect.
     *
     * \param aRightValue
     */
    void SetRight(int aRightValue);

    /**
     * \brief Get the top left coordinate.
     *
     * \return Point
     */
    const Point &GetTopLeft() const
    {
        return mLeftTop;
    }

    /**
     * \brief Get the bottom right coordinate.
     *
     * \return Point
     */
    const Point &GetBottomRight() const
    {
        return mRightBottom;
    }

    /**
     * \brief Get the width of the Rect.
     *
     * \return int
     */
    int GetWidth() const;
    /**
     * \brief Set the width of the Rect.
     *
     * \param aWidth
     */
    void SetWidth(int aWidth);

    /**
     * \brief Get the height of the Rect.
     *
     * \return int
     */
    int GetHeight() const;
    /**
     * \brief Set the height of the Rect.
     *
     * \param aHeight
     */
    void SetHeight(int aHeight);

    /**
     * \brief Determines if a point is inside area covered by the Rect.
     *
     * \param aPoint
     * \return bool
     */
    bool IsHit(const Point &aPoint) const;
    /**
     * \brief Verifies the dimensions of the rectangle by checking if height and width are above zero
     *
     * \return True of height and width are above zero
     */
    bool VerifyDimensions() const;

  protected:
    // Allow friends to access members for speed optimizations.
    friend Canvas;
    friend Framebuffer;

    Point mLeftTop;
    Point mRightBottom;
};

std::ostream &operator<<(std::ostream &aStream, const Rect &arRect);

} // namespace rsp::graphics

#endif // RECT_H
