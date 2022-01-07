/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Canvas.h>

#include <chrono>
#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>

namespace rsp::graphics
{
void Canvas::DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Color &aColor)
{
    throw rsp::utils::NotImplementedException("");
}

void Canvas::DrawCircle(const Point &aCenter, int aRadius, const Color &aColor)
{
    int error = -aRadius;
    // int x = aRadius;
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
    int i, x, y, deltaX, deltaY, absDeltaX, absDeltaY, signumX, signumY, px, py;

    deltaX = aB.mX - aA.mX;
    deltaY = aB.mY - aA.mY;
    absDeltaX = abs(deltaX);
    absDeltaY = abs(deltaY);
    signumX = (deltaX > 0) ? 1 : -1;
    signumY = (deltaY > 0) ? 1 : -1;
    x = absDeltaX >> 1;
    y = absDeltaY >> 1;
    px = aA.mX;
    py = aA.mY;

    SetPixel(aA, aColor);
    if (absDeltaX >= absDeltaY) {
        for (i = 0; i < absDeltaX; i++) {
            y += absDeltaY;
            if (y >= absDeltaX) {
                y -= absDeltaX;
                py += signumY;
            }
            px += signumX;
            SetPixel(Point(px, py), aColor);
        }
    } else {
        for (i = 0; i < absDeltaY; i++) {
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

void Canvas::DrawRectangle(const Rect &aRect, const Color &aColor)
{
    for (int i = aRect.mLeftTop.mX; i <= aRect.mRightBottom.mX; i++) {
        SetPixel(Point(i, aRect.mLeftTop.mY), aColor);     // top
        SetPixel(Point(i, aRect.mRightBottom.mY), aColor); // bottom
    }
    for (int i = aRect.mLeftTop.mY; i <= aRect.mRightBottom.mY; i++) {
        SetPixel(Point(aRect.mLeftTop.mX, i), aColor);     // left
        SetPixel(Point(aRect.mRightBottom.mX, i), aColor); // right
    }
}

void Canvas::DrawImage(const Point &aLeftTop, const Bitmap &aBitmap)
{
    int iter = 0;
    auto pixels = aBitmap.GetPixels();
    for (size_t h = 0; h < aBitmap.GetHeight(); h++) {
        for (size_t w = 0; w < aBitmap.GetWidth(); w++) {
            SetPixel(Point(aLeftTop.mX + w, aLeftTop.mY + h), pixels[iter]);
            iter++;
        }
    }
}

void Canvas::DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit)
{
    throw rsp::utils::NotImplementedException("Draw Text is not yet implemented");
}
} // namespace rsp::graphics