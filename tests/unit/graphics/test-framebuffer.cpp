/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>

#include "FramebufferCanvas.h"

TEST_CASE("Framebuffer Drawing Primitives") {
    srand(time(NULL));  //generates random seed val
    Framebuffer fb;
    fb.Clear();
    Colour col(0, rand() % 256, rand() % 256, rand() % 256);
    Pen pen(rand() % 10 + 1, col);
    SUBCASE("Drawing Lines") {
        //Arrange
        Point pointA(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
        Point pointB(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);

        //Act
        fb.DrawLine(pointA, pointB, pen);

        //Assert
        int i, x, y, deltaX, deltaY, absDeltaX, absDeltaY, signumX, signumY, px, py;

        deltaX = pointB.x - pointA.x;
        deltaY = pointB.y - pointA.y;
        absDeltaX = abs(deltaX);
        absDeltaY = abs(deltaY);
        signumX = (deltaX > 0) ? 1 : -1;
        signumY = (deltaY > 0) ? 1 : -1;
        x = absDeltaX >> 1;
        y = absDeltaY >> 1;
        px = pointA.x;
        py = pointA.y;
        if (absDeltaX >= absDeltaY) {
            for (i = 0; i < absDeltaX; i++) {
                y += absDeltaY;
                if (y >= absDeltaX) {
                    y -= absDeltaX;
                    py += signumY;
                }
                px += signumX;
                CHECK(fb.GetPixel(Point(px, py), false) == col);
            }
        } else {
            for (i = 0; i < absDeltaY; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK(fb.GetPixel(Point(px, py), false) == col);
            }
        }
        //fb.SwapBuffer();
    }
    SUBCASE("Drawing Rectangles") {
        //Arrange
        //Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(rand() % (fb.vinfo.xres / 2),
                      rand() % (fb.vinfo.yres / 2));
        //Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                          rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));
        Rect rect(leftTop, rightBottom);

        //Act
        fb.DrawRectangle(rect, pen);

        //Assert
        //Expect all four side to hold values
        for (size_t i = 0; i <= rect.GetWidth(); i++) {
            //Check top side
            CHECK(col == fb.GetPixel(Point(leftTop.x + i, leftTop.y), false));
            //Check bottom side
            CHECK(col == fb.GetPixel(Point(leftTop.x + i, rightBottom.y), false));
        }
        for (size_t i = 0; i <= rect.GetHeight(); i++) {
            //Check left side
            CHECK(col == fb.GetPixel(Point(leftTop.x, rightBottom.y - i), false));
            //Check right side
            CHECK(col == fb.GetPixel(Point(rightBottom.x, rightBottom.y - i), false));
        }
        //fb.SwapBuffer();
    }
    SUBCASE("Drawing Circles") {
        //Arrange
        Point centerPoint(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
        int radius = rand() % (fb.vinfo.xres / 2);

        //Act
        fb.DrawDot(centerPoint, pen);
        fb.DrawCircle(centerPoint, radius, pen);

        //Assert
        fb.SwapBuffer();
    }
}