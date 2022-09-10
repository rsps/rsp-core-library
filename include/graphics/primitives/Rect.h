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
    Rect() {};

    /**
     * \brief Constructs a Rect from coordinate and size.
     *
     * \param aLeft
     * \param aTop
     * \param aWidth
     * \param aHeight
     */
    Rect(GuiUnit_t aLeft, GuiUnit_t aTop, GuiUnit_t aWidth, GuiUnit_t aHeight);

    /**
     * \brief Construct a Rect from two points.
     *
     * \param aLeftTop
     * \param aRightBottom
     */
    Rect(const Point &arLeftTop, const Point &arRightBottom);

    /**
     * \brief Construct a Rect from origin point and size.
     *
     * \param aLeftTop
     * \param aWidth
     * \param aHeight
     */
    Rect(const Point &arLeftTop, GuiUnit_t aWidth, GuiUnit_t aHeight);

    /**
     * \brief Copy constructor.
     *
     * \param aRect
     */
    Rect(const Rect &aOther) = default;

    /**
     * \brief Assignment operator
     *
     * \param arRect
     * \return
     */
    Rect& operator=(const Rect &arRect) = default;

    /**
     * \brief Check if the rect covers no area
     * \return true if height or width is zero.
     */
    bool empty() const;

    /**
     * \brief Equal to operator
     * \param arRect
     * \return True if equal
     */
    inline bool operator==(const Rect &arRect)
    {
        return !(*this != arRect);
    }

    /**
     * \brief Not equal to operator
     * \param arRect
     * \return True if not equal
     */
    inline bool operator!=(const Rect &arOther)
    {
        return (mLeftTop != arOther.mLeftTop) || (mWidth != arOther.mWidth) || (mHeight != arOther.mHeight);
    }

    /**
     * \brief Get the overlapping area of two rect's.
     * \param arRect
     * \return New Rect with overlapping area of the two
     */
    Rect operator&(const Rect &arOther) const;
    Rect& operator&=(const Rect &arOther);

    /**
     * \brief Get the bounding area of two rect's.
     * \param arRect
     * \return New Rect that contains the areas of the two
     */
    Rect operator|(const Rect &arOther) const;
    Rect& operator|=(const Rect &arOther);

    /**
     * \brief Get the top value
     *
     * \return int
     */
    GuiUnit_t GetTop() const;
    /**
     * \brief Set the top value of the Rect.
     *
     * \param aTopValue
     */
    Rect& SetTop(GuiUnit_t aTopValue);

    /**
     * \brief Get the bottom value
     *
     * \return int
     */
    GuiUnit_t GetBottom() const;
    /**
     * \brief Set the bottom value of the Rect.
     *
     * \param aBotValue
     */
    Rect& SetBottom(GuiUnit_t aBotValue);

    /**
     * \brief Get the left value
     *
     * \return int
     */
    GuiUnit_t GetLeft() const;
    /**
     * \brief Set the left value of the Rect.
     *
     * \param aLeftValue
     */
    Rect& SetLeft(GuiUnit_t aLeftValue);

    /**
     * \brief Get the right value.
     *
     * \return int
     */
    GuiUnit_t GetRight() const;
    /**
     * \brief Set the right value of the Rect.
     *
     * \param aRightValue
     */
    Rect& SetRight(GuiUnit_t aRightValue);

    /**
     * \brief Set TopLeft to given point, move BottomRight to keep size.
     * \param arPoint
     */
    Rect& MoveTo(const Point &arPoint);
    Rect& operator=(const Point &arPosition) { return MoveTo(arPosition); }

    /**
     * \brief Move this rect by the given x/y offsets.
     * \param aX
     * \param aY
     * \return self
     */
    Rect& Move(int aX, int aY);

    /**
     * \brief Set the size of this rect
     * \param aWidth
     * \param aHeight
     * \return self
     */
    Rect& SetSize(GuiUnit_t aWidth, GuiUnit_t aHeight);
    /**
     * \brief Add given values to the size of this rect
     * \param aWidth
     * \param aHeight
     * \return self
     */
    Rect& AddSize(int aWidth, int aHeight);


    /**
     * \brief Get the top left coordinate.
     *
     * \return Point
     */
    Point& GetTopLeft()
    {
        return mLeftTop;
    }
    const Point& GetTopLeft() const
    {
        return mLeftTop;
    }

    /**
     * \brief Get the bottom right coordinate.
     *
     * \return Point
     */
    Point GetBottomRight() const
    {
        return Point(
            std::max(static_cast<int>(mLeftTop.mX), static_cast<int>(mLeftTop.mX + mWidth - 1)),
            std::max(static_cast<int>(mLeftTop.mY), static_cast<int>(mLeftTop.mY + mHeight - 1)));
    }

    /**
     * \brief Get the width of the Rect.
     *
     * \return GuiUnit_t
     */
    GuiUnit_t GetWidth() const;
    /**
     * \brief Set the width of the Rect.
     *
     * \param aWidth
     */
    Rect& SetWidth(GuiUnit_t aWidth);

    /**
     * \brief Get the height of the Rect.
     *
     * \return GuiUnit_t
     */
    GuiUnit_t GetHeight() const;

    /**
     * \brief Set the height of the Rect.
     *
     * \param aHeight
     */
    Rect& SetHeight(GuiUnit_t aHeight);

    /**
     * \brief Determines if a point is inside area covered by the Rect.
     *
     * \param aPoint
     * \return bool
     */
    bool IsHit(const Point &aPoint) const;
    /**
     * \brief Limits the dimensions of the rectangle by checking if height and width are above zero
     *
     * \throws AssertException
     */
    void LimitDimensions();

  protected:
    // Allow friends to access members for speed optimizations.
    friend Canvas;
    friend Framebuffer;

    Point mLeftTop = {0, 0};
    GuiUnit_t mWidth = 0;
    GuiUnit_t mHeight = 0;
};

std::ostream &operator<<(std::ostream &aStream, const Rect &arRect);

} // namespace rsp::graphics

#endif // RECT_H
