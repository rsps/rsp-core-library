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
#include "Font.h"
#include "Point.h"
#include "Rect.h"

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
     * Virtual destructor for the abstract class.
     */
    virtual ~Canvas()
    {
    }

    /**
     * Draw a a full or partial eclipse
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
     * Draw a full circle
     *
     * \param aCenter
     * \param aRadius
     * \param aColor
     */
    void DrawCircle(const Point &aCenter, int aRadius, const Color &aColor);

    /**
     * Draw a straight line from A to B.
     *
     * \param aA
     * \param aB
     * \param aColor
     */
    void DrawLine(const Point &arA, const Point &arB, const Color &arColor);

    /**
     * Draw a rectangle
     *
     * The rectangle is not rotated in any way, but simply a box with horizontal and vertical sides.
     *
     * \param aRect
     * \param aColor
     */
    void DrawRectangle(const Rect &arRect, const Color &arColor);

    /**
     * Copies the bitmap content into the canvas.
     *
     * \param LeftTop
     * \param aBitmap
     */
    void DrawImage(const Point &arLeftTop, const Bitmap &arBitmap);

    /**
     * Draws the given text within the given rectangle on the canvas.
     *
     * \param aRect
     * \param aFont
     * \param apText
     * \param aScaleToFit
     */
    void DrawText(const Rect &arRect, const Font &arFont, const char *apText, bool aScaleToFit);

    /**
     * Get the color value of a single pixel.
     *
     * It defaults to read the pixel value from the backbuffer.
     *
     * \param Point aPoint
     * \param bool aFront Set to read pixel from frontbuffer
     */
    virtual uint32_t GetPixel(const Point &, const bool) const = 0;

    /**
     * Set the color value of a single pixel.
     *
     * This always writes to the back buffer.
     *
     * \param aPoint
     * \param aColor
     */
    virtual inline void SetPixel(const Point &, const Color) = 0;

    /**
     * Get the width of the canvas.
     *
     * \return uint32_t
     */
    uint32_t GetWidth() const
    {
        // return mVariableInfo.xres;
        return mWidth;
    }

    /**
     * Get the height of the canvas.
     *
     * \return uint32_t
     */
    uint32_t GetHeight() const
    {
        // return mVariableInfo.yres;
        return mHeight;
    }

    /**
     * Get the color depth of the canvas.
     *
     * \return uint32_t
     */
    uint32_t GetColorDepth() const
    {
        // return mVariableInfo.bits_per_pixel;
        return mBytesPerPixel;
    }

    /**
     * Checks if coordinates is inside screens boundary
     *
     * \param aPoint
     * \return bool
     */
    inline bool IsInsideScreen(const Point &arPoint) const
    {
        // return !(aPoint.mX < 0 || aPoint.mY < 0 || static_cast<uint32_t>(aPoint.mY) >= mVariableInfo.yres || static_cast<uint32_t>(aPoint.mX) >= mVariableInfo.xres);
        return !(arPoint.mX < 0 || arPoint.mY < 0 || static_cast<uint32_t>(arPoint.mY) >= mHeight || static_cast<uint32_t>(arPoint.mX) >= mWidth);
    }

  protected:
    uint32_t mHeight;
    uint32_t mWidth;
    uint32_t mBytesPerPixel;

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
