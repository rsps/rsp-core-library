/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <graphics/Color.h>
#include <graphics/PixelData.h>
#include <graphics/Point.h>
#include <graphics/Rect.h>

namespace rsp::graphics
{

/**
 * Canvas class
 *
 * Class function declarations for low level drawing operations, storage is internal memory.
 */
class Canvas : public PixelData
{
public:
    Canvas() noexcept : PixelData() {}
    Canvas(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth = ColorDepth::RGBA);
//    Canvas(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *aData, size_t aDataSize);

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
     * \brief Copies part of a pixel data buffer into this canvas.
     * \param arLeftTop
     * \param arPixelData
     * \param arSection
     * \param aColor
     */
    void DrawPixelData(const Point &arDestination, const PixelData &arPixelData, const Rect &arSourceRect, Color aColor);
    void DrawPixelData(const Point &arDestination, const PixelData &arPixelData) { DrawPixelData(arDestination, arPixelData, arPixelData.GetRect(), Color::White); }

    PixelData& GetPixelData() { return *this; }
    [[nodiscard]] const PixelData& GetPixelData() const { return *this; }

protected:
    void plot4Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor);
    void plot8Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor);
};

} // namespace rsp::graphics
#endif // CANVAS_H
