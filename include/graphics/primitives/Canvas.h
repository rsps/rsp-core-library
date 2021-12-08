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

#include "Bitmap.h"
#include "Color.h"
#include "Font.h"
#include "Pen.h"
//#include "Point.h"
#include "Rect.h"

namespace rsp::graphics
{

/**
 * Canvas interface class
 *
 * Abstract class with function declarations for low level drawing operations.
 */
class Canvas
{
  public:
    /**
     * Enumerated SwapOperations
     *
     * In case of double buffer support, these values can control
     * the content of the new buffer:
     *  NoOp:  No initialization of the buffer is performed
     *  Copy:  The current view content is copied int new buffer
     *  Clear: The new buffer is filled with the background color.
     */
    enum class SwapOperations {
        NoOp,
        Copy,
        Clear
    };

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
     * \param aPen
     */
    virtual void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen) = 0;
    /**
     * Draw a full circle
     *
     * \param aCenter
     * \param aRadius
     * \param aPen
     */
    virtual void DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen) = 0;

    /**
     * Draw a straight line from A to B.
     *
     * \param aA
     * \param aB
     * \param aPen
     */
    virtual void DrawLine(const Point &aA, const Point &aB, const Pen &aPen) = 0;

    /**
     * Draw a rectangle
     *
     * The rectangle is not rotated in any way, but simply a box with horizontal and vertical sides.
     *
     * \param aRect
     * \param aPen
     */
    virtual void DrawRectangle(const Rect &aRect, const Pen &aPen) = 0;

    /**
     * Copies the bitmap content into the canvas.
     *
     * \param LeftTop
     * \param aBitmap
     */
    virtual void DrawImage(const Point &LeftTop, const Bitmap &aBitmap) = 0;

    /**
     * Draws the given text within the given rectangle on the canvas.
     *
     * \param aRect
     * \param aFont
     * \param apText
     * \param aScaleToFit
     */
    virtual void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit) = 0;

    /**
     * Get the color value of a single pixel.
     *
     * It defaults to read the pixel value from the backbuffer.
     *
     * \param Point aPoint
     * \param bool aFront Set to read pixel from frontbuffer
     */
    virtual uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const = 0;

    /**
     * Set the color value of a single pixel.
     *
     * This always writes to the back buffer.
     *
     * \param aPoint
     * \param aColor
     */
    virtual void SetPixel(const Point &aPoint, const Color aColor) = 0;

    /**
     * Swap the view port.
     *
     * For systems without double buffer capabilities, the Copy operation will
     * result in a NoOp.
     *
     * \param aSwapOp
     */
    virtual void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy) = 0;

    /**
     * Get the width of the canvas.
     *
     * \return uint32_t
     */
    virtual uint32_t GetWidth() const = 0;

    /**
     * Get the height of the canvas.
     *
     * \return uint32_t
     */
    virtual uint32_t GetHeight() const = 0;

    /**
     * Get the color depth of the canvas.
     *
     * \return uint32_t
     */
    virtual uint32_t GetColorDepth() const = 0;
};

} // namespace rsp::graphics
#endif // CANVAS_H
