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
void Canvas::DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Color &aColor)
{
    THROW_WITH_BACKTRACE1(rsp::utils::NotImplementedException, "Canvas::DrawArc has not been implemented yet.");
}

void Canvas::DrawCircle(const Point &aCenter, int aRadius, const Color &aColor)
{
    int error = -aRadius;
    int y = 0;

    while (aRadius >= y) {
        plot8Points(aCenter.mX, aCenter.mY, aRadius, y, aColor);
        error += y;
        y++;
        error += y;

        if (error >= 0) {
            error += -aRadius;
            aRadius--;
            error += -aRadius;
        }
    }
}

void Canvas::DrawLine(const Point &aA, const Point &aB, const Color &aColor)
{
    int deltaX = aB.mX - aA.mX;
    int deltaY = aB.mY - aA.mY;
    int absDeltaX = abs(deltaX);
    int absDeltaY = abs(deltaY);
    int signumX = (deltaX > 0) ? 1 : -1;
    int signumY = (deltaY > 0) ? 1 : -1;
    int x = absDeltaX >> 1;
    int y = absDeltaY >> 1;
    int px = aA.mX;
    int py = aA.mY;

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
    if (aFilled) {
        for (int y = aRect.mLeftTop.mY; y <= aRect.mRightBottom.mY; y++) {
            for (int x = aRect.mLeftTop.mX; x <= aRect.mRightBottom.mX; x++) {
                SetPixel(Point(x, y), aColor);
            }
        }
    }
    else {
        for (int i = aRect.mLeftTop.mX; i <= aRect.mRightBottom.mX; i++) {
            SetPixel(Point(i, aRect.mLeftTop.mY), aColor);     // top
            SetPixel(Point(i, aRect.mRightBottom.mY), aColor); // bottom
        }
        for (int i = aRect.mLeftTop.mY; i <= aRect.mRightBottom.mY; i++) {
            SetPixel(Point(aRect.mLeftTop.mX, i), aColor);     // left
            SetPixel(Point(aRect.mRightBottom.mX, i), aColor); // right
        }
    }
}

void Canvas::DrawImage(const Point &arLeftTop, const Bitmap &arBitmap, Color aColor)
{
    DrawImageSection(arLeftTop, arBitmap, Rect(0u, 0u, arBitmap.GetWidth(), arBitmap.GetHeight()), aColor);
}

void Canvas::DrawImageSection(const Point &arLeftTop, const Bitmap &arBitmap, const Rect &arSection, Color aColor)
{
//    std::cout << "DrawImageSection(Point(" << arLeftTop << "), Bitmap(" << arBitmap << "), Rect(" << arSection << "), "<< aColor << ")" << std::endl;
//    Point origin(aLeftTop.GetX(), aLeftTop.GetY() + int(aBitmap.GetHeight()));
    Point origin = arLeftTop;
    auto pixels = arBitmap.GetPixelData();
    for (int y = arSection.GetTop(); y < arSection.GetHeight(); y++) {
        origin.mX = arLeftTop.GetX();
        for (int x = arSection.GetLeft(); x < arSection.GetWidth(); x++) {
//            std::cout << "SetPixel(" << origin << ", GetPixel(" << x << ", " << y << ")->" << pixels.GetPixelAt(x, y, aColor) << ")" << std::endl;
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
    for (const Glyph &glyph : arText.GetGlyphs()) {
        for (int y = 0; y < glyph.mHeight; y++) {
            long unsigned int index = static_cast<long unsigned int>(y * glyph.mWidth);
            for (int x = 0; x < glyph.mWidth; x++) {
                uint8_t c = glyph.mPixels[index++];
                auto p = Point(x + glyph.mLeft + arText.GetArea().GetLeft(), y + glyph.mTop + arText.GetArea().GetTop());
                if (c && arText.GetArea().IsHit(p)) {
                    col.SetAlpha(c);
                    SetPixel(p, col);
                }
            }
        }
    }
}

} // namespace rsp::graphics
