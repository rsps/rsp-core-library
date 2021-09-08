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
    SUBCASE("Drawing Lines") {
        //Arrange
        for (size_t i = 0; i < 10; i++) {
            Point pointA(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
            Point pointB(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
            Colour col(0, rand() % 256, rand() % 256, rand() % 256);
            Pen pen(rand() % 10 + 1, col);

            //Act
            fb.DrawLine(pointA, pointB, pen);

            //Assert
            //CHECK(col == fb.GetPixel(pointA, false));
            //CHECK(col == fb.GetPixel(pointB, false));
        }
        //fb.SwapBuffer();
    }
    SUBCASE("Drawing Rectangles") {
        SUBCASE("Giving Points correctly") {
            //Arrange
            //Generate random values in the LEFT and TOP halves of the screen
            Point leftTop(rand() % (fb.vinfo.xres / 2),
                          rand() % (fb.vinfo.yres / 2));
            //Generate random values in the RIGHT and BOTTOM halves of the screen
            Point rightBottom(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                              rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));

            Colour col(0, rand() % 256, rand() % 256, rand() % 256);
            Rect rect(leftTop, rightBottom);
            Pen pen(rand() % 10 + 1, col);

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
    }
}