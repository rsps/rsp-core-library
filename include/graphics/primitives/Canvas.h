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
#ifndef CANVAS_H
#define CANVAS_H

#include "Color.h"
#include "Point.h"
#include "Rect.h"
#include "Text.h"

namespace rsp::graphics
{
/**
 * Canvas interface class
 *
 * Abstract class with function declarations for low level drawing operations.
 */
class Bitmap;

class Canvas
{
  public:
    Canvas()
        : mHeight(0), mWidth(0), mBytesPerPixel(0) {}
    Canvas(int aHeight, int aWidth, int aBytesPerPixel)
        : mHeight(aHeight), mWidth(aWidth), mBytesPerPixel(aBytesPerPixel) {}
    /**
     * \brief Virtual destructor for the abstract class.
     */
    virtual ~Canvas()
    {
    }

    /**
     * \brief Draw a a full or partial eclipse
     *
     * \param aCenter
     * \param aRadius1
     * \param aRadius2
     * \param aStartAngel
     * \param aSweepAngle
     * \param aColor
     */
    void DrawArc(const Point &arCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Color &arColor);
    /**
     * \brief Draw a full circle
     *
     * \param aCenter
     * \param aRadius
     * \param aColor
     */
    void DrawCircle(const Point &aCenter, int aRadius, const Color &aColor);

    /**
     * \brief Draw a straight line from A to B.
     *
     * \param aA
     * \param aB
     * \param aColor
     */
    void DrawLine(const Point &arA, const Point &arB, const Color &arColor);

    /**
     * \brief Draw a rectangle
     *
     * The rectangle is not rotated in any way, but simply a box with horizontal and vertical sides.
     * If aFilled is set, the entire rectangle is filled with given color.
     *
     * \param aRect
     * \param aColor
     * \param aFilled
     */
    void DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled = false);

    /**
     * \brief Copies the bitmap content into the canvas.
     *
     * \param LeftTop
     * \param aBitmap
     */
    void DrawImage(const Point &arLeftTop, const Bitmap &arBitmap);

    /**
     * \brief Draws the given Text object in the given color on the canvas.
     *
     * \param aRect
     * \param arColor
     */
    void DrawText(const Text &arText, const Color &arColor);

    /**
     * \brief Draws the given Text object in the color of the objects own font.
     *
     * \param arText
     */
    void DrawText(Text &arText);

    /**
     * \brief Get the color value of a single pixel.
     *
     * It defaults to read the pixel value from the backbuffer.
     *
     * \param Point aPoint
     * \param bool aFront Set to read pixel from frontbuffer
     */
    virtual uint32_t GetPixel(const Point &, const bool) const = 0;

    /**
     * \brief Set the color value of a single pixel.
     *
     * This always writes to the back buffer.
     *
     * \param aPoint
     * \param aColor
     */
    virtual inline void SetPixel(const Point &, const Color) = 0;

    /**
     * \brief Get the width of the canvas.
     *
     * \return uint32_t
     */
    int GetWidth() const
    {
        return mWidth;
    }

    /**
     * \brief Get the height of the canvas.
     *
     * \return uint32_t
     */
    int GetHeight() const
    {
        return mHeight;
    }

    /**
     * \brief Get the color depth of the canvas.
     *
     * \return uint32_t
     */
    int GetColorDepth() const
    {
        return mBytesPerPixel;
    }

    /**
     * \brief Checks if coordinates is inside screens boundary
     *
     * \param aPoint
     * \return bool
     */
    inline bool IsInsideScreen(const Point &arPoint) const
    {
        return !(arPoint.mX < 0 || arPoint.mY < 0 || arPoint.mY >= mHeight || arPoint.mX >= mWidth);
    }

  protected:
    int mHeight;
    int mWidth;
    int mBytesPerPixel;

    inline void plot4Points(int aCenterX, int aCenterY, int aX, int aY, const Color &arColor)
    {
        SetPixel(Point(aCenterX + aX, aCenterY + aY), arColor);
        SetPixel(Point(aCenterX - aX, aCenterY + aY), arColor);
        SetPixel(Point(aCenterX + aX, aCenterY - aY), arColor);
        SetPixel(Point(aCenterX - aX, aCenterY - aY), arColor);
    }

    inline void plot8Points(int aCenterX, int aCenterY, int aX, int aY, const Color &arColor)
    {
        plot4Points(aCenterX, aCenterY, aX, aY, arColor);
        plot4Points(aCenterX, aCenterY, aY, aX, arColor);
    }
};

} // namespace rsp::graphics
#endif // CANVAS_H
