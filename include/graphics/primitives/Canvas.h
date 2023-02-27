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
#include "PixelData.h"

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
    virtual void DrawArc(const Point &arCenter, GuiUnit_t aRadius1, GuiUnit_t aRadius2, int aStartAngel, int aSweepAngle, const Color &arColor) = 0;
    /**
     * \brief Draw a full circle
     *
     * \param aCenter
     * \param aRadius
     * \param aColor
     */
    virtual void DrawCircle(const Point &arCenter, GuiUnit_t aRadius, const Color &arColor) = 0;

    /**
     * \brief Draw a straight line from A to B.
     *
     * \param aA
     * \param aB
     * \param aColor
     */
    virtual void DrawLine(const Point &arA, const Point &arB, const Color &arColor) = 0;

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
    virtual void DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled = false) = 0;

    /**
     * \brief Copies the bitmap content into the canvas.
     *
     * \param arLeftTop The destination on the canvas.
     * \param arBitmap The source bitmap to copy from
     * \param aColor Foreground color used if the bitmap is monochrome or alpha.
     */
    virtual void DrawImage(const Point &arLeftTop, const Bitmap &arBitmap, Color aColor = Color::White) = 0;

    /**
     * \brief Copies the section part of the bitmap content into the canvas.
     *
     * \param arLeftTop The destination on the canvas.
     * \param arBitmap The source bitmap to copy from
     * \param arSection Rectangular section of the source bitmap
     * \param aColor Foreground color used if the bitmap is monochrome or alpha.
     */
    virtual void DrawImageSection(const Point &arLeftTop, const Bitmap &arBitmap, const Rect &arSection, const Color &arColor = Color::White) = 0;

    /**
     * \brief Copies part of a pixel data buffer into this canvas.
     * \param arLeftTop
     * \param arPixelData
     * \param arSection
     * \param aColor
     */
    virtual void DrawPixelData(const Point &arLeftTop, const PixelData &arPixelData, const Rect &arSection, Color aColor) = 0;

    /**
     * \brief Draws the given Text object in the given color on the canvas.
     *
     * \param aRect
     * \param arColor
     */
    virtual void DrawText(const Text &arText, Color aColor, Color aBackColor = Color::None) = 0;

    /**
     * \brief Draws the given Text object in the color of the objects own font.
     *
     * \param arText
     */
//    virtual void DrawText(Text &arText) = 0;

    /**
     * \brief Get the color value of a single pixel.
     *
     * It defaults to read the pixel value from the backbuffer.
     *
     * \param Point aPoint
     * \param bool aFront Set to read pixel from frontbuffer
     */
    virtual uint32_t GetPixel(const Point& arPoint, bool aFront) const = 0;

    /**
     * \brief Set the color value of a single pixel.
     *
     * This always writes to the back buffer.
     *
     * \param aPoint
     * \param aColor
     */
    virtual void SetPixel(const Point& arPoint, const Color &arColor) = 0;

    /**
     * \brief Get the width of the canvas.
     *
     * \return uint32_t
     */
    virtual GuiUnit_t GetWidth() const = 0;

    /**
     * \brief Get the height of the canvas.
     *
     * \return uint32_t
     */
    virtual GuiUnit_t GetHeight() const = 0;

    /**
     * \brief Get the color depth of the canvas.
     *
     * \return uint32_t
     */
    virtual std::uint32_t GetColorDepth() const = 0;

    /**
     * \brief Check if point is inside this canvas
     * \param arPoint
     * \return True if point is inside
     */
    virtual bool IsHit(const Point &arPoint) const = 0;

    /**
     * \brief Set the clipping rect for this canvas
     * \param arClipRect
     * \return self
     */
    virtual Canvas& SetClipRect(const Rect &arClipRect) = 0;

    /**
     * \brief Get the clipping rect for this canvas
     * \return Rect
     */
    virtual Rect& GetClipRect() = 0;//{ return mClipRect; }

};

} // namespace rsp::graphics
#endif // CANVAS_H
