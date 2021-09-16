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
inline void CheckPixel(const Point &aPoint, const Colour &aColour, const Framebuffer &fb) {
    if (fb.IsInsideScreen(aPoint)) {
        CHECK(fb.GetPixel(aPoint) == aColour);
    } else {
        CHECK(fb.GetPixel(aPoint) == 0);
    }
}
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
        SUBCASE("Lines are Inclusive") {
            CHECK_EQ(fb.GetPixel(pointA, false), col);
            CHECK_EQ(fb.GetPixel(pointB, false), col);
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
        //fb.DrawDot(centerPoint, pen);
        fb.DrawCircle(centerPoint, radius, pen);

        //Assert
        int error = -radius;
        int y = 0;
        while (radius >= y) {
            CheckPixel(Point(centerPoint.x + radius, centerPoint.y + y), col, fb);
            CheckPixel(Point(centerPoint.x - radius, centerPoint.y + y), col, fb);
            CheckPixel(Point(centerPoint.x + radius, centerPoint.y - y), col, fb);
            CheckPixel(Point(centerPoint.x - radius, centerPoint.y - y), col, fb);
            CheckPixel(Point(centerPoint.x + y, centerPoint.y + radius), col, fb);
            CheckPixel(Point(centerPoint.x - y, centerPoint.y + radius), col, fb);
            CheckPixel(Point(centerPoint.x + y, centerPoint.y - radius), col, fb);
            CheckPixel(Point(centerPoint.x - y, centerPoint.y - radius), col, fb);
            error += y;
            y++;
            error += y;

            if (error >= 0) {
                error += -radius;
                radius--;
                error += -radius;
            }
        }
        fb.SwapBuffer();
    }
    /*SUBCASE("Set/Get pixel outside screen") {
        //Arrange
        Point outSideXAxis(-1, 0);
        Point outSideYAxis(0, -1);

        //Act
        CHECK_NOTHROW(fb.DrawDot(outSideXAxis, pen));
        CHECK_NOTHROW(fb.DrawDot(outSideYAxis, pen));

        //Assert
        CHECK_EQ(fb.GetPixel(outSideXAxis), 0);
        CHECK_EQ(fb.GetPixel(outSideYAxis), 0);
    }

    SUBCASE("Drawing Images") {
        //Arrange
        Point topLeftPoint(100, 200);

        std::string filepath = "testImages/testImageCross.bmp";
        Bitmap bitmap(filepath);

        //Act
        fb.DrawImage(topLeftPoint, bitmap);

        //Assert
        fb.SwapBuffer();
    }*/
}
