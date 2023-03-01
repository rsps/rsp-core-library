/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/Canvas.h>

namespace rsp::graphics
{

void Canvas::DrawArc(const Point &arCenter, GuiUnit_t aRadius1, GuiUnit_t aRadius2,
    int aStartAngel, int aSweepAngle, const Color &arColor)
{
    THROW_WITH_BACKTRACE1(rsp::utils::NotImplementedException, "Canvas::DrawArc has not been implemented yet.");
}

void Canvas::DrawCircle(const Point &arCenter, GuiUnit_t aRadius, const Color &arColor)
{
    int error = -static_cast<int>(aRadius);
    GuiUnit_t y = 0;

    while (aRadius >= y) {
        plot8Points(arCenter.mX, arCenter.mY, aRadius, y, arColor);
        error += static_cast<int>(y);
        y++;
        error += static_cast<int>(y);

        if (error >= 0) {
            error += -static_cast<int>(aRadius);
            aRadius--;
            error += -static_cast<int>(aRadius);
        }
    }
}

void Canvas::DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled)
{
    Rect r = arRect & mClipRect;
    GuiUnit_t h_end = r.mLeftTop.mY + r.mHeight;
    GuiUnit_t w_end = r.mLeftTop.mX + r.mWidth;
    if (aFilled) {
        for (GuiUnit_t y = r.mLeftTop.mY; y < h_end; y++) {
            for (GuiUnit_t x = r.mLeftTop.mX; x < w_end; x++) {
                mPixelData.SetPixelAt(x, y, arColor);
            }
        }
    }
    else {
        Point rb = r.GetBottomRight();
        for (GuiUnit_t x = r.mLeftTop.mX; x < w_end; x++) {
            mPixelData.SetPixelAt(x, r.mLeftTop.mY, arColor); // top
            mPixelData.SetPixelAt(x, rb.mY-1, arColor); // bottom
        }
        for (GuiUnit_t y = r.mLeftTop.mY; y < h_end; y++) {
            mPixelData.SetPixelAt(r.mLeftTop.mX, y, arColor); // left
            mPixelData.SetPixelAt(rb.mX-1, y, arColor); // right
        }
    }
}

void Canvas::DrawLine(const Point &arA, const Point &arB, const Color &arColor)
{
    int deltaX = static_cast<int>(arB.mX - arA.mX);
    int deltaY = static_cast<int>(arB.mY - arA.mY);
    int absDeltaX = abs(deltaX);
    int absDeltaY = abs(deltaY);
    int signumX = (deltaX > 0) ? 1 : -1;
    int signumY = (deltaY > 0) ? 1 : -1;
    int x = absDeltaX >> 1;
    int y = absDeltaY >> 1;
    int px = static_cast<int>(arA.mX);
    int py = static_cast<int>(arA.mY);

    mPixelData.SetPixelAt(px, py, arColor);
    if (absDeltaX >= absDeltaY) {
        for (int i = 0; i < absDeltaX; i++) {
            y += absDeltaY;
            if (y >= absDeltaX) {
                y -= absDeltaX;
                py += signumY;
            }
            px += signumX;
            mPixelData.SetPixelAt(px, py, arColor);
        }
    }
    else {
        for (int i = 0; i < absDeltaY; i++) {
            x += absDeltaX;
            if (x >= absDeltaY) {
                x -= absDeltaY;
                px += signumX;
            }
            py += signumY;
            mPixelData.SetPixelAt(px, py, arColor);
        }
    }
}

void Canvas::DrawPixelData(const Point &arLeftTop, const PixelData &arPixelData,
    const Rect &arSection, Color aColor)
{
    auto oy = arLeftTop.GetY();
    for (int y = arSection.GetTop(); y < arSection.GetHeight(); y++) {
        auto ox = arLeftTop.GetX();
        for (int x = arSection.GetLeft(); x < arSection.GetWidth(); x++) {
            mPixelData.SetPixelAt(ox, oy, arPixelData.GetPixelAt(x, y, aColor));
            ox++;
        }
        oy++;
    }
}

void Canvas::plot4Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
{
    mPixelData.SetPixelAt(aCenterX + aX, aCenterY + aY, arColor);
    mPixelData.SetPixelAt(aCenterX - aX, aCenterY + aY, arColor);
    mPixelData.SetPixelAt(aCenterX + aX, aCenterY - aY, arColor);
    mPixelData.SetPixelAt(aCenterX - aX, aCenterY - aY, arColor);
}

void Canvas::plot8Points(GuiUnit_t aCenterX, GuiUnit_t aCenterY, GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
{
    plot4Points(aCenterX, aCenterY, aX, aY, arColor);
    plot4Points(aCenterX, aCenterY, aY, aX, arColor);
}

void Canvas::Fill(Color aColor)
{
    mPixelData.Fill(aColor);
}

} /* namespace rsp::graphics */

