/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/primitives/Bitmap.h"

#include <algorithm>
#include <chrono>
#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <logging/Logger.h>

namespace rsp::graphics
{
void Canvas::DrawArc(const Point &aCenter, GuiUnit_t aRadius1, GuiUnit_t aRadius2, int aStartAngel, int aSweepAngle, const Color &aColor)
{
    THROW_WITH_BACKTRACE1(rsp::utils::NotImplementedException, "Canvas::DrawArc has not been implemented yet.");
}

void Canvas::DrawCircle(const Point &aCenter, GuiUnit_t aRadius, const Color &aColor)
{
    int error = -static_cast<int>(aRadius);
    GuiUnit_t y = 0;

    while (aRadius >= y) {
        plot8Points(aCenter.mX, aCenter.mY, aRadius, y, aColor);
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

void Canvas::DrawLine(const Point &aA, const Point &aB, const Color &aColor)
{
    int deltaX = static_cast<int>(aB.mX - aA.mX);
    int deltaY = static_cast<int>(aB.mY - aA.mY);
    int absDeltaX = abs(deltaX);
    int absDeltaY = abs(deltaY);
    int signumX = (deltaX > 0) ? 1 : -1;
    int signumY = (deltaY > 0) ? 1 : -1;
    int x = absDeltaX >> 1;
    int y = absDeltaY >> 1;
    int px = static_cast<int>(aA.mX);
    int py = static_cast<int>(aA.mY);

    SetPixel(aA, aColor);
    if (absDeltaX >= absDeltaY) {
        for (int i = 0; i < absDeltaX; i++) {
            y += absDeltaY;
            if (y >= absDeltaX) {
                y -= absDeltaX;
                py += signumY;
            }
            px += signumX;
            SetPixel(Point(px, py), aColor);
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
            SetPixel(Point(px, py), aColor);
        }
    }
}

void Canvas::DrawRectangle(const Rect &aRect, const Color &aColor, bool aFilled)
{
    Rect r = aRect & mClipRect;
    GuiUnit_t h_end = r.mLeftTop.mY + r.mHeight;
    GuiUnit_t w_end = r.mLeftTop.mX + r.mWidth;
    if (aFilled) {
        for (GuiUnit_t y = r.mLeftTop.mY; y < h_end; y++) {
            for (GuiUnit_t x = r.mLeftTop.mX; x < w_end; x++) {
                SetPixel(Point(x, y), aColor);
            }
        }
    }
    else {
        Point rb = r.GetBottomRight();
        for (GuiUnit_t x = r.mLeftTop.mX; x < w_end; x++) {
            SetPixel(Point(x, r.mLeftTop.mY), aColor); // top
            SetPixel(Point(x, rb.mY), aColor); // bottom
        }
        for (GuiUnit_t y = r.mLeftTop.mY; y < h_end; y++) {
            SetPixel(Point(r.mLeftTop.mX, y), aColor); // left
            SetPixel(Point(rb.mX, y), aColor); // right
        }
    }
}

void Canvas::DrawImage(const Point &arLeftTop, const Bitmap &arBitmap, Color aColor)
{
    DrawImageSection(arLeftTop, arBitmap, Rect(0u, 0u, arBitmap.GetWidth(), arBitmap.GetHeight()), aColor);
}

void Canvas::DrawImageSection(const Point &arLeftTop, const Bitmap &arBitmap, const Rect &arSection, Color aColor)
{
//    std::cout << "DrawImageSection(Point(" << arLeftTop << "), Bitmap(" << arBitmap << "), Rect(" << arSection << "), "<< std::hex << std::setw(8) << std::setfill('0') << aColor << std::dec << ")" << std::endl;
    Point origin = arLeftTop;
    auto &pixels = arBitmap.GetPixelData();
    for (int y = arSection.GetTop(); y < arSection.GetHeight(); y++) {
        origin.mX = arLeftTop.GetX();
        for (int x = arSection.GetLeft(); x < arSection.GetWidth(); x++) {
            SetPixel(origin, pixels.GetPixelAt(x, y, aColor));
            origin.mX++;
        }
        origin.mY++;
    }
}

void Canvas::DrawText(Text &arText)
{
    DrawText(arText, arText.GetFont().GetColor());
}

void Canvas::DrawText(const Text &arText, const Color &arColor)
{
    Color col = arColor;
    auto &glyphs = arText.GetGlyphs();
    for (int i=0; i < glyphs.GetCount() ; ++i) {
        Glyph &glyph = glyphs.GetGlyph(i);
        for (int y = 0; y < glyph.mHeight; y++) {
            uint8_t p_row = glyph.GetPixelRow(y);
            for (int x = 0; x < glyph.mWidth; x++) {
                auto c = *p_row++;
                auto p = Point(x + glyph.mLeft + arText.GetArea().GetLeft(), y + glyph.mTop + arText.GetArea().GetTop());
                if (c && arText.GetArea().IsHit(p)) {
                    col.SetAlpha(c);
                    SetPixel(p, col);
                }
            }
        }
    }
}

Canvas& Canvas::SetClipRect(const Rect &arClipRect)
{
    mClipRect = Rect(0u, 0u, mWidth, mHeight) & arClipRect;
    return *this;
}

} // namespace rsp::graphics
