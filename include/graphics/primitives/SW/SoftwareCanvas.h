/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_SW_SOFTWARECANVAS_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_SW_SOFTWARECANVAS_H_

#include <graphics/primitives/Canvas.h>

namespace rsp::graphics {

class SoftwareCanvas: public Canvas
{
public:
    SoftwareCanvas()
        : mHeight(0), mWidth(0), mBytesPerPixel(0), mClipRect()
    {
    }

    SoftwareCanvas(GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel)
        : mHeight(aHeight), mWidth(aWidth), mBytesPerPixel(aBytesPerPixel), mClipRect(0, 0, aWidth, aHeight)
    {
    }

    void DrawArc(const Point &arCenter, GuiUnit_t aRadius1, GuiUnit_t aRadius2, int aStartAngel, int aSweepAngle, const Color &arColor) override;
    void DrawCircle(const Point &arCenter, GuiUnit_t aRadius, const Color &arColor) override;
    void DrawImage(const Point &arLeftTop, const Bitmap &arBitmap, Color aColor) override;
    void DrawImageSection(const Point &arLeftTop, const Bitmap &arBitmap, const Rect &arSection, const Color &arColor) override;
    void DrawLine(const Point &arA, const Point &arB, const Color &arColor) override;
    void DrawPixelData(const Point &arLeftTop, const PixelData &arPixelData, const Rect &arSection, Color aColor) override;
    void DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled) override;
    void DrawText(const Text &arText, Color aColor, Color aBackColor) override;
    void DrawText(Text &arText);

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;
    std::uint32_t GetColorDepth() const override;

    Rect& GetClipRect() override;
    Canvas& SetClipRect(const Rect &arClipRect) override;
    bool IsHit(const Point &arPoint) const override;

protected:
    GuiUnit_t mHeight;
    GuiUnit_t mWidth;
    std::uint32_t mBytesPerPixel;
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

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_SW_SOFTWARECANVAS_H_ */
