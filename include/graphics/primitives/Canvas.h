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
        : mHeight(0), mWidth(0), mBytesPerPixel(0), mClipRect() {}
    Canvas(GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel)
        : mHeight(aHeight), mWidth(aWidth), mBytesPerPixel(aBytesPerPixel), mClipRect(0, 0, aWidth, aHeight) {}
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
    void DrawArc(const Point &arCenter, GuiUnit_t aRadius1, GuiUnit_t aRadius2, int aStartAngel, int aSweepAngle, const Color &arColor);
    /**
     * \brief Draw a full circle
     *
     * \param aCenter
     * \param aRadius
     * \param aColor
     */
    void DrawCircle(const Point &arCenter, GuiUnit_t aRadius, const Color &arColor);

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
     * \param arLeftTop The destination on the canvas.
     * \param arBitmap The source bitmap to copy from
     * \param aColor Foreground color used if the bitmap is monochrome or alpha.
     */
    void DrawImage(const Point &arLeftTop, const Bitmap &arBitmap, Color aColor = Color::White);

    /**
     * \brief Copies the section part of the bitmap content into the canvas.
     *
     * \param arLeftTop The destination on the canvas.
     * \param arBitmap The source bitmap to copy from
     * \param arSection Rectangular section of the source bitmap
     * \param aColor Foreground color used if the bitmap is monochrome or alpha.
     */
    void DrawImageSection(const Point &arLeftTop, const Bitmap &arBitmap, const Rect &arSection, Color aColor = Color::White);

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
    virtual uint32_t GetPixel(const Point& arPoint, bool aFront) const = 0;

    /**
     * \brief Set the color value of a single pixel.
     *
     * This always writes to the back buffer.
     *
     * \param aPoint
     * \param aColor
     */
    virtual inline void SetPixel(const Point& arPoint, const Color &arColor) = 0;

    /**
     * \brief Get the width of the canvas.
     *
     * \return uint32_t
     */
    GuiUnit_t GetWidth() const
    {
        return mWidth;
    }

    /**
     * \brief Get the height of the canvas.
     *
     * \return uint32_t
     */
    GuiUnit_t GetHeight() const
    {
        return mHeight;
    }

    /**
     * \brief Get the color depth of the canvas.
     *
     * \return uint32_t
     */
    unsigned int GetColorDepth() const
    {
        return mBytesPerPixel;
    }

    /**
     * \brief Checks if coordinates is inside screens boundary
     *
     * \param aPoint
     * \return bool
     */
    inline bool IsInsideCanvas(const Point &arPoint) const
    {
        return mClipRect.IsHit(arPoint);
//        if (arPoint.GetX() >= 0 &&
//            arPoint.GetX() <  mWidth &&
//            arPoint.GetY() >= 0 &&
//            arPoint.GetY() <  mHeight) {
//            return true;
//        }
//        return false;
    }

    Canvas& SetClipRect(const Rect &arClipRect);
    Rect& GetClipRect() { return mClipRect; }


protected:
    GuiUnit_t mHeight;
    GuiUnit_t mWidth;
    unsigned int mBytesPerPixel;
    Rect mClipRect;

    void plot4Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
    {
        SetPixel(Point(aCenterX + aX, aCenterY + aY), arColor);
        SetPixel(Point(aCenterX - aX, aCenterY + aY), arColor);
        SetPixel(Point(aCenterX + aX, aCenterY - aY), arColor);
        SetPixel(Point(aCenterX - aX, aCenterY - aY), arColor);
    }

    void plot8Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
    {
        plot4Points(aCenterX, aCenterY, aX, aY, arColor);
        plot4Points(aCenterX, aCenterY, aY, aX, arColor);
    }
};

} // namespace rsp::graphics
#endif // CANVAS_H
